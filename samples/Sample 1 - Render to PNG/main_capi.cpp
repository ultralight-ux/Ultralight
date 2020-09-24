#include <Ultralight/CAPI.h>
#include <AppCore/CAPI.h>
#include <stdio.h>

const char* htmlString();

///
///  Welcome to Sample 1!
///
///  In this sample we'll load a string of HTML and render it to a PNG.
///  
///  Since we're rendering offscreen and don't need to create any windows or
///  handle any user input, we won't be using App::Create() and will instead be using
///  the Ultralight API directly with our own custom main loop.
///
///  Our main loop waits for the page to finish loading by subscribing to the
///  LoadListener interface then writes the rendering surface to a PNG on disk.
///

bool done = false;

void OnFinishLoading(void* user_data, ULView caller,
  unsigned long long frame_id, bool is_main_frame, ULString url) {
  ///
  /// Our page is done when the main frame is finished loading.
  ///
  if (is_main_frame) {
    printf("Our page has loaded!\n");

    ///
    /// Set our done flag to true to exit the Run loop.
    ///
    done = true;
  }
}

int main() {
  ///
  /// Setup our config. The config can be used to customize various
  /// options within the renderer and WebCore module.
  ///
  /// Our config uses 2x DPI scale and "Arial" as the default font.
  ///
  ULConfig config = ulCreateConfig();

  ulConfigSetDeviceScale(config, 2.0);

  ULString font_family = ulCreateString("Arial");
  ulConfigSetFontFamilyStandard(config, font_family);
  ulDestroyString(font_family);

  ///
  /// We need to tell config where our resources are so it can load our
  /// bundled certificate chain and make HTTPS requests.
  ///
  ULString resource_path = ulCreateString("./resources/");
  ulConfigSetResourcePath(config, resource_path);
  ulDestroyString(resource_path);

  ///
  /// Make sure the GPU renderer is disabled so we can render to an offscreen
  /// pixel buffer surface. (This is disabled by default, so not needed here)
  ///
  ulConfigSetUseGPURenderer(config, false);

  ///
  /// Use AppCore's font loader singleton to load fonts from the OS.
  ///
  ulEnablePlatformFontLoader();

  ///
  /// Use AppCore's file system singleton to load file:/// URLs from the OS.
  ///
  ULString base_dir = ulCreateString("./assets/");
  ulEnablePlatformFileSystem(base_dir);
  ulDestroyString(base_dir);

  ///
  /// Use AppCore's defaut logger to write the log file to disk.
  ///
  ULString log_path = ulCreateString("./ultralight.log");
  ulEnableDefaultLogger(log_path);
  ulDestroyString(log_path);

  ///
  /// Create our renderer using the Config we just set up.
  ///
  /// The Renderer singleton maintains the lifetime of the library and
  /// is required before creating any Views. It should outlive any Views.
  ///
  ULRenderer renderer = ulCreateRenderer(config);

  ///
  /// Create our View.
  ///
  /// Views are sized containers for loading and displaying web content.
  ///
  ULView view = ulCreateView(renderer, 1600, 1600, false, 0, false);

  ///
  /// Register OnFinishLoading() callback with our View.
  ///
  ulViewSetFinishLoadingCallback(view, &OnFinishLoading, 0);

  ///
  /// Load a string of HTML into our View. (For code readability, the string
  /// is defined in the htmlString() function at the bottom of this file)
  ///
  /// **Note**:
  ///   This operation may not complete immediately-- we will call
  ///   Renderer::Update continuously and wait for the OnFinishLoading event
  ///   before rendering our View.
  ///
  /// Views can also load remote URLs, try replacing the code below with:
  ///
  ///   ULString url_string = ulCreateString("https://en.wikipedia.org");
  ///   ulViewLoadURL(view, url_string);
  ///   ulDestroyString(url_string);
  ///
  ULString html_string = ulCreateString(htmlString());
  ulViewLoadHTML(view, html_string);
  ulDestroyString(html_string);

  printf("Starting Run(), waiting for page to load...\n");

  ///
  /// Continuously update our Renderer until are done flag is set to true.
  ///
  /// **Note**:
  ///   Calling Renderer::Update handles any pending network requests,
  ///   resource loads, and JavaScript timers.
  ///
  while (!done) {
    ulUpdate(renderer);
    ulRender(renderer);
  }

  ///
  /// Get our View's rendering surface.
  ///
  ULSurface surface = ulViewGetSurface(view);

  ///
  /// Get the underlying bitmap.
  ///
  /// @note  We're using the default surface definition which is BitmapSurface,
  ///        you can override the surface implementation via
  ///        ulPlatformSetSurfaceDefinition()
  ///
  ULBitmap bitmap = ulBitmapSurfaceGetBitmap(surface);
    
  ///
  /// The renderer uses a BGRA pixel format but PNG expects RGBA format,
  /// let's convert the format by swapping Red and Blue channels.
  ///
  ulBitmapSwapRedBlueChannels(bitmap);
    
  ///
  /// Write our bitmap to a PNG in the current working directory.
  ///
  ulBitmapWritePNG(bitmap, "result.png");
    
  printf("Saved a render of our page to result.png.\n");

  printf("Finished.\n");
}

const char* htmlString() {
  return R"(
    <html>
      <head>
        <style type="text/css">
          body {
            margin: 0;
            padding: 0;
            overflow: hidden;
            color: black;
            font-family: Arial;
            background: linear-gradient(-45deg, #acb4ff, #f5d4e2);
            display: flex;
            justify-content: center;
            align-items: center;
          }
          div {
            width: 350px;
            height: 350px;
            text-align: center;
            border-radius: 25px;
            background: linear-gradient(-45deg, #e5eaf9, #f9eaf6);
            box-shadow: 0 7px 18px -6px #8f8ae1;
          }
          h1 {
            padding: 1em;
          }
          p {
            background: white;
            padding: 2em;
            margin: 40px;
            border-radius: 25px;
          }
        </style>
      </head>
      <body>
        <div>
          <h1>Hello World!</h1>
          <p>Welcome to Ultralight!</p>
        </div>
      </body>
    </html>
    )";
}