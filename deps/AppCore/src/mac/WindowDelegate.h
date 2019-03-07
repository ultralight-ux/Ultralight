#import <Cocoa/Cocoa.h>

namespace ultralight { class WindowMac; }

@interface WindowDelegate : NSObject <NSWindowDelegate>

- (void)initWithWindow:(ultralight::WindowMac*)window;

@end
