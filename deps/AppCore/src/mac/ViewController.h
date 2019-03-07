#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

namespace ultralight { class WindowMac; }

@interface ViewController : NSViewController

- (void)initWithWindow:(ultralight::WindowMac*)window frame:(NSRect)rect;
- (MTKView*)metalView;

@end
