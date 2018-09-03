#pragma once
#include <Ultralight/platform/FileSystem.h>
#include <string>
#include <map>
#include <fstream>
#include <memory>

namespace ultralight {

/**
 * Basic FileSystem interface, implemented using the C++ STL.
 *
 * This provides only the minimum needed to load file URLs, eg:
 *          - FileSystem::FileExists
 *          - FileSystem::GetFileSize
 *          - FileSystem::GetFileMimeType
 *          - FileSystem::OpenFile
 *          - FileSystem::ReadFromFile
 *          - FileSystem::CloseFile
 */
class FileSystemBasic : public FileSystem {
 public:
    FileSystemBasic(const char* baseDir);
    virtual ~FileSystemBasic() {}
    
    virtual bool FileExists(const String16& path);
    
    virtual bool DeleteFile_(const String16& path) { return false; }

    virtual bool DeleteEmptyDirectory(const String16& path) { return false; }

    virtual bool MoveFile_(const String16& old_path, const String16& new_path) { return false; }

    virtual bool GetFileSize(const String16& path, int64_t& result);

    virtual bool GetFileSize(FileHandle handle, int64_t& result);

    virtual bool GetFileMimeType(const String16& path, String16& result);

    virtual bool GetFileModificationTime(const String16& path, time_t& result) { return false; }

    virtual bool GetFileCreationTime(const String16& path, time_t& result) { return false; }

    virtual MetadataType GetMetadataType(const String16& path) { return kMetadataType_Unknown; }

    virtual String16 GetPathByAppendingComponent(const String16& path, const String16& component) { return String16(); }

    virtual bool CreateDirectory_(const String16& path) { return false; }

    virtual String16 GetHomeDirectory() { return String16(); }

    virtual String16 GetFilenameFromPath(const String16& path) { return String16(); }

    virtual String16 GetDirectoryNameFromPath(const String16& path) { return String16(); } 

    virtual bool GetVolumeFreeSpace(const String16& path, uint64_t& result) { return false; }

    virtual int32_t GetVolumeId(const String16& path) { return 0; }

    virtual Ref<String16Vector> ListDirectory(const String16& path, const String16& filter) { return String16Vector::Create(); }

    virtual String16 OpenTemporaryFile(const String16& prefix, FileHandle& handle) { return String16(); }

    virtual FileHandle OpenFile(const String16& path, bool open_for_writing);

    virtual void CloseFile(FileHandle& handle);

    virtual int64_t SeekFile(FileHandle handle, int64_t offset, FileSeekOrigin origin) { return 0; }

    virtual bool TruncateFile(FileHandle handle, int64_t offset) { return false; }

    virtual int64_t WriteToFile(FileHandle handle, const char* data, int64_t length) { return 0; }

    virtual int64_t ReadFromFile(FileHandle handle, char* data, int64_t length);

    virtual bool CopyFile_(const String16& source_path, const String16& destination_path) { return false; }
    
protected:
    std::string baseDir_;
    std::string getRelative(const String16& path);
    FileHandle next_handle_ = 0;
    std::map<FileHandle, std::unique_ptr<std::ifstream>> open_files_;
};
    
}  // namespace ultralight

