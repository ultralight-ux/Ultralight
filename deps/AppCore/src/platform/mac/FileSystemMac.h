#pragma once
#include <Ultralight/platform/FileSystem.h>

namespace ultralight {

class FileSystemMac : public FileSystem {
 public:
    FileSystemMac(const String16& baseDir);
    virtual ~FileSystemMac();
    
    virtual bool FileExists(const String16& path);
    
    virtual bool DeleteFile_(const String16& path);

    virtual bool DeleteEmptyDirectory(const String16& path);

    virtual bool MoveFile_(const String16& old_path, const String16& new_path);

    virtual bool GetFileSize(const String16& path, int64_t& result);

    virtual bool GetFileSize(FileHandle handle, int64_t& result);

    virtual bool GetFileMimeType(const String16& path, String16& result);

    virtual bool GetFileModificationTime(const String16& path, time_t& result);

    virtual bool GetFileCreationTime(const String16& path, time_t& result);

    virtual MetadataType GetMetadataType(const String16& path);

    virtual String16 GetPathByAppendingComponent(const String16& path, const String16& component);

    virtual bool CreateDirectory_(const String16& path);

    virtual String16 GetHomeDirectory();

    virtual String16 GetFilenameFromPath(const String16& path);

    virtual String16 GetDirectoryNameFromPath(const String16& path);

    virtual bool GetVolumeFreeSpace(const String16& path, uint64_t& result);

    virtual int32_t GetVolumeId(const String16& path);

    virtual Ref<String16Vector> ListDirectory(const String16& path, const String16& filter);

    virtual String16 OpenTemporaryFile(const String16& prefix, FileHandle& handle);

    virtual FileHandle OpenFile(const String16& path, bool open_for_writing);

    virtual void CloseFile(FileHandle& handle);

    virtual int64_t SeekFile(FileHandle handle, int64_t offset, FileSeekOrigin origin);

    virtual bool TruncateFile(FileHandle handle, int64_t offset);

    virtual int64_t WriteToFile(FileHandle handle, const char* data, int64_t length);

    virtual int64_t ReadFromFile(FileHandle handle, char* data, int64_t length);

    virtual bool CopyFile_(const String16& source_path, const String16& destination_path);
    
protected:
    String16 base_dir_;
    
    String16 getRelative(const String16& path);
    bool appendFileContentsToFileHandle(const String16& path, FileHandle target);
};
    
}  // namespace ultralight

    
