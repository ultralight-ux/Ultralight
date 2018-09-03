#include <Ultralight/platform/FileSystem.h>
#include <Windows.h>
#include <memory>

namespace ultralight {

/**
 * FileSystem implementation for Windows.
 */
class FileSystemWin : public FileSystem {
public:
  // Construct FileSystemWin instance.
  // 
  // @note You can pass a valid baseDir here which will be prepended to
  //       all file paths. This is useful for making all File URLs relative
  //       to your HTML asset directory.
  FileSystemWin(LPCWSTR baseDir);

  virtual ~FileSystemWin();

  virtual bool FileExists(const String16& path) override;

  virtual bool DeleteFile_(const String16& path) override;

  virtual bool DeleteEmptyDirectory(const String16& path) override;

  virtual bool MoveFile_(const String16& old_path, const String16& new_path) override;

  virtual bool GetFileSize(const String16& path, int64_t& result) override;

  virtual bool GetFileSize(FileHandle handle, int64_t& result) override;

  virtual bool GetFileMimeType(const String16& path, String16& result) override;

  virtual bool GetFileModificationTime(const String16& path, time_t& result) override;

  virtual bool GetFileCreationTime(const String16& path, time_t& result) override;

  virtual MetadataType GetMetadataType(const String16& path) override;

  virtual String16 GetPathByAppendingComponent(const String16& path, const String16& component) override;

  virtual bool CreateDirectory_(const String16& path) override;

  virtual String16 GetHomeDirectory() override;

  virtual String16 GetFilenameFromPath(const String16& path) override;

  virtual String16 GetDirectoryNameFromPath(const String16& path) override;

  virtual bool GetVolumeFreeSpace(const String16& path, uint64_t& result) override;

  virtual int32_t GetVolumeId(const String16& path) override;

  virtual Ref<String16Vector> ListDirectory(const String16& path, const String16& filter) override;

  virtual String16 OpenTemporaryFile(const String16& prefix, FileHandle& handle) override;

  virtual FileHandle OpenFile(const String16& path, bool open_for_writing) override;

  virtual void CloseFile(FileHandle& handle) override;

  virtual int64_t SeekFile(FileHandle handle, int64_t offset, FileSeekOrigin origin) override;

  virtual bool TruncateFile(FileHandle handle, int64_t offset) override;

  virtual int64_t WriteToFile(FileHandle handle, const char* data, int64_t length) override;

  virtual int64_t ReadFromFile(FileHandle handle, char* data, int64_t length) override;

  virtual bool CopyFile_(const String16& source_path, const String16& destination_path) override;

protected:
  std::unique_ptr<WCHAR[]> GetRelative(const String16& path);

  std::unique_ptr<WCHAR[]> baseDir_;
};

}  // namespace ultralight
