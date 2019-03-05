#import "FileSystemMac.h"
#import <CoreFoundation/CoreFoundation.h>
#import <CoreServices/CoreServices.h>
#import <Foundation/Foundation.h>
#include <CoreFoundation/CFString.h>
#include <Ultralight/String.h>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <fnmatch.h>
#include <dirent.h>

@interface WebFileManagerDelegate : NSObject <NSFileManagerDelegate>
@end

@implementation WebFileManagerDelegate

- (BOOL)fileManager:(NSFileManager *)fileManager shouldProceedAfterError:(NSError *)error movingItemAtURL:(NSURL *)srcURL toURL:(NSURL *)dstURL
{
    return error.code == NSFileWriteFileExistsError;
}

@end

namespace ultralight {
    
static NSString* ToNSString(const String16& str) {
    auto pathData = [[NSData alloc] initWithBytesNoCopy:const_cast<void*>(reinterpret_cast<const void*>(str.data()))
                                                 length:str.length() * sizeof(Char16)
                                           freeWhenDone:NO];
    return [[NSString alloc] initWithData:pathData
                                 encoding:NSUTF16LittleEndianStringEncoding];
}

std::string fileSystemRepresentation(const String16& str) {
    if (str.empty())
        return std::string();
    
    CFStringRef strCF = CFStringCreateWithCharacters(kCFAllocatorDefault, reinterpret_cast<const UniChar*>(str.data()), str.length());
    if (!strCF)
        return std::string();
    
    CFIndex size = CFStringGetMaximumSizeOfFileSystemRepresentation(strCF);
    std::vector<char> buffer(size);
    if (!CFStringGetFileSystemRepresentation(strCF, buffer.data(), buffer.size()))
        return std::string();
    
    return std::string(buffer.data(), strlen(buffer.data()));
}

String16 ToString16(CFStringRef str) {
    if (!str)
        return String16();
    CFIndex size = CFStringGetLength(str);
    std::vector<Char16> buffer(size);
    CFStringGetCharacters(str, CFRangeMake(0, size), (UniChar*)buffer.data());
    return String16(buffer.data(), size);
}

String16 ToString16(const std::string& utf8) {
    if (utf8.empty())
        return String16();
    return String(utf8.data(), utf8.length()).utf16();
}
    
static bool Equals(const String16& a, const String16& b) {
    if (a.length() != b.length())
        return false;
    
    for (size_t i = 0; i < a.length(); ++i)
        if (a.data()[i] != b.data()[i])
            return false;
    
    return true;
}

FileSystemMac::FileSystemMac(const String16& baseDir) : base_dir_(baseDir) {

    if (Equals(base_dir_, String16("@resource_path")))
        base_dir_ = ToString16((__bridge CFStringRef)[[NSBundle mainBundle] resourcePath]);
}
    
FileSystemMac::~FileSystemMac() {}

bool FileSystemMac::FileExists(const String16& path) {
    if (path.empty())
        return false;
    
    std::string fsRep = fileSystemRepresentation(getRelative(path));
    
    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return false;
    
    return access(fsRep.data(), F_OK) != -1;
}

bool FileSystemMac::DeleteFile_(const String16& path) {
    std::string fsRep = fileSystemRepresentation(getRelative(path));
    
    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return false;
    
    // unlink(...) returns 0 on successful deletion of the path and non-zero in any other case (including invalid permissions or non-existent file)
    return !unlink(fsRep.data());
}

bool FileSystemMac::DeleteEmptyDirectory(const String16& path) {
    auto fileManager = [[NSFileManager alloc] init];
    
    auto relPath = getRelative(path);
    auto pathStr = ToNSString(relPath);
    
    if (NSArray *directoryContents = [fileManager contentsOfDirectoryAtPath:pathStr error:nullptr]) {
        // Explicitly look for and delete .DS_Store files.
        if (directoryContents.count == 1 && [directoryContents.firstObject isEqualToString:@".DS_Store"])
            [fileManager removeItemAtPath:[pathStr stringByAppendingPathComponent:directoryContents.firstObject] error:nullptr];
    }
    
    // rmdir(...) returns 0 on successful deletion of the path and non-zero in any other case (including invalid permissions or non-existent file)
    return !rmdir(fileSystemRepresentation(relPath).data());
}

bool FileSystemMac::MoveFile_(const String16& old_path, const String16& new_path) {
    // Overwrite existing files.
    auto manager = [[NSFileManager alloc] init];
    auto delegate = [[WebFileManagerDelegate alloc] init];
    [manager setDelegate:delegate];
    
    NSError *error = nil;
    bool success = [manager moveItemAtURL:[NSURL fileURLWithPath:ToNSString(getRelative(old_path))]
                                    toURL:[NSURL fileURLWithPath:ToNSString(getRelative(new_path))] error:&error];
    if (!success)
        NSLog(@"Error in FileSystemMac::MoveFile: %@", error);
    return success;
}

bool FileSystemMac::GetFileSize(const String16& path, int64_t& result) {
    std::string fsRep = fileSystemRepresentation(getRelative(path));
    
    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return false;
    
    struct stat fileInfo;
    
    if (stat(fsRep.data(), &fileInfo))
        return false;
    
    result = fileInfo.st_size;
    return true;
}

bool FileSystemMac::GetFileSize(FileHandle handle, int64_t& result) {
    struct stat fileInfo;
    if (fstat((int)handle, &fileInfo))
        return false;
    
    result = fileInfo.st_size;
    return true;
}

bool FileSystemMac::GetFileMimeType(const String16& path, String16& result) {
    auto pathStr = ToNSString(getRelative(path));
    CFStringRef extension = (__bridge CFStringRef)[pathStr pathExtension];
    CFStringRef mime_type = NULL;
    CFStringRef identifier = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, extension, NULL);
    if (identifier)
        mime_type = UTTypeCopyPreferredTagWithClass(identifier, kUTTagClassMIMEType);
    CFRelease(identifier);
    result = ToString16(mime_type);
    return !result.empty();
}

bool FileSystemMac::GetFileModificationTime(const String16& path, time_t& result) {
    std::string fsRep = fileSystemRepresentation(getRelative(path));
    
    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return false;
    
    struct stat fileInfo;
    
    if (stat(fsRep.data(), &fileInfo))
        return false;
    
    result = fileInfo.st_mtime;
    return true;
}

bool FileSystemMac::GetFileCreationTime(const String16& path, time_t& result) {
    std::string fsRep = fileSystemRepresentation(getRelative(path));
    
    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return false;
    
    struct stat fileInfo;
    
    if (stat(fsRep.data(), &fileInfo))
        return false;
    
    result = fileInfo.st_birthtime;
    return true;
}

MetadataType FileSystemMac::GetMetadataType(const String16& path) {
    std::string fsRep = fileSystemRepresentation(getRelative(path));
    
    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return kMetadataType_Unknown;
    
    struct stat fileInfo;
    if (stat(fsRep.data(), &fileInfo))
        return kMetadataType_Unknown;
    
    if (S_ISDIR(fileInfo.st_mode))
        return kMetadataType_Directory;
    
    if (S_ISLNK(fileInfo.st_mode))
        return kMetadataType_Unknown;
    
    return kMetadataType_File;
}

String16 FileSystemMac::GetPathByAppendingComponent(const String16& path, const String16& component) {
    if (path.empty())
        return component;
    
    String16 suffix = component;
    if (suffix.empty())
        return path;
    
    // Strip slash prefix from suffix
    if (suffix.data()[0] == (Char16)'/')
        suffix = String16(suffix.data() + 1, suffix.length() - 1);
    
    // Length includes terminating null character so gotta subtract 2
    Char16 lastChar = path.data()[path.length() - 2];
    if (lastChar == (Char16)'/')
        return path + suffix;
    
    const char* pathSeparator = "/";
    return path + String16(pathSeparator) + suffix;
}

bool FileSystemMac::CreateDirectory_(const String16& path)
{
    std::string fullPath = fileSystemRepresentation(getRelative(path));
    if (!access(fullPath.data(), F_OK))
        return true;
    
    char* p = &fullPath[0];
    int length = (int)fullPath.length();
    
    if(p[length - 1] == '/')
        p[length - 1] = '\0';
    for (; *p; ++p)
        if (*p == '/') {
            *p = '\0';
            if (access(fullPath.data(), F_OK))
                if (mkdir(fullPath.data(), S_IRWXU))
                    return false;
            *p = '/';
        }
    if (access(fullPath.data(), F_OK))
        if (mkdir(fullPath.data(), S_IRWXU))
            return false;
    
    return true;
}

String16 FileSystemMac::GetHomeDirectory() {
    return ToString16((__bridge CFStringRef)NSHomeDirectory());
}

String16 FileSystemMac::GetFilenameFromPath(const String16& path) {
    std::string fullPath = fileSystemRepresentation(path);
    std::string filename = fullPath.substr(fullPath.find_last_of("/") + 1);
    return ToString16(filename);
}

String16 FileSystemMac::GetDirectoryNameFromPath(const String16& path) {
    std::string fsRep = fileSystemRepresentation(path);
    
    if (!fsRep.data() || fsRep.data()[0] == '\0')
        return String16();
    
    std::string dir = dirname(&fsRep[0]);
    return ToString16(dir);
}

bool FileSystemMac::GetVolumeFreeSpace(const String16& path, uint64_t& result) {
    NSDictionary *fileSystemAttributesDictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:ToNSString(getRelative(path)) error:NULL];
    if (!fileSystemAttributesDictionary)
        return false;
    result = [[fileSystemAttributesDictionary objectForKey:NSFileSystemFreeSize] unsignedLongLongValue];
    return true;
}

int32_t FileSystemMac::GetVolumeId(const String16& path) {
    // TODO
    return 0;
}

Ref<String16Vector> FileSystemMac::ListDirectory(const String16& path, const String16& filter) {
    Ref<String16Vector> entries = String16Vector::Create();
    std::string cpath = fileSystemRepresentation(getRelative(path));
    std::string cfilter = fileSystemRepresentation(filter);
    DIR* dir = opendir(cpath.data());
    if (dir) {
        struct dirent* dp;
        while ((dp = readdir(dir))) {
            const char* name = dp->d_name;
            if (!strcmp(name, ".") || !strcmp(name, ".."))
                continue;
            if (fnmatch(cfilter.data(), name, 0))
                continue;
            char filePath[PATH_MAX];
            if (static_cast<int>(sizeof(filePath) - 1) < snprintf(filePath, sizeof(filePath), "%s/%s", cpath.data(), name))
                continue; // buffer overflow
            
            auto string = ToString16(filePath);
            
            // Some file system representations cannot be represented as a UTF-16 string,
            // so this string might be null.
            if (!string.empty())
                entries->push_back(string);
        }
        closedir(dir);
    }
    return entries;
}

String16 FileSystemMac::OpenTemporaryFile(const String16& prefix, FileHandle& handle) {
    handle = invalidFileHandle;
    
    std::vector<char> path(PATH_MAX);
    if (!confstr(_CS_DARWIN_USER_TEMP_DIR, path.data(), path.size()))
        return String16();
    
    // Shrink the vector.
    path.resize(strlen(path.data()));
    
    // FIXME: Change to a runtime assertion that the path ends with a slash once <rdar://problem/23579077> is
    // fixed in all iOS Simulator versions that we use.
    if (path.back() != '/')
        path.push_back('/');
    
    // Append the file name.
    auto str = ultralight::String(prefix);
    String8 prefixUTF8 = ultralight::String(prefix).utf8();
    path.insert(path.end(), prefixUTF8.data(), prefixUTF8.data() + prefixUTF8.length());
    const char* xxx = "XXXXXX";
    path.insert(path.end(), xxx, xxx + strlen(xxx));
    path.push_back('\0');
    
    handle = mkstemp(path.data());
    if (handle == invalidFileHandle)
        return String16();
    
    std::string pathStr(path.data(), path.size());
    return ToString16(pathStr);
}

FileHandle FileSystemMac::OpenFile(const String16& path, bool open_for_writing) {
    std::string fsRep = fileSystemRepresentation(getRelative(path));
    
    if (fsRep.empty())
        return invalidFileHandle;
    
    int platformFlag = 0;
    platformFlag |= open_for_writing? (O_WRONLY | O_CREAT | O_TRUNC) : O_RDONLY;
    
    int handle = open(fsRep.data(), platformFlag, 0666);
    
    return handle;
}

void FileSystemMac::CloseFile(FileHandle& handle) {
    if (handle != invalidFileHandle)
        close((int)handle);
    handle = invalidFileHandle;
}

int64_t FileSystemMac::SeekFile(FileHandle handle, int64_t offset, FileSeekOrigin origin) {
    int whence = SEEK_SET;
    switch (origin) {
        case kFileSeekOrigin_Beginning:
            whence = SEEK_SET;
            break;
        case kFileSeekOrigin_Current:
            whence = SEEK_CUR;
            break;
        case kFileSeekOrigin_End:
            whence = SEEK_END;
            break;
        default:
            return 0;
    }
    return static_cast<int64_t>(lseek((int)handle, offset, whence));
}

bool FileSystemMac::TruncateFile(FileHandle handle, int64_t offset) {
    // ftruncate returns 0 to indicate the success.
    return !ftruncate((int)handle, offset);
}

int64_t FileSystemMac::WriteToFile(FileHandle handle, const char* data, int64_t length) {
    do {
        auto bytesWritten = write((int)handle, data, static_cast<size_t>(length));
        if (bytesWritten >= 0)
            return bytesWritten;
    } while (errno == EINTR);
    return -1;
}

int64_t FileSystemMac::ReadFromFile(FileHandle handle, char* data, int64_t length) {
    do {
        auto bytesRead = read((int)handle, data, static_cast<size_t>(length));
        if (bytesRead >= 0)
            return bytesRead;
    } while (errno == EINTR);
    return -1;
}

bool FileSystemMac::CopyFile_(const String16& source_path, const String16& destination_path) {
    if (source_path.empty() || destination_path.empty())
        return false;
    
    std::string fsSource = fileSystemRepresentation(getRelative(source_path));
    if (fsSource.empty())
        return false;
    
    std::string fsDestination = fileSystemRepresentation(getRelative(destination_path));
    if (fsDestination.empty())
        return false;
    
    if (!link(fsSource.data(), fsDestination.data()))
        return true;
    
    // Hard link failed. Perform a copy instead.
    auto handle = open(fsDestination.data(), O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (handle == -1)
        return false;
    
    bool appendResult = appendFileContentsToFileHandle(source_path, (FileHandle)handle);
    close(handle);
    
    // If the copy failed, delete the unusable file.
    if (!appendResult)
        unlink(fsDestination.data());
    
    return appendResult;
}

String16 FileSystemMac::getRelative(const ultralight::String16 &path) {
    return GetPathByAppendingComponent(base_dir_, path);
}
    
bool FileSystemMac::appendFileContentsToFileHandle(const String16& path, FileHandle target) {
    auto source = OpenFile(path, false);
    if (source == invalidFileHandle)
        return false;
    
    static int bufferSize = 1 << 19;
    std::unique_ptr<char[]> buffer(new char[bufferSize]);
    
    do {
        int64_t readBytes = ReadFromFile(source, buffer.get(), bufferSize);
        
        if (readBytes < 0) {
            CloseFile(source);
            return false;
        }
        
        if (WriteToFile(target, buffer.get(), readBytes) != readBytes) {
            CloseFile(source);
            return false;
        }
        
        if (readBytes < bufferSize) {
            CloseFile(source);
            return true;
        }
    } while (true);

    // This shouldn't be reached.
    return false;
}
    
} // namespace ultralight

namespace framework {
    
ultralight::FileSystem* CreatePlatformFileSystem(const char* baseDir) {
    return new ultralight::FileSystemMac(baseDir);
}
    
}  // namespace framework
