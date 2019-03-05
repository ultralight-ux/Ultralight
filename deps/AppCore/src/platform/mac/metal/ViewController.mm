//
//  ViewController.mm
//  Browser
//
//  Created by Adam Simmons on 7/10/18.
//  Copyright © 2018 ultralight. All rights reserved.
//

#import "ViewController.h"
#import <MetalKit/MetalKit.h>
#import "../../../Application.h"
#import "PlatformMetal.h"
#import "MainLoopMetal.h"

@implementation ViewController
{
    MTKView *_view;
    MainLoopMetal *_main_loop;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Set the view to use the default device
    _view = (MTKView *)self.view;
    _view.device = MTLCreateSystemDefaultDevice();
    
    if(!_view.device)
    {
        NSLog(@"Metal is not supported on this device");
        return;
    }
    
    framework::PlatformWindowConfig config;
    config.view = _view;
    
    _main_loop = [[MainLoopMetal alloc] initWithPlatformWindowConfig:config];
    
    if(!_main_loop)
    {
        NSLog(@"Application failed initialization");
        return;
    }
    
    // Initialize our main loop with the view size
    [_main_loop mtkView:_view drawableSizeWillChange:_view.drawableSize];
    
    _view.delegate = _main_loop;
    
    NSTrackingArea* trackingArea = [[NSTrackingArea alloc]
                                    initWithRect:[_view bounds]
                                    options:NSTrackingMouseMoved | NSTrackingActiveAlways
                                    owner:self userInfo:nil];
    [_view addTrackingArea:trackingArea];
}

- (void)scrollWheel:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [_view convertPoint:eventLocation fromView:nil];
    point.y = _view.bounds.size.height - point.y;
    
    [_main_loop scrollWheel:[event deltaY] * 8 mouseX:point.x mouseY:point.y];
}

- (void)mouseMoved:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [_view convertPoint:eventLocation fromView:nil];
    point.y = _view.bounds.size.height - point.y;
    
    [_main_loop mouseMoved:point.x mouseY:point.y];
}

- (void)mouseDown:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [_view convertPoint:eventLocation fromView:nil];
    point.y = _view.bounds.size.height - point.y;
    
    [_main_loop leftMouseDown:point.x mouseY:point.y];
}

- (void)mouseUp:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [_view convertPoint:eventLocation fromView:nil];
    point.y = _view.bounds.size.height - point.y;
    
    [_main_loop leftMouseUp:point.x mouseY:point.y];
}

- (void)rightMouseDown:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [_view convertPoint:eventLocation fromView:nil];
    point.y = _view.bounds.size.height - point.y;
    
    [_main_loop rightMouseDown:point.x mouseY:point.y];
}

- (void)rightMouseUp:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint point = [_view convertPoint:eventLocation fromView:nil];
    point.y = _view.bounds.size.height - point.y;
    
    [_main_loop rightMouseUp:point.x mouseY:point.y];
}

- (void)keyUp:(NSEvent *)event
{
    [_main_loop keyEvent:event];
}

- (void)keyDown:(NSEvent *)event
{
    [_main_loop keyEvent:event];
    [self interpretKeyEvents:[NSArray arrayWithObject:event]];
}

- (void)insertText:(id)string {
    [_main_loop textEvent:string];
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
