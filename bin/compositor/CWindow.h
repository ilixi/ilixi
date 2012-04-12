/*
 Copyright 2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This file is part of ilixi.

 ilixi is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ilixi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with ilixi.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ILIXI_CWINDOW_H_
#define ILIXI_CWINDOW_H_

#include "ui/TextBase.h"
#include "ui/SurfaceView.h"
#include "lib/TweenAnimation.h"

namespace ilixi
{

  class WindowThumbnail : public SurfaceView, public TextBase
  {
  public:
    WindowThumbnail(const std::string& name, Widget* parent = 0);

    virtual
    ~WindowThumbnail();

    virtual Size
    preferredSize() const;

    DFBWindowID
    windowID() const;

    float
    opacity() const;

    float
    scale() const;

    void
    setOpacity(float value);

    void
    setScale(float value);

    sigc::signal<void> sigSelected;
    sigc::signal<void, WindowThumbnail*> sigFocused;

  protected:
    virtual void
    opacitySlot();

    virtual void
    scaleSlot();

    virtual void
    pointerButtonUpEvent(const PointerEvent& pointerEvent);

    virtual void
    keyUpEvent(const KeyEvent& keyEvent);

    virtual void
    focusInEvent();

    virtual void
    focusOutEvent();

    virtual void
    compose();

  private:
    float _scale;
    float _opacity;
    float _overlay;
    IDirectFBWindow* _window;

    Tween* _opacityTween;
    Tween* _overlayTween;
    Tween* _scaleTween;

    TweenAnimation* _opacityAni;
    TweenAnimation* _scaleAni;

    virtual void
    renderSource();

    void
    updateTextLayoutGeometry();

    void
    showOverlay();

    void
    hideOverlay();
  };

} /* namespace ilixi */
#endif /* ILIXI_CWINDOW_H_ */
