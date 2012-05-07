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

#include "AppCompositor.h"
#include "core/Logger.h"

namespace ilixi
{

  AppCompositor::AppCompositor(Widget* parent, AppFlags flags) :
      Widget(parent), _appFlags(flags), _zoomFactor(1)
  {
    setInputMethod(NoInput);
    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &AppCompositor::updateAppCompositorGeometry));
  }

  AppCompositor::~AppCompositor()
  {
  }

  void
  AppCompositor::addWindow(IDirectFBWindow* window)
  {
    SurfaceView* view = new SurfaceView();
    view->setSourceFromWindow(window);
    addChild(view);
    updateAppCompositorGeometry();
  }

  void
  AppCompositor::removeWindow(IDirectFBWindow* window)
  {
    for (WidgetList::iterator it = _children.begin(); it != _children.end();
        ++it)
      {
        SurfaceView* view = reinterpret_cast<SurfaceView*>(*it);
        if (view && view->dfbWindow() == window)
          removeChild(*it);
      }
    updateAppCompositorGeometry();
  }

  float
  AppCompositor::zoomFactor() const
  {
    return _zoomFactor;
  }

  void
  AppCompositor::setZoomFactor(float zoomFactor)
  {
    _zoomFactor = zoomFactor;
    updateAppCompositorGeometry();
  }

  void
  AppCompositor::compose()
  {
  }

  void
  AppCompositor::updateAppCompositorGeometry()
  {
    int w = _zoomFactor * width();
    int h = _zoomFactor * height();

    for (WidgetList::iterator it = _children.begin(); it != _children.end();
        ++it)
      {
        SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
        if (view)
          {
            if (_appFlags & APP_NO_MAINWINDOW)
              {
                int x, y;
                view->dfbWindow()->GetPosition(view->dfbWindow(), &x, &y);
                Size s = view->preferredSize();
                view->setGeometry(x - 10000, y, s.width() * _zoomFactor,
                    s.height() * _zoomFactor);
              }
            else
              view->setGeometry((width() - w) / 2, (height() - h) / 2, w, h);
          }
      }
  }

  void
  AppCompositor::onWindowConfig(DFBWindowID windowID,
      const DFBWindowConfig* config, DFBWindowConfigFlags flags)
  {
    for (WidgetList::iterator it = _children.begin(); it != _children.end();
        ++it)
      {
        ILOG_DEBUG(ILX, "Config %x\n", flags);
        SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
        if (view && view->dfbWindowID() == windowID)
          {

            if (flags & DWCONF_POSITION)
              view->moveTo(config->bounds.x - 10000, config->bounds.y);

            if (flags & DWCONF_SIZE)
              view->setSize(config->bounds.w, config->bounds.h);

            view->update();
            return;
          }
      }
  }

} /* namespace ilixi */
