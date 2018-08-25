#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <Ultralight/platform/GPUDriver.h>
#include <map>
#include <vector>
#include <memory>
#include "PlatformD3D11.h"

using Microsoft::WRL::ComPtr;

namespace ultralight {

/**
 * GPUDriver implementation for Direct3D 11.
 */
class GPUDriverD3D11 : public GPUDriver {
public:
  GPUDriverD3D11(framework::PlatformGPUContextD3D11* context);
  virtual ~GPUDriverD3D11();

  virtual void BeginSynchronize() override;

  virtual void EndSynchronize() override;

  virtual uint32_t NextTextureId() override;

  virtual void CreateTexture(uint32_t texture_id,
    Ref<Bitmap> bitmap) override;

  virtual void UpdateTexture(uint32_t texture_id,
    Ref<Bitmap> bitmap) override;

  virtual void BindTexture(uint8_t texture_unit,
    uint32_t texture_id) override;

  virtual void DestroyTexture(uint32_t texture_id) override;

  virtual uint32_t NextRenderBufferId() override;

  virtual void CreateRenderBuffer(uint32_t render_buffer_id,
    const RenderBuffer& buffer) override;

  virtual void BindRenderBuffer(uint32_t render_buffer_id) override;

  virtual void SetRenderBufferViewport(uint32_t render_buffer_id,
    uint32_t width,
    uint32_t height) override;

  virtual void ClearRenderBuffer(uint32_t render_buffer_id) override;

  virtual void DestroyRenderBuffer(uint32_t render_buffer_id) override;

  virtual uint32_t NextGeometryId() override;

  virtual void CreateGeometry(uint32_t geometry_id,
    const VertexBuffer& vertices,
    const IndexBuffer& indices) override;

  virtual void UpdateGeometry(uint32_t geometry_id,
    const VertexBuffer& vertices,
    const IndexBuffer& indices) override;

  virtual void DrawGeometry(uint32_t geometry_id,
    uint32_t indices_count,
    uint32_t indices_offset,
    const GPUState& state) override;

  virtual void DestroyGeometry(uint32_t geometry_id) override;

  virtual void UpdateCommandList(const CommandList& list) override;

  virtual bool HasCommandsPending() override { return !command_list_.empty(); }

  virtual void DrawCommandList() override;

  // Public Methods

  int batch_count() const { return batch_count_; }

protected:
  ComPtr<ID3D11PixelShader> GetShader(uint8_t shader);
  ComPtr<ID3D11InputLayout> GetVertexLayout();
  ComPtr<ID3D11SamplerState> GetSamplerState();
  ComPtr<ID3D11Buffer> GetConstantBuffer();
  void UpdateConstantBuffer(const GPUState& state);

  framework::PlatformGPUContextD3D11* context_;
  ComPtr<ID3D11VertexShader> vertex_shader_;
  ComPtr<ID3D11InputLayout> vertex_layout_;
  ComPtr<ID3D11SamplerState> sampler_state_;
  ComPtr<ID3D11Buffer> constant_buffer_;

  uint32_t next_texture_id_ = 1;
  uint32_t next_render_buffer_id_ = 1; // render buffer id 0 is reserved for default render target view.
  uint32_t next_geometry_id_ = 1;

  uint32_t render_buffer_width_ = 0;
  uint32_t render_buffer_height_ = 0;

  typedef std::pair<ComPtr<ID3D11Buffer>, ComPtr<ID3D11Buffer>> GeometryEntry;
  typedef std::map<uint32_t, GeometryEntry> GeometryMap;
  GeometryMap geometry_;

  typedef std::pair<ComPtr<ID3D11Texture2D>, ComPtr<ID3D11ShaderResourceView>> TextureEntry;
  typedef std::map<uint32_t, TextureEntry> TextureMap;
  TextureMap textures_;

  struct RenderTarget { ComPtr<ID3D11RenderTargetView> rt_view; uint32_t width; uint32_t height; };
  typedef std::map<uint32_t, RenderTarget> RenderTargetMap;
  RenderTargetMap render_targets_;

  typedef std::map<ShaderType, ComPtr<ID3D11PixelShader>> ShaderMap;
  ShaderMap shaders_;

  std::vector<Command> command_list_;
  int batch_count_;
};

}  // namespace ultralight
