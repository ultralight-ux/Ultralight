#import "MainLoopMetal.h"
#import "../../../Window.h"
#import "../../../Application.h"
#include <memory>
#include <vector>

static inline ultralight::String toString(NSString* str) {
    CFStringRef cfStr = (__bridge CFStringRef)str;
    if (!cfStr)
        return ultralight::String();
    CFIndex size = CFStringGetLength(cfStr);
    std::vector<ultralight::Char16> buffer(size);
    CFStringGetCharacters(cfStr, CFRangeMake(0, size), (UniChar*)buffer.data());
    return ultralight::String16(buffer.data(), size);
}

@implementation MainLoopMetal
{
    std::unique_ptr<framework::Window> _window;
    std::unique_ptr<framework::Application> _application;
    ultralight::MouseEvent::Button _cur_button;
}

- (nonnull instancetype)initWithPlatformWindowConfig:(framework::PlatformWindowConfig&)config
{
    self = [super init];
    if(self)
    {
        _window = framework::Application::InitWindow(config);
        if (_window)
            _application = framework::Application::Create(*_window.get());
    }
    
    return self;
}

/// Called whenever view changes orientation or is resized
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    // Save the size of the drawable as we'll pass these
    //   values to our vertex shader when we draw
    //_viewportSize.x = size.width;
    //_viewportSize.y = size.height;
}

/// Called whenever the view needs to render a frame
- (void)drawInMTKView:(nonnull MTKView *)view
{
    _application->Update();
}

- (void)mouseMoved:(CGFloat)x mouseY:(CGFloat)y
{
    if (_window->listener())
        _window->listener()->OnMouseEvent({ ultralight::MouseEvent::kType_MouseMoved, (int)x, (int)y, ultralight::MouseEvent::kButton_None });
}

- (void)leftMouseDown:(CGFloat)x mouseY:(CGFloat)y
{
    if (_window->listener())
        _window->listener()->OnMouseEvent({ ultralight::MouseEvent::kType_MouseDown, (int)x, (int)y, ultralight::MouseEvent::kButton_Left });
}

- (void)leftMouseUp:(CGFloat)x mouseY:(CGFloat)y
{
    if (_window->listener())
        _window->listener()->OnMouseEvent({ ultralight::MouseEvent::kType_MouseUp, (int)x, (int)y, ultralight::MouseEvent::kButton_Left });
}

- (void)rightMouseDown:(CGFloat)x mouseY:(CGFloat)y
{
    if (_window->listener())
        _window->listener()->OnMouseEvent({ ultralight::MouseEvent::kType_MouseDown, (int)x, (int)y, ultralight::MouseEvent::kButton_Right });
}

- (void)rightMouseUp:(CGFloat)x mouseY:(CGFloat)y
{
    if (_window->listener())
        _window->listener()->OnMouseEvent({ ultralight::MouseEvent::kType_MouseUp, (int)x, (int)y, ultralight::MouseEvent::kButton_Right });
}

- (void)scrollWheel:(CGFloat)deltaY mouseX:(CGFloat)x mouseY:(CGFloat)y
{
    if (_window->listener())
        _window->listener()->OnScrollEvent({ ultralight::ScrollEvent::kType_ScrollByPixel, 0, (int)deltaY });
}

- (void)keyEvent:(NSEvent*)event
{
    if (_window->listener())
        _window->listener()->OnKeyEvent(ultralight::KeyEvent(event));
}

- (void)textEvent:(id)string
{
    ultralight::KeyEvent charEvent;
    charEvent.type = ultralight::KeyEvent::kType_Char;
    charEvent.text = toString(string);
    charEvent.unmodified_text = toString(string);
    
    if (_window->listener())
        _window->listener()->OnKeyEvent(charEvent);
}

@end
