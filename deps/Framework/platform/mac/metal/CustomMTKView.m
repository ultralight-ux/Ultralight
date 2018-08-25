//
//  CustomMTKView.m
//  Browser
//
//  Created by Adam Simmons on 7/24/18.
//  Copyright © 2018 ultralight. All rights reserved.
//

#import <MetalKit/MetalKit.h>

@interface CustomMTKView : MTKView

@end

@implementation CustomMTKView

- (BOOL)acceptsFirstResponder {
    return YES;
}

@end
