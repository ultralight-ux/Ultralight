#import "GPUDriverMetal.h"
#import "GPUContextMetal.h"
#include "../../../shaders/metal/ShaderTypes.h"

namespace ultralight {

GPUDriverMetal::GPUDriverMetal(GPUContextMetal* context) : context_(context) {
    gpu_frame_id_ = 0;
}

GPUDriverMetal::~GPUDriverMetal() {}

void GPUDriverMetal::BeginSynchronize() {
}

void GPUDriverMetal::EndSynchronize() {}

// Textures

void GPUDriverMetal::CreateTexture(uint32_t texture_id,
                                   Ref<Bitmap> bitmap) {
    if (bitmap->format() != kBitmapFormat_RGBA8) {
        NSLog(@"Failed to create texture, unhandled bitmap format.");
        return;
    }
    
    MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor
                                               texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                               width: bitmap->width()
                                               height: bitmap->height()
                                               mipmapped: NO];
    textureDescriptor.usage = bitmap->IsEmpty()? MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead : MTLTextureUsageShaderRead;
    auto& texture_entry = textures_[texture_id];
    auto& texture = texture_entry.current().texture;
    texture = [context_->device() newTextureWithDescriptor:textureDescriptor];
    
    if (!bitmap->IsEmpty()) {
        void* pixels = bitmap->LockPixels();
        [texture replaceRegion: { { 0, 0, 0 }, {bitmap->width(), bitmap->height(), 1} }
                   mipmapLevel:0
                     withBytes:pixels
                   bytesPerRow:bitmap->row_bytes()];
        bitmap->UnlockPixels();
    }
}

void GPUDriverMetal::UpdateTexture(uint32_t texture_id,
                                   Ref<Bitmap> bitmap) {
    auto i = textures_.find(texture_id);
    if (i == textures_.end()) {
        NSLog(@"Texture ID doesn't exist.");
        return;
    }
    
    auto& texture_entry = i->second;
    
    // GPU is running behind, overflowing our ring buffer, wait a bit
    while (texture_entry.current().owning_frame_id_ && (texture_entry.current().owning_frame_id_ - gpu_frame_id_.load() >= (int64_t)RingBufferSize))
        usleep(1000);
    
    if (texture_entry.current().owning_frame_id_ > gpu_frame_id_.load())
        texture_entry.iterate();
    
    auto& texture = texture_entry.current().texture;
    
    if (!texture) {
        MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor
                                                   texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                                   width: bitmap->width()
                                                   height: bitmap->height()
                                                   mipmapped: NO];
        textureDescriptor.usage = bitmap->IsEmpty()? MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead : MTLTextureUsageShaderRead;
        textures_[texture_id].current().texture = [context_->device() newTextureWithDescriptor:textureDescriptor];
    }
    
    if (!bitmap->IsEmpty()) {
        void* pixels = bitmap->LockPixels();
        [texture replaceRegion: { { 0, 0, 0 }, {bitmap->width(), bitmap->height(), 1} }
                   mipmapLevel:0
                     withBytes:pixels
                   bytesPerRow:bitmap->row_bytes()];
        bitmap->UnlockPixels();
    }
}

void GPUDriverMetal::BindTexture(uint8_t texture_unit,
                                 uint32_t texture_id) {
    auto i = textures_.find(texture_id);
    if (i == textures_.end()) {
        NSLog(@"Texture ID %d doesn't exist.", texture_id);
        return;
    }
    
    auto& texture = i->second.current();
    
    texture.owning_frame_id_ = cpu_frame_id_;
    
    if (render_encoder_)
        [render_encoder_ setFragmentTexture:texture.texture
                                    atIndex:texture_unit];
}

void GPUDriverMetal::DestroyTexture(uint32_t texture_id) {
    auto i = textures_.find(texture_id);
    if (i != textures_.end()) {
        textures_.erase(i);
    }
}

// Offscreen Rendering

void GPUDriverMetal::CreateRenderBuffer(uint32_t render_buffer_id,
                                        const RenderBuffer& buffer) {
    render_buffers_[render_buffer_id] = buffer;
}

void GPUDriverMetal::BindRenderBuffer(uint32_t render_buffer_id) {
    
    if (render_encoder_ && render_encoder_render_buffer_id_ == render_buffer_id)
        return;
    
    id<MTLTexture> texture;
    MTLClearColor clearColor;
    
    if (render_buffer_id) {
        auto i = render_buffers_.find(render_buffer_id);
        if (i == render_buffers_.end())
            return;
        
        auto renderBuffer = i->second;
        auto j = textures_.find(renderBuffer.texture_id);
        if (j == textures_.end())
            return;
        
        texture = j->second.current().texture;
        
        clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 0.0);
    } else {
        texture = context_->view().currentDrawable.texture;
        uint32_t screen_width, screen_height;
        context_->screen_size(screen_width, screen_height);
        
        clearColor = MTLClearColorMake(1.0, 1.0, 1.0, 1.0);
    }
    
    if (render_encoder_) {
        [render_encoder_ endEncoding];
        render_encoder_ = nullptr;
    }
    
    auto renderPassDescriptor = [MTLRenderPassDescriptor new];
    renderPassDescriptor.colorAttachments[ 0 ].loadAction = (needs_render_buffer_clear_ && render_buffer_clear_id_ == render_buffer_id)? MTLLoadActionClear : MTLLoadActionLoad;
    renderPassDescriptor.colorAttachments[ 0 ].clearColor = clearColor;
    renderPassDescriptor.colorAttachments[ 0 ].storeAction = MTLStoreActionStore;
    renderPassDescriptor.colorAttachments[ 0 ].texture = texture;
    
    needs_render_buffer_clear_ = false;
    
    render_encoder_ =
    [context_->command_buffer() renderCommandEncoderWithDescriptor:renderPassDescriptor];
    render_encoder_render_buffer_id_ = render_buffer_id;
    render_encoder_.label = @"UltralightRenderEncoder";
}

void GPUDriverMetal::ClearRenderBuffer(uint32_t render_buffer_id) {
    needs_render_buffer_clear_ = true;
    render_buffer_clear_id_ = render_buffer_id;
}

void GPUDriverMetal::DestroyRenderBuffer(uint32_t render_buffer_id) {
    auto i = render_buffers_.find(render_buffer_id);
    if (i != render_buffers_.end()) {
        render_buffers_.erase(i);
    }
}

// Geometry

void GPUDriverMetal::CreateGeometry(uint32_t geometry_id,
                                    const VertexBuffer& vertices,
                                    const IndexBuffer& indices) {
    auto& geometry_entry = geometry_[geometry_id];
    auto& buffer = geometry_entry.current();
    auto& vertex_buffer = buffer.vertex_buffer_;
    auto& index_buffer = buffer.index_buffer_;

    static_assert(sizeof(ultralight::Vertex_2f_4ub_2f_2f_28f) == sizeof(Vertex), "must be equal");
    
    vertex_buffer = [context_->device() newBufferWithLength:vertices.size
                                                    options:MTLResourceStorageModeManaged];
    memcpy([vertex_buffer contents], vertices.data, vertices.size);
    [vertex_buffer didModifyRange:NSMakeRange(0, vertices.size)];
    
    index_buffer = [context_->device() newBufferWithLength:indices.size
                                                   options:MTLResourceStorageModeManaged];
    memcpy([index_buffer contents], indices.data, indices.size);
    [index_buffer didModifyRange:NSMakeRange(0, indices.size)];
}

void GPUDriverMetal::UpdateGeometry(uint32_t geometry_id,
                                    const VertexBuffer& vertices,
                                    const IndexBuffer& indices) {
    auto i = geometry_.find(geometry_id);
    if (i == geometry_.end()) {
        NSLog(@"Geometry ID doesn't exist.");
        return;
    }
    
    auto& geometry_entry = i->second;
    
    // GPU is running behind, overflowing our ring buffer, wait a bit
    while (geometry_entry.current().owning_frame_id_ && (geometry_entry.current().owning_frame_id_ - gpu_frame_id_.load() >= (int64_t)RingBufferSize))
        usleep(1000);
    
    if (geometry_entry.current().owning_frame_id_ > gpu_frame_id_.load())
        geometry_entry.iterate();
    
    auto& buffer = geometry_entry.current();
    auto& vertex_buffer = buffer.vertex_buffer_;
    auto& index_buffer = buffer.index_buffer_;
    
    if (!vertex_buffer) {
        vertex_buffer = [context_->device() newBufferWithLength:vertices.size
                                                        options:MTLResourceStorageModeManaged];
        index_buffer = [context_->device() newBufferWithLength:indices.size
                                                       options:MTLResourceStorageModeManaged];
    }
    
    memcpy([vertex_buffer contents], vertices.data, vertices.size);
    [vertex_buffer didModifyRange:NSMakeRange(0, vertices.size)];
    
    memcpy([index_buffer contents], indices.data, indices.size);
    [index_buffer didModifyRange:NSMakeRange(0, indices.size)];
}

simd_float4 ToFloat4(const ultralight::vec4& v) {
    return { v.x, v.y, v.z, v.w };
}

simd_float4x4 ToFloat4x4(const ultralight::Matrix4x4& m) {
    simd_float4 X = { m.data[0], m.data[1], m.data[2], m.data[3] };
    simd_float4 Y = { m.data[4], m.data[5], m.data[6], m.data[7] };
    simd_float4 Z = { m.data[8], m.data[9], m.data[10], m.data[11] };
    simd_float4 W = { m.data[12], m.data[13], m.data[14], m.data[15] };
    return { X, Y, Z, W };
}

void GPUDriverMetal::DrawGeometry(uint32_t geometry_id,
                                  uint32_t indices_count,
                                  uint32_t indices_offset,
                                  const GPUState& state) {
    auto i = geometry_.find(geometry_id);
    if (i == geometry_.end()) {
        NSLog(@"In DrawGeometry, geometry id: %u does not exist.", geometry_id);
        return;
    }
    
    BindRenderBuffer(state.render_buffer_id);
    
    // Set the region of the drawable to which we'll draw.
    [render_encoder_ setViewport:(MTLViewport){0.0, 0.0, state.viewport_width * context_->scale(), state.viewport_height * context_->scale(), -1.0, 1.0 }];
    
    if (state.shader_type == kShaderType_Fill)
        [render_encoder_ setRenderPipelineState:context_->render_pipeline_state()];
    else
        [render_encoder_ setRenderPipelineState:context_->path_render_pipeline_state()];
    
    if (state.texture_1_id)
        BindTexture(0, state.texture_1_id);
    
    if (state.texture_2_id)
        BindTexture(1, state.texture_2_id);
    
    SetGPUState(state);
    
    auto& geometry_entry = i->second;
    auto& buffer = geometry_entry.current();
    auto& vertex_buffer = buffer.vertex_buffer_;
    auto& index_buffer = buffer.index_buffer_;
    
    buffer.owning_frame_id_ = cpu_frame_id_;
    
    [render_encoder_ setVertexBuffer:vertex_buffer
                              offset:0
                             atIndex:VertexIndex_Vertices];
    
    [render_encoder_ drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                indexCount:indices_count
                                 indexType:MTLIndexTypeUInt32
                               indexBuffer:index_buffer
                         indexBufferOffset:indices_offset*sizeof(uint32_t)];
}


void GPUDriverMetal::DestroyGeometry(uint32_t geometry_id) {}

// Command Queue

void GPUDriverMetal::UpdateCommandList(const CommandList& list) {
    if (list.size) {
        command_list_.resize(list.size);
        memcpy(&command_list_[0], list.commands, sizeof(Command) * list.size);
    }
}

bool GPUDriverMetal::HasCommandsPending() { return !command_list_.empty(); }

void GPUDriverMetal::DrawCommandList() {
    if (command_list_.empty())
        return;
    
    for (auto& cmd : command_list_) {
        if ( cmd.command_type == kCommandType_DrawGeometry)
            DrawGeometry(cmd.geometry_id, cmd.indices_count, cmd.indices_offset, cmd.gpu_state);
        else if (cmd.command_type == kCommandType_ClearRenderBuffer)
            ClearRenderBuffer(cmd.gpu_state.render_buffer_id);
    }
    
    std::atomic<int64_t>& block_gpu_frame_id = gpu_frame_id_;
    [context_->command_buffer() addCompletedHandler:^(id<MTLCommandBuffer> buffer)
     {
         block_gpu_frame_id++;
     }];

    cpu_frame_id_++;
    
    command_list_.clear();
}

void GPUDriverMetal::EndDrawing() {
    if (render_encoder_) {
        [render_encoder_ endEncoding];
        render_encoder_ = nullptr;
    }
}

void GPUDriverMetal::SetGPUState(const GPUState& state) {
    uint32_t screen_width, screen_height;
    context_->screen_size(screen_width, screen_height);
    Uniforms uniforms;
    uniforms.State = { 0.0f, (float)state.viewport_width, (float)state.viewport_height, (float)context_->scale() };
    uniforms.Transform = ToFloat4x4(state.transform);
    uniforms.Scalar4[0] = { state.uniform_scalar[0], state.uniform_scalar[1], state.uniform_scalar[2], state.uniform_scalar[3] };
    uniforms.Scalar4[1] = { state.uniform_scalar[4], state.uniform_scalar[5], state.uniform_scalar[6], state.uniform_scalar[7] };
    uniforms.Vector[0] = ToFloat4(state.uniform_vector[0]);
    uniforms.Vector[1] = ToFloat4(state.uniform_vector[1]);
    uniforms.Vector[2] = ToFloat4(state.uniform_vector[2]);
    uniforms.Vector[3] = ToFloat4(state.uniform_vector[3]);
    uniforms.Vector[4] = ToFloat4(state.uniform_vector[4]);
    uniforms.Vector[5] = ToFloat4(state.uniform_vector[5]);
    uniforms.Vector[6] = ToFloat4(state.uniform_vector[6]);
    uniforms.Vector[7] = ToFloat4(state.uniform_vector[7]);
    uniforms.ClipSize = state.clip_size;
    for (size_t i = 0; i < (size_t)state.clip_size; i++)
        uniforms.Clip[i] = ToFloat4x4(state.clip[i]);
    
    [render_encoder_ setVertexBytes:&uniforms
                             length:sizeof(Uniforms)
                            atIndex:VertexIndex_Uniforms];
    
    [render_encoder_ setFragmentBytes:&uniforms
                             length:sizeof(Uniforms)
                            atIndex:FragmentIndex_Uniforms];
}
    
} // namespace ultralight
