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

ultralight::Ref<ultralight::Buffer> GetFontData(const HFONT fontHandle)
{
  HDC hdc = ::CreateCompatibleDC(NULL);
  if (hdc != NULL && fontHandle != NULL)
  {
    ::SelectObject(hdc, fontHandle);
    const DWORD size = ::GetFontData(hdc, 0, 0, NULL, 0);
    if (size > 0)
    {
      char* buffer = new char[size];
      if (::GetFontData(hdc, 0, 0, buffer, size) == size)
      {
        auto result = ultralight::Buffer::Create(buffer, size);
        delete[] buffer;
        ::DeleteDC(hdc);
        return result;
      }
      delete[] buffer;
    }
    ::DeleteDC(hdc);
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
