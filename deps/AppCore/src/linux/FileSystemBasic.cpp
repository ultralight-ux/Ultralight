#include "FileSystemBasic.h"
#include <Ultralight/String.h>
#include <fstream>
#include <cstring>

static const char* FileExtensionToMimeType(const char* ext);

namespace ultralight {

// Replace all back-slashes with forward-slashes
static inline void NormalizePath(std::string& str) {
  if (!str.empty()) {
    for (size_t i = 0; i < str.length(); ++i) {
      if (str[i] == '\\')
        str[i] = '/';
    }
  }
}

FileSystemBasic::FileSystemBasic(const char* baseDir) : baseDir_(baseDir) {
  NormalizePath(baseDir_);
  if (!baseDir_.empty()) {
    // Make sure directory path ends with a slash
    if (baseDir_[baseDir_.length() - 1] != '/')
      baseDir_ += std::string("/");
  }
}

std::string FileSystemBasic::getRelative(const String16& path) {
  if (path.empty())
    return baseDir_;

  String8 utf8 = String(path).utf8();
  std::string relPath(utf8.data(), utf8.length());

  NormalizePath(relPath);
  // Strip any slash from beginning of path:
  if (relPath[0] == '/')
    relPath = relPath.substr(1);

  return baseDir_ + relPath;
}

bool FileSystemBasic::FileExists(const String16& path) {
  std::ifstream filestream(getRelative(path));
  return filestream.good();
}

bool FileSystemBasic::GetFileSize(const String16& path, int64_t& result) {
  std::ifstream in(getRelative(path), std::ifstream::ate | std::ifstream::binary);
  if (in.good()) {
    result = static_cast<int64_t>(in.tellg());
    return true;
  }
  return false;
}

bool FileSystemBasic::GetFileSize(FileHandle handle, int64_t& result) {
  auto i = open_files_.find(handle);
  if (i != open_files_.end()) {
    auto& file = i->second;
    file->seekg(0, file->end);
    result = (int64_t)file->tellg();
    return true;
  }

  return false;
}

bool FileSystemBasic::GetFileMimeType(const String16& path, String16& result) {
  String8 utf8 = String(path).utf8();
  std::string filepath(utf8.data(), utf8.length()); 
  std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
  result = String16(FileExtensionToMimeType(ext.c_str()));
  return true;
}

FileHandle FileSystemBasic::OpenFile(const String16& path, bool open_for_writing) {
  std::unique_ptr<std::ifstream> file(new std::ifstream(getRelative(path), std::ifstream::ate | std::ifstream::binary));
  if (!file->good())
    return invalidFileHandle;

  FileHandle handle = next_handle_++;
  open_files_[handle] = std::move(file);
  return handle;
}

void FileSystemBasic::CloseFile(FileHandle& handle) {
  open_files_.erase(handle);
  handle = invalidFileHandle;
}

int64_t FileSystemBasic::ReadFromFile(FileHandle handle, char* data, int64_t length) {
  auto i = open_files_.find(handle);
  if (i != open_files_.end()) {
    auto& file = i->second;
    file->seekg(0, file->beg);
    file->read(data, (std::streamsize)length);
    return (int64_t)file->gcount();
  }

  return 0;
}

}  // namespace ultralight

namespace framework {

ultralight::FileSystem* CreatePlatformFileSystem(const char* baseDir) {
  return new ultralight::FileSystemBasic(baseDir);
}

}  // namespace ultralight

const char* FileExtensionToMimeType(const char* ext) {
  if (!strcmp(ext, "323")) return "text/h323";
  if (!strcmp(ext, "3g2")) return "video/3gpp2";
  if (!strcmp(ext, "3gp")) return "video/3gpp";
  if (!strcmp(ext, "3gp2")) return "video/3gpp2";
  if (!strcmp(ext, "3gpp")) return "video/3gpp";
  if (!strcmp(ext, "7z")) return "application/x-7z-compressed";
  if (!strcmp(ext, "aa")) return "audio/audible";
  if (!strcmp(ext, "AAC")) return "audio/aac";
  if (!strcmp(ext, "aaf")) return "application/octet-stream";
  if (!strcmp(ext, "aax")) return "audio/vnd.audible.aax";
  if (!strcmp(ext, "ac3")) return "audio/ac3";
  if (!strcmp(ext, "aca")) return "application/octet-stream";
  if (!strcmp(ext, "accda")) return "application/msaccess.addin";
  if (!strcmp(ext, "accdb")) return "application/msaccess";
  if (!strcmp(ext, "accdc")) return "application/msaccess.cab";
  if (!strcmp(ext, "accde")) return "application/msaccess";
  if (!strcmp(ext, "accdr")) return "application/msaccess.runtime";
  if (!strcmp(ext, "accdt")) return "application/msaccess";
  if (!strcmp(ext, "accdw")) return "application/msaccess.webapplication";
  if (!strcmp(ext, "accft")) return "application/msaccess.ftemplate";
  if (!strcmp(ext, "acx")) return "application/internet-property-stream";
  if (!strcmp(ext, "AddIn")) return "text/xml";
  if (!strcmp(ext, "ade")) return "application/msaccess";
  if (!strcmp(ext, "adobebridge")) return "application/x-bridge-url";
  if (!strcmp(ext, "adp")) return "application/msaccess";
  if (!strcmp(ext, "ADT")) return "audio/vnd.dlna.adts";
  if (!strcmp(ext, "ADTS")) return "audio/aac";
  if (!strcmp(ext, "afm")) return "application/octet-stream";
  if (!strcmp(ext, "ai")) return "application/postscript";
  if (!strcmp(ext, "aif")) return "audio/aiff";
  if (!strcmp(ext, "aifc")) return "audio/aiff";
  if (!strcmp(ext, "aiff")) return "audio/aiff";
  if (!strcmp(ext, "air")) return "application/vnd.adobe.air-application-installer-package+zip";
  if (!strcmp(ext, "amc")) return "application/mpeg";
  if (!strcmp(ext, "anx")) return "application/annodex";
  if (!strcmp(ext, "apk")) return "application/vnd.android.package-archive";
  if (!strcmp(ext, "application")) return "application/x-ms-application";
  if (!strcmp(ext, "art")) return "image/x-jg";
  if (!strcmp(ext, "asa")) return "application/xml";
  if (!strcmp(ext, "asax")) return "application/xml";
  if (!strcmp(ext, "ascx")) return "application/xml";
  if (!strcmp(ext, "asd")) return "application/octet-stream";
  if (!strcmp(ext, "asf")) return "video/x-ms-asf";
  if (!strcmp(ext, "ashx")) return "application/xml";
  if (!strcmp(ext, "asi")) return "application/octet-stream";
  if (!strcmp(ext, "asm")) return "text/plain";
  if (!strcmp(ext, "asmx")) return "application/xml";
  if (!strcmp(ext, "aspx")) return "application/xml";
  if (!strcmp(ext, "asr")) return "video/x-ms-asf";
  if (!strcmp(ext, "asx")) return "video/x-ms-asf";
  if (!strcmp(ext, "atom")) return "application/atom+xml";
  if (!strcmp(ext, "au")) return "audio/basic";
  if (!strcmp(ext, "avi")) return "video/x-msvideo";
  if (!strcmp(ext, "axa")) return "audio/annodex";
  if (!strcmp(ext, "axs")) return "application/olescript";
  if (!strcmp(ext, "axv")) return "video/annodex";
  if (!strcmp(ext, "bas")) return "text/plain";
  if (!strcmp(ext, "bcpio")) return "application/x-bcpio";
  if (!strcmp(ext, "bin")) return "application/octet-stream";
  if (!strcmp(ext, "bmp")) return "image/bmp";
  if (!strcmp(ext, "c")) return "text/plain";
  if (!strcmp(ext, "cab")) return "application/octet-stream";
  if (!strcmp(ext, "caf")) return "audio/x-caf";
  if (!strcmp(ext, "calx")) return "application/vnd.ms-office.calx";
  if (!strcmp(ext, "cat")) return "application/vnd.ms-pki.seccat";
  if (!strcmp(ext, "cc")) return "text/plain";
  if (!strcmp(ext, "cd")) return "text/plain";
  if (!strcmp(ext, "cdda")) return "audio/aiff";
  if (!strcmp(ext, "cdf")) return "application/x-cdf";
  if (!strcmp(ext, "cer")) return "application/x-x509-ca-cert";
  if (!strcmp(ext, "cfg")) return "text/plain";
  if (!strcmp(ext, "chm")) return "application/octet-stream";
  if (!strcmp(ext, "class")) return "application/x-java-applet";
  if (!strcmp(ext, "clp")) return "application/x-msclip";
  if (!strcmp(ext, "cmd")) return "text/plain";
  if (!strcmp(ext, "cmx")) return "image/x-cmx";
  if (!strcmp(ext, "cnf")) return "text/plain";
  if (!strcmp(ext, "cod")) return "image/cis-cod";
  if (!strcmp(ext, "config")) return "application/xml";
  if (!strcmp(ext, "contact")) return "text/x-ms-contact";
  if (!strcmp(ext, "coverage")) return "application/xml";
  if (!strcmp(ext, "cpio")) return "application/x-cpio";
  if (!strcmp(ext, "cpp")) return "text/plain";
  if (!strcmp(ext, "crd")) return "application/x-mscardfile";
  if (!strcmp(ext, "crl")) return "application/pkix-crl";
  if (!strcmp(ext, "crt")) return "application/x-x509-ca-cert";
  if (!strcmp(ext, "cs")) return "text/plain";
  if (!strcmp(ext, "csdproj")) return "text/plain";
  if (!strcmp(ext, "csh")) return "application/x-csh";
  if (!strcmp(ext, "csproj")) return "text/plain";
  if (!strcmp(ext, "css")) return "text/css";
  if (!strcmp(ext, "csv")) return "text/csv";
  if (!strcmp(ext, "cur")) return "application/octet-stream";
  if (!strcmp(ext, "cxx")) return "text/plain";
  if (!strcmp(ext, "dat")) return "application/octet-stream";
  if (!strcmp(ext, "datasource")) return "application/xml";
  if (!strcmp(ext, "dbproj")) return "text/plain";
  if (!strcmp(ext, "dcr")) return "application/x-director";
  if (!strcmp(ext, "def")) return "text/plain";
  if (!strcmp(ext, "deploy")) return "application/octet-stream";
  if (!strcmp(ext, "der")) return "application/x-x509-ca-cert";
  if (!strcmp(ext, "dgml")) return "application/xml";
  if (!strcmp(ext, "dib")) return "image/bmp";
  if (!strcmp(ext, "dif")) return "video/x-dv";
  if (!strcmp(ext, "dir")) return "application/x-director";
  if (!strcmp(ext, "disco")) return "text/xml";
  if (!strcmp(ext, "divx")) return "video/divx";
  if (!strcmp(ext, "dll")) return "application/x-msdownload";
  if (!strcmp(ext, "dll.config")) return "text/xml";
  if (!strcmp(ext, "dlm")) return "text/dlm";
  if (!strcmp(ext, "doc")) return "application/msword";
  if (!strcmp(ext, "docm")) return "application/vnd.ms-word.document.macroEnabled.12";
  if (!strcmp(ext, "docx")) return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
  if (!strcmp(ext, "dot")) return "application/msword";
  if (!strcmp(ext, "dotm")) return "application/vnd.ms-word.template.macroEnabled.12";
  if (!strcmp(ext, "dotx")) return "application/vnd.openxmlformats-officedocument.wordprocessingml.template";
  if (!strcmp(ext, "dsp")) return "application/octet-stream";
  if (!strcmp(ext, "dsw")) return "text/plain";
  if (!strcmp(ext, "dtd")) return "text/xml";
  if (!strcmp(ext, "dtsConfig")) return "text/xml";
  if (!strcmp(ext, "dv")) return "video/x-dv";
  if (!strcmp(ext, "dvi")) return "application/x-dvi";
  if (!strcmp(ext, "dwf")) return "drawing/x-dwf";
  if (!strcmp(ext, "dwg")) return "application/acad";
  if (!strcmp(ext, "dwp")) return "application/octet-stream";
  if (!strcmp(ext, "dxf")) return "application/x-dxf";
  if (!strcmp(ext, "dxr")) return "application/x-director";
  if (!strcmp(ext, "eml")) return "message/rfc822";
  if (!strcmp(ext, "emz")) return "application/octet-stream";
  if (!strcmp(ext, "eot")) return "application/vnd.ms-fontobject";
  if (!strcmp(ext, "eps")) return "application/postscript";
  if (!strcmp(ext, "etl")) return "application/etl";
  if (!strcmp(ext, "etx")) return "text/x-setext";
  if (!strcmp(ext, "evy")) return "application/envoy";
  if (!strcmp(ext, "exe")) return "application/octet-stream";
  if (!strcmp(ext, "exe.config")) return "text/xml";
  if (!strcmp(ext, "fdf")) return "application/vnd.fdf";
  if (!strcmp(ext, "fif")) return "application/fractals";
  if (!strcmp(ext, "filters")) return "application/xml";
  if (!strcmp(ext, "fla")) return "application/octet-stream";
  if (!strcmp(ext, "flac")) return "audio/flac";
  if (!strcmp(ext, "flr")) return "x-world/x-vrml";
  if (!strcmp(ext, "flv")) return "video/x-flv";
  if (!strcmp(ext, "fsscript")) return "application/fsharp-script";
  if (!strcmp(ext, "fsx")) return "application/fsharp-script";
  if (!strcmp(ext, "generictest")) return "application/xml";
  if (!strcmp(ext, "gif")) return "image/gif";
  if (!strcmp(ext, "gpx")) return "application/gpx+xml";
  if (!strcmp(ext, "group")) return "text/x-ms-group";
  if (!strcmp(ext, "gsm")) return "audio/x-gsm";
  if (!strcmp(ext, "gtar")) return "application/x-gtar";
  if (!strcmp(ext, "gz")) return "application/x-gzip";
  if (!strcmp(ext, "h")) return "text/plain";
  if (!strcmp(ext, "hdf")) return "application/x-hdf";
  if (!strcmp(ext, "hdml")) return "text/x-hdml";
  if (!strcmp(ext, "hhc")) return "application/x-oleobject";
  if (!strcmp(ext, "hhk")) return "application/octet-stream";
  if (!strcmp(ext, "hhp")) return "application/octet-stream";
  if (!strcmp(ext, "hlp")) return "application/winhlp";
  if (!strcmp(ext, "hpp")) return "text/plain";
  if (!strcmp(ext, "hqx")) return "application/mac-binhex40";
  if (!strcmp(ext, "hta")) return "application/hta";
  if (!strcmp(ext, "htc")) return "text/x-component";
  if (!strcmp(ext, "htm")) return "text/html";
  if (!strcmp(ext, "html")) return "text/html";
  if (!strcmp(ext, "htt")) return "text/webviewhtml";
  if (!strcmp(ext, "hxa")) return "application/xml";
  if (!strcmp(ext, "hxc")) return "application/xml";
  if (!strcmp(ext, "hxd")) return "application/octet-stream";
  if (!strcmp(ext, "hxe")) return "application/xml";
  if (!strcmp(ext, "hxf")) return "application/xml";
  if (!strcmp(ext, "hxh")) return "application/octet-stream";
  if (!strcmp(ext, "hxi")) return "application/octet-stream";
  if (!strcmp(ext, "hxk")) return "application/xml";
  if (!strcmp(ext, "hxq")) return "application/octet-stream";
  if (!strcmp(ext, "hxr")) return "application/octet-stream";
  if (!strcmp(ext, "hxs")) return "application/octet-stream";
  if (!strcmp(ext, "hxt")) return "text/html";
  if (!strcmp(ext, "hxv")) return "application/xml";
  if (!strcmp(ext, "hxw")) return "application/octet-stream";
  if (!strcmp(ext, "hxx")) return "text/plain";
  if (!strcmp(ext, "i")) return "text/plain";
  if (!strcmp(ext, "ico")) return "image/x-icon";
  if (!strcmp(ext, "ics")) return "application/octet-stream";
  if (!strcmp(ext, "idl")) return "text/plain";
  if (!strcmp(ext, "ief")) return "image/ief";
  if (!strcmp(ext, "iii")) return "application/x-iphone";
  if (!strcmp(ext, "inc")) return "text/plain";
  if (!strcmp(ext, "inf")) return "application/octet-stream";
  if (!strcmp(ext, "ini")) return "text/plain";
  if (!strcmp(ext, "inl")) return "text/plain";
  if (!strcmp(ext, "ins")) return "application/x-internet-signup";
  if (!strcmp(ext, "ipa")) return "application/x-itunes-ipa";
  if (!strcmp(ext, "ipg")) return "application/x-itunes-ipg";
  if (!strcmp(ext, "ipproj")) return "text/plain";
  if (!strcmp(ext, "ipsw")) return "application/x-itunes-ipsw";
  if (!strcmp(ext, "iqy")) return "text/x-ms-iqy";
  if (!strcmp(ext, "isp")) return "application/x-internet-signup";
  if (!strcmp(ext, "ite")) return "application/x-itunes-ite";
  if (!strcmp(ext, "itlp")) return "application/x-itunes-itlp";
  if (!strcmp(ext, "itms")) return "application/x-itunes-itms";
  if (!strcmp(ext, "itpc")) return "application/x-itunes-itpc";
  if (!strcmp(ext, "IVF")) return "video/x-ivf";
  if (!strcmp(ext, "jar")) return "application/java-archive";
  if (!strcmp(ext, "java")) return "application/octet-stream";
  if (!strcmp(ext, "jck")) return "application/liquidmotion";
  if (!strcmp(ext, "jcz")) return "application/liquidmotion";
  if (!strcmp(ext, "jfif")) return "image/pjpeg";
  if (!strcmp(ext, "jnlp")) return "application/x-java-jnlp-file";
  if (!strcmp(ext, "jpb")) return "application/octet-stream";
  if (!strcmp(ext, "jpe")) return "image/jpeg";
  if (!strcmp(ext, "jpeg")) return "image/jpeg";
  if (!strcmp(ext, "jpg")) return "image/jpeg";
  if (!strcmp(ext, "js")) return "application/javascript";
  if (!strcmp(ext, "json")) return "application/json";
  if (!strcmp(ext, "jsx")) return "text/jscript";
  if (!strcmp(ext, "jsxbin")) return "text/plain";
  if (!strcmp(ext, "latex")) return "application/x-latex";
  if (!strcmp(ext, "library-ms")) return "application/windows-library+xml";
  if (!strcmp(ext, "lit")) return "application/x-ms-reader";
  if (!strcmp(ext, "loadtest")) return "application/xml";
  if (!strcmp(ext, "lpk")) return "application/octet-stream";
  if (!strcmp(ext, "lsf")) return "video/x-la-asf";
  if (!strcmp(ext, "lst")) return "text/plain";
  if (!strcmp(ext, "lsx")) return "video/x-la-asf";
  if (!strcmp(ext, "lzh")) return "application/octet-stream";
  if (!strcmp(ext, "m13")) return "application/x-msmediaview";
  if (!strcmp(ext, "m14")) return "application/x-msmediaview";
  if (!strcmp(ext, "m1v")) return "video/mpeg";
  if (!strcmp(ext, "m2t")) return "video/vnd.dlna.mpeg-tts";
  if (!strcmp(ext, "m2ts")) return "video/vnd.dlna.mpeg-tts";
  if (!strcmp(ext, "m2v")) return "video/mpeg";
  if (!strcmp(ext, "m3u")) return "audio/x-mpegurl";
  if (!strcmp(ext, "m3u8")) return "audio/x-mpegurl";
  if (!strcmp(ext, "m4a")) return "audio/m4a";
  if (!strcmp(ext, "m4b")) return "audio/m4b";
  if (!strcmp(ext, "m4p")) return "audio/m4p";
  if (!strcmp(ext, "m4r")) return "audio/x-m4r";
  if (!strcmp(ext, "m4v")) return "video/x-m4v";
  if (!strcmp(ext, "mac")) return "image/x-macpaint";
  if (!strcmp(ext, "mak")) return "text/plain";
  if (!strcmp(ext, "man")) return "application/x-troff-man";
  if (!strcmp(ext, "manifest")) return "application/x-ms-manifest";
  if (!strcmp(ext, "map")) return "text/plain";
  if (!strcmp(ext, "master")) return "application/xml";
  if (!strcmp(ext, "mbox")) return "application/mbox";
  if (!strcmp(ext, "mda")) return "application/msaccess";
  if (!strcmp(ext, "mdb")) return "application/x-msaccess";
  if (!strcmp(ext, "mde")) return "application/msaccess";
  if (!strcmp(ext, "mdp")) return "application/octet-stream";
  if (!strcmp(ext, "me")) return "application/x-troff-me";
  if (!strcmp(ext, "mfp")) return "application/x-shockwave-flash";
  if (!strcmp(ext, "mht")) return "message/rfc822";
  if (!strcmp(ext, "mhtml")) return "message/rfc822";
  if (!strcmp(ext, "mid")) return "audio/mid";
  if (!strcmp(ext, "midi")) return "audio/mid";
  if (!strcmp(ext, "mix")) return "application/octet-stream";
  if (!strcmp(ext, "mk")) return "text/plain";
  if (!strcmp(ext, "mk3d")) return "video/x-matroska-3d";
  if (!strcmp(ext, "mka")) return "audio/x-matroska";
  if (!strcmp(ext, "mkv")) return "video/x-matroska";
  if (!strcmp(ext, "mmf")) return "application/x-smaf";
  if (!strcmp(ext, "mno")) return "text/xml";
  if (!strcmp(ext, "mny")) return "application/x-msmoney";
  if (!strcmp(ext, "mod")) return "video/mpeg";
  if (!strcmp(ext, "mov")) return "video/quicktime";
  if (!strcmp(ext, "movie")) return "video/x-sgi-movie";
  if (!strcmp(ext, "mp2")) return "video/mpeg";
  if (!strcmp(ext, "mp2v")) return "video/mpeg";
  if (!strcmp(ext, "mp3")) return "audio/mpeg";
  if (!strcmp(ext, "mp4")) return "video/mp4";
  if (!strcmp(ext, "mp4v")) return "video/mp4";
  if (!strcmp(ext, "mpa")) return "video/mpeg";
  if (!strcmp(ext, "mpe")) return "video/mpeg";
  if (!strcmp(ext, "mpeg")) return "video/mpeg";
  if (!strcmp(ext, "mpf")) return "application/vnd.ms-mediapackage";
  if (!strcmp(ext, "mpg")) return "video/mpeg";
  if (!strcmp(ext, "mpp")) return "application/vnd.ms-project";
  if (!strcmp(ext, "mpv2")) return "video/mpeg";
  if (!strcmp(ext, "mqv")) return "video/quicktime";
  if (!strcmp(ext, "ms")) return "application/x-troff-ms";
  if (!strcmp(ext, "msg")) return "application/vnd.ms-outlook";
  if (!strcmp(ext, "msi")) return "application/octet-stream";
  if (!strcmp(ext, "mso")) return "application/octet-stream";
  if (!strcmp(ext, "mts")) return "video/vnd.dlna.mpeg-tts";
  if (!strcmp(ext, "mtx")) return "application/xml";
  if (!strcmp(ext, "mvb")) return "application/x-msmediaview";
  if (!strcmp(ext, "mvc")) return "application/x-miva-compiled";
  if (!strcmp(ext, "mxp")) return "application/x-mmxp";
  if (!strcmp(ext, "nc")) return "application/x-netcdf";
  if (!strcmp(ext, "nsc")) return "video/x-ms-asf";
  if (!strcmp(ext, "nws")) return "message/rfc822";
  if (!strcmp(ext, "ocx")) return "application/octet-stream";
  if (!strcmp(ext, "oda")) return "application/oda";
  if (!strcmp(ext, "odb")) return "application/vnd.oasis.opendocument.database";
  if (!strcmp(ext, "odc")) return "application/vnd.oasis.opendocument.chart";
  if (!strcmp(ext, "odf")) return "application/vnd.oasis.opendocument.formula";
  if (!strcmp(ext, "odg")) return "application/vnd.oasis.opendocument.graphics";
  if (!strcmp(ext, "odh")) return "text/plain";
  if (!strcmp(ext, "odi")) return "application/vnd.oasis.opendocument.image";
  if (!strcmp(ext, "odl")) return "text/plain";
  if (!strcmp(ext, "odm")) return "application/vnd.oasis.opendocument.text-master";
  if (!strcmp(ext, "odp")) return "application/vnd.oasis.opendocument.presentation";
  if (!strcmp(ext, "ods")) return "application/vnd.oasis.opendocument.spreadsheet";
  if (!strcmp(ext, "odt")) return "application/vnd.oasis.opendocument.text";
  if (!strcmp(ext, "oga")) return "audio/ogg";
  if (!strcmp(ext, "ogg")) return "audio/ogg";
  if (!strcmp(ext, "ogv")) return "video/ogg";
  if (!strcmp(ext, "ogx")) return "application/ogg";
  if (!strcmp(ext, "one")) return "application/onenote";
  if (!strcmp(ext, "onea")) return "application/onenote";
  if (!strcmp(ext, "onepkg")) return "application/onenote";
  if (!strcmp(ext, "onetmp")) return "application/onenote";
  if (!strcmp(ext, "onetoc")) return "application/onenote";
  if (!strcmp(ext, "onetoc2")) return "application/onenote";
  if (!strcmp(ext, "opus")) return "audio/ogg";
  if (!strcmp(ext, "orderedtest")) return "application/xml";
  if (!strcmp(ext, "osdx")) return "application/opensearchdescription+xml";
  if (!strcmp(ext, "otf")) return "application/font-sfnt";
  if (!strcmp(ext, "otg")) return "application/vnd.oasis.opendocument.graphics-template";
  if (!strcmp(ext, "oth")) return "application/vnd.oasis.opendocument.text-web";
  if (!strcmp(ext, "otp")) return "application/vnd.oasis.opendocument.presentation-template";
  if (!strcmp(ext, "ots")) return "application/vnd.oasis.opendocument.spreadsheet-template";
  if (!strcmp(ext, "ott")) return "application/vnd.oasis.opendocument.text-template";
  if (!strcmp(ext, "oxt")) return "application/vnd.openofficeorg.extension";
  if (!strcmp(ext, "p10")) return "application/pkcs10";
  if (!strcmp(ext, "p12")) return "application/x-pkcs12";
  if (!strcmp(ext, "p7b")) return "application/x-pkcs7-certificates";
  if (!strcmp(ext, "p7c")) return "application/pkcs7-mime";
  if (!strcmp(ext, "p7m")) return "application/pkcs7-mime";
  if (!strcmp(ext, "p7r")) return "application/x-pkcs7-certreqresp";
  if (!strcmp(ext, "p7s")) return "application/pkcs7-signature";
  if (!strcmp(ext, "pbm")) return "image/x-portable-bitmap";
  if (!strcmp(ext, "pcast")) return "application/x-podcast";
  if (!strcmp(ext, "pct")) return "image/pict";
  if (!strcmp(ext, "pcx")) return "application/octet-stream";
  if (!strcmp(ext, "pcz")) return "application/octet-stream";
  if (!strcmp(ext, "pdf")) return "application/pdf";
  if (!strcmp(ext, "pfb")) return "application/octet-stream";
  if (!strcmp(ext, "pfm")) return "application/octet-stream";
  if (!strcmp(ext, "pfx")) return "application/x-pkcs12";
  if (!strcmp(ext, "pgm")) return "image/x-portable-graymap";
  if (!strcmp(ext, "pic")) return "image/pict";
  if (!strcmp(ext, "pict")) return "image/pict";
  if (!strcmp(ext, "pkgdef")) return "text/plain";
  if (!strcmp(ext, "pkgundef")) return "text/plain";
  if (!strcmp(ext, "pko")) return "application/vnd.ms-pki.pko";
  if (!strcmp(ext, "pls")) return "audio/scpls";
  if (!strcmp(ext, "pma")) return "application/x-perfmon";
  if (!strcmp(ext, "pmc")) return "application/x-perfmon";
  if (!strcmp(ext, "pml")) return "application/x-perfmon";
  if (!strcmp(ext, "pmr")) return "application/x-perfmon";
  if (!strcmp(ext, "pmw")) return "application/x-perfmon";
  if (!strcmp(ext, "png")) return "image/png";
  if (!strcmp(ext, "pnm")) return "image/x-portable-anymap";
  if (!strcmp(ext, "pnt")) return "image/x-macpaint";
  if (!strcmp(ext, "pntg")) return "image/x-macpaint";
  if (!strcmp(ext, "pnz")) return "image/png";
  if (!strcmp(ext, "pot")) return "application/vnd.ms-powerpoint";
  if (!strcmp(ext, "potm")) return "application/vnd.ms-powerpoint.template.macroEnabled.12";
  if (!strcmp(ext, "potx")) return "application/vnd.openxmlformats-officedocument.presentationml.template";
  if (!strcmp(ext, "ppa")) return "application/vnd.ms-powerpoint";
  if (!strcmp(ext, "ppam")) return "application/vnd.ms-powerpoint.addin.macroEnabled.12";
  if (!strcmp(ext, "ppm")) return "image/x-portable-pixmap";
  if (!strcmp(ext, "pps")) return "application/vnd.ms-powerpoint";
  if (!strcmp(ext, "ppsm")) return "application/vnd.ms-powerpoint.slideshow.macroEnabled.12";
  if (!strcmp(ext, "ppsx")) return "application/vnd.openxmlformats-officedocument.presentationml.slideshow";
  if (!strcmp(ext, "ppt")) return "application/vnd.ms-powerpoint";
  if (!strcmp(ext, "pptm")) return "application/vnd.ms-powerpoint.presentation.macroEnabled.12";
  if (!strcmp(ext, "pptx")) return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
  if (!strcmp(ext, "prf")) return "application/pics-rules";
  if (!strcmp(ext, "prm")) return "application/octet-stream";
  if (!strcmp(ext, "prx")) return "application/octet-stream";
  if (!strcmp(ext, "ps")) return "application/postscript";
  if (!strcmp(ext, "psc1")) return "application/PowerShell";
  if (!strcmp(ext, "psd")) return "application/octet-stream";
  if (!strcmp(ext, "psess")) return "application/xml";
  if (!strcmp(ext, "psm")) return "application/octet-stream";
  if (!strcmp(ext, "psp")) return "application/octet-stream";
  if (!strcmp(ext, "pst")) return "application/vnd.ms-outlook";
  if (!strcmp(ext, "pub")) return "application/x-mspublisher";
  if (!strcmp(ext, "pwz")) return "application/vnd.ms-powerpoint";
  if (!strcmp(ext, "qht")) return "text/x-html-insertion";
  if (!strcmp(ext, "qhtm")) return "text/x-html-insertion";
  if (!strcmp(ext, "qt")) return "video/quicktime";
  if (!strcmp(ext, "qti")) return "image/x-quicktime";
  if (!strcmp(ext, "qtif")) return "image/x-quicktime";
  if (!strcmp(ext, "qtl")) return "application/x-quicktimeplayer";
  if (!strcmp(ext, "qxd")) return "application/octet-stream";
  if (!strcmp(ext, "ra")) return "audio/x-pn-realaudio";
  if (!strcmp(ext, "ram")) return "audio/x-pn-realaudio";
  if (!strcmp(ext, "rar")) return "application/x-rar-compressed";
  if (!strcmp(ext, "ras")) return "image/x-cmu-raster";
  if (!strcmp(ext, "rat")) return "application/rat-file";
  if (!strcmp(ext, "rc")) return "text/plain";
  if (!strcmp(ext, "rc2")) return "text/plain";
  if (!strcmp(ext, "rct")) return "text/plain";
  if (!strcmp(ext, "rdlc")) return "application/xml";
  if (!strcmp(ext, "reg")) return "text/plain";
  if (!strcmp(ext, "resx")) return "application/xml";
  if (!strcmp(ext, "rf")) return "image/vnd.rn-realflash";
  if (!strcmp(ext, "rgb")) return "image/x-rgb";
  if (!strcmp(ext, "rgs")) return "text/plain";
  if (!strcmp(ext, "rm")) return "application/vnd.rn-realmedia";
  if (!strcmp(ext, "rmi")) return "audio/mid";
  if (!strcmp(ext, "rmp")) return "application/vnd.rn-rn_music_package";
  if (!strcmp(ext, "roff")) return "application/x-troff";
  if (!strcmp(ext, "rpm")) return "audio/x-pn-realaudio-plugin";
  if (!strcmp(ext, "rqy")) return "text/x-ms-rqy";
  if (!strcmp(ext, "rtf")) return "application/rtf";
  if (!strcmp(ext, "rtx")) return "text/richtext";
  if (!strcmp(ext, "rvt")) return "application/octet-stream";
  if (!strcmp(ext, "ruleset")) return "application/xml";
  if (!strcmp(ext, "s")) return "text/plain";
  if (!strcmp(ext, "safariextz")) return "application/x-safari-safariextz";
  if (!strcmp(ext, "scd")) return "application/x-msschedule";
  if (!strcmp(ext, "scr")) return "text/plain";
  if (!strcmp(ext, "sct")) return "text/scriptlet";
  if (!strcmp(ext, "sd2")) return "audio/x-sd2";
  if (!strcmp(ext, "sdp")) return "application/sdp";
  if (!strcmp(ext, "sea")) return "application/octet-stream";
  if (!strcmp(ext, "searchConnector-ms")) return "application/windows-search-connector+xml";
  if (!strcmp(ext, "setpay")) return "application/set-payment-initiation";
  if (!strcmp(ext, "setreg")) return "application/set-registration-initiation";
  if (!strcmp(ext, "settings")) return "application/xml";
  if (!strcmp(ext, "sgimb")) return "application/x-sgimb";
  if (!strcmp(ext, "sgml")) return "text/sgml";
  if (!strcmp(ext, "sh")) return "application/x-sh";
  if (!strcmp(ext, "shar")) return "application/x-shar";
  if (!strcmp(ext, "shtml")) return "text/html";
  if (!strcmp(ext, "sit")) return "application/x-stuffit";
  if (!strcmp(ext, "sitemap")) return "application/xml";
  if (!strcmp(ext, "skin")) return "application/xml";
  if (!strcmp(ext, "skp")) return "application/x-koan";
  if (!strcmp(ext, "sldm")) return "application/vnd.ms-powerpoint.slide.macroEnabled.12";
  if (!strcmp(ext, "sldx")) return "application/vnd.openxmlformats-officedocument.presentationml.slide";
  if (!strcmp(ext, "slk")) return "application/vnd.ms-excel";
  if (!strcmp(ext, "sln")) return "text/plain";
  if (!strcmp(ext, "slupkg-ms")) return "application/x-ms-license";
  if (!strcmp(ext, "smd")) return "audio/x-smd";
  if (!strcmp(ext, "smi")) return "application/octet-stream";
  if (!strcmp(ext, "smx")) return "audio/x-smd";
  if (!strcmp(ext, "smz")) return "audio/x-smd";
  if (!strcmp(ext, "snd")) return "audio/basic";
  if (!strcmp(ext, "snippet")) return "application/xml";
  if (!strcmp(ext, "snp")) return "application/octet-stream";
  if (!strcmp(ext, "sol")) return "text/plain";
  if (!strcmp(ext, "sor")) return "text/plain";
  if (!strcmp(ext, "spc")) return "application/x-pkcs7-certificates";
  if (!strcmp(ext, "spl")) return "application/futuresplash";
  if (!strcmp(ext, "spx")) return "audio/ogg";
  if (!strcmp(ext, "src")) return "application/x-wais-source";
  if (!strcmp(ext, "srf")) return "text/plain";
  if (!strcmp(ext, "SSISDeploymentManifest")) return "text/xml";
  if (!strcmp(ext, "ssm")) return "application/streamingmedia";
  if (!strcmp(ext, "sst")) return "application/vnd.ms-pki.certstore";
  if (!strcmp(ext, "stl")) return "application/vnd.ms-pki.stl";
  if (!strcmp(ext, "sv4cpio")) return "application/x-sv4cpio";
  if (!strcmp(ext, "sv4crc")) return "application/x-sv4crc";
  if (!strcmp(ext, "svc")) return "application/xml";
  if (!strcmp(ext, "svg")) return "image/svg+xml";
  if (!strcmp(ext, "swf")) return "application/x-shockwave-flash";
  if (!strcmp(ext, "step")) return "application/step";
  if (!strcmp(ext, "stp")) return "application/step";
  if (!strcmp(ext, "t")) return "application/x-troff";
  if (!strcmp(ext, "tar")) return "application/x-tar";
  if (!strcmp(ext, "tcl")) return "application/x-tcl";
  if (!strcmp(ext, "testrunconfig")) return "application/xml";
  if (!strcmp(ext, "testsettings")) return "application/xml";
  if (!strcmp(ext, "tex")) return "application/x-tex";
  if (!strcmp(ext, "texi")) return "application/x-texinfo";
  if (!strcmp(ext, "texinfo")) return "application/x-texinfo";
  if (!strcmp(ext, "tgz")) return "application/x-compressed";
  if (!strcmp(ext, "thmx")) return "application/vnd.ms-officetheme";
  if (!strcmp(ext, "thn")) return "application/octet-stream";
  if (!strcmp(ext, "tif")) return "image/tiff";
  if (!strcmp(ext, "tiff")) return "image/tiff";
  if (!strcmp(ext, "tlh")) return "text/plain";
  if (!strcmp(ext, "tli")) return "text/plain";
  if (!strcmp(ext, "toc")) return "application/octet-stream";
  if (!strcmp(ext, "tr")) return "application/x-troff";
  if (!strcmp(ext, "trm")) return "application/x-msterminal";
  if (!strcmp(ext, "trx")) return "application/xml";
  if (!strcmp(ext, "ts")) return "video/vnd.dlna.mpeg-tts";
  if (!strcmp(ext, "tsv")) return "text/tab-separated-values";
  if (!strcmp(ext, "ttf")) return "application/font-sfnt";
  if (!strcmp(ext, "tts")) return "video/vnd.dlna.mpeg-tts";
  if (!strcmp(ext, "txt")) return "text/plain";
  if (!strcmp(ext, "u32")) return "application/octet-stream";
  if (!strcmp(ext, "uls")) return "text/iuls";
  if (!strcmp(ext, "user")) return "text/plain";
  if (!strcmp(ext, "ustar")) return "application/x-ustar";
  if (!strcmp(ext, "vb")) return "text/plain";
  if (!strcmp(ext, "vbdproj")) return "text/plain";
  if (!strcmp(ext, "vbk")) return "video/mpeg";
  if (!strcmp(ext, "vbproj")) return "text/plain";
  if (!strcmp(ext, "vbs")) return "text/vbscript";
  if (!strcmp(ext, "vcf")) return "text/x-vcard";
  if (!strcmp(ext, "vcproj")) return "application/xml";
  if (!strcmp(ext, "vcs")) return "text/plain";
  if (!strcmp(ext, "vcxproj")) return "application/xml";
  if (!strcmp(ext, "vddproj")) return "text/plain";
  if (!strcmp(ext, "vdp")) return "text/plain";
  if (!strcmp(ext, "vdproj")) return "text/plain";
  if (!strcmp(ext, "vdx")) return "application/vnd.ms-visio.viewer";
  if (!strcmp(ext, "vml")) return "text/xml";
  if (!strcmp(ext, "vscontent")) return "application/xml";
  if (!strcmp(ext, "vsct")) return "text/xml";
  if (!strcmp(ext, "vsd")) return "application/vnd.visio";
  if (!strcmp(ext, "vsi")) return "application/ms-vsi";
  if (!strcmp(ext, "vsix")) return "application/vsix";
  if (!strcmp(ext, "vsixlangpack")) return "text/xml";
  if (!strcmp(ext, "vsixmanifest")) return "text/xml";
  if (!strcmp(ext, "vsmdi")) return "application/xml";
  if (!strcmp(ext, "vspscc")) return "text/plain";
  if (!strcmp(ext, "vss")) return "application/vnd.visio";
  if (!strcmp(ext, "vsscc")) return "text/plain";
  if (!strcmp(ext, "vssettings")) return "text/xml";
  if (!strcmp(ext, "vssscc")) return "text/plain";
  if (!strcmp(ext, "vst")) return "application/vnd.visio";
  if (!strcmp(ext, "vstemplate")) return "text/xml";
  if (!strcmp(ext, "vsto")) return "application/x-ms-vsto";
  if (!strcmp(ext, "vsw")) return "application/vnd.visio";
  if (!strcmp(ext, "vsx")) return "application/vnd.visio";
  if (!strcmp(ext, "vtt")) return "text/vtt";
  if (!strcmp(ext, "vtx")) return "application/vnd.visio";
  if (!strcmp(ext, "wasm")) return "application/wasm";
  if (!strcmp(ext, "wav")) return "audio/wav";
  if (!strcmp(ext, "wave")) return "audio/wav";
  if (!strcmp(ext, "wax")) return "audio/x-ms-wax";
  if (!strcmp(ext, "wbk")) return "application/msword";
  if (!strcmp(ext, "wbmp")) return "image/vnd.wap.wbmp";
  if (!strcmp(ext, "wcm")) return "application/vnd.ms-works";
  if (!strcmp(ext, "wdb")) return "application/vnd.ms-works";
  if (!strcmp(ext, "wdp")) return "image/vnd.ms-photo";
  if (!strcmp(ext, "webarchive")) return "application/x-safari-webarchive";
  if (!strcmp(ext, "webm")) return "video/webm";
  if (!strcmp(ext, "webp")) return "image/webp"; /* https://en.wikipedia.org/wiki/WebP */
  if (!strcmp(ext, "webtest")) return "application/xml";
  if (!strcmp(ext, "wiq")) return "application/xml";
  if (!strcmp(ext, "wiz")) return "application/msword";
  if (!strcmp(ext, "wks")) return "application/vnd.ms-works";
  if (!strcmp(ext, "WLMP")) return "application/wlmoviemaker";
  if (!strcmp(ext, "wlpginstall")) return "application/x-wlpg-detect";
  if (!strcmp(ext, "wlpginstall3")) return "application/x-wlpg3-detect";
  if (!strcmp(ext, "wm")) return "video/x-ms-wm";
  if (!strcmp(ext, "wma")) return "audio/x-ms-wma";
  if (!strcmp(ext, "wmd")) return "application/x-ms-wmd";
  if (!strcmp(ext, "wmf")) return "application/x-msmetafile";
  if (!strcmp(ext, "wml")) return "text/vnd.wap.wml";
  if (!strcmp(ext, "wmlc")) return "application/vnd.wap.wmlc";
  if (!strcmp(ext, "wmls")) return "text/vnd.wap.wmlscript";
  if (!strcmp(ext, "wmlsc")) return "application/vnd.wap.wmlscriptc";
  if (!strcmp(ext, "wmp")) return "video/x-ms-wmp";
  if (!strcmp(ext, "wmv")) return "video/x-ms-wmv";
  if (!strcmp(ext, "wmx")) return "video/x-ms-wmx";
  if (!strcmp(ext, "wmz")) return "application/x-ms-wmz";
  if (!strcmp(ext, "woff")) return "application/font-woff";
  if (!strcmp(ext, "woff2")) return "application/font-woff2";
  if (!strcmp(ext, "wpl")) return "application/vnd.ms-wpl";
  if (!strcmp(ext, "wps")) return "application/vnd.ms-works";
  if (!strcmp(ext, "wri")) return "application/x-mswrite";
  if (!strcmp(ext, "wrl")) return "x-world/x-vrml";
  if (!strcmp(ext, "wrz")) return "x-world/x-vrml";
  if (!strcmp(ext, "wsc")) return "text/scriptlet";
  if (!strcmp(ext, "wsdl")) return "text/xml";
  if (!strcmp(ext, "wvx")) return "video/x-ms-wvx";
  if (!strcmp(ext, "x")) return "application/directx";
  if (!strcmp(ext, "xaf")) return "x-world/x-vrml";
  if (!strcmp(ext, "xaml")) return "application/xaml+xml";
  if (!strcmp(ext, "xap")) return "application/x-silverlight-app";
  if (!strcmp(ext, "xbap")) return "application/x-ms-xbap";
  if (!strcmp(ext, "xbm")) return "image/x-xbitmap";
  if (!strcmp(ext, "xdr")) return "text/plain";
  if (!strcmp(ext, "xht")) return "application/xhtml+xml";
  if (!strcmp(ext, "xhtml")) return "application/xhtml+xml";
  if (!strcmp(ext, "xla")) return "application/vnd.ms-excel";
  if (!strcmp(ext, "xlam")) return "application/vnd.ms-excel.addin.macroEnabled.12";
  if (!strcmp(ext, "xlc")) return "application/vnd.ms-excel";
  if (!strcmp(ext, "xld")) return "application/vnd.ms-excel";
  if (!strcmp(ext, "xlk")) return "application/vnd.ms-excel";
  if (!strcmp(ext, "xll")) return "application/vnd.ms-excel";
  if (!strcmp(ext, "xlm")) return "application/vnd.ms-excel";
  if (!strcmp(ext, "xls")) return "application/vnd.ms-excel";
  if (!strcmp(ext, "xlsb")) return "application/vnd.ms-excel.sheet.binary.macroEnabled.12";
  if (!strcmp(ext, "xlsm")) return "application/vnd.ms-excel.sheet.macroEnabled.12";
  if (!strcmp(ext, "xlsx")) return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  if (!strcmp(ext, "xlt")) return "application/vnd.ms-excel";
  if (!strcmp(ext, "xltm")) return "application/vnd.ms-excel.template.macroEnabled.12";
  if (!strcmp(ext, "xltx")) return "application/vnd.openxmlformats-officedocument.spreadsheetml.template";
  if (!strcmp(ext, "xlw")) return "application/vnd.ms-excel";
  if (!strcmp(ext, "xml")) return "text/xml";
  if (!strcmp(ext, "xmp")) return "application/octet-stream";
  if (!strcmp(ext, "xmta")) return "application/xml";
  if (!strcmp(ext, "xof")) return "x-world/x-vrml";
  if (!strcmp(ext, "XOML")) return "text/plain";
  if (!strcmp(ext, "xpm")) return "image/x-xpixmap";
  if (!strcmp(ext, "xps")) return "application/vnd.ms-xpsdocument";
  if (!strcmp(ext, "xrm-ms")) return "text/xml";
  if (!strcmp(ext, "xsc")) return "application/xml";
  if (!strcmp(ext, "xsd")) return "text/xml";
  if (!strcmp(ext, "xsf")) return "text/xml";
  if (!strcmp(ext, "xsl")) return "text/xml";
  if (!strcmp(ext, "xslt")) return "text/xml";
  if (!strcmp(ext, "xsn")) return "application/octet-stream";
  if (!strcmp(ext, "xss")) return "application/xml";
  if (!strcmp(ext, "xspf")) return "application/xspf+xml";
  if (!strcmp(ext, "xtp")) return "application/octet-stream";
  if (!strcmp(ext, "xwd")) return "image/x-xwindowdump";
  if (!strcmp(ext, "z")) return "application/x-compress";
  if (!strcmp(ext, "zip")) return "application/zip";

  return "application/octet-stream";
}


