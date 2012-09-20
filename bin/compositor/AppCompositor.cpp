/*
 Copyright 2010-2012 Tarik Sekmen.

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
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPCOMPOSITOR, "ilixi/comp/appcomp", "AppCompositor");

AppCompositor::AppCompositor(ILXCompositor* compositor, AppInstance* instance,
                             Widget* parent)
        : Widget(parent),
          _compositor(compositor),
          _instance(instance),
          _cState(APPCOMP_NONE),
          _zoomFactor(1)
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
AppCompositor::addWindow(IDirectFBWindow* window, bool eventHandling,
                         bool blocking)
{
    SurfaceView* view = new SurfaceView();
    if (!eventHandling)
        view->setInputMethod(NoInput);
    view->setBlocking(blocking);
    if (_instance->appInfo()->appFlags() & APP_NEEDS_BLENDING)
        view->setBlendingEnabled(true);
    view->setSourceFromWindow(window);
    view->sigSourceReady.connect(
            sigc::mem_fun(this, &AppCompositor::madeAvailable));
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
AppCompositor::compose(const PaintEvent& event)
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

            if (info)
            {
                if (info->appFlags() & APP_NO_MAINWINDOW)
                {
                    int x, y;
                    view->dfbWindow()->GetPosition(view->dfbWindow(), &x, &y);
                    Size s = view->preferredSize();
                    view->setGeometry(x, y, s.width(), s.height());
                } else
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
        SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
        if (view && view->dfbWindowID() == windowID)
        {
            ILOG_DEBUG(ILX_APPCOMPOSITOR,
                       "Config 0x%02x\n", view->dfbWindowID(), reconfig->flags);
            if (reconfig->flags & SWMCF_STACKING)
            {
                if (reconfig->request.association)
                {
                    ILOG_ERROR(ILX_APPCOMPOSITOR, "Not yet implemented!\n");
                } else
                {
                    if (reconfig->request.opacity == 1)
                    {
                        lowerChildToBottom(view);
                        ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> LowerToBottom\n");
                    } else
                    {
                        raiseChildToFront(view);
                        ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> RaiseToTop\n");
                    }
                }
            } else
            {
                if (reconfig->flags & SWMCF_OPACITY)
                {
                    ILOG_DEBUG(ILX_APPCOMPOSITOR,
                               " -> opacity(%d)\n", reconfig->request.opacity);
                    view->setOpacity(reconfig->request.opacity);
                }

                if (reconfig->flags & SWMCF_POSITION)
                {
                    ILOG_DEBUG(
                            ILX_APPCOMPOSITOR,
                            " -> moveTo(%d, %d)\n", reconfig->request.bounds.x, reconfig->request.bounds.y);
                    view->moveTo(reconfig->request.bounds.x,
                                 reconfig->request.bounds.y);
                }

                if (reconfig->flags & SWMCF_SIZE)
                {
                    ILOG_DEBUG(
                            ILX_APPCOMPOSITOR,
                            " -> setSize(%d, %d)\n", reconfig->request.bounds.w, reconfig->request.bounds.h);
                    view->setSize(reconfig->request.bounds.w,
                                  reconfig->request.bounds.h);
                }
            }

            view->update();
            return;
        }
    }
}

void
AppCompositor::madeAvailable()
{
    ILOG_TRACE_W(ILX_APPCOMPOSITOR);
    _cState = APPCOMP_READY;
}

} /* namespace ilixi */
