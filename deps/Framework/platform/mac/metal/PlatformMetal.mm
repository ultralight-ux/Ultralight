#import "PlatformMetal.h"
#import "FontLoaderMac.h"
#import "FileSystemMac.h"

namespace framework {
    
ultralight::FontLoader* CreatePlatformFontLoader() {
    return new ultralight::FontLoaderMac();
}

ultralight::FileSystem* CreatePlatformFileSystem(const char* baseDir) {
    return new ultralight::FileSystemMac(baseDir);
}
    
}  // namespace framework
