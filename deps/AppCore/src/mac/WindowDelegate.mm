#import "WindowDelegate.h"
#include "WindowMac.h"

@interface WindowDelegate ()

@end

@implementation WindowDelegate
{
    ultralight::WindowMac* window_;
}

- (void)initWithWindow:(ultralight::WindowMac*)window {
    window_ = window;
}

- (void)windowDidResize:(NSNotification *)notification {
    NSWindow* window = [notification object];
    window_->OnResize(window.frame.size.width, window.frame.size.height);
}

- (void)windowWillStartLiveResize:(NSNotification *)notification {
    
}

- (void)windowDidEndLiveResize:(NSNotification *)notification {
    
}

@end
