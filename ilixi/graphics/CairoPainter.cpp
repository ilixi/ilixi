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

#include <graphics/CairoPainter.h>
#include <types/TextLayout.h>
#include <core/Logger.h>

#ifdef ILIXI_HAVE_CAIROGLES
#include <cairo-gl.h>
#include <ui/EGLWidget.h>
#endif

namespace ilixi
{

//! PI
const double CPM_PI = 3.14159265;

//! PI /180 (Used for conversion from degrees to radians)
const double CPM_D2R = 0.0174532925;

D_DEBUG_DOMAIN(ILX_CPAINTER, "ilixi/graphics/CairoPainter", "CairoPainter");

CairoPainter::CairoPainter(Widget* widget)
        : _myWidget(widget),
          _antiAliasMode(AliasSubPixel),
          _state(PFNone)
{
    ILOG_TRACE(ILX_CPAINTER);
}

CairoPainter::~CairoPainter()
{
    ILOG_TRACE(ILX_CPAINTER);
    end();
}

void
CairoPainter::begin(const PaintEvent& event)
{
    ILOG_TRACE(ILX_CPAINTER);
    _myWidget->surface()->lock();
    _context = _myWidget->surface()->cairoContext();
    cairo_set_antialias(_context, (cairo_antialias_t) _antiAliasMode);
#ifdef ILIXI_STEREO_OUTPUT
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
    {
        if (event.eye == PaintEvent::LeftEye)
        _myWidget->surface()->clip(event.rect);
        else
        _myWidget->surface()->clip(event.right);
    } else
    {
        if (event.eye == PaintEvent::LeftEye)
        _myWidget->surface()->clip(Rectangle(event.rect.x() - _myWidget->absX() - _myWidget->z(), event.rect.y() - _myWidget->absY(), event.rect.width(), event.rect.height()));
        else
        _myWidget->surface()->clip(Rectangle(event.right.x() - _myWidget->absX() + _myWidget->z(), event.right.y() - _myWidget->absY(), event.right.width(), event.right.height()));
    }
#else
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
        _myWidget->surface()->clip(event.rect);
    else
        _myWidget->surface()->clip(Rectangle(event.rect.x() - _myWidget->absX(), event.rect.y() - _myWidget->absY(), event.rect.width(), event.rect.height()));
#endif
    _state = PFActive;
    _myWidget->surface()->dfbSurface()->SetDrawingFlags(_myWidget->surface()->dfbSurface(), DSDRAW_NOFX);
    _myWidget->surface()->dfbSurface()->SetPorterDuff(_myWidget->surface()->dfbSurface(), DSPD_SRC_OVER);
}

void
CairoPainter::end()
{
    ILOG_TRACE(ILX_CPAINTER);
    if (_state & PFActive)
    {
        _state = PFNone;
        cairo_surface_flush(_myWidget->surface()->cairoSurface());
        _myWidget->surface()->unlock();
    }
}

#ifdef ILIXI_HAVE_CAIROGLES
void
CairoPainter::beginGL(EGLWidget* widget, const PaintEvent& event)
{
    ILOG_TRACE(ILX_CPAINTER);
    if (_state & PFActive)
    {
        ILOG_ERROR(ILX_CPAINTER, "Error: Cannot call beginGL(). Already called begin().\n");
        return;
    }
    _myWidget->surface()->lock();
    cairo_device_t* deviceGL;
    if (_myWidget->surface()->cairoDeviceGL())
        deviceGL = _myWidget->surface()->cairoDeviceGL();
    else
    {
        ILOG_DEBUG(ILX_CPAINTER, " -> creating cairo device..\n");
        deviceGL = cairo_egl_device_create(widget->eglDisplay(), widget->eglContext());
//        cairo_gl_device_set_thread_aware(deviceGL, 1);
        if (cairo_device_status(deviceGL))
            ILOG_ERROR(ILX_CPAINTER, "Error while creating cairo egl device\n");
        _myWidget->surface()->setCairoDeviceGL(deviceGL);
    }

    cairo_surface_t* surfaceGL;
    if (_myWidget->surface()->cairoSurfaceGL())
        surfaceGL = _myWidget->surface()->cairoSurfaceGL();
    else
    {
        ILOG_DEBUG(ILX_CPAINTER, " -> creating cairo gl surface..\n");
        surfaceGL = cairo_gl_surface_create_for_egl(deviceGL, widget->eglSurface(), widget->width(), widget->height());
        if (cairo_surface_status(surfaceGL))
            ILOG_ERROR(ILX_CPAINTER, "Error while creating cairo gl surface\n");
        else
            ILOG_DEBUG(ILX_CPAINTER, " -> created gl surface %p (width: %d height: %d)\n", surfaceGL, cairo_gl_surface_get_width(surfaceGL), cairo_gl_surface_get_height(surfaceGL));
        _myWidget->surface()->setCairoSurfaceGL(surfaceGL);
    }

    _context = _myWidget->surface()->cairoContext();
    cairo_set_antialias(_context, (cairo_antialias_t) _antiAliasMode);
    _state = PFActive;
}

void
CairoPainter::endGL()
{
    ILOG_TRACE(ILX_CPAINTER);
    if (_state & PFActive)
    {
        _state = PFNone;
        cairo_surface_flush(_myWidget->surface()->cairoSurfaceGL());
        cairo_gl_surface_swapbuffers(_myWidget->surface()->cairoSurfaceGL());
        _myWidget->surface()->unlock();
    }
}
#endif

void
CairoPainter::setBrush(const Brush& brush)
{
    ILOG_TRACE(ILX_CPAINTER);
    _brush = brush;
    _brush.applyBrush(_context);
}

void
CairoPainter::setPen(const Pen& pen)
{
    ILOG_TRACE(ILX_CPAINTER);
    _pen = pen;
}

void
CairoPainter::setFont(const Font& font)
{
    ILOG_TRACE(ILX_CPAINTER);
    _font = font;
    _state = (PainterFlags) (_state | PFFontModified);
}

cairo_t*
CairoPainter::cairoContext() const
{
    return _context;
}

CairoPainter::AntiAliasMode
CairoPainter::getAntiAliasMode() const
{
    return _antiAliasMode;
}

void
CairoPainter::drawArc(double x, double y, double width, double height, double angle1, double angle2)
{
    ILOG_TRACE(ILX_CPAINTER);
    cairo_save(_context);
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
        cairo_translate(_context, _myWidget->absX() + x + width / 2.0, _myWidget->absY() + y + height / 2.0);
    else
        cairo_translate(_context, x + width / 2.0, y + height / 2.0);
    cairo_scale(_context, width / 2.0, height / 2.0);
    cairo_arc(_context, 0.0, 0.0, 1.0, angle1 * CPM_D2R, angle2 * CPM_D2R);
    cairo_restore(_context);
    applyCairoPen();
    cairo_stroke(_context);
}

void
CairoPainter::drawArc(const Rectangle& rect, double angle1, double angle2)
{
    drawArc(rect.x(), rect.y(), rect.width(), rect.height(), angle1, angle2);
}

void
CairoPainter::drawChord(double x, double y, double width, double height, double angle1, double angle2, DrawingMode mode)
{
    ILOG_TRACE(ILX_CPAINTER);
    cairo_save(_context);
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
        cairo_translate(_context, _myWidget->absX() + x + width / 2.0, _myWidget->absY() + y + height / 2.0);
    else
        cairo_translate(_context, x + width / 2.0, y + height / 2.0);
    cairo_scale(_context, width / 2.0, height / 2.0);
    cairo_arc(_context, 0.0, 0.0, 1.0, angle1 * CPM_D2R, angle2 * CPM_D2R);
    cairo_close_path(_context);
    cairo_restore(_context);
    applyDrawingMode(mode);
}

void
CairoPainter::drawChord(const Rectangle& rect, double angle1, double angle2, DrawingMode mode)
{
    drawChord(rect.x(), rect.y(), rect.width(), rect.height(), angle1, angle2, mode);
}

void
CairoPainter::drawEllipse(double x, double y, double width, double height, DrawingMode mode)
{
    ILOG_TRACE(ILX_CPAINTER);
    cairo_save(_context);
    cairo_new_sub_path(_context);
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
        cairo_translate(_context, _myWidget->absX() + x + width / 2.0, _myWidget->absY() + y + height / 2.0);
    else
        cairo_translate(_context, x + width / 2.0, y + height / 2.0);
    cairo_scale(_context, width / 2.0, height / 2.0);
    cairo_arc(_context, 0.0, 0.0, 1.0, 0.0, 2 * CPM_PI);
    cairo_close_path(_context);
    cairo_restore(_context);
    applyDrawingMode(mode);
}

void
CairoPainter::drawEllipse(const Rectangle& rect, DrawingMode mode)
{
    drawEllipse(rect.x(), rect.y(), rect.width(), rect.height(), mode);
}

void
CairoPainter::drawLine(double x1, double y1, double x2, double y2, DrawingMode mode)
{
    ILOG_TRACE(ILX_CPAINTER);
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
    {
        cairo_move_to(_context, _myWidget->absX() + x1, _myWidget->absY() + y1);
        cairo_line_to(_context, _myWidget->absX() + x2, _myWidget->absY() + y2);
    } else
    {
        cairo_move_to(_context, x1, y1);
        cairo_line_to(_context, x2, y2);
    }
    applyDrawingMode(mode);
}

void
CairoPainter::drawLine(const Rectangle& rect, DrawingMode mode)
{
    drawLine(rect.x(), rect.y(), rect.bottom(), rect.right(), mode);
}

void
CairoPainter::drawLine(const Point& p1, const Point& p2, DrawingMode mode)
{
    drawLine(p1.x(), p1.y(), p2.x(), p2.y(), mode);
}

void
CairoPainter::drawPolygon(const Point* points, int pointCount, DrawingMode mode)
{
    ILOG_TRACE(ILX_CPAINTER);
    cairo_new_sub_path(_context);
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
    {
        cairo_move_to(_context, _myWidget->absX() + points[0].x(), _myWidget->absY() + points[0].y());
        for (int i = 1; i < pointCount; i++)
            cairo_line_to(_context, _myWidget->absX() + points[i].x(), _myWidget->absY() + points[i].y());
    } else
    {
        cairo_move_to(_context, points[0].x(), points[0].y());
        for (int i = 1; i < pointCount; i++)
            cairo_line_to(_context, points[i].x(), points[i].y());
    }
    cairo_close_path(_context);
    applyDrawingMode(mode);
}

void
CairoPainter::drawRectangle(double x, double y, double width, double height, DrawingMode mode)
{
    ILOG_TRACE(ILX_CPAINTER);
    if (mode != FillPath)
        getUserCoordinates(x, y, width, height);
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
        cairo_rectangle(_context, _myWidget->absX() + x, _myWidget->absY() + y, width, height);
    else
        cairo_rectangle(_context, x, y, width, height);
    applyDrawingMode(mode);
}

void
CairoPainter::drawRectangle(const Rectangle& rect, DrawingMode mode)
{
    drawRectangle(rect.x(), rect.y(), rect.width(), rect.height(), mode);
}

void
CairoPainter::drawRoundRectangle(double x, double y, double width, double height, int radius, DrawingMode mode, Corners corners)
{
    ILOG_TRACE(ILX_CPAINTER);
    if (mode != FillPath)
        getUserCoordinates(x, y, width, height);
    cairo_save(_context);
    cairo_new_sub_path(_context);

    // Top-right
    if (corners & TopRight)
    {
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
            cairo_arc(_context, _myWidget->absX() + x + width - radius, _myWidget->absY() + y + radius, radius, -1.57079628, 0);
        else
            cairo_arc(_context, x + width - radius, y + radius, radius, -1.57079628, 0);
    } else
    {
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
            cairo_line_to(_context, _myWidget->absX() + x + width, _myWidget->absY() + y);
        else
            cairo_line_to(_context, x + width, y);
    }

    // Bottom-right
    if (corners & BottomRight)
    {
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
            cairo_arc(_context, _myWidget->absX() + x + width - radius, _myWidget->absY() + y + height - radius, radius, 0, 1.57079628);
        else
            cairo_line_to(_context, x + width, y + height);
    } else
    {
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
            cairo_line_to(_context, _myWidget->absX() + x + width, _myWidget->absY() + y + height);
        else
            cairo_line_to(_context, x + width, y + height);
    }

    // Bottom-left
    if (corners & BottomLeft)
    {
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
            cairo_arc(_context, _myWidget->absX() + x + radius, _myWidget->absY() + y + height - radius, radius, 1.57079628, 3.14159265);
        else
            cairo_arc(_context, x + radius, y + height - radius, radius, 1.57079628, 3.14159265);
    } else
    {
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
            cairo_line_to(_context, _myWidget->absX() + x, _myWidget->absY() + y + height);
        else
            cairo_line_to(_context, x, y + height);
    }

    // Top-left
    if (corners & TopLeft)
    {
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
            cairo_arc(_context, _myWidget->absX() + x + radius, _myWidget->absY() + y + radius, radius, 3.14159265, 4.712388975);
        else
            cairo_arc(_context, x + radius, y + radius, radius, 3.14159265, 4.712388975);
    } else
    {
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
            cairo_line_to(_context, _myWidget->absX() + x, _myWidget->absY() + y);
        else
            cairo_line_to(_context, x, y);
    }

    cairo_close_path(_context);
    cairo_restore(_context);
    applyDrawingMode(mode);
}

void
CairoPainter::drawRoundRectangle(const Rectangle& rect, int radius, DrawingMode mode, Corners corners)
{
    drawRoundRectangle(rect.x(), rect.y(), rect.width(), rect.height(), radius, mode, corners);
}

void
CairoPainter::drawPoint(double x, double y)
{
    ILOG_TRACE(ILX_CPAINTER);
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
    {
        cairo_move_to(_context, _myWidget->absX() + x, _myWidget->absY() + y);
        cairo_line_to(_context, _myWidget->absX() + x, _myWidget->absY() + y);
    } else
    {
        cairo_move_to(_context, x, y);
        cairo_line_to(_context, x, y);
    }
    applyCairoPen();
}

Rectangle
CairoPainter::textExtents(const std::string& text) {

}

void
CairoPainter::drawText(const std::string& text, int x, int y)
{
    ILOG_TRACE(ILX_CPAINTER);
    if (text.empty())
        return;

    if (_state & PFActive)
    {
        applyCairoFont();
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
            cairo_move_to(_context, _myWidget->absX() + x, _myWidget->absY() + y);
        else
            cairo_move_to(_context, x, y);
        cairo_show_text(_context, text.c_str());
        applyCairoBrush();
    }
}

void
CairoPainter::drawLayout(const TextLayout& layout, int x, int y)
{
    ILOG_TRACE(ILX_CPAINTER);
    if (layout.text().empty())
        return;

    if (_state & PFActive)
    {
        applyCairoFont();
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
            layout.drawTextLayout(_context, _myWidget->absX() + x, _myWidget->absY() + y);
        else
            layout.drawTextLayout(_context, x, y);
        applyCairoBrush();
    }
}

void
CairoPainter::applyCairoBrush()
{
    if (!(_state & PFBrushActive) || _brush._modified)
    {
        _brush.applyBrush(_context);
        _state = (PainterFlags) (_state | PFBrushActive);
    }
}

void
CairoPainter::applyCairoPen()
{
    if ((_state & PFBrushActive) || _pen._modified)
    {
        _pen.applyPen(_context);
        _state = (PainterFlags) (_state & ~PFBrushActive);
    }
}

void
CairoPainter::applyCairoFont()
{
    if (_state & PFFontModified)
    {
        if (!_font.applyFont(_context))
            _myWidget->stylist()->defaultFont()->applyFont(_context);
    } else
        _myWidget->stylist()->defaultFont()->applyFont(_context);
}

void
CairoPainter::applyDrawingMode(DrawingMode mode)
{
    if (mode == AddPath)
        return;
    else if (mode == StrokePath)
    {
        applyCairoPen();
        cairo_stroke(_context);
    } else if (mode == FillPath)
    {
        applyCairoBrush();
        cairo_fill(_context);
    } else if (mode == StrokeAndFill)
    {
        applyCairoBrush();
        cairo_fill_preserve(_context);
        applyCairoPen();
        cairo_stroke(_context);
    } else if (mode == ClipPath)
        cairo_clip(_context);
}

void
CairoPainter::getUserCoordinates(double &x, double &y, double &width, double &height)
{
    int lw = _pen.lineWidth();
    double offset = .5 * lw;
    double offsetEnd = (lw > 1) ? lw : 1;
    x += offset;
    y += offset;
    width -= offsetEnd;
    height -= offsetEnd;
}

} /* namespace ilixi */
