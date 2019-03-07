#import "GPUContextMetal.h"
#import "GPUDriverMetal.h"
#include "../../../shaders/metal/metal_shader.h"
#import <dispatch/dispatch.h>
#include <memory>

namespace ultralight {

GPUContextMetal::GPUContextMetal(MTKView* view, int screen_width, int screen_height, double screen_scale, bool fullscreen, bool enable_vsync) {
    device_ = view.device;
    view_ = view;
    
    set_screen_size(screen_width, screen_height);
    set_scale(screen_scale);
    
    NSError *error = NULL;
    
    // Load all the shader files with a .metal file extension in the project
    //id<MTLLibrary> defaultLibrary = [device_ newDefaultLibrary];
    dispatch_data_t data = dispatch_data_create(metal_shader, metal_shader_len, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    id<MTLLibrary> defaultLibrary = [device_ newLibraryWithData:data error:nil];
    
    MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineStateDescriptor.label = @"Fill Pipeline";
    pipelineStateDescriptor.vertexFunction = [defaultLibrary newFunctionWithName:@"vertexShader"];
    pipelineStateDescriptor.fragmentFunction = [defaultLibrary newFunctionWithName:@"fragmentShader"];
    pipelineStateDescriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
    
    auto colorAttachmentDesc = pipelineStateDescriptor.colorAttachments[0];
    colorAttachmentDesc.blendingEnabled = true;
    colorAttachmentDesc.rgbBlendOperation = MTLBlendOperationAdd;
    colorAttachmentDesc.alphaBlendOperation = MTLBlendOperationAdd;
    colorAttachmentDesc.sourceRGBBlendFactor = MTLBlendFactorOne;
    colorAttachmentDesc.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    colorAttachmentDesc.sourceAlphaBlendFactor = MTLBlendFactorOneMinusDestinationAlpha;
    colorAttachmentDesc.destinationAlphaBlendFactor = MTLBlendFactorOne;
    
    render_pipeline_state_ = [device_ newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                     error:&error];
    if (!render_pipeline_state_)
    {
        // Pipeline State creation could fail if we haven't properly set up our pipeline descriptor.
        //  If the Metal API validation is enabled, we can find out more information about what
        //  went wrong.  (Metal API validation is enabled by default when a debug build is run
        //  from Xcode)
        NSLog(@"Failed to create pipeline state, error %@", error);
        NSAlert *alert = [[NSAlert alloc] init];
        [alert setMessageText:@"Failed to create pipeline state."];
        [alert runModal];
        exit(-1);
    }
    
    MTLRenderPipelineDescriptor *pathPipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pathPipelineStateDescriptor.label = @"Fill Path Pipeline";
    pathPipelineStateDescriptor.vertexFunction = [defaultLibrary newFunctionWithName:@"pathVertexShader"];
    pathPipelineStateDescriptor.fragmentFunction = [defaultLibrary newFunctionWithName:@"pathFragmentShader"];
    pathPipelineStateDescriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
    
    auto pathColorAttachmentDesc = pathPipelineStateDescriptor.colorAttachments[0];
    pathColorAttachmentDesc.blendingEnabled = true;
    pathColorAttachmentDesc.rgbBlendOperation = MTLBlendOperationAdd;
    pathColorAttachmentDesc.alphaBlendOperation = MTLBlendOperationAdd;
    pathColorAttachmentDesc.sourceRGBBlendFactor = MTLBlendFactorOne;
    pathColorAttachmentDesc.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    pathColorAttachmentDesc.sourceAlphaBlendFactor = MTLBlendFactorOneMinusDestinationAlpha;
    pathColorAttachmentDesc.destinationAlphaBlendFactor = MTLBlendFactorOne;
    
    path_render_pipeline_state_ = [device_ newRenderPipelineStateWithDescriptor:pathPipelineStateDescriptor
                                                                          error:&error];
    if (!path_render_pipeline_state_)
    {
        // Pipeline State creation could fail if we haven't properly set up our pipeline descriptor.
        //  If the Metal API validation is enabled, we can find out more information about what
        //  went wrong.  (Metal API validation is enabled by default when a debug build is run
        //  from Xcode)
        NSLog(@"Failed to create path pipeline state, error %@", error);
        NSAlert *alert = [[NSAlert alloc] init];
        [alert setMessageText:@"Failed to create path pipeline state."];
        [alert runModal];
        exit(-1);
    }
    
    // Create the command queue
    command_queue_ = [device_ newCommandQueue];
    
    MTLCaptureManager *sharedCaptureManager = [MTLCaptureManager sharedCaptureManager];
    id<MTLCaptureScope> myCaptureScope = [sharedCaptureManager newCaptureScopeWithDevice:device_];
    myCaptureScope.label = @"My Capture Scope";
    sharedCaptureManager.defaultCaptureScope = myCaptureScope;
    
    driver_.reset(new ultralight::GPUDriverMetal(this));
}

void GPUContextMetal::BeginDrawing() {
    MTLCaptureManager *sharedCaptureManager = [MTLCaptureManager sharedCaptureManager];
    
    [sharedCaptureManager.defaultCaptureScope beginScope];
    
    // Create a new command buffer for each render pass to the current drawable
    command_buffer_ = [command_queue_ commandBuffer];
    command_buffer_.label = @"UltralightCommandBuffer";
}
    
void GPUContextMetal::EndDrawing() {
    if (!command_buffer_)
        return;
    
    driver_->EndDrawing();
    
    [command_buffer_ commit];
    [command_buffer_ waitUntilScheduled];
    command_buffer_ = nullptr;
    
    MTLCaptureManager *sharedCaptureManager = [MTLCaptureManager sharedCaptureManager];
    [sharedCaptureManager.defaultCaptureScope endScope];
}
    
void GPUContextMetal::PresentFrame() {
    if (command_buffer_ && view_.currentDrawable)
        [command_buffer_ presentDrawable:view_.currentDrawable];
}
    
void GPUContextMetal::Resize(int width, int height) {
    set_screen_size(width, height);
}
    
}  // namespace ultralight
