#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <Ultralight/platform/GPUDriver.h>
#include <map>
#include <vector>
#include <memory>

using Microsoft::WRL::ComPtr;

namespace ultralight {

class GPUContextD3D11;

/**
 * GPUDriver implementation for Direct3D 11.
 */
class GPUDriverD3D11 : public GPUDriver {
public:
  GPUDriverD3D11(GPUContextD3D11* context);
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
  void LoadVertexShader(const WCHAR* path, ID3D11VertexShader** ppVertexShader,
    const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, ID3D11InputLayout** ppInputLayout);
  void LoadPixelShader(const WCHAR* path, ID3D11PixelShader** ppPixelShader);
  void LoadCompiledVertexShader(unsigned char* data, unsigned int len, ID3D11VertexShader** ppVertexShader,
    const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, ID3D11InputLayout** ppInputLayout);
  void LoadCompiledPixelShader(unsigned char* data, unsigned int len, ID3D11PixelShader** ppPixelShader);
  void LoadShaders();
  void BindShader(uint8_t shader);
  void BindVertexLayout(VertexBufferFormat format);
  void BindGeometry(uint32_t id);
  ComPtr<ID3D11SamplerState> GetSamplerState();
  ComPtr<ID3D11Buffer> GetConstantBuffer();
  void SetViewport(float width, float height);
  void UpdateConstantBuffer(const GPUState& state);

  GPUContextD3D11* context_;
  ComPtr<ID3D11InputLayout> vertex_layout_2f_4ub_2f_;
  ComPtr<ID3D11InputLayout> vertex_layout_2f_4ub_2f_2f_28f_;
  ComPtr<ID3D11SamplerState> sampler_state_;
  ComPtr<ID3D11Buffer> constant_buffer_;

  uint32_t next_texture_id_ = 1;
  uint32_t next_render_buffer_id_ = 1; // render buffer id 0 is reserved for default render target view.
  uint32_t next_geometry_id_ = 1;

  struct GeometryEntry { VertexBufferFormat format; ComPtr<ID3D11Buffer> vertexBuffer; ComPtr<ID3D11Buffer> indexBuffer; };
  typedef std::map<uint32_t, GeometryEntry> GeometryMap;
  GeometryMap geometry_;

  typedef std::pair<ComPtr<ID3D11Texture2D>, ComPtr<ID3D11ShaderResourceView>> TextureEntry;
  typedef std::map<uint32_t, TextureEntry> TextureMap;
  TextureMap textures_;

  typedef std::map<uint32_t, ComPtr<ID3D11RenderTargetView>> RenderTargetMap;
  RenderTargetMap render_targets_;

  typedef std::map<ShaderType, std::pair<ComPtr<ID3D11VertexShader>, ComPtr<ID3D11PixelShader>>> ShaderMap;
  ShaderMap shaders_;

  std::vector<Command> command_list_;
  int batch_count_;
};

}  // namespace ultralight
