#include <Framework/Application.h>
#include <Framework/GPUContext.h>
#include <Ultralight/Renderer.h>
#include "UI.h"

class Browser : public framework::Application,
                public framework::WindowListener {
public:
  Browser(framework::Window& window);
  virtual ~Browser();

  // Inherited from Application
  virtual void Update();

  // Inherited from WindowListener
  virtual void OnKeyEvent(const ultralight::KeyEvent& evt);
  virtual void OnMouseEvent(const ultralight::MouseEvent& evt);
  virtual void OnScrollEvent(const ultralight::ScrollEvent& evt);
  virtual void OnResize(int width, int height);

protected:
  framework::Window& window_;
  std::unique_ptr<framework::GPUContext> gpu_context_;
  ultralight::RefPtr<ultralight::Renderer> renderer_;
  std::unique_ptr<UI> ui_;
};
