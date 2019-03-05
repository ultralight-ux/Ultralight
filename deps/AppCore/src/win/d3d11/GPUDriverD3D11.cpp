#include "GPUDriverD3D11.h"
#include "GPUContextD3D11.h"
#include <directxcolors.h>
#include <d3dcompiler.h>
#include <string>
#include <sstream>
#include "../../../shaders/hlsl/fill_fxc.h"
#include "../../../shaders/hlsl/fill_path_fxc.h"
#include "../../../shaders/hlsl/v2f_c4f_t2f_fxc.h"
#include "../../../shaders/hlsl/v2f_c4f_t2f_t2f_d28f_fxc.h"

#define SHADER_PATH L".\\shaders\\hlsl\\"

namespace {

struct Vertex_2f_4ub_2f {
  DirectX::XMFLOAT2 pos;
  uint8_t color[4];
  DirectX::XMFLOAT2 obj;
};

struct Vertex_2f_4ub_2f_2f_28f {
  DirectX::XMFLOAT2 pos;
  uint8_t color[4];
  DirectX::XMFLOAT2 tex;
  DirectX::XMFLOAT2 obj;
  DirectX::XMFLOAT4 data_0;
  DirectX::XMFLOAT4 data_1;
  DirectX::XMFLOAT4 data_2;
  DirectX::XMFLOAT4 data_3;
  DirectX::XMFLOAT4 data_4;
  DirectX::XMFLOAT4 data_5;
  DirectX::XMFLOAT4 data_6;
};

struct Uniforms {
  DirectX::XMFLOAT4 State;
  DirectX::XMMATRIX Transform;
  DirectX::XMFLOAT4 Scalar4[2];
  DirectX::XMFLOAT4 Vector[8];
  uint32_t ClipSize;
  DirectX::XMMATRIX Clip[8];
};

HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint,
  LPCSTR szShaderModel, ID3DBlob** ppBlobOut) {
  DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
  dwShaderFlags |= D3DCOMPILE_DEBUG;
  dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
  dwShaderFlags |= D3DCOMPILE_PARTIAL_PRECISION;

  // Note that this may cause slow Application startup because the Shader Compiler
  // must perform heavy optimizations. In a production build you should use D3D's
  // HLSL Effect Compiler (fxc.exe) to compile the HLSL files offline which grants
  // near-instantaneous load times.
  dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL2;
#endif

  ComPtr<ID3DBlob> pErrorBlob;
  std::wstring path = SHADER_PATH;
  path += szFileName;
  HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, nullptr, szEntryPoint,
    szShaderModel, dwShaderFlags, 0, ppBlobOut, pErrorBlob.GetAddressOf());

  if (FAILED(hr) && pErrorBlob)
    OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));

  return hr;
}

} // namespace (unnamed)

namespace ultralight {

GPUDriverD3D11::GPUDriverD3D11(GPUContextD3D11* context) : context_(context) {
}

GPUDriverD3D11::~GPUDriverD3D11() {
}

void GPUDriverD3D11::BeginSynchronize() {
}

void GPUDriverD3D11::EndSynchronize() {
}

uint32_t GPUDriverD3D11::NextTextureId() { return next_texture_id_++; }

void GPUDriverD3D11::CreateTexture(uint32_t texture_id,
  Ref<Bitmap> bitmap) {
  auto i = textures_.find(texture_id);
  if (i != textures_.end()) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::CreateTexture, texture id already exists.", L"Error", MB_OK);
    return;
  }

  if (bitmap->format() != kBitmapFormat_RGBA8) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::CreateTexture, unsupported format.", L"Error", MB_OK);
  }

  D3D11_TEXTURE2D_DESC desc;
  ZeroMemory(&desc, sizeof(desc));
  desc.Width = bitmap->width();
  desc.Height = bitmap->height();
  desc.MipLevels = desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DYNAMIC;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  desc.MiscFlags = 0;

  auto& texture_entry = textures_[texture_id];
  HRESULT hr;

  if (bitmap->IsEmpty()) {
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;

    hr = context_->device()->CreateTexture2D(
      &desc, NULL, texture_entry.first.GetAddressOf());
  } else {
    D3D11_SUBRESOURCE_DATA tex_data;
    ZeroMemory(&tex_data, sizeof(tex_data));
    tex_data.pSysMem = bitmap->LockPixels();
    tex_data.SysMemPitch = bitmap->row_bytes();
    tex_data.SysMemSlicePitch = (UINT)bitmap->size();

    hr = context_->device()->CreateTexture2D(
      &desc, &tex_data, texture_entry.first.GetAddressOf());
    bitmap->UnlockPixels();
  }

  if (FAILED(hr)) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::CreateTexture, unable to create texture.", L"Error", MB_OK);
  }

  D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
  ZeroMemory(&srv_desc, sizeof(srv_desc));
  srv_desc.Format = desc.Format;
  srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srv_desc.Texture2D.MostDetailedMip = 0;
  srv_desc.Texture2D.MipLevels = 1;

  hr = context_->device()->CreateShaderResourceView(
    texture_entry.first.Get(), &srv_desc, texture_entry.second.GetAddressOf());
}

void GPUDriverD3D11::UpdateTexture(uint32_t texture_id,
  Ref<Bitmap> bitmap) {
  auto i = textures_.find(texture_id);
  if (i == textures_.end()) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::UpdateTexture, texture id doesn't exist.", L"Error", MB_OK);
    return;
  }

  auto& entry = i->second;
  D3D11_MAPPED_SUBRESOURCE res;
  context_->immediate_context()->Map(entry.first.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0,
    &res);

  if (res.RowPitch == bitmap->row_bytes()) {
    memcpy(res.pData, bitmap->LockPixels(), bitmap->size());
    bitmap->UnlockPixels();
  }
  else {
    Ref<Bitmap> mapped_bitmap = Bitmap::Create(bitmap->width(), bitmap->height(), kBitmapFormat_RGBA8,
      res.RowPitch, res.pData, res.RowPitch * bitmap->height(), false);
    IntRect dest_rect = { 0, 0, (int)bitmap->width(), (int)bitmap->height() };
    mapped_bitmap->DrawBitmap(dest_rect, dest_rect, bitmap, false);
  }

  context_->immediate_context()->Unmap(entry.first.Get(), 0);
}

void GPUDriverD3D11::BindTexture(uint8_t texture_unit,
  uint32_t texture_id) {
  auto i = textures_.find(texture_id);
  if (i == textures_.end()) {
    //MessageBoxW(nullptr,
    //  L"GPUDriverD3D11::BindTexture, texture id doesn't exist.", L"Error", MB_OK);
    return;
  }

  auto& entry = i->second;
  context_->immediate_context()->PSSetShaderResources(texture_unit, 1, entry.second.GetAddressOf());
}

void GPUDriverD3D11::DestroyTexture(uint32_t texture_id) {
  auto i = textures_.find(texture_id);
  if (i != textures_.end()) {
    i->second.first.Reset();
    i->second.second.Reset();

    textures_.erase(i);
  }
}

uint32_t GPUDriverD3D11::NextRenderBufferId() { return next_render_buffer_id_++; }

void GPUDriverD3D11::CreateRenderBuffer(uint32_t render_buffer_id,
  const RenderBuffer& buffer) {
  if (render_buffer_id == 0) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::CreateRenderBuffer, render buffer ID 0 is reserved for default render target view.", L"Error", MB_OK);
    return;
  }

  auto i = render_targets_.find(render_buffer_id);
  if (i != render_targets_.end()) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::CreateRenderBuffer, render buffer id already exists.", L"Error", MB_OK);
    return;
  }

  auto tex_entry = textures_.find(buffer.texture_id);
  if (tex_entry == textures_.end()) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::CreateRenderBuffer, texture id doesn't exist.", L"Error", MB_OK);
    return;
  }

  D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
  ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
  renderTargetViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

  ComPtr<ID3D11Texture2D> tex = tex_entry->second.first;
  auto& render_target_entry = render_targets_[render_buffer_id];
  HRESULT hr = context_->device()->CreateRenderTargetView(tex.Get(), &renderTargetViewDesc,
    render_target_entry.GetAddressOf());

  if (FAILED(hr)) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::CreateRenderBuffer, unable to create render target.", L"Error", MB_OK);
  }
}

void GPUDriverD3D11::BindRenderBuffer(uint32_t render_buffer_id) {
  // Unbind any textures/shader resources to avoid warnings in case a render
  // buffer that we would like to bind is already bound as an input texture.
  ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
  context_->immediate_context()->PSSetShaderResources(0, 1, nullSRV);
  context_->immediate_context()->PSSetShaderResources(1, 1, nullSRV);
  context_->immediate_context()->PSSetShaderResources(2, 1, nullSRV);
  
  ID3D11RenderTargetView* target;
  if (render_buffer_id == 0) {
    target = context_->render_target_view();
  } else {
    auto i = render_targets_.find(render_buffer_id);
    if (i == render_targets_.end()) {
      MessageBoxW(nullptr,
        L"GPUDriverD3D11::BindRenderBuffer, render buffer id doesn't exist.", L"Error", MB_OK);
      return;
    }

    target = i->second.Get();
  }

  context_->immediate_context()->OMSetRenderTargets(1, &target, nullptr);
}

void GPUDriverD3D11::ClearRenderBuffer(uint32_t render_buffer_id) {
  float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

  if (render_buffer_id == 0) {
    context_->immediate_context()->ClearRenderTargetView(context_->render_target_view(), color);
    return;
  }

  auto i = render_targets_.find(render_buffer_id);
  if (i == render_targets_.end()) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::ClearRenderBuffer, render buffer id doesn't exist.", L"Error", MB_OK);
    return;
  }

  context_->immediate_context()->ClearRenderTargetView(i->second.Get(), color);
}

void GPUDriverD3D11::DestroyRenderBuffer(uint32_t render_buffer_id) {
  auto i = render_targets_.find(render_buffer_id);
  if (i != render_targets_.end()) {
    i->second.Reset();
    render_targets_.erase(i);
  }
}

uint32_t GPUDriverD3D11::NextGeometryId() { return next_geometry_id_++; }

void GPUDriverD3D11::CreateGeometry(uint32_t geometry_id,
  const VertexBuffer& vertices,
  const IndexBuffer& indices) {
  BindVertexLayout(vertices.format);

  if (geometry_.find(geometry_id) != geometry_.end())
    return;

  GeometryEntry geometry;
  geometry.format = vertices.format;

  HRESULT hr;

  D3D11_BUFFER_DESC vertex_desc;
  ZeroMemory(&vertex_desc, sizeof(vertex_desc));
  vertex_desc.Usage = D3D11_USAGE_DYNAMIC;
  vertex_desc.ByteWidth = vertices.size;
  vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  D3D11_SUBRESOURCE_DATA vertex_data;
  ZeroMemory(&vertex_data, sizeof(vertex_data));
  vertex_data.pSysMem = vertices.data;

  hr = context_->device()->CreateBuffer(&vertex_desc, &vertex_data, 
    geometry.vertexBuffer.GetAddressOf());
  if (FAILED(hr))
    return;

  D3D11_BUFFER_DESC index_desc;
  ZeroMemory(&index_desc, sizeof(index_desc));
  index_desc.Usage = D3D11_USAGE_DYNAMIC;
  index_desc.ByteWidth = indices.size;
  index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  index_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  D3D11_SUBRESOURCE_DATA index_data;
  ZeroMemory(&index_data, sizeof(index_data));
  index_data.pSysMem = indices.data;

  hr = context_->device()->CreateBuffer(&index_desc, &index_data, 
    geometry.indexBuffer.GetAddressOf());
  if (FAILED(hr))
    return;

  geometry_.insert({ geometry_id, std::move(geometry) });
}

void GPUDriverD3D11::UpdateGeometry(uint32_t geometry_id,
  const VertexBuffer& vertices,
  const IndexBuffer& indices) {
  auto i = geometry_.find(geometry_id);
  if (i == geometry_.end()) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::UpdateGeometry, geometry id doesn't exist.", L"Error", MB_OK);
    return;
  }

  auto& entry = i->second;
  D3D11_MAPPED_SUBRESOURCE res;

  context_->immediate_context()->Map(entry.vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
  memcpy(res.pData, vertices.data, vertices.size);
  context_->immediate_context()->Unmap(entry.vertexBuffer.Get(), 0);

  context_->immediate_context()->Map(entry.indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
  memcpy(res.pData, indices.data, indices.size);
  context_->immediate_context()->Unmap(entry.indexBuffer.Get(), 0);
}

void GPUDriverD3D11::DrawGeometry(uint32_t geometry_id,
  uint32_t indices_count,
  uint32_t indices_offset,
  const GPUState& state) {
  BindRenderBuffer(state.render_buffer_id);

  SetViewport(state.viewport_width, state.viewport_height);

  if (state.texture_1_id)
    BindTexture(0, state.texture_1_id);

  if (state.texture_2_id)
    BindTexture(1, state.texture_2_id);

  UpdateConstantBuffer(state);

  BindGeometry(geometry_id);

  auto immediate_ctx = context_->immediate_context();

  auto sampler_state = GetSamplerState();
  immediate_ctx->PSSetSamplers(0, 1, sampler_state.GetAddressOf());

  BindShader(state.shader_type);

  immediate_ctx->VSSetConstantBuffers(0, 1, constant_buffer_.GetAddressOf());
  immediate_ctx->PSSetConstantBuffers(0, 1, constant_buffer_.GetAddressOf());
  immediate_ctx->DrawIndexed(indices_count, indices_offset, 0);
  batch_count_++;
}

void GPUDriverD3D11::DestroyGeometry(uint32_t geometry_id) {
  auto i = geometry_.find(geometry_id);
  if (i != geometry_.end()) {
    i->second.vertexBuffer.Reset();
    i->second.indexBuffer.Reset();
    geometry_.erase(i);
  }
}

void GPUDriverD3D11::UpdateCommandList(const CommandList& list) {
  if (list.size) {
    command_list_.resize(list.size);
    memcpy(&command_list_[0], list.commands, sizeof(Command) * list.size);
  }
}

void GPUDriverD3D11::DrawCommandList() {
  if (command_list_.empty())
    return;

  batch_count_ = 0;

  for (auto& cmd : command_list_) {
    if ( cmd.command_type == kCommandType_DrawGeometry)
      DrawGeometry(cmd.geometry_id, cmd.indices_count, cmd.indices_offset, cmd.gpu_state);
    else if (cmd.command_type == kCommandType_ClearRenderBuffer)
      ClearRenderBuffer(cmd.gpu_state.render_buffer_id);
  }

  command_list_.clear();
}

void GPUDriverD3D11::LoadVertexShader(const WCHAR* path, ID3D11VertexShader** ppVertexShader,
  const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, ID3D11InputLayout** ppInputLayout) {
  HRESULT hr;
  ComPtr<ID3DBlob> vs_blob;
  hr = CompileShaderFromFile(path, "VS", "vs_4_0", vs_blob.GetAddressOf());

  // Create the vertex shader
  hr = context_->device()->CreateVertexShader(vs_blob->GetBufferPointer(),
    vs_blob->GetBufferSize(), nullptr, ppVertexShader);

  if (FAILED(hr)) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::LoadVertexShader, Vertex shader could not be compiled. Check your working directory.", L"Error", MB_OK);
    return;
  }

  // Create the input layout
  hr = context_->device()->CreateInputLayout(pInputElementDescs, NumElements,
    vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), ppInputLayout);

  if (FAILED(hr)) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::LoadVertexShader, Could not create vertex input layout.", L"Error", MB_OK);
    return;
  }
}

void GPUDriverD3D11::LoadPixelShader(const WCHAR* path, ID3D11PixelShader** ppPixelShader) {
  HRESULT hr;

  ComPtr<ID3DBlob> ps_blob;
  hr = CompileShaderFromFile(path, "PS", "ps_4_0", ps_blob.GetAddressOf());

  // Create the pixel shader
  hr = context_->device()->CreatePixelShader(ps_blob->GetBufferPointer(),
    ps_blob->GetBufferSize(), nullptr, ppPixelShader);

  if (FAILED(hr)) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::LoadPixelShader, Pixel shader could not be compiled. Check your working directory.", L"Error", MB_OK);
    return;
  }
}

void GPUDriverD3D11::LoadCompiledVertexShader(unsigned char* data, unsigned int len, ID3D11VertexShader** ppVertexShader,
  const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, ID3D11InputLayout** ppInputLayout) {
  HRESULT hr;

  // Create the vertex shader
  hr = context_->device()->CreateVertexShader(data, len, nullptr, ppVertexShader);

  if (FAILED(hr)) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::LoadVertexShader, Vertex shader could not be compiled. Check your working directory.", L"Error", MB_OK);
    return;
  }

  // Create the input layout
  hr = context_->device()->CreateInputLayout(pInputElementDescs, NumElements,
    data, len, ppInputLayout);

  if (FAILED(hr)) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::LoadVertexShader, Could not create vertex input layout.", L"Error", MB_OK);
    return;
  }
}

void GPUDriverD3D11::LoadCompiledPixelShader(unsigned char* data, unsigned int len, ID3D11PixelShader** ppPixelShader) {
  HRESULT hr;

  // Create the pixel shader
  hr = context_->device()->CreatePixelShader(data, len, nullptr, ppPixelShader);

  if (FAILED(hr)) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::LoadPixelShader, Pixel shader could not be compiled. Check your working directory.", L"Error", MB_OK);
    return;
  }
}

void GPUDriverD3D11::LoadShaders() {
  if (!shaders_.empty())
    return;

  const D3D11_INPUT_ELEMENT_DESC layout_2f_4ub_2f[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UINT,      0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };

  auto& shader_fill_path = shaders_[kShaderType_FillPath];
  /*
  LoadVertexShader(L"vs\\v2f_c4f_t2f.hlsl", shader_fill_path.first.GetAddressOf(),
    layout_2f_4ub_2f, ARRAYSIZE(layout_2f_4ub_2f), vertex_layout_2f_4ub_2f_.GetAddressOf());
  LoadPixelShader(L"ps\\fill_path.hlsl", shader_fill_path.second.GetAddressOf());
  */
  LoadCompiledVertexShader(v2f_c4f_t2f_fxc, v2f_c4f_t2f_fxc_len, shader_fill_path.first.GetAddressOf(),
    layout_2f_4ub_2f, ARRAYSIZE(layout_2f_4ub_2f), vertex_layout_2f_4ub_2f_.GetAddressOf());
  LoadCompiledPixelShader(fill_path_fxc, fill_path_fxc_len, shader_fill_path.second.GetAddressOf());

  const D3D11_INPUT_ELEMENT_DESC layout_2f_4ub_2f_2f_28f[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UINT,      0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    4, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    5, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    6, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    7, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };

  auto& shader_fill = shaders_[kShaderType_Fill];
  /*
  LoadVertexShader(L"vs\\v2f_c4f_t2f_t2f_d28f.hlsl", shader_fill.first.GetAddressOf(),
    layout_2f_4ub_2f_2f_28f, ARRAYSIZE(layout_2f_4ub_2f_2f_28f), vertex_layout_2f_4ub_2f_2f_28f_.GetAddressOf());
  LoadPixelShader(L"ps\\fill.hlsl", shader_fill.second.GetAddressOf());
  */
  LoadCompiledVertexShader(v2f_c4f_t2f_t2f_d28f_fxc, v2f_c4f_t2f_t2f_d28f_fxc_len, shader_fill.first.GetAddressOf(),
    layout_2f_4ub_2f_2f_28f, ARRAYSIZE(layout_2f_4ub_2f_2f_28f), vertex_layout_2f_4ub_2f_2f_28f_.GetAddressOf());
  LoadCompiledPixelShader(fill_fxc, fill_fxc_len, shader_fill.second.GetAddressOf());
}

void GPUDriverD3D11::BindShader(uint8_t shader) {
  LoadShaders();

  ShaderType shader_type = (ShaderType)shader;
  switch (shader_type) {
  case kShaderType_Fill: {
    auto& shader = shaders_[kShaderType_Fill];
    context_->immediate_context()->VSSetShader(shader.first.Get(), nullptr, 0);
    context_->immediate_context()->PSSetShader(shader.second.Get(), nullptr, 0);
    break;
  }
  case kShaderType_FillPath: {
    auto& shader = shaders_[kShaderType_FillPath];
    context_->immediate_context()->VSSetShader(shader.first.Get(), nullptr, 0);
    context_->immediate_context()->PSSetShader(shader.second.Get(), nullptr, 0);
    break;
  }
  }
}

void GPUDriverD3D11::BindVertexLayout(VertexBufferFormat format) {
  LoadShaders();

  switch (format) {
  case kVertexBufferFormat_2f_4ub_2f: 
    context_->immediate_context()->IASetInputLayout(vertex_layout_2f_4ub_2f_.Get());
    break;
  case kVertexBufferFormat_2f_4ub_2f_2f_28f:
    context_->immediate_context()->IASetInputLayout(vertex_layout_2f_4ub_2f_2f_28f_.Get());
    break;
  };
}

void GPUDriverD3D11::BindGeometry(uint32_t id) {
  auto i = geometry_.find(id);
  if (i == geometry_.end())
    return;

  auto immediate_ctx = context_->immediate_context();

  auto& geometry = i->second;
  UINT stride = geometry.format == kVertexBufferFormat_2f_4ub_2f? sizeof(Vertex_2f_4ub_2f) : sizeof(Vertex_2f_4ub_2f_2f_28f);
  UINT offset = 0;
  immediate_ctx->IASetVertexBuffers(0, 1, geometry.vertexBuffer.GetAddressOf(), &stride, &offset);
  immediate_ctx->IASetIndexBuffer(geometry.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
  immediate_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  BindVertexLayout(geometry.format);
}

ComPtr<ID3D11SamplerState> GPUDriverD3D11::GetSamplerState() {
  if (sampler_state_)
    return sampler_state_;

  D3D11_SAMPLER_DESC sampler_desc;
  ZeroMemory(&sampler_desc, sizeof(sampler_desc));
  sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampler_desc.MinLOD = 0;
  //sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
  sampler_desc.MaxLOD = 0;
  HRESULT hr = context_->device()->CreateSamplerState(&sampler_desc, &sampler_state_);

  if (FAILED(hr)) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::GetSamplerState, unable to create sampler state.", L"Error", MB_OK);
  }

  return sampler_state_;
}

ComPtr<ID3D11Buffer> GPUDriverD3D11::GetConstantBuffer() {
  if (constant_buffer_)
    return constant_buffer_;

  D3D11_BUFFER_DESC desc;
  ZeroMemory(&desc, sizeof(desc));
  desc.Usage = D3D11_USAGE_DYNAMIC;
  desc.ByteWidth = sizeof(Uniforms);
  desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  HRESULT hr = context_->device()->CreateBuffer(&desc, nullptr, constant_buffer_.GetAddressOf());
  if (FAILED(hr)) {
    MessageBoxW(nullptr,
      L"GPUDriverD3D11::GetConstantBuffer, unable to create constant buffer.", L"Error", MB_OK);
  }

  return constant_buffer_;
}

void GPUDriverD3D11::SetViewport(float width, float height) {
  D3D11_VIEWPORT vp;
  ZeroMemory(&vp, sizeof(vp));
  vp.Width = width * (float)context_->scale();
  vp.Height = height * (float)context_->scale();
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  context_->immediate_context()->RSSetViewports(1, &vp);
}

void GPUDriverD3D11::UpdateConstantBuffer(const GPUState& state) {
  auto buffer = GetConstantBuffer();
  Uniforms uniforms;
  uniforms.State = { 0.0, state.viewport_width, state.viewport_height, (float)context_->scale() };
  uniforms.Transform = DirectX::XMMATRIX(state.transform.data);
  uniforms.Scalar4[0] =
    { state.uniform_scalar[0], state.uniform_scalar[1], state.uniform_scalar[2], state.uniform_scalar[3] };
  uniforms.Scalar4[1] = 
    { state.uniform_scalar[4], state.uniform_scalar[5], state.uniform_scalar[6], state.uniform_scalar[7] };
  for (size_t i = 0; i < 8; ++i)
    uniforms.Vector[i] = DirectX::XMFLOAT4(state.uniform_vector[i].value);
  uniforms.ClipSize = state.clip_size;
  for (size_t i = 0; i < state.clip_size; ++i)
    uniforms.Clip[i] = DirectX::XMMATRIX(state.clip[i].data);

  D3D11_MAPPED_SUBRESOURCE res;
  context_->immediate_context()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
  memcpy(res.pData, &uniforms, sizeof(Uniforms));
  context_->immediate_context()->Unmap(buffer.Get(), 0);
}

}  // namespace ultralight
