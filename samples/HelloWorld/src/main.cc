#include <Ultralight/Ultralight.h>
#include <iostream>
#include <string>

using namespace ultralight;

static bool finished = false;

class LoadListenerImpl : public LoadListener {
public:
  virtual ~LoadListenerImpl() {}
    
  virtual void OnFinishLoading(ultralight::View* caller) {
    finished = true;
  }
};

int main() {
  std::cout << "\n\n";
  std::cout << "Creating Renderer..." << std::endl;

  auto renderer = ultralight::Renderer::Create();

  std::cout << "Creating View..." << std::endl;

  auto view = renderer->CreateView(200, 200, false);

  LoadListenerImpl* load_listener = new LoadListenerImpl();
  view->set_load_listener(load_listener);

  view->LoadHTML(R"(
  <html>
    <head>
      <style type="text/css">
      h1 { background: yellow; }
      h1, p { padding: 8px; text-align: center; }
      </style>
      <title>Hello World!</title>
    </head>
    <body>
      <h1>Hello World!</h1>
      <p>Welcome to Ultralight!</p>
    </body>
  </html>
  )");

  std::cout << "Starting update loop..." << std::endl;
  
  while (!finished)
    renderer->Update();

  std::cout << "Writing bitmap to output.png" << std::endl;

  renderer->Render();

  view->bitmap()->WritePNG("output.png");

  std::cout << "Done." << std::endl;

  std::cout << "\n\n";

  view->set_load_listener(nullptr);
  delete load_listener;

  return 0;
}
