/*
* Copryight (C) 2017 Ultralight, Inc. All rights reserved.
* Copyright (C) 2007-2017 Apple Inc. All rights reserved.
* Copyright (C) 2008 Collabora, Ltd. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1.  Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
* 2.  Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
* 3.  Neither the name of Apple Inc. ("Apple") nor the names of
*     its contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "FileSystemWin.h"
#include <io.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <sys/stat.h>
#include <windows.h>
#include <limits>
#include <stdio.h>
#include <Ultralight/String.h>
#include <string>
#include <algorithm>
#include <memory>
#include <Strsafe.h>

namespace ultralight {

class PathWalker {
public:
  PathWalker(LPCWSTR directory, LPCWSTR pattern) {
    WCHAR szDir[MAX_PATH];

    StringCchCopyW(szDir, MAX_PATH, directory);
    StringCchCatW(szDir, MAX_PATH, pattern);

    m_handle = ::FindFirstFileW(szDir, &m_data);
  }

  ~PathWalker() {
    if (!isValid())
      return;
    ::FindClose(m_handle);
  }

  bool isValid() const { return m_handle != INVALID_HANDLE_VALUE; }
  const WIN32_FIND_DATAW& data() const { return m_data; }

  bool step() {
    return !!::FindNextFileW(m_handle, &m_data);
  }

private:
  HANDLE m_handle;
  WIN32_FIND_DATAW m_data;
};

static const ULONGLONG kSecondsFromFileTimeToTimet = 11644473600;

static bool getFindData(LPCWSTR path, WIN32_FIND_DATAW& findData)
{
  HANDLE handle = FindFirstFileW(path, &findData);
  if (handle == INVALID_HANDLE_VALUE)
    return false;
  FindClose(handle);
  return true;
}

static bool getFileSizeFromFindData(const WIN32_FIND_DATAW& findData, int64_t& size)
{
  ULARGE_INTEGER fileSize;
  fileSize.HighPart = findData.nFileSizeHigh;
  fileSize.LowPart = findData.nFileSizeLow;

  if (fileSize.QuadPart > static_cast<ULONGLONG>(std::numeric_limits<int64_t>::max()))
    return false;

  size = fileSize.QuadPart;
  return true;
}

static bool getFileSizeFromByHandleFileInformationStructure(const BY_HANDLE_FILE_INFORMATION& fileInformation, int64_t& size)
{
  ULARGE_INTEGER fileSize;
  fileSize.HighPart = fileInformation.nFileSizeHigh;
  fileSize.LowPart = fileInformation.nFileSizeLow;

  if (fileSize.QuadPart > static_cast<ULONGLONG>(std::numeric_limits<int64_t>::max()))
    return false;

  size = fileSize.QuadPart;
  return true;
}

static void getFileCreationTimeFromFindData(const WIN32_FIND_DATAW& findData, time_t& time)
{
  ULARGE_INTEGER fileTime;
  fileTime.HighPart = findData.ftCreationTime.dwHighDateTime;
  fileTime.LowPart = findData.ftCreationTime.dwLowDateTime;

  // Information about converting time_t to FileTime is available at http://msdn.microsoft.com/en-us/library/ms724228%28v=vs.85%29.aspx
  time = fileTime.QuadPart / 10000000 - kSecondsFromFileTimeToTimet;
}


static void getFileModificationTimeFromFindData(const WIN32_FIND_DATAW& findData, time_t& time)
{
  ULARGE_INTEGER fileTime;
  fileTime.HighPart = findData.ftLastWriteTime.dwHighDateTime;
  fileTime.LowPart = findData.ftLastWriteTime.dwLowDateTime;

  // Information about converting time_t to FileTime is available at http://msdn.microsoft.com/en-us/library/ms724228%28v=vs.85%29.aspx
  time = fileTime.QuadPart / 10000000 - kSecondsFromFileTimeToTimet;
}

bool getFileSize(LPCWSTR path, int64_t& size)
{
  WIN32_FIND_DATAW findData;
  if (!getFindData(path, findData))
    return false;

  return getFileSizeFromFindData(findData, size);
}

bool getFileSize(FileHandle fileHandle, int64_t& size)
{
  BY_HANDLE_FILE_INFORMATION fileInformation;
  if (!::GetFileInformationByHandle((HANDLE)fileHandle, &fileInformation))
    return false;

  return getFileSizeFromByHandleFileInformationStructure(fileInformation, size);
}

bool getFileModificationTime(LPCWSTR path, time_t& time)
{
  WIN32_FIND_DATAW findData;
  if (!getFindData(path, findData))
    return false;

  getFileModificationTimeFromFindData(findData, time);
  return true;
}

bool getFileCreationTime(LPCWSTR path, time_t& time)
{
  WIN32_FIND_DATAW findData;
  if (!getFindData(path, findData))
    return false;

  getFileCreationTimeFromFindData(findData, time);
  return true;
}

std::wstring GetMimeType(const std::wstring &szExtension)
{
  // return mime type for extension
  HKEY hKey = NULL;
  std::wstring szResult = L"application/unknown";

  // open registry key
  if (RegOpenKeyExW(HKEY_CLASSES_ROOT, szExtension.c_str(),
    0, KEY_READ, &hKey) == ERROR_SUCCESS)
  {
    // define buffer
    wchar_t szBuffer[256] = { 0 };
    DWORD dwBuffSize = sizeof(szBuffer);

    // get content type
    if (RegQueryValueExW(hKey, L"Content Type", NULL, NULL,
      (LPBYTE)szBuffer, &dwBuffSize) == ERROR_SUCCESS)
    {
      // success
      szResult = szBuffer;
    }

    // close key
    RegCloseKey(hKey);
  }

  // return result
  return szResult;
}

FileSystemWin::FileSystemWin(LPCWSTR baseDir) {
  baseDir_.reset(new WCHAR[_MAX_PATH]);
  StringCchCopyW(baseDir_.get(), MAX_PATH, baseDir);
}

FileSystemWin::~FileSystemWin() {}

bool FileSystemWin::FileExists(const String16& path) {
  WIN32_FIND_DATAW findData;
  return getFindData(GetRelative(path).get(), findData);
}

bool FileSystemWin::DeleteFile_(const String16& path) {
  return !!DeleteFileW(GetRelative(path).get());
}

bool FileSystemWin::DeleteEmptyDirectory(const String16& path) {
  return !!RemoveDirectoryW(GetRelative(path).get());
}

bool FileSystemWin::MoveFile_(const String16& old_path, const String16& new_path) {
  return !!::MoveFileExW(GetRelative(old_path).get(), GetRelative(new_path).get(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
}

bool FileSystemWin::GetFileSize(const String16& path, int64_t& result) {
  WIN32_FIND_DATAW findData;
  if (!getFindData(GetRelative(path).get(), findData))
    return false;

  return getFileSizeFromFindData(findData, result);
}

bool FileSystemWin::GetFileMimeType(const String16& path, String16& result)
{
  LPWSTR ext = PathFindExtensionW(path.data());
  std::wstring mimetype = GetMimeType(ext);
  result = String16(mimetype.c_str(), mimetype.length());
  return true;
}

bool FileSystemWin::GetFileSize(FileHandle handle, int64_t& result) {
  BY_HANDLE_FILE_INFORMATION fileInformation;
  if (!::GetFileInformationByHandle((HANDLE)handle, &fileInformation))
    return false;

  return getFileSizeFromByHandleFileInformationStructure(fileInformation, result);
}

bool FileSystemWin::GetFileModificationTime(const String16& path, time_t& result) {
  WIN32_FIND_DATAW findData;
  if (!getFindData(GetRelative(path).get(), findData))
    return false;

  getFileModificationTimeFromFindData(findData, result);
  return true;
}

bool FileSystemWin::GetFileCreationTime(const String16& path, time_t& result) {
  WIN32_FIND_DATAW findData;
  if (!getFindData(GetRelative(path).get(), findData))
    return false;

  getFileCreationTimeFromFindData(findData, result);
  return true;
}

MetadataType FileSystemWin::GetMetadataType(const String16& path) {
  WIN32_FIND_DATAW findData;
  if (!getFindData(GetRelative(path).get(), findData))
    return kMetadataType_Unknown;

  return (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? kMetadataType_Directory : kMetadataType_File;
}

String16 FileSystemWin::GetPathByAppendingComponent(const String16& path, const String16& component) {
  if (path.length() > MAX_PATH)
    return String16();

  std::unique_ptr<Char16[]> buffer(new Char16[MAX_PATH]);
  memcpy(buffer.get(), path.data(), path.length() * sizeof(Char16));

  if (!PathAppendW(buffer.get(), component.data()))
    return String16();

  return String16(buffer.get(), wcslen(buffer.get()));
}

bool FileSystemWin::CreateDirectory_(const String16& path) {
  if (SHCreateDirectoryExW(0, GetRelative(path).get(), 0) != ERROR_SUCCESS) {
    DWORD error = GetLastError();
    if (error != ERROR_FILE_EXISTS && error != ERROR_ALREADY_EXISTS) {
      String8 utf8 = String(path).utf8();
      fprintf(stderr, "FileSystemWin::CreateDirectory_, Failed to create path %s", utf8.data());
      return false;
    }
  }
  return true;
}

String16 FileSystemWin::GetHomeDirectory() {
  return String16();
}

String16 FileSystemWin::GetFilenameFromPath(const String16& path) {
  LPWSTR filename = ::PathFindFileNameW(path.data());
  return String16(filename, wcslen(filename));
}

String16 FileSystemWin::GetDirectoryNameFromPath(const String16& path) {
  std::unique_ptr<Char16[]> utf16(new Char16[path.length()]);
  memcpy(utf16.get(), path.data(), path.length() * sizeof(Char16));
  if (::PathRemoveFileSpecW(utf16.get()))
    return String16(utf16.get(), wcslen(utf16.get()));

  return path;
}

bool FileSystemWin::GetVolumeFreeSpace(const String16& path, uint64_t& result) {
  return false;
}

int32_t FileSystemWin::GetVolumeId(const String16& path) {
  HANDLE handle = (HANDLE)OpenFile(path, false);
  if (handle == INVALID_HANDLE_VALUE)
    return 0;

  BY_HANDLE_FILE_INFORMATION fileInformation = {};
  if (!::GetFileInformationByHandle(handle, &fileInformation)) {
    CloseFile((FileHandle&)handle);
    return 0;
  }

  CloseFile((FileHandle&)handle);

  return fileInformation.dwVolumeSerialNumber;
}

Ref<String16Vector> FileSystemWin::ListDirectory(const String16& path, const String16& filter) {
  Ref<String16Vector> entries = String16Vector::Create();

  PathWalker walker(GetRelative(path).get(), filter.data());
  if (!walker.isValid())
    return entries;

  do {
    if (walker.data().dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      continue;

    String16 filename(walker.data().cFileName, wcslen(walker.data().cFileName));
    entries->push_back(path + "\\" + filename);
  } while (walker.step());

  return entries;
}

String16 FileSystemWin::OpenTemporaryFile(const String16& prefix, FileHandle& handle) {
  handle = (FileHandle)INVALID_HANDLE_VALUE;

  wchar_t tempPath[MAX_PATH];
  int tempPathLength = ::GetTempPathW(MAX_PATH, tempPath);
  if (tempPathLength <= 0 || tempPathLength > MAX_PATH)
    return String16();

  String16 proposedPath;
  do {
    wchar_t tempFile[] = L"XXXXXXXX.tmp"; // Use 8.3 style name (more characters aren't helpful due to 8.3 short file names)
    const int randomPartLength = 8;

    // Limit to valid filesystem characters, also excluding others that could be problematic, like punctuation.
    // don't include both upper and lowercase since Windows file systems are typically not case sensitive.
    const char validChars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < randomPartLength; ++i)
      tempFile[i] = validChars[rand() % (sizeof(validChars) - 1)];

    proposedPath = GetPathByAppendingComponent(String16(tempPath, wcslen(tempPath)), String16(tempFile, wcslen(tempFile)));
    if (proposedPath.empty())
      break;

    // use CREATE_NEW to avoid overwriting an existing file with the same name
    handle = (FileHandle)::CreateFileW(proposedPath.data(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
  } while (handle == (FileHandle)INVALID_HANDLE_VALUE && GetLastError() == ERROR_ALREADY_EXISTS);

  if (handle == (FileHandle)INVALID_HANDLE_VALUE)
    return String16();

  return proposedPath;
}

FileHandle FileSystemWin::OpenFile(const String16& path, bool open_for_writing) {
  DWORD desiredAccess = 0;
  DWORD creationDisposition = 0;
  DWORD shareMode = 0;
  if (open_for_writing) {
    desiredAccess = GENERIC_WRITE;
    creationDisposition = CREATE_ALWAYS;
  } else {
    desiredAccess = GENERIC_READ;
    creationDisposition = OPEN_EXISTING;
    shareMode = FILE_SHARE_READ;
  }

  return (FileHandle)CreateFileW(GetRelative(path).get(), desiredAccess, shareMode, 
    0, creationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
}

void FileSystemWin::CloseFile(FileHandle& handle) {
  if (handle != (FileHandle)INVALID_HANDLE_VALUE) {
    ::CloseHandle((HANDLE)handle);
    handle = (FileHandle)INVALID_HANDLE_VALUE;
  }
}

int64_t FileSystemWin::SeekFile(FileHandle handle, int64_t offset, FileSeekOrigin origin) {
  DWORD moveMethod = FILE_BEGIN;

  if (origin == kFileSeekOrigin_Current)
    moveMethod = FILE_CURRENT;
  else if (origin == kFileSeekOrigin_End)
    moveMethod = FILE_END;

  LARGE_INTEGER largeOffset;
  largeOffset.QuadPart = offset;

  largeOffset.LowPart = SetFilePointer((HANDLE)handle, largeOffset.LowPart, &largeOffset.HighPart, moveMethod);

  if (largeOffset.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
    return -1;

  return largeOffset.QuadPart;
}

bool FileSystemWin::TruncateFile(FileHandle handle, int64_t offset) {
  return false;
}

int64_t FileSystemWin::WriteToFile(FileHandle handle, const char* data, int64_t length) {
  if (handle == (FileHandle)INVALID_HANDLE_VALUE)
    return -1;

  DWORD bytesWritten;
  bool success = !!WriteFile((HANDLE)handle, data, (DWORD)length, &bytesWritten, 0);

  if (!success)
    return -1;
  return static_cast<int64_t>(bytesWritten);
}

int64_t FileSystemWin::ReadFromFile(FileHandle handle, char* data, int64_t length) {
  if (handle == (FileHandle)INVALID_HANDLE_VALUE)
    return -1;

  DWORD bytesRead;
  bool success = !!::ReadFile((HANDLE)handle, data, (DWORD)length, &bytesRead, 0);

  if (!success)
    return -1;
  return static_cast<int64_t>(bytesRead);
}

bool FileSystemWin::CopyFile_(const String16& source, const String16& destination) {
  return !!::CopyFileW(GetRelative(source).get(), GetRelative(destination).get(), TRUE);
}

std::unique_ptr<WCHAR[]> FileSystemWin::GetRelative(const String16& path) {
  std::unique_ptr<WCHAR[]> relPath(new WCHAR[_MAX_PATH]);
  PathCombineW(relPath.get(), baseDir_.get(), path.data());
  return relPath;
}

}  // namespace ultralight

namespace framework {

ultralight::FileSystem* CreatePlatformFileSystem(const char* baseDir) {
  std::string baseDirStr(baseDir);
  std::wstring baseDirWStr(baseDirStr.begin(), baseDirStr.end());

  WCHAR cur_dir[_MAX_PATH];
  GetCurrentDirectoryW(_MAX_PATH, cur_dir);
  WCHAR absolute_dir[_MAX_PATH];
  PathCombineW(absolute_dir, cur_dir, baseDirWStr.c_str());

  return new ultralight::FileSystemWin(absolute_dir);
}

}
