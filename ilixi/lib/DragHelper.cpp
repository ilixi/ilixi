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

#include <lib/DragHelper.h>
#include <core/Application.h>
#include <core/PlatformManager.h>
#include <core/Engine.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_DRAGHELPER, "ilixi/lib/DragHelper", "DragHelper");

DragHelper* DragHelper::__instance = NULL;

DragHelper::DragHelper(Widget* owner)
        : WindowWidget(),
          _owner(owner),
          _data(NULL),
          _surface(NULL),
          _hotSpot(0, 0),
          _operation(DragOperationNone)
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    __instance = this;
    _dragWindow = true;
    if (PlatformManager::instance().appOptions() & OptExclusive)
        setLayerName("cursor");
    else
        setLayerName(owner->surface()->layerName());
}

DragHelper::~DragHelper()
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    __instance = NULL;
    releaseDragHelper();
}

Size
DragHelper::preferredSize() const
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    if (_surface)
    {
        int width = 0;
        int height = 0;
        _surface->GetSize(_surface, &width, &height);
        return Size(width, height);
    }
    return Size(100, 100);
}

void*
DragHelper::data() const
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    return _data;
}

const Point&
DragHelper::hotspot() const
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    return _hotSpot;
}

DragHelper::DragOperation
DragHelper::dragOperation()
{
    if (__instance)
        return __instance->_operation;
    return DragOperationNone;
}

void
DragHelper::setHotspot(const Point& hotspot)
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    _hotSpot = hotspot;
    // TODO use hotspot for setting window position
}

void
DragHelper::setData(void* data)
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    _data = data;
}

void
DragHelper::setImage(Image* image, const Size& size)
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    setImageFromDFBSurface(image->getDFBSurface(), size);
}

void
DragHelper::setImageFromDFBSurface(IDirectFBSurface* surface, const Size& size)
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    if (!surface)
        return;

    if (_surface)
        _surface->Release(_surface);

    int width = 0;
    int height = 0;
    surface->GetSize(surface, &width, &height);

    DFBSurfaceDescription desc;
    desc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS);
    if (size.isValid())
    {
        desc.width = size.width();
        desc.height = size.height();
    } else
    {
        desc.width = width;
        desc.height = height;
    }
    desc.pixelformat = PlatformManager::instance().forcedPixelFormat();
    desc.caps = DSCAPS_VIDEOONLY;

    PlatformManager::instance().getDFB()->CreateSurface(PlatformManager::instance().getDFB(), &desc, &_surface);
    if (width > desc.width || height > desc.height)
        _surface->StretchBlit(_surface, surface, NULL, NULL);
    else
        _surface->Blit(_surface, surface, NULL, 0, 0);
}

#if ILIXI_HAVE_CAIRO
void
DragHelper::setImageFromCairoSurface(cairo_surface_t* cairoSurface)
{
    ILOG_TRACE_W(ILX_DRAGHELPER);

    if (_surface)
        _surface->Release(_surface);

    int width = cairo_image_surface_get_width(cairoSurface);
    int height = cairo_image_surface_get_height(cairoSurface);

    DFBSurfaceDescription desc;
    desc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS);
    desc.width = width;
    desc.height = height;
    desc.pixelformat = PlatformManager::instance().forcedPixelFormat();
    desc.caps = DSCAPS_VIDEOONLY;

    PlatformManager::instance().getDFB()->CreateSurface(PlatformManager::instance().getDFB(), &desc, &_surface);

    cairo_surface_t* surface = cairo_directfb_surface_create(PlatformManager::instance().getDFB(), _surface);
    cairo_t* context = cairo_create(surface);
    cairo_set_source_surface(context, cairoSurface, 0, 0);
    cairo_paint(context);
    cairo_destroy(context);
    cairo_surface_destroy(surface);
}
#endif

void
DragHelper::startDrag(int x, int y)
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    Application::setDragging(true);
    setVisible(true);
    Size s = preferredSize();
    if (PlatformManager::instance().appOptions() & OptExclusive)
        PlatformManager::instance().setLayerRectangle("cursor", Rectangle(x - s.width() / 2, y - s.height() / 2, s.width(), s.height()));
    showWindow(Point(x - s.width() / 2, y - s.height() / 2));
    _window->dfbWindow()->GrabPointer(_window->dfbWindow());
}

void
DragHelper::setDragOperation(DragOperation operation)
{
    if (__instance)
        __instance->_operation = operation;
}

bool
DragHelper::handleWindowEvent(const DFBWindowEvent& event, bool dragging)
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    DFBWindowEvent dragEvent = event;
    dragEvent.window_id = 0;
    // map to window here
    dragEvent.x = event.cx;
    dragEvent.y = event.cy;

    switch (event.type)
    {
    case DWET_BUTTONUP:
    {
        ILOG_DEBUG(ILX_DRAGHELPER, " -> Drag end at x, y: (%d, %d) - cx, cy: (%d, %d)\n", event.x, event.y, event.cx, event.cy);
        Application::handleDragEvents(dragEvent);
        Application::setDragging(false);
        sigDragEnded(Point(event.cx, event.cy));
        delete this;
        DFBPoint p;
        p.x = event.cx;
        p.y = event.cy;
        PlatformManager::instance().renderCursor(p, false);
        break;
    }
    case DWET_MOTION:
    {
        if (event.buttons & DIBM_LEFT)
        {
            dragEvent.buttons = (DFBInputDeviceButtonMask) 0;

            if (PlatformManager::instance().appOptions() & OptExclusive)
                PlatformManager::instance().setLayerPosition("cursor", Point(event.cx - width() / 2, event.cy - height() / 2));
            else
            {
                ILOG_DEBUG(ILX_DRAGHELPER, " -> Dragging to x, y: (%d, %d) - cx, cy: (%d, %d)\n", event.x, event.y, event.cx, event.cy);
                _window->moveTo(event.cx - width() / 2, event.cy - height() / 2);
            }
            Application::handleDragEvents(dragEvent);
        }
        break;
    }
    default:
        break;
    }

    return true;
}

void
DragHelper::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    if (_surface)
        surface()->blit(_surface, 0, 0);
    surface()->blit(PlatformManager::instance().getCursorImage(), width() / 2, height() / 2);
}

void
DragHelper::releaseDragHelper()
{
    ILOG_TRACE_W(ILX_DRAGHELPER);
    _window->dfbWindow()->UngrabPointer(_window->dfbWindow());
    if (_surface)
        _surface->Release(_surface);
    closeWindow();
}

} /* namespace ilixi */
