#pragma once
#include <Framework/Overlay.h>
#include <Ultralight/Listener.h>

class UI;

/**
* Browser Tab UI implementation. Renders the actual page content in bottom pane.
*/
class Tab : public framework::Overlay,
            public ultralight::ViewListener,
            public ultralight::LoadListener {
public:
  Tab(UI* ui, size_t id);
  ~Tab();

  void set_ready_to_close(bool ready) { ready_to_close_ = ready; }
  bool ready_to_close() { return ready_to_close_; }

  // Inherited from ultralight::Listener::View
  virtual void OnChangeTitle(ultralight::View* caller,
    const ultralight::String& title) override;
  virtual void OnChangeURL(ultralight::View* caller,
    const ultralight::String& url) override;
  virtual void OnChangeTooltip(ultralight::View* caller,
    const ultralight::String& tooltip) override;
  virtual void OnChangeCursor(ultralight::View* caller,
    ultralight::Cursor cursor) override;
  virtual void OnAddConsoleMessage(ultralight::View* caller,
    ultralight::MessageSource source,
    ultralight::MessageLevel level,
    const ultralight::String& message,
    uint32_t line_number,
    uint32_t column_number,
    const ultralight::String& source_id) override;

  // Inherited from ultralight::Listener::Load
  virtual void OnBeginLoading(ultralight::View* caller) override;
  virtual void OnFinishLoading(ultralight::View* caller) override;
  virtual void OnUpdateHistory(ultralight::View* caller) override;
  virtual void OnDOMReady(ultralight::View* caller) override;

protected:
  UI* ui_;
  size_t id_;
  bool ready_to_close_ = false;
};
