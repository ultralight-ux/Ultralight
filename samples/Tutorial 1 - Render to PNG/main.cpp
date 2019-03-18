#include <Ultralight/Ultralight.h>
#include <iostream>
#include <string>

using namespace ultralight;

static bool finished = false;

class Listener : public LoadListener {
public:
  virtual ~Listener() {}
    
  virtual void OnFinishLoading(ultralight::View* caller) {
    finished = true;
  }
};

int main() {
  auto renderer = ultralight::Renderer::Create();
  auto view = renderer->CreateView(200, 200, false);

  Listener* load_listener = new Listener();
  view->set_load_listener(load_listener);

  view->LoadHTML(R"(
  <html>
    <body>
      <h1>Hello World!</h1>
      <p>Welcome to Ultralight!</p>
    </body>
  </html>
  )");

  while (!finished)
    renderer->Update();

  renderer->Render();

  view->bitmap()->WritePNG("output.png");

  view->set_load_listener(nullptr);
  delete load_listener;

  return 0;
}
