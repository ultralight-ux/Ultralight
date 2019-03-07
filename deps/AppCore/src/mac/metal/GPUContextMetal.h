#pragma once
#import <MetalKit/MetalKit.h>
#import <Ultralight/platform/GPUDriver.h>
#import <Ultralight/platform/Config.h>
#include <memory>

namespace ultralight {
    
class GPUDriverMetal;
    
class GPUContextMetal {
public:
    GPUContextMetal(MTKView* view, int screen_width, int screen_height, double screen_scale, bool fullscreen, bool enable_vsync);
    
    virtual ~GPUContextMetal() {}

    // Inherited from GPUContext
    virtual ultralight::GPUDriver* driver() const { return (GPUDriver*)driver_.get(); }
    
    virtual ultralight::FaceWinding face_winding() const {
        return ultralight::kFaceWinding_CounterClockwise;
    }
    
    virtual void BeginDrawing();
    
    virtual void EndDrawing();
    
    virtual void PresentFrame();
    
    virtual void Resize(int width, int height);
    
    // Inherited from PlatformGPUContext
    virtual id<MTLDevice> device() { return device_; }
    virtual id<MTLRenderPipelineState> render_pipeline_state() { return render_pipeline_state_; }
    virtual id<MTLRenderPipelineState> path_render_pipeline_state() { return path_render_pipeline_state_; }

    virtual id<MTLCommandBuffer> command_buffer() { return command_buffer_; }
    virtual MTKView* view() { return view_; }
    virtual void set_scale(double scale) { scale_ = scale; }
    virtual double scale() const { return scale_; }
    
    virtual void set_screen_size(uint32_t width, uint32_t height) {
        width_ = width;
        height_ = height;
    }
    
    virtual void screen_size(uint32_t& width, uint32_t& height) {
        width = width_;
        height = height_;
    }
    
protected:
    id<MTLDevice> device_;
    id<MTLRenderPipelineState> render_pipeline_state_;
    id<MTLRenderPipelineState> path_render_pipeline_state_;
    id<MTLCommandQueue> command_queue_;
    id<MTLCommandBuffer> command_buffer_;
    MTKView* view_;
    double scale_;
    uint32_t width_;
    uint32_t height_;
    std::unique_ptr<ultralight::GPUDriverMetal> driver_;
};
    
}  // namespace ultralight

