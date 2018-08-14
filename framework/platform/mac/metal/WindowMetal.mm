#include "../../../Window.h"
#include "PlatformMetal.h"

namespace framework {
    
class WindowMac : public Window {
public:
    WindowMac() : view_(nullptr) {}
    virtual ~WindowMac() {}
    
    bool Initialize(const PlatformWindowConfig& config) {
        view_ = config.view;
        
        NSScreen *mainScreen = [NSScreen mainScreen];
        scale_ = mainScreen.backingScaleFactor;

        return true;
    }
    
    virtual void set_listener(WindowListener* listener) { listener_ = listener; }
    
    virtual WindowListener* listener() { return listener_; }
    
    virtual int width() const { return PixelsToDevice((int)view_.drawableSize.width); }
    
    virtual int height() const { return PixelsToDevice((int)view_.drawableSize.height); }
    
    virtual bool is_fullscreen() const { return view_.isInFullScreenMode; }
    
    virtual PlatformWindowHandle handle() const { return { view_ }; }
    
    virtual double scale() const { return scale_; }
    
    virtual int DeviceToPixels(int val) const { return (int)round(val * scale_); }
    
    virtual int PixelsToDevice(int val) const { return (int)round(val / scale_); }
    
    virtual void SetTitle(const char* title) { view_.window.title = [NSString stringWithUTF8String:title]; }
    
    virtual void SetCursor(ultralight::Cursor cursor) {}
    
    virtual void Close() { [NSApp terminate: nil]; }
    
protected:
    MTKView* view_;
    WindowListener* listener_ = nullptr;
    double scale_;
};
    
std::unique_ptr<Window> Window::Create(const PlatformWindowConfig& config) {
    auto window = std::make_unique<WindowMac>();
    if (window->Initialize(config))
        return std::move(window);
    
    return nullptr;
}
    
}  // namespace framework
