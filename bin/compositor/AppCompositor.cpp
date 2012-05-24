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
#include "Compositor.h"
#include "core/Logger.h"

namespace ilixi
{

  D_DEBUG_DOMAIN( ILX_APPCOMPOSITOR, "ilixi/compositor/appcomp",
      "AppCompositor");

  AppCompositor::AppCompositor(Compositor* compositor, AppInstance* instance,
      Widget* parent) :
      Widget(parent), _compositor(compositor), _instance(instance), _zoomFactor(
          1)
  {
    setInputMethod(NoInput);
    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &AppCompositor::updateAppCompositorGeometry));
  }

  AppCompositor::~AppCompositor()
  {
  }

  AppInstance*
  AppCompositor::instance() const
  {
    return _instance;
  }

  void
  AppCompositor::addWindow(IDirectFBWindow* window, bool eventHandling)
  {
    SurfaceView* view = new SurfaceView();
    if (!eventHandling)
      view->setInputMethod(NoInput);
    view->setSourceFromWindow(window);
    addChild(view);
    updateAppCompositorGeometry();
  }

  void
  AppCompositor::removeWindow(DFBWindowID windowID)
  {
    for (WidgetList::iterator it = _children.begin(); it != _children.end();
        ++it)
      {
        SurfaceView* view = reinterpret_cast<SurfaceView*>(*it);
        if (view && view->dfbWindowID() == windowID)
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
    int i = 0;
    for (WidgetList::iterator it = _children.begin(); it != _children.end();
        ++it)
      {
        SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
        if (view)
          {
            AppInfo* info = _compositor->appMan()->infoByInstanceID(
                _instance->instanceID());

//            ILOG_DEBUG(ILX_APPCOMPOSITOR,
//                "INFO: %p - InstanceID: %d\n", info, _instance->instanceID());
//            ILOG_DEBUG(ILX_APPCOMPOSITOR, "Flags: %x\n", info->appFlags());

            if (info->appFlags() & APP_NO_MAINWINDOW)
              {
                int x, y;
                view->dfbWindow()->GetPosition(view->dfbWindow(), &x, &y);
                Size s = view->preferredSize();
                view->setGeometry(x, y, s.width() * _zoomFactor,
                    s.height() * _zoomFactor);
              }
            else
              {
                if (i == 0)
                  view->setGeometry((width() - w) / 2, (height() - h) / 2, w,
                      h);
                else
                  {
                    int x, y;
                    view->dfbWindow()->GetPosition(view->dfbWindow(), &x, &y);
                    Size s = view->preferredSize();
                    view->setGeometry(x, y, s.width() * _zoomFactor,
                        s.height() * _zoomFactor);
                  }
              }
            i++;
          }
      }
  }

  void
  AppCompositor::onWindowConfig(DFBWindowID windowID,
      const SaWManWindowReconfig *reconfig)
  {
    for (WidgetList::iterator it = _children.begin(); it != _children.end();
        ++it)
      {
        ILOG_DEBUG(ILX_APPCOMPOSITOR, "Config %x\n", reconfig->flags);
        SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
        if (view && view->dfbWindowID() == windowID)
          {
            if (reconfig->flags & SWMCF_OPACITY)
              view->setOpacity(reconfig->request.opacity);

            if (reconfig->flags & SWMCF_POSITION)
              view->moveTo(reconfig->request.bounds.x,
                  reconfig->request.bounds.y);

            if (reconfig->flags & SWMCF_SIZE)
              view->setSize(reconfig->request.bounds.w,
                  reconfig->request.bounds.h);

            view->update();
            return;
          }
      }
  }

} /* namespace ilixi */
