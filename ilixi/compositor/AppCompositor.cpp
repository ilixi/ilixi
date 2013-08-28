/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include <compositor/AppCompositor.h>
#include <compositor/Compositor.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPCOMPOSITOR, "ilixi/compositor/AppComp", "AppCompositor");

AppCompositor::AppCompositor(ILXCompositor* compositor, AppInstance* instance, Widget* parent)
        : Widget(parent),
          _compositor(compositor),
          _instance(instance),
          _cState(APPCOMP_NONE),
          _zoomFactor(1),
          _hScale(1),
          _vScale(1)
{
    ILOG_TRACE_W(ILX_APPCOMPOSITOR);
    setInputMethod(NoInput);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &AppCompositor::updateAppCompositorGeometry));
}

AppCompositor::~AppCompositor()
{
    ILOG_TRACE_W(ILX_APPCOMPOSITOR);
}

AppInstance*
AppCompositor::instance() const
{
    return _instance;
}

void
AppCompositor::addWindow(IDirectFBWindow* window, bool eventHandling, bool blocking)
{
    ILOG_TRACE_W(ILX_APPCOMPOSITOR);
    ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> window: %p\n", window);
    ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> eventHandling: %d\n", eventHandling);
    ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> blocking: %d\n", blocking);

    SurfaceView* view = new SurfaceView();
    if (!eventHandling)
        view->setInputMethod(NoInput);
    view->setBlocking(blocking);
    if (_instance->appInfo()->appFlags() & APP_NEEDS_BLENDING)
        view->setBlendingEnabled(true);

    if (_instance->windowCount() > 1)
        view->setBlendingEnabled(true);
    view->setSourceFromWindow(window);
    view->sigSourceReady.connect(sigc::mem_fun(this, &AppCompositor::madeAvailable));
    addChild(view);
    sigGeometryUpdated();
    setWindowFocus();
    update();
}

void
AppCompositor::removeWindow(DFBWindowID windowID)
{
    ILOG_TRACE_W(ILX_APPCOMPOSITOR);
    ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> windowID: %u\n", windowID);
    for (WidgetList::iterator it = _children.begin(); it != _children.end(); ++it)
    {
        SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
        if (view && view->dfbWindowID() == windowID)
        {
            if (removeChild(*it))
            {
                ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> removeChild( %p )\n", (*it));
                sigGeometryUpdated();
                setWindowFocus();
                update();
                break;
            } else
                ILOG_ERROR(ILX_APPCOMPOSITOR, " -> Cannot remove child window!\n");
        }
    }
}

float
AppCompositor::zoomFactor() const
{
    return _zoomFactor;
}

void
AppCompositor::setZoomFactor(float zoomFactor)
{
    if (_zoomFactor != zoomFactor)
    {
        _zoomFactor = zoomFactor;
        updateAppCompositorGeometry();
    }
}

void
AppCompositor::setWindowFocus()
{
    for (WidgetList::reverse_iterator it = _children.rbegin(); it != _children.rend(); ++it)
    {
        SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
        if (view)
        {
            view->setFocus();
            break;
        }
    }
}

void
AppCompositor::compose(const PaintEvent& event)
{
}

void
AppCompositor::updateAppCompositorGeometry()
{
    ILOG_TRACE_W(ILX_APPCOMPOSITOR);
    float w = _zoomFactor * width();
    float h = _zoomFactor * height();

    if (w == 0 || h == 0)
        return;

    AppInfo* info = _instance->appInfo();
    ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> name: %s\n", info->name().c_str());
    ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> w: %f h: %f\n", w, h);
    if (info)
    {
        if (info->appFlags() & APP_NO_MAINWINDOW)
        {
            int i = 0;
            ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> APP_NO_MAINWINDOW - ZoomFactor: %f\n", _zoomFactor);
            // calc bounding box.
            Rectangle bounds;
            for (WidgetList::iterator it = _children.begin(); it != _children.end(); ++it)
            {
                SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
                if (view)
                {
                    int x, y;
                    view->dfbWindow()->GetPosition(view->dfbWindow(), &x, &y);
                    Size s = view->preferredSize();
                    bounds.unite(Rectangle(x, y, s.width(), s.height()));
                }
            }

            // resize
            _hScale = width() < _compositor->getAppGeometry().width() ? w / _compositor->getAppGeometry().width() : 1;
            _vScale =
                    height() < _compositor->getAppGeometry().height() ? h / _compositor->getAppGeometry().height() : 1;
            for (WidgetList::iterator it = _children.begin(); it != _children.end(); ++it)
            {
                SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
                if (view)
                {
                    int x, y;
                    view->dfbWindow()->GetPosition(view->dfbWindow(), &x, &y);
                    Size s = view->preferredSize();
                    view->setGeometry(x * _hScale, y * _vScale, s.width() * _hScale, s.height() * _vScale);
                    ILOG_DEBUG(ILX_APPCOMPOSITOR, "  -> window[%d]: %d, %d - %d x %d\n", i, view->x(), view->y(), view->width(), view->height());
                    ++i;
                }
            }
        } else
        {
            int i = 0;
            ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> APP_WITH_MAINWINDOW - ZoomFactor: %f\n", _zoomFactor);
            for (WidgetList::iterator it = _children.begin(); it != _children.end(); ++it)
            {
                SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
                if (view)
                {
                    if (i == 0)
                    {
                        view->setGeometry((width() - w) / 2, (height() - h) / 2, w, h);
                        ILOG_DEBUG(ILX_APPCOMPOSITOR, "  -> window[%d]: %d, %d - %d x %d\n", i, view->x(), view->y(), view->width(), view->height());
                        // Calculate scaling factors using mainwindow as base
                        Size s = view->preferredSize();
                        _hScale = w / s.width();
                        _vScale = h / s.height();
                        ILOG_DEBUG(ILX_APPCOMPOSITOR, "  -> hScale: %f vScale: %f\n", _hScale, _vScale);
                    } else
                    {
                        int x, y;
                        view->dfbWindow()->GetPosition(view->dfbWindow(), &x, &y);
                        Size s = view->preferredSize();
                        view->setGeometry(x * _hScale, y * _vScale, s.width() * _hScale, s.height() * _vScale);
                        ILOG_DEBUG(ILX_APPCOMPOSITOR, "  -> window[%d]: %d, %d - %d x %d\n", i, view->x(), view->y(), view->width(), view->height());
                    }
                    ++i;
                }
            }
        }
    }
}

void
AppCompositor::onWindowConfig(DFBWindowID windowID, const SaWManWindowReconfig *reconfig)
{
    ILOG_TRACE_W(ILX_APPCOMPOSITOR);
    for (WidgetList::iterator it = _children.begin(); it != _children.end(); ++it)
    {
        SurfaceView* view = dynamic_cast<SurfaceView*>(*it);
        if (view && view->dfbWindowID() == windowID)
        {
            ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> windowID[%u] Flags[0x%02x]\n", view->dfbWindowID(), reconfig->flags);
            if (reconfig->flags & SWMCF_STACKING)
            {
                if (reconfig->request.association)
                {
                    ILOG_ERROR(ILX_APPCOMPOSITOR, "Not yet implemented!\n");
                } else
                {
                    if (reconfig->request.opacity == 1)
                    {
                        ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> LowerToBottom\n");
                        lowerChild(view);
                        view->update();
                    } else
                    {
                        ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> RaiseToTop\n");
                        raiseChild(view);
                        view->update();
                    }
                    sigRestacked();
                }
            } else
            {
                if (reconfig->flags & SWMCF_OPACITY)
                {
                    ILOG_DEBUG(ILX_APPCOMPOSITOR, " -> opacity(%d)\n", reconfig->request.opacity);
                    view->setOpacity(reconfig->request.opacity);
                    view->update();
                }

                if (reconfig->flags & SWMCF_POSITION)
                {
                    ILOG_DEBUG( ILX_APPCOMPOSITOR, " -> moveTo(%d, %d)\n", reconfig->request.bounds.x, reconfig->request.bounds.y);
                    view->moveTo(reconfig->request.bounds.x * _hScale, reconfig->request.bounds.y * _vScale);
                    view->update();
                }

                if (reconfig->flags & SWMCF_SIZE)
                {
                    ILOG_DEBUG( ILX_APPCOMPOSITOR, " -> setSize(%d, %d)\n", reconfig->request.bounds.w, reconfig->request.bounds.h);
                    view->setSize(reconfig->request.bounds.w * _hScale, reconfig->request.bounds.h * _vScale);
                    view->update();
                }
            }
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
