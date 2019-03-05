#pragma once
#include <Ultralight/platform/FontLoader.h>

namespace ultralight {
    
class FontLoaderMac : public FontLoader {
public:
    FontLoaderMac() {}
    virtual ~FontLoaderMac() {}
    virtual String16 fallback_font() const override;
    virtual String16 fallback_font_for_characters(const String16& characters, int weight, bool italic, float size) const override;
    virtual Ref<Buffer> Load(const String16& family, int weight, bool italic, float size) override;
};
    
}  // namespace ultralight
