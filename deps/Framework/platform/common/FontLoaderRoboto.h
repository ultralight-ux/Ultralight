#include <Ultralight/platform/FontLoader.h>

namespace ultralight {

/**
 * FontLoader implementation that just returns an embedded font (Roboto).
 */
class FontLoaderRoboto : public FontLoader {
public:
  FontLoaderRoboto() {}
  virtual ~FontLoaderRoboto() {}
  virtual String16 fallback_font() const override;
  virtual Ref<Buffer> Load(const String16& family, int weight, bool italic, float size) override;
};

}  // namespace ultralight
