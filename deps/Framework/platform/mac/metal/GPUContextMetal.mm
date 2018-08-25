#import "../../../GPUContext.h"
#import "PlatformMetal.h"
#import "GPUDriverMetal.h"
#include <memory>

namespace framework {
    
class GPUContextMetal : public GPUContext,
                        public PlatformGPUContext {
public:
    GPUContextMetal() {}
    virtual ~GPUContextMetal() {}

    // Inherited from GPUContext
    virtual ultralight::GPUDriver* driver() const { return driver_.get(); }
    virtual ultralight::FaceWinding face_winding() const {
        return ultralight::kFaceWinding_CounterClockwise;
    }
    virtual void BeginDrawing() {
        MTLCaptureManager *sharedCaptureManager = [MTLCaptureManager sharedCaptureManager];
        
        [sharedCaptureManager.defaultCaptureScope beginScope];
        
        // Create a new command buffer for each render pass to the current drawable
        command_buffer_ = [command_queue_ commandBuffer];
        command_buffer_.label = @"UltralightCommandBuffer";
    }
                            
    virtual void EndDrawing() {
        if (!command_buffer_)
            return;
        
        driver_->EndDrawing();
        
        [command_buffer_ commit];
        command_buffer_ = nullptr;
        
        MTLCaptureManager *sharedCaptureManager = [MTLCaptureManager sharedCaptureManager];
        [sharedCaptureManager.defaultCaptureScope endScope];
    }
                            
    virtual void PresentFrame() {
        if (command_buffer_)
            [command_buffer_ presentDrawable:view_.currentDrawable];
    }
                            
    virtual void Resize(int width, int height) {
        // Not implemented yet
    }
           
    // Inherited from PlatformGPUContext
    virtual id<MTLDevice> device() { return device_; }
    virtual id<MTLRenderPipelineState> render_pipeline_state() { return render_pipeline_state_; }
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
                            
    bool Initialize(MTKView* view, int screen_width, int screen_height, double screen_scale, bool fullscreen, bool enable_vsync) {
        device_ = view.device;
        view_ = view;
        
        set_screen_size(screen_width, screen_height);
        set_scale(screen_scale);
        
        NSError *error = NULL;
        
        // Load all the shader files with a .metal file extension in the project
        id<MTLLibrary> defaultLibrary = [device_ newDefaultLibrary];
        
        // Load the vertex function from the library
        id<MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:@"vertexShader"];
        
        // Load the fragment function from the library
        id<MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:@"fragmentShader"];
        
        // Configure a pipeline descriptor that is used to create a pipeline state
        MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineStateDescriptor.label = @"Simple Pipeline";
        pipelineStateDescriptor.vertexFunction = vertexFunction;
        pipelineStateDescriptor.fragmentFunction = fragmentFunction;
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
            NSLog(@"Failed to created pipeline state, error %@", error);
            return false;
        }
        
        // Create the command queue
        command_queue_ = [device_ newCommandQueue];
        
        MTLCaptureManager *sharedCaptureManager = [MTLCaptureManager sharedCaptureManager];
        id<MTLCaptureScope> myCaptureScope = [sharedCaptureManager newCaptureScopeWithDevice:device_];
        myCaptureScope.label = @"My Capture Scope";
        sharedCaptureManager.defaultCaptureScope = myCaptureScope;
        
        driver_.reset(new ultralight::GPUDriverMetal(this));
        return true;
    }
                            
protected:
    id<MTLDevice> device_;
    id<MTLRenderPipelineState> render_pipeline_state_;
    id<MTLCommandQueue> command_queue_;
    id<MTLCommandBuffer> command_buffer_;
    MTKView* view_;
    double scale_;
    uint32_t width_;
    uint32_t height_;
    std::unique_ptr<ultralight::GPUDriverMetal> driver_;
};
    
std::unique_ptr<GPUContext> GPUContext::Create(const Window& window, bool enable_vsync) {
    auto context = std::make_unique<GPUContextMetal>();
    if (context->Initialize(window.handle().view, window.width(), window.height(), window.scale(), window.is_fullscreen(), enable_vsync))
        return std::move(context);
    
    return nullptr;
}
    
}  // namespace framework
