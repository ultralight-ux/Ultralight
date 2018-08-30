#include <Framework/platform/common/FileSystemBasic.h>

namespace ultralight {

FileSystemBasic::FileSystemBasic(const String16& baseDir) : base_dir_(baseDir) {}

String16 FileSystemBasic::getRelative(const String16& path) {
}

bool FileSystemBasic::FileExists(const String16& path) {
}

bool FileSystemBasic::GetFileSize(const String16& path, int64_t& result) {
}

bool FileSystemBasic::GetFileSize(FileHandle handle, int64_t& result) {
}

bool FileSystemBasic::GetFileMimeType(const String16& path, String16& result) {
}

FileHandle FileSystemBasic::OpenFile(const String16& path, bool open_for_writing) {
}

void FileSystemBasic::CloseFile(FileHandle& handle) {
}

int64_t FileSystemBasic::ReadFromFile(FileHandle handle, char* data, int64_t length) {
}

}  // namespace ultralight

