#include <Framework/GPUContext.h>
#include "GPUDriverD3D11.h"
#include <memory>

namespace framework {

class GPUContextD3D11 : public GPUContext,
                        public PlatformGPUContextD3D11 {
public:
  GPUContextD3D11() {}

  virtual ~GPUContextD3D11() {
    if (device_)
      immediate_context_->ClearState();
  }

  // Inherited from GPUContext
  virtual ultralight::GPUDriver* driver() const { return driver_.get(); }
  virtual ultralight::FaceWinding face_winding() const { return ultralight::kFaceWinding_Clockwise; }
  virtual void BeginDrawing() {}
  virtual void EndDrawing() {}
  virtual void PresentFrame() {
    swap_chain()->Present(enable_vsync_? 1 : 0, 0);
  }

  virtual void Resize(int width, int height) {
    set_screen_size(width, height);

    immediate_context_->OMSetRenderTargets(0, 0, 0);
    ID3D11Texture2D* back_buffer = nullptr;
    swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
    back_buffer->Release();

    back_buffer_view_.Reset();

    // Get the actual device width/height (may be different than screen size)
    RECT rc;
    ::GetClientRect(hwnd_, &rc);
    UINT client_width = rc.right - rc.left;
    UINT client_height = rc.bottom - rc.top;

    HRESULT hr;
    hr = swap_chain_->ResizeBuffers(0, client_width, client_height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
      MessageBoxW(nullptr,
        L"GPUContextD3D11::Resize, unable to resize, IDXGISwapChain::ResizeBuffers failed.", L"Error", MB_OK);
      exit(-1);
    }

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
    {
      MessageBoxW(nullptr,
        L"GPUContextD3D11::Resize, unable to get back buffer.", L"Error", MB_OK);
      exit(-1);
    }

    hr = device_->CreateRenderTargetView(pBackBuffer, nullptr, back_buffer_view_.GetAddressOf());
    pBackBuffer->Release();
    if (FAILED(hr))
    {
      MessageBoxW(nullptr,
        L"GPUContextD3D11::Resize, unable to create new render target view.", L"Error", MB_OK);
      exit(-1);
    }

    immediate_context_->OMSetRenderTargets(1, back_buffer_view_.GetAddressOf(), nullptr);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.Width = (float)client_width * scale();
    vp.Height = (float)client_height * scale();
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    immediate_context_->RSSetViewports(1, &vp);
  }

  // Inherited from D3DContext:
  virtual ID3D11Device* device() override { assert(device_.Get()); return device_.Get(); }
  virtual ID3D11DeviceContext* immediate_context() override { assert(immediate_context_.Get()); return immediate_context_.Get(); }
  virtual IDXGISwapChain* swap_chain() override { assert(swap_chain_.Get()); return swap_chain_.Get(); }
  virtual ID3D11RenderTargetView* render_target_view() override { assert(back_buffer_view_.Get()); return back_buffer_view_.Get(); }
  virtual void set_scale(double scale) { scale_ = scale; }
  virtual double scale() const { return scale_; }
  virtual void set_screen_size(uint32_t width, uint32_t height) override { width_ = width; height_ = height; }
  virtual void screen_size(uint32_t& width, uint32_t& height) override { width = width_; height = height_; }

  bool Initialize(HWND hWnd, int screen_width, int screen_height, double screen_scale, bool fullscreen, bool enable_vsync, bool sRGB, int samples) {
    samples_ = samples;
    enable_vsync_ = enable_vsync;
    set_screen_size(screen_width, screen_height);
    set_scale(screen_scale);

    HRESULT hr = S_OK;

    hwnd_ = hWnd;
    
    // Get the actual device width/height (may be different than screen size)
    RECT rc;
    ::GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
    // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

    D3D_DRIVER_TYPE driverTypes[] =
    {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_REFERENCE,
    };
    unsigned numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
    };
    unsigned numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ::ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = samples_;
    sd.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
    sd.Windowed = !fullscreen;
    sd.Flags = fullscreen ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

    for (unsigned driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
      D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
      hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
        D3D11_SDK_VERSION, &sd, swap_chain_.GetAddressOf(), device_.GetAddressOf(), &feature_level_, immediate_context_.GetAddressOf());
      if (SUCCEEDED(hr))
      {
        break;
      }
    }
    if (FAILED(hr))
    {
      // Unable to create device  & swap chain
      return false;
    }

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
    {
      // Unable get back buffer
      return false;
    }

    hr = device_->CreateRenderTargetView(pBackBuffer, nullptr, back_buffer_view_.GetAddressOf());
    pBackBuffer->Release();
    if (FAILED(hr))
    {
      // Unable create back buffer view
      return false;
    }

    immediate_context_->OMSetRenderTargets(1, back_buffer_view_.GetAddressOf(), nullptr);

    D3D11_RENDER_TARGET_BLEND_DESC rt_blend_desc;
    ZeroMemory(&rt_blend_desc, sizeof(rt_blend_desc));
    rt_blend_desc.BlendEnable = true;
    rt_blend_desc.SrcBlend = D3D11_BLEND_ONE;
    rt_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    rt_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
    rt_blend_desc.SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
    rt_blend_desc.DestBlendAlpha = D3D11_BLEND_ONE;
    rt_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rt_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    D3D11_BLEND_DESC blend_desc;
    ZeroMemory(&blend_desc, sizeof(blend_desc));
    blend_desc.AlphaToCoverageEnable = false;
    blend_desc.IndependentBlendEnable = false;
    blend_desc.RenderTarget[0] = rt_blend_desc;

    device()->CreateBlendState(&blend_desc, blend_state_.GetAddressOf());

    immediate_context_->OMSetBlendState(blend_state_.Get(), NULL, 0xffffffff);

    D3D11_RASTERIZER_DESC rasterizer_desc;
    ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
    rasterizer_desc.FillMode = D3D11_FILL_SOLID;
    rasterizer_desc.CullMode = D3D11_CULL_NONE;
    rasterizer_desc.FrontCounterClockwise = false;
    rasterizer_desc.DepthBias = 0;
    rasterizer_desc.SlopeScaledDepthBias = 0.0f;
    rasterizer_desc.DepthBiasClamp = 0.0f;
    rasterizer_desc.DepthClipEnable = false;
    rasterizer_desc.ScissorEnable = false;
    rasterizer_desc.MultisampleEnable = false;
    rasterizer_desc.AntialiasedLineEnable = false;

    device()->CreateRasterizerState(&rasterizer_desc, rasterizer_state_.GetAddressOf());

    immediate_context_->RSSetState(rasterizer_state_.Get());

    // Setup the viewport
    D3D11_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.Width = (float)width * scale();
    vp.Height = (float)height * scale();
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    immediate_context_->RSSetViewports(1, &vp);

    back_buffer_width_ = width;
    back_buffer_height_ = height;

    // Initialize backbuffer with white so we don't get flash of black while loading views.
    float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    immediate_context_->ClearRenderTargetView(render_target_view(), color);

    driver_ = std::make_unique<ultralight::GPUDriverD3D11>(this);
    
    return true;
  }

  UINT back_buffer_width() { return back_buffer_width_; }
  UINT back_buffer_height() { return back_buffer_height_; }

 private:
  HWND hwnd_;
  ComPtr<ID3D11Device> device_;
  ComPtr<ID3D11DeviceContext> immediate_context_;
  ComPtr<IDXGISwapChain> swap_chain_;
  ComPtr<ID3D11RenderTargetView> back_buffer_view_;
  ComPtr<ID3D11BlendState> blend_state_;
  ComPtr<ID3D11RasterizerState> rasterizer_state_;
  D3D_FEATURE_LEVEL feature_level_;
  double scale_;
  bool enable_vsync_;
  uint32_t width_;
  uint32_t height_;
  UINT back_buffer_width_;
  UINT back_buffer_height_;
  int samples_ = 1;
  std::unique_ptr<ultralight::GPUDriverD3D11> driver_;
};

std::unique_ptr<GPUContext> GPUContext::Create(const Window& window, bool enable_vsync) {
  auto gpu_context = std::make_unique<GPUContextD3D11>();
  if (gpu_context->Initialize(window.handle(), window.width(), window.height(), window.scale(), window.is_fullscreen(), enable_vsync, false, 1))
    return std::move(gpu_context);

  return nullptr;
}

}  // namespace framework