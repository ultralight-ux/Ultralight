///
/// @file FileSystem.h
///
/// @brief The header for the FileSystem interface.
///
/// @author
///
/// This file is a part of Ultralight, a fast, lightweight, HTML UI engine
///
/// Website: <http://ultralig.ht>
///
/// Copyright (C) 2019 Ultralight, Inc. All rights reserved.
///
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/String16.h>
#include <time.h>

namespace ultralight {

///
/// File Handle type used as unique ID for opened files.
///
#if defined(__WIN32__) || defined(_WIN32)
typedef size_t FileHandle;
#else
typedef int FileHandle;
#endif

///
/// Handle used to denote an invalid file.
///
const FileHandle invalidFileHandle = (FileHandle)-1;

///
/// The position to seek from in a file, @see FileSystem::SeekFile
///
enum FileSeekOrigin {
  kFileSeekOrigin_Beginning = 0,
  kFileSeekOrigin_Current,
  kFileSeekOrigin_End
};

///
/// The type of path, @see FileSystem::GetMetadata
///
enum MetadataType {
  kMetadataType_Unknown = 0,
  kMetadataType_File,
  kMetadataType_Directory,
};

///
/// @brief  FileSystem interface, used for all file system operations.
///          
/// This is used for things like loading File URLs (eg, <file:///page.html> and
/// the JavaScript FileSystem API. If you don't implement this interface, you
/// will not be able to load any File URLs.
///
///
/// This is intended to be implemented by users and defined before creating the
/// Renderer. @see Platform::set_file_system.
///
/// @note  To support File URL loading, you ONLY need to implement the 
///        following functions:
///          - FileSystem::FileExists
///          - FileSystem::GetFileSize
///          - FileSystem::GetFileMimeType
///          - FileSystem::OpenFile
///          - FileSystem::ReadFromFile
///          - FileSystem::CloseFile
///
class UExport FileSystem {
public:
  virtual ~FileSystem();

  ///
  /// Check if file path exists, return true if exists.
  ///
  virtual bool FileExists(const String16& path) = 0;

  ///
  /// Delete file, return true on success.
  ///
  virtual bool DeleteFile_(const String16& path) = 0;

  ///
  /// Delete empty directory, return true on success.
  ///
  virtual bool DeleteEmptyDirectory(const String16& path) = 0;

  ///
  /// Move file, return true on success.
  ///
  virtual bool MoveFile_(const String16& old_path, const String16& new_path) = 0;

  ///
  /// Get file size, store result in 'result'. Return true on success.
  ///
  virtual bool GetFileSize(const String16& path, int64_t& result) = 0;

  ///
  /// Get file size of previously opened file, store result in 'result'. Return true on success.
  ///
  virtual bool GetFileSize(FileHandle handle, int64_t& result) = 0;

  ///
  /// Get file mime type (eg "text/html"), store result in 'result'. Return true on success.
  ///
  virtual bool GetFileMimeType(const String16& path, String16& result) = 0;

  ///
  /// Get file last modification time, store result in 'result'. Return true on success.
  ///
  virtual bool GetFileModificationTime(const String16& path, time_t& result) = 0;

  ///
  /// Get file creation time, store result in 'result'. Return true on success.
  ///
  virtual bool GetFileCreationTime(const String16& path, time_t& result) = 0;

  ///
  /// Get path type (file or directory).
  ///
  virtual MetadataType GetMetadataType(const String16& path) = 0;

  ///
  /// Concatenate path with another path component. Return concatenated result.
  ///
  virtual String16 GetPathByAppendingComponent(const String16& path, const String16& component) = 0;

  ///
  /// Create directory, return true on success.
  ///
  virtual bool CreateDirectory_(const String16& path) = 0;

  ///
  /// Get home directory path.
  ///
  virtual String16 GetHomeDirectory() = 0;

  ///
  /// Get filename component from path.
  ///
  virtual String16 GetFilenameFromPath(const String16& path) = 0;

  ///
  /// Get directory name from path.
  ///
  virtual String16 GetDirectoryNameFromPath(const String16& path) = 0;

  ///
  /// Get volume from path and store free space in 'result'. Return true on success.
  ///
  virtual bool GetVolumeFreeSpace(const String16& path, uint64_t& result) = 0;

  ///
  /// Get volume from path and return its unique volume id.
  ///
  virtual int32_t GetVolumeId(const String16& path) = 0;

  ///
  /// Get file listing for directory path with optional filter, return vector of file paths.
  ///
  virtual Ref<String16Vector> ListDirectory(const String16& path, const String16& filter) = 0;

  ///
  /// Open a temporary file with suggested prefix, store handle in 'handle'. Return path of temporary file.
  ///
  virtual String16 OpenTemporaryFile(const String16& prefix, FileHandle& handle) = 0;

  ///
  /// Open file path for reading or writing. Return file handle on success, or invalidFileHandle on failure.
  ///
  virtual FileHandle OpenFile(const String16& path, bool open_for_writing) = 0;

  ///
  /// Close previously-opened file.
  ///
  virtual void CloseFile(FileHandle& handle) = 0;

  ///
  /// Seek currently-opened file, with offset relative to certain origin. Return new file offset.
  ///
  virtual int64_t SeekFile(FileHandle handle, int64_t offset, FileSeekOrigin origin) = 0;

  ///
  /// Truncate currently-opened file with offset, return true on success.
  ///
  virtual bool TruncateFile(FileHandle handle, int64_t offset) = 0;

  ///
  /// Write to currently-opened file, return number of bytes written or -1 on failure.
  ///
  virtual int64_t WriteToFile(FileHandle handle, const char* data, int64_t length) = 0;

  ///
  /// Read from currently-opened file, return number of bytes read or -1 on failure.
  ///
  virtual int64_t ReadFromFile(FileHandle handle, char* data, int64_t length) = 0;

  ///
  /// Copy file from source to destination, return true on success.
  ///
  virtual bool CopyFile_(const String16& source_path, const String16& destination_path) = 0;
};

}  // namespace ultralight
