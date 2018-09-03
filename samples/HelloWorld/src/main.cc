#include <Ultralight/Renderer.h>
#include <Ultralight/View.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <Ultralight/platform/GPUDriver.h>
#include "../../../framework/platform/common/FontLoaderRoboto.h"
#include <iostream>
#include <string>

using namespace ultralight;

static bool finished = false;

class GPUDriverStub : public GPUDriver {
public:
  GPUDriverStub() {}
  virtual ~GPUDriverStub() {}

  // Synchronization

  virtual void BeginSynchronize() {}

  virtual void EndSynchronize() {}

  // Textures

  virtual uint32_t NextTextureId() { return 0; }

  virtual void CreateTexture(uint32_t texture_id,
    Ref<Bitmap> bitmap) {}

  virtual void UpdateTexture(uint32_t texture_id,
    Ref<Bitmap> bitmap) {}

  virtual void BindTexture(uint8_t texture_unit,
    uint32_t texture_id) {}

  virtual void DestroyTexture(uint32_t texture_id) {}

  // Offscreen Rendering

  virtual uint32_t NextRenderBufferId() { return 0; }

  virtual void CreateRenderBuffer(uint32_t render_buffer_id,
    const RenderBuffer& buffer) {}

  virtual void BindRenderBuffer(uint32_t render_buffer_id) {}

  virtual void SetRenderBufferViewport(uint32_t render_buffer_id,
    uint32_t width,
    uint32_t height) {}

  virtual void ClearRenderBuffer(uint32_t render_buffer_id) {}

  virtual void DestroyRenderBuffer(uint32_t render_buffer_id) {}

  // Geometry

  virtual uint32_t NextGeometryId() { return 0; }

  virtual void CreateGeometry(uint32_t geometry_id,
    const VertexBuffer& vertices,
    const IndexBuffer& indices) {}

  virtual void UpdateGeometry(uint32_t geometry_id,
    const VertexBuffer& buffer,
    const IndexBuffer& indices) {}


  virtual void DrawGeometry(uint32_t geometry_id,
    uint32_t indices_count,
    uint32_t indices_offset,
    const GPUState& state) {}


  virtual void DestroyGeometry(uint32_t geometry_id) {}

  // Command Queue

  virtual void UpdateCommandList(const CommandList& queue) {
    std::cout << "[GPUDriver] UpdateCommandList: " << queue.size << "." << std::endl;
  }

  virtual bool HasCommandsPending() { return false; }

  virtual void DrawCommandList() {}
};

class LoadListenerImpl : public LoadListener {
public:
    virtual ~LoadListenerImpl() {}
    
    // Called when the page begins loading new URL into main frame
    virtual void OnBeginLoading(ultralight::View* caller) {
        std::cout << "[LoadListener] OnBeginLoading" << std::endl;
    }
    
    // Called when the page finishes loading URL into main frame
    virtual void OnFinishLoading(ultralight::View* caller) {
        std::cout << "[LoadListener] OnFinishLoading" << std::endl;
        finished = true;
    }
    
    // Called when the history (back/forward state) is modified
    virtual void OnUpdateHistory(ultralight::View* caller) {
        std::cout << "[LoadListener] OnUpdateHistory" << std::endl;
    }
    
    // Called when all JavaScript has been parsed and the document is ready.
    // This is the best time to make any initial JavaScript calls to your page.
    virtual void OnDOMReady(ultralight::View* caller) {
        std::cout << "[LoadListener] OnDOMReady" << std::endl;
    }
};

int main() {
  std::cout << "\n\n";
    
  ultralight::Platform& platform = ultralight::Platform::instance();
  platform.set_font_loader(new FontLoaderRoboto());
  platform.set_gpu_driver(new GPUDriverStub());

  std::cout << "Creating Renderer..." << std::endl;

  auto renderer = ultralight::Renderer::Create();

  std::cout << "Creating View..." << std::endl;

  auto view = renderer->CreateView(100, 100, false);
  view->set_load_listener(new LoadListenerImpl());
  view->LoadHTML("<html><head><title>Hello World</title></head><body></body></html>");

  std::cout << "Starting update loop..." << std::endl;
  
  while (!finished) {
    renderer->Update();
    renderer->Render();
  }

  std::cout << "Loaded page with title: \n\t " << view->title().utf8().data() << std::endl;
  
  std::cout << "Done." << std::endl;

  std::cout << "\n\n";

  return 0;
}
