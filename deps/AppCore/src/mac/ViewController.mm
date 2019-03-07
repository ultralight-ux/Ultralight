#import "ViewController.h"
#import <MetalKit/MetalKit.h>
#import "ViewDelegate.h"

@interface CustomMTKView : MTKView

@end

@implementation CustomMTKView

- (BOOL)acceptsFirstResponder {
    return YES;
}

@end

@implementation ViewController
{
    ultralight::WindowMac* _window;
    NSRect _initialFrame;
    ViewDelegate *_delegate;
}

- (void)initWithWindow:(ultralight::WindowMac*)window frame:(NSRect)rect
{
    _window = window;
    _initialFrame = rect;
}

- (MTKView*)metalView
{
    return (MTKView *)self.view;
}

- (void)loadView
{
    MTKView* view = [[CustomMTKView alloc] initWithFrame:_initialFrame device:MTLCreateSystemDefaultDevice()];
    
    [view setClearColor:MTLClearColorMake(0, 0, 0, 1)];
    [view setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [view setDepthStencilPixelFormat:MTLPixelFormatDepth32Float];
    [view setAutoResizeDrawable:false];
    
    [self setView:view];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    if(![self metalView].device)
    {
        NSLog(@"Metal is not supported on this device");
        return;
    }
    
    _delegate = [[ViewDelegate alloc] initWithWindow:_window];
    
    if(!_delegate)
    {
        NSLog(@"Application failed initialization");
        return;
    }
    
    // Initialize our delegate with the view size
    [_delegate mtkView:[self metalView] drawableSizeWillChange:[self metalView].drawableSize];
    
    [self metalView].delegate = _delegate;
    
    NSTrackingArea* trackingArea = [[NSTrackingArea alloc]
                                    initWithRect:self.view.bounds
                                    options:NSTrackingMouseMoved | NSTrackingActiveAlways
                                    owner:self userInfo:nil];
    [self.view addTrackingArea:trackingArea];
}

- (void)scrollWheel:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [self.view convertPoint:eventLocation fromView:nil];
    point.y = self.view.bounds.size.height - point.y;
    
    [_delegate scrollWheel:[event deltaY] * 8 mouseX:point.x mouseY:point.y];
}

- (void)mouseMoved:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [self.view convertPoint:eventLocation fromView:nil];
    point.y = self.view.bounds.size.height - point.y;
    
    [_delegate mouseMoved:point.x mouseY:point.y];
}

- (void)mouseDown:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [self.view convertPoint:eventLocation fromView:nil];
    point.y = self.view.bounds.size.height - point.y;
    
    [_delegate leftMouseDown:point.x mouseY:point.y];
}

- (void)mouseUp:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [self.view convertPoint:eventLocation fromView:nil];
    point.y = self.view.bounds.size.height - point.y;
    
    [_delegate leftMouseUp:point.x mouseY:point.y];
}

- (void)rightMouseDown:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [self.view convertPoint:eventLocation fromView:nil];
    point.y = self.view.bounds.size.height - point.y;
    
    [_delegate rightMouseDown:point.x mouseY:point.y];
}

- (void)rightMouseUp:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [self.view convertPoint:eventLocation fromView:nil];
    point.y = self.view.bounds.size.height - point.y;
    
    [_delegate rightMouseUp:point.x mouseY:point.y];
}

- (void)keyUp:(NSEvent *)event
{
    [_delegate keyEvent:event];
}

- (void)keyDown:(NSEvent *)event
{
    [_delegate keyEvent:event];
    [self interpretKeyEvents:[NSArray arrayWithObject:event]];
}

- (void)insertText:(id)string {
    [_delegate textEvent:string];
}

- (void) deleteBackward: (id) sender {
}

- (void) insertNewline: (id) sender {
    [self insertText: @"\r"];
}

- (void) insertTab: (id) sender {
    [self insertText: @"\t"];
}

- (void) moveLeft: (id) sender {
}

- (void) moveRight: (id) sender {
}

- (void) moveUp: (id) sender {
}

- (void) moveDown: (id) sender {
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];
    
    // Update the view, if already loaded.
}


@end

