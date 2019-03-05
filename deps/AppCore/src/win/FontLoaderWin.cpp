/*
Copyright 2019 Ultralight, Inc.
Copyright 2012 Google Inc.

Portions below are under the following license:
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#include "FontLoaderWin.h"
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <usp10.h>
#include <winerror.h>
#include <MLang.h>
#include <vector>
#include <memory>
#include <unordered_map>

static const wchar_t kFontLinkKey[] =
L"Software\\Microsoft\\Windows NT\\CurrentVersion\\FontLink\\SystemLink";

struct MatchImprovingProcData {
  MatchImprovingProcData(LONG desiredWeight, bool desiredItalic)
    : m_desiredWeight(desiredWeight)
    , m_desiredItalic(desiredItalic)
    , m_hasMatched(false)
  {
  }

  LONG m_desiredWeight;
  bool m_desiredItalic;
  bool m_hasMatched;
  LOGFONT m_chosen;
};

static int CALLBACK matchImprovingEnumProc(CONST LOGFONT* candidate, CONST TEXTMETRIC* metrics, DWORD fontType, LPARAM lParam)
{
  MatchImprovingProcData* matchData = reinterpret_cast<MatchImprovingProcData*>(lParam);

  if (!matchData->m_hasMatched) {
    matchData->m_hasMatched = true;
    matchData->m_chosen = *candidate;
    return 1;
  }

  if (!candidate->lfItalic != !matchData->m_chosen.lfItalic) {
    if (!candidate->lfItalic == !matchData->m_desiredItalic)
      matchData->m_chosen = *candidate;

    return 1;
  }

  unsigned chosenWeightDeltaMagnitude = abs(matchData->m_chosen.lfWeight - matchData->m_desiredWeight);
  unsigned candidateWeightDeltaMagnitude = abs(candidate->lfWeight - matchData->m_desiredWeight);

  // If both are the same distance from the desired weight, prefer the candidate if it is further from regular.
  if (chosenWeightDeltaMagnitude == candidateWeightDeltaMagnitude && abs(candidate->lfWeight - FW_NORMAL) > abs(matchData->m_chosen.lfWeight - FW_NORMAL)) {
    matchData->m_chosen = *candidate;
    return 1;
  }

  // Otherwise, prefer the one closer to the desired weight.
  if (candidateWeightDeltaMagnitude < chosenWeightDeltaMagnitude)
    matchData->m_chosen = *candidate;

  return 1;
}

static USHORT ReadUShort(BYTE* p)
{
  return ((USHORT)p[0] << 8) + ((USHORT)p[1]);
}

static DWORD ReadDWord(BYTE* p)
{
  return ((LONG)p[0] << 24) + ((LONG)p[1] << 16) + ((LONG)p[2] << 8) + ((LONG)p[3]);
}

static DWORD ReadTag(BYTE* p)
{
  return ((LONG)p[3] << 24) + ((LONG)p[2] << 16) + ((LONG)p[1] << 8) + ((LONG)p[0]);
}

static void WriteDWord(BYTE* p, DWORD dw)
{
  p[0] = (BYTE)((dw >> 24) & 0xFF);
  p[1] = (BYTE)((dw >> 16) & 0xFF);
  p[2] = (BYTE)((dw >> 8) & 0xFF);
  p[3] = (BYTE)((dw) & 0xFF);
}

static DWORD RoundUpToDWord(DWORD val)
{
  return (val + 3) & ~3;
}

#define TTC_FILE 0x66637474

const DWORD sizeOfFixedHeader = 12;
const DWORD offsetOfTableCount = 4;

const DWORD sizeOfTableEntry = 16;
const DWORD offsetOfTableTag = 0;
const DWORD offsetOfTableChecksum = 4;
const DWORD offsetOfTableOffset = 8;
const DWORD offsetOfTableLength = 12;

static bool ExtractFontData(HDC hdc, DWORD& fontDataSize, BYTE*& fontData)
{
  bool ok = false;
  fontData = NULL;
  fontDataSize = 0;

  // Check if font is in TrueType collection
  if (GetFontData(hdc, TTC_FILE, 0, NULL, 0) != GDI_ERROR)
  {
    // Extract font data from TTC (TrueType Collection)
    // 1. Read number of tables in the font (ushort value at offset 2)
    USHORT nTables;
    BYTE uShortBuf[2];
    if (GetFontData(hdc, 0, 4, uShortBuf, 2) == GDI_ERROR)
    {
      return false;
    }
    nTables = ReadUShort(uShortBuf);

    // 2. Calculate memory needed for the whole font header and read it into buffer
    DWORD headerSize = sizeOfFixedHeader + nTables * sizeOfTableEntry;
    BYTE* fontHeader = new BYTE[headerSize];
    if (!fontHeader)
    {
      return false;
    }

    if (GetFontData(hdc, 0, 0, fontHeader, headerSize) == GDI_ERROR)
    {
      delete[] fontHeader;
      return false;
    }

    // 3. Calculate total font size. 
    //    Tables are padded to 4-byte boundaries, so length should be rounded up to dword.
    DWORD bufferSize = headerSize;
    USHORT i;
    for (i = 0; i < nTables; i++)
    {
      DWORD tableLength = ReadDWord(fontHeader + sizeOfFixedHeader + i * sizeOfTableEntry + offsetOfTableLength);
      if (i < nTables - 1)
      {
        bufferSize += RoundUpToDWord(tableLength);
      }
      else
      {
        bufferSize += tableLength;
      }
    }

    // 4. Copying header into target buffer.
    //    Patch offsets with correct values while copying data.
    BYTE* buffer = new BYTE[bufferSize];
    if (buffer == NULL)
    {
      delete[] fontHeader;
      return false;
    }
    memcpy(buffer, fontHeader, headerSize);

    // 5. Get table data from GDI, write it into known place 
    //    inside target buffer and fix offset value.
    DWORD runningOffset = headerSize;

    for (i = 0; i < nTables; i++)
    {
      BYTE* entryData = fontHeader + sizeOfFixedHeader + i * sizeOfTableEntry;
      DWORD tableTag = ReadTag(entryData + offsetOfTableTag);
      DWORD tableLength = ReadDWord(entryData + offsetOfTableLength);

      // Write new offset for this table.
      WriteDWord(buffer + sizeOfFixedHeader + i * sizeOfTableEntry + offsetOfTableOffset, runningOffset);

      //Get font data from GDI and place it into target buffer
      if (GetFontData(hdc, tableTag, 0, buffer + runningOffset, tableLength) == GDI_ERROR)
      {
        delete[] buffer;
        delete[] fontHeader;
        return false;
      }
      runningOffset += tableLength;

      // Pad tables (except last) with zero's
      if (i < nTables - 1)
      {
        while (runningOffset % 4 != 0)
        {
          buffer[runningOffset] = 0;
          ++runningOffset;
        }
      }
    }
    delete[] fontHeader;
    fontDataSize = bufferSize;
    fontData = buffer;
  }
  else
  {
    // Check if font is TrueType
    DWORD bufferSize = GetFontData(hdc, 0, 0, NULL, 0);
    if (bufferSize != GDI_ERROR)
    {
      BYTE* buffer = new BYTE[bufferSize];
      if (buffer != NULL)
      {
        ok = (GetFontData(hdc, 0, 0, buffer, bufferSize) != GDI_ERROR);
        if (ok)
        {
          fontDataSize = bufferSize;
          fontData = buffer;
        }
        else
        {
          delete[] buffer;
        }
      }
    }
  }
  return ok;
}

ultralight::Ref<ultralight::Buffer> GetFontData(const HFONT fontHandle)
{
  HDC hdc = ::CreateCompatibleDC(NULL);
  if (hdc != NULL && fontHandle != NULL)
  {
    ::SelectObject(hdc, fontHandle);
    DWORD fontDataSize;
    BYTE* fontData;
    bool ok = ExtractFontData(hdc, fontDataSize, fontData);
    auto result = ultralight::Buffer::Create(fontData, fontDataSize);
    delete[] fontData;
    ::DeleteDC(hdc);
    return result;
  }

  return ultralight::Buffer::Create(nullptr, 0);
}

static LONG toGDIFontWeight(LONG fontWeight)
{
  if (fontWeight < LONG(150))
    return FW_THIN;
  if (fontWeight < LONG(250))
    return FW_EXTRALIGHT;
  if (fontWeight < LONG(350))
    return FW_LIGHT;
  if (fontWeight < LONG(450))
    return FW_NORMAL;
  if (fontWeight < LONG(550))
    return FW_MEDIUM;
  if (fontWeight < LONG(650))
    return FW_SEMIBOLD;
  if (fontWeight < LONG(750))
    return FW_BOLD;
  if (fontWeight < LONG(850))
    return FW_EXTRABOLD;
  return FW_HEAVY;
}

static HFONT createGDIFont(const std::wstring& family, LONG desiredWeight, bool desiredItalic, int size, bool synthesizeItalic)
{
  HDC hdc = GetDC(0);

  LOGFONT logFont;
  memset(&logFont, 0, sizeof(LOGFONT));
  logFont.lfCharSet = DEFAULT_CHARSET;
#ifdef UNICODE
  std::wstring truncatedFamily = family.substr(0, static_cast<unsigned>(LF_FACESIZE - 1));
  memcpy(logFont.lfFaceName, truncatedFamily.data(), truncatedFamily.length() * sizeof(wchar_t));
#else
  std::vector<char> family_utf8;
  int sz = WideCharToMultiByte(CP_UTF8, 0, family.c_str(), -1, NULL, 0, NULL, NULL);
  if (sz <= 0)
    return nullptr;
  family_utf8.resize(sz);
  WideCharToMultiByte(CP_UTF8, 0, family.c_str(), -1, &family_utf8[0], (int)family_utf8.size(), NULL, NULL);
  std::string family_utf8Str = std::string(&family_utf8[0]);
  std::string truncatedFamily = family_utf8Str.substr(0, static_cast<unsigned>(LF_FACESIZE - 1));
  memcpy(logFont.lfFaceName, truncatedFamily.data(), truncatedFamily.length() * sizeof(char));
#endif
  logFont.lfFaceName[truncatedFamily.length()] = 0;
  logFont.lfPitchAndFamily = 0;

  MatchImprovingProcData matchData(toGDIFontWeight(desiredWeight), desiredItalic);
  EnumFontFamiliesEx(hdc, &logFont, matchImprovingEnumProc, reinterpret_cast<LPARAM>(&matchData), 0);

  if (!matchData.m_hasMatched)
    return nullptr;

  matchData.m_chosen.lfHeight = -size;
  matchData.m_chosen.lfWidth = 0;
  matchData.m_chosen.lfEscapement = 0;
  matchData.m_chosen.lfOrientation = 0;
  matchData.m_chosen.lfUnderline = false;
  matchData.m_chosen.lfStrikeOut = false;
  matchData.m_chosen.lfCharSet = DEFAULT_CHARSET;
  matchData.m_chosen.lfOutPrecision = OUT_TT_ONLY_PRECIS;
  matchData.m_chosen.lfQuality = DEFAULT_QUALITY;
  matchData.m_chosen.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

  if (desiredItalic && !matchData.m_chosen.lfItalic && synthesizeItalic)
    matchData.m_chosen.lfItalic = 1;

  auto chosenFont = ::CreateFontIndirect(&matchData.m_chosen);
  if (!chosenFont)
    return nullptr;

  SaveDC(hdc);
  SelectObject(hdc, chosenFont);
  TCHAR actualName[LF_FACESIZE];
  GetTextFace(hdc, LF_FACESIZE, actualName);
  RestoreDC(hdc, -1);

  if (_tcsicmp(matchData.m_chosen.lfFaceName, actualName))
    return nullptr;

  return chosenFont;
}

static bool CurrentFontContainsCharacter(HDC hdc, wchar_t character) {
  std::unique_ptr<char[]> buffer(new char[::GetFontUnicodeRanges(hdc, 0)]);
  GLYPHSET* glyphset = reinterpret_cast<GLYPHSET*>(buffer.get());
  ::GetFontUnicodeRanges(hdc, glyphset);
  for (unsigned i = 0; i < glyphset->cRanges; ++i) {
    if (glyphset->ranges[i].wcLow <= character &&
      glyphset->ranges[i].wcLow + glyphset->ranges[i].cGlyphs > character) {
      return true;
    }
  }
  return false;
}

static IMLangFontLink2* GetFontLinkInterface() {
  static IMultiLanguage* multi_language = NULL;
  if (!multi_language) {
    if (::CoCreateInstance(CLSID_CMultiLanguage, 0, CLSCTX_ALL,
      IID_IMultiLanguage,
      reinterpret_cast<void**>(&multi_language)) != S_OK)
      return 0;
  }

  static IMLangFontLink2* font_link = NULL;
  if (!font_link) {
    if (multi_language->QueryInterface(&font_link) != S_OK)
      return 0;
  }

  return font_link;
}

static int CALLBACK MetaFileEnumProc(
  HDC hdc,
  HANDLETABLE* table,
  CONST ENHMETARECORD* record,
  int table_entries,
  LPARAM log_font) {
  // Enumerate the MetaFile operations and find the last CreateFontIndirect call
  // to determined what font uniscribe used in the metafile.
  if (record->iType == EMR_EXTCREATEFONTINDIRECTW) {
    const EMREXTCREATEFONTINDIRECTW* create_font_record =
      reinterpret_cast<const EMREXTCREATEFONTINDIRECTW*>(record);
    *reinterpret_cast<LOGFONT*>(log_font) = create_font_record->elfw.elfLogFont;
  }
  return true;
}

static LOGFONT CreateMLangFont(IMLangFontLink2* font_link,
  HDC hdc,
  DWORD code_pages,
  wchar_t character) {
  HFONT mlang_font = NULL;
  LOGFONT lf = { 0 };
  if (SUCCEEDED(font_link->MapFont(hdc, code_pages, character, &mlang_font)) &&
    mlang_font) {
    ::GetObject(mlang_font, sizeof(LOGFONT), &lf);
    font_link->ReleaseFont(mlang_font);
  }
  return lf;
}

static bool FontContainsCharacter(const LOGFONT& logfont,
  HDC dc,
  wchar_t character) {
  HFONT font = ::CreateFontIndirect(&logfont);
  bool contains = font;
  if (contains) {
    HGDIOBJ old_font = ::SelectObject(dc, font);
    contains = CurrentFontContainsCharacter(dc, character);
    ::SelectObject(dc, old_font);
  }
  ::DeleteObject(font);
  return contains;
}

static const std::vector<std::wstring>* GetLinkedFonts(const std::wstring& family) {
  typedef std::unordered_map<std::wstring, std::vector<std::wstring> > FontLinkMap;
  static FontLinkMap system_link_map;
  FontLinkMap::iterator it = system_link_map.find(family);
  if (it != system_link_map.end()) {
    return &it->second;
  }
  HKEY key = NULL;
  if (FAILED(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
    kFontLinkKey,
    0,
    KEY_READ,
    &key))) {
    return NULL;
  }
  DWORD size = 0;
  RegQueryValueEx(key, family.c_str(), 0, NULL, NULL, &size);
  std::unique_ptr<BYTE[]> buffer(new BYTE[size]);
  if (SUCCEEDED(RegQueryValueEx(key,
    family.c_str(),
    0,
    NULL,
    buffer.get(),
    &size))) {
    // The value in the registry is a MULTI_SZ like:
    // <filename>,<font family>\0
    // for example:
    // MSMINCHO.TTC,MS Mincho\0
    // MINGLIU.TTC,MingLiU\0
    // SIMSUN.TTC,SimSun\0
    // And we need to find all the font families.
    std::vector<std::wstring>& linked_fonts = system_link_map[family];
    unsigned i = 0;
    unsigned length = size / sizeof(wchar_t);
    wchar_t* string_value = reinterpret_cast<wchar_t*>(buffer.get());
    while (i < length) {
      while (i < length && string_value[i] != ',')
        i++;
      i++;
      unsigned j = i;
      while (j < length && string_value[j])
        j++;
      linked_fonts.push_back(std::wstring(string_value + i, j - i));
      i = j + 1;
    }
    return &linked_fonts;
  }
  return NULL;
}

// Fallback font is determined by following steps:
// 1. Use MLang to map current font.
// 2. Try drawing with the mapped font and find which font uniscribe used.
// 3. if the font in 2 does not contain the character, try its linked fonts.
LOGFONT GetFallbackFont(HDC dc, wchar_t character) {
  LOGFONT fallback_font = { 0 };
  ::GetObject(::GetCurrentObject(dc, OBJ_FONT),
    sizeof(fallback_font),
    &fallback_font);
  if (IMLangFontLink2* font_link = GetFontLinkInterface()) {
    DWORD char_code_pages = 0;
    font_link->GetCharCodePages(character, &char_code_pages);
    fallback_font = CreateMLangFont(font_link, dc, char_code_pages,
      character);
  }
  // To find out what font Uniscribe would use, we make it draw into a
  // metafile and intercept calls to CreateFontIndirect().
  HDC meta_file_dc = ::CreateEnhMetaFile(dc, NULL, NULL, NULL);
  ::SelectObject(meta_file_dc, ::GetCurrentObject(dc, OBJ_FONT));

  bool success = false;
  SCRIPT_STRING_ANALYSIS ssa;

  if (SUCCEEDED(::ScriptStringAnalyse(
    meta_file_dc, &character, 1, 0, -1,
    SSA_METAFILE | SSA_FALLBACK | SSA_GLYPHS | SSA_LINK,
    0, NULL, NULL, NULL, NULL, NULL, &ssa))) {
    success = SUCCEEDED(::ScriptStringOut(ssa, 0, 0, 0, NULL, 0, 0, FALSE));
    ::ScriptStringFree(&ssa);
  }
  HENHMETAFILE meta_file = ::CloseEnhMetaFile(meta_file_dc);
  if (success)
    ::EnumEnhMetaFile(0, meta_file, MetaFileEnumProc, &fallback_font, NULL);
  ::DeleteEnhMetaFile(meta_file);
  if (FontContainsCharacter(fallback_font, dc, character))
    return fallback_font;
  const std::vector<std::wstring>* linked_fonts = GetLinkedFonts(
    fallback_font.lfFaceName);
  if (!linked_fonts)
    return fallback_font;
  for (size_t i = 0; i < linked_fonts->size(); ++i) {
    LOGFONT logfont = { 0 };
    logfont.lfCharSet = DEFAULT_CHARSET;
    ::wcscpy_s(logfont.lfFaceName, LF_FACESIZE, linked_fonts->at(i).c_str());
    if (FontContainsCharacter(logfont, dc, character))
      return logfont;
  }
  return fallback_font;
}

uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed) {
  uint32_t h = seed;
  if (len > 3) {
    const uint32_t* key_x4 = (const uint32_t*)key;
    size_t i = len >> 2;
    do {
      uint32_t k = *key_x4++;
      k *= 0xcc9e2d51;
      k = (k << 15) | (k >> 17);
      k *= 0x1b873593;
      h ^= k;
      h = (h << 13) | (h >> 19);
      h = (h * 5) + 0xe6546b64;
    } while (--i);
    key = (const uint8_t*)key_x4;
  }
  if (len & 3) {
    size_t i = len & 3;
    uint32_t k = 0;
    key = &key[i - 1];
    do {
      k <<= 8;
      k |= *key--;
    } while (--i);
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    h ^= k;
  }
  h ^= len;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}

namespace ultralight {

String16 FontLoaderWin::fallback_font() const {
  return "Times New Roman";
}

static int CALLBACK metaFileEnumProc(HDC hdc, HANDLETABLE* table, CONST ENHMETARECORD* record, int tableEntries, LPARAM logFont)
{
  if (record->iType == EMR_EXTCREATEFONTINDIRECTW) {
    const EMREXTCREATEFONTINDIRECTW* createFontRecord = reinterpret_cast<const EMREXTCREATEFONTINDIRECTW*>(record);
    *reinterpret_cast<LOGFONTW*>(logFont) = createFontRecord->elfw.elfLogFont;
  }
  return true;
}

IMLangFontLink2* getFontLinkInterface()
{
  static IMultiLanguage *multiLanguage;
  if (!multiLanguage) {
    if (CoCreateInstance(CLSID_CMultiLanguage, 0, CLSCTX_ALL, IID_IMultiLanguage, (void**)&multiLanguage) != S_OK)
      return 0;
  }

  static IMLangFontLink2* langFontLink;
  if (!langFontLink) {
    if (multiLanguage->QueryInterface(&langFontLink) != S_OK)
      return 0;
  }

  return langFontLink;
}

String16 FontLoaderWin::fallback_font_for_characters(const String16& characters, int weight, bool italic, float size) const {
  String16 result = fallback_font();
  LOGFONT logFont = GetFallbackFont(GetDC(0), characters[0]);
  if (logFont.lfFaceName[0])
    result = String16(logFont.lfFaceName, lstrlenW(logFont.lfFaceName));
  
  return result;
}

Ref<Buffer> FontLoaderWin::Load(const String16& family, int weight, bool italic, float size) {
  HFONT font = createGDIFont(family.data(), weight, italic, (int)size, false);
  ultralight::Ref<ultralight::Buffer> data = GetFontData(font);

  uint64_t hash = (uint64_t)murmur3_32((const uint8_t*)data->data(), data->size(), 0xBEEF);

  auto i = fonts_.find(hash);
  if (i == fonts_.end()) {
    fonts_.insert({ hash, data });
    return data;
  }
  else {
    return *i->second.get();
  }
}

}  // namespace ultralight

namespace framework {

ultralight::FontLoader* CreatePlatformFontLoader() {
  return new ultralight::FontLoaderWin();
}

}  // namespace framework
