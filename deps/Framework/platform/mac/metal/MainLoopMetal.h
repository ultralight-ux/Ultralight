#import <MetalKit/MetalKit.h>
#import "../../../Platform.h"

@interface MainLoopMetal : NSObject<MTKViewDelegate>

- (nonnull instancetype)initWithPlatformWindowConfig:(framework::PlatformWindowConfig&)config;

- (void)mouseMoved:(CGFloat)x mouseY:(CGFloat)y;

- (void)leftMouseDown:(CGFloat)x mouseY:(CGFloat)y;

- (void)leftMouseUp:(CGFloat)x mouseY:(CGFloat)y;

- (void)rightMouseDown:(CGFloat)x mouseY:(CGFloat)y;

- (void)rightMouseUp:(CGFloat)x mouseY:(CGFloat)y;

- (void)scrollWheel:(CGFloat)deltaY mouseX:(CGFloat)x mouseY:(CGFloat)y;

- (void)keyEvent:(NSEvent*)event;

- (void)textEvent:(id)string;

@end
