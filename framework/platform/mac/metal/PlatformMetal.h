#pragma once
#include <MetalKit/MetalKit.h>

namespace framework {
    
class PlatformGPUContext {
public:
    virtual ~PlatformGPUContext() {}
    
    // Platform-specific stuff
    virtual id<MTLDevice> device() = 0;
    virtual id<MTLRenderPipelineState> render_pipeline_state() = 0;
    virtual id<MTLCommandBuffer> command_buffer() = 0;
    virtual MTKView* view() = 0;
    
    // Scale is calculated from monitor DPI, see Application::SetScale
    virtual void set_scale(double scale) = 0;
    virtual double scale() const = 0;
    
    // This is in virtual units, not actual pixels.
    virtual void set_screen_size(uint32_t width, uint32_t height) = 0;
    virtual void screen_size(uint32_t& width, uint32_t& height) = 0;
};
    
struct PlatformWindowHandle {
    MTKView* view;
};

struct PlatformWindowConfig {
    MTKView* view;
};
    
}  // namespace framework
