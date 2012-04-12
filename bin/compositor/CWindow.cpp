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

#include "CWindow.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

namespace ilixi
{

  WindowThumbnail::WindowThumbnail(const std::string& n, Widget* parent) :
      SurfaceView(0, parent), TextBase(n, parent), _scale(1), _opacity(.5), _overlay(
          0), _window(NULL)
  {
    setConstraints(FixedConstraint, FixedConstraint);
    setInputMethod(KeyAndPointerInput);

    _opacityAni = new TweenAnimation();
    _opacityAni->setDuration(500);
    _opacityAni->sigExec.connect(
        sigc::mem_fun(this, &WindowThumbnail::opacitySlot));
    _opacityTween = new Tween(Tween::SINE, Tween::EASE_OUT, _opacity, _opacity,
        1);
    _overlayTween = new Tween(Tween::SINE, Tween::EASE_OUT, _overlay, _overlay,
        1);
    _opacityAni->addTween(_overlayTween);
    _opacityAni->addTween(_opacityTween);

    _scaleAni = new TweenAnimation();
    _scaleAni->setDuration(500);
    _scaleAni->sigExec.connect(
        sigc::mem_fun(this, &WindowThumbnail::scaleSlot));
    _scaleTween = new Tween(Tween::SINE, Tween::EASE_OUT, _scale, _scale, 1);
    _scaleAni->addTween(_scaleTween);
  }

  WindowThumbnail::~WindowThumbnail()
  {
    delete _opacityAni;
    delete _scaleAni;
  }

  Size
  WindowThumbnail::preferredSize() const
  {
    return Size(196, 196);
  }

  DFBWindowID
  WindowThumbnail::windowID() const
  {
    DFBWindowID id;
    if (_window)
      _window->GetID(_window, &id);
    return id;
  }

  float
  WindowThumbnail::opacity() const
  {
    return _opacity;
  }

  float
  WindowThumbnail::scale() const
  {
    return _scale;
  }

  void
  WindowThumbnail::setOpacity(float value)
  {
    _opacityAni->stop();
    if (value < 0)
      value = 0;

    _opacityTween->setInitialValue(_opacity);
    _opacityTween->setEndValue(value);
    _opacityAni->start();
  }

  void
  WindowThumbnail::setScale(float value)
  {
    _scaleAni->stop();
    if (value < 0)
      value = 0;

    _scaleTween->setInitialValue(_scale);
    _scaleTween->setEndValue(value);
    _scaleAni->start();
  }

  void
  WindowThumbnail::opacitySlot()
  {
    update();
  }

  void
  WindowThumbnail::scaleSlot()
  {
    update();
  }

  void
  WindowThumbnail::pointerButtonUpEvent(const PointerEvent& pointerEvent)
  {
    sigSelected();
  }

  void
  WindowThumbnail::keyUpEvent(const KeyEvent& keyEvent)
  {
    if (keyEvent.keySymbol == DIKS_SPACE)
      sigSelected();
  }

  void
  WindowThumbnail::focusInEvent()
  {
    sigFocused(this);
//    setOpacity(1);
    showOverlay();
  }

  void
  WindowThumbnail::focusOutEvent()
  {
//    setOpacity(0.5);
    hideOverlay();
  }

  void
  WindowThumbnail::compose()
  {
  }

  void
  WindowThumbnail::renderSource()
  {
    if (_source)
      {
        DFBRectangle rect =
          { 0, 0, _scale * width(), _scale * height() };
        IDirectFBSurface* dfbSurface = surface()->DFBSurface();
        dfbSurface->StretchBlit(dfbSurface, _source, NULL, &rect);
      }

    Painter p(this);
    p.begin();

    int y = height() - _overlay * 50;
    p.setBrush(Color(0, 0, 0, 125));
    p.fillRectangle(0, y, width(), 50);

    p.setBrush(Color(255, 255, 255, 255));
//    p.setFont(*font());
    p.drawLayout(_layout, _layout.x(), 15 + y);

    p.end();
  }

  void
  WindowThumbnail::updateTextLayoutGeometry()
  {
    Size s = textExtents();
    int x = (width() - s.width()) / 2;
    _layout.setBounds(x, 0, width() - x, s.height());
    _layout.doLayout(font());
  }

  void
  WindowThumbnail::showOverlay()
  {
    _overlayTween->setInitialValue(0);
    _overlayTween->setEndValue(1);
    _opacityAni->start();
  }

  void
  WindowThumbnail::hideOverlay()
  {
    _overlayTween->setInitialValue(1);
    _overlayTween->setEndValue(0);
    _opacityAni->start();
  }

} /* namespace ilixi */
