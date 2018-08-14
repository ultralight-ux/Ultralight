#include "../../Platform.h"
#include "FontLoaderWin.h"
#include "FileSystemWin.h"
#include <shlwapi.h>

namespace framework {

ultralight::FontLoader* CreatePlatformFontLoader() {
  return new ultralight::FontLoaderWin();
}

ultralight::FileSystem* CreatePlatformFileSystem(const char* baseDir) {
  std::string baseDirStr(baseDir);
  std::wstring baseDirWStr(baseDirStr.begin(), baseDirStr.end());

  TCHAR cur_dir[_MAX_PATH];
  GetCurrentDirectory(_MAX_PATH, cur_dir);
  TCHAR absolute_dir[_MAX_PATH];
  PathCombine(absolute_dir, cur_dir, baseDirWStr.c_str());

  return new ultralight::FileSystemWin(absolute_dir);
}

}  // namespace framework
