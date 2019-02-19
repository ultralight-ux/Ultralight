#include "FontLoaderWin.h"
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <vector>

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

HFONT createGDIFont(const std::wstring& family, LONG desiredWeight, bool desiredItalic, int size, bool synthesizeItalic)
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

String16 FontLoaderWin::fallback_font_for_characters(const String16& characters, int weight, bool italic, float size) const {
  // TODO, use WinAPI to get the correct font (eg, Yu Gothic for Japanese on Win10)
  return "Yu Gothic";
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
