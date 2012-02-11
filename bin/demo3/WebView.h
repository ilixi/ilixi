#ifndef ILIXI_WEBVIEW_H_
#define ILIXI_WEBVIEW_H_

#include "ui/Widget.h"

namespace ilixi
{

  class WebView : public Widget
  {
  public:
    WebView(Widget* parent = 0);

    virtual
    ~WebView();

    void
    load(const char* url);

    void
    back();

    void
    forward();

    void
    stop();

    void
    reload();

  private:
    virtual void
    pointerButtonDownEvent(const PointerEvent& mouseEvent);

    void
    pointerButtonUpEvent(const PointerEvent& mouseEvent);

    void
    pointerMotionEvent(const PointerEvent& mouseEvent);

    virtual void
    keyUpEvent(const KeyEvent& keyEvent);

    virtual void
    focusInEvent();

    virtual void
    focusOutEvent();

    void
    compose();
  };

} /* namespace ilixi */
#endif /* ILIXI_WEBVIEW_H_ */
