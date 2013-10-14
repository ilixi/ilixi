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
#include <core/Logger.h>

namespace ilixi
{

//! PI
const double M_PI = 3.14159265;

//! PI /180 (Used for conversion from degrees to radians)
const double M_D2R = 0.0174532925;

D_DEBUG_DOMAIN(ILX_CPAINTER, "ilixi/graphics/CairoPainter", "CairoPainter");

CairoPainter::CairoPainter(Widget* widget)
        : _myWidget(widget),
          _antiAliasMode(AliasSubPixel),
          _state(None)
{
    _context = _myWidget->surface()->cairoContext();
    cairo_set_antialias(_context, (cairo_antialias_t) _antiAliasMode);
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
    _state = Active;
    _myWidget->surface()->dfbSurface()->SetDrawingFlags(_myWidget->surface()->dfbSurface(), DSDRAW_NOFX);
    _myWidget->surface()->dfbSurface()->SetPorterDuff(_myWidget->surface()->dfbSurface(), DSPD_SRC_OVER);
}

void
CairoPainter::end()
{
    if (_state & Active)
    {
        ILOG_TRACE(ILX_CPAINTER);
        _state = None;
        _myWidget->surface()->unlock();
    }
}

void
CairoPainter::setBrush(const Brush& brush)
{
    _brush = brush;
    _brush.applyBrush(_context);
    ILOG_DEBUG(ILX_CPAINTER, "setBrush() %p\n", this);
}

void
CairoPainter::setPen(const Pen& pen)
{
    _pen = pen;
    ILOG_DEBUG(ILX_CPAINTER, "setPen() %p\n", this);
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
    cairo_save(_context);
    cairo_translate(_context, x + width / 2.0, y + height / 2.0);
    cairo_scale(_context, width / 2.0, height / 2.0);
    cairo_arc(_context, 0.0, 0.0, 1.0, angle1 * M_D2R, angle2 * M_D2R);
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
    cairo_save(_context);
    cairo_translate(_context, x + width / 2.0, y + height / 2.0);
    cairo_scale(_context, width / 2.0, height / 2.0);
    cairo_arc(_context, 0.0, 0.0, 1.0, angle1 * M_D2R, angle2 * M_D2R);
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
    cairo_save(_context);
    cairo_new_sub_path(_context);
    cairo_translate(_context, x + width / 2.0, y + height / 2.0);
    cairo_scale(_context, width / 2.0, height / 2.0);
    cairo_arc(_context, 0.0, 0.0, 1.0, 0.0, 2 * M_PI);
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
    cairo_move_to(_context, x1, y1);
    cairo_line_to(_context, x2, y2);
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
    cairo_new_sub_path(_context);
    cairo_move_to(_context, points[0].x(), points[0].y());
    for (int i = 1; i < pointCount; i++)
        cairo_line_to(_context, points[i].x(), points[i].y());
    cairo_close_path(_context);
    applyDrawingMode(mode);
}

void
CairoPainter::drawRectangle(double x, double y, double width, double height, DrawingMode mode)
{
    if (mode != FillPath)
        getUserCoordinates(x, y, width, height);
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
    if (mode != FillPath)
        getUserCoordinates(x, y, width, height);
    cairo_save(_context);
    cairo_new_sub_path(_context);

    // Top-right
    if (corners & TopRight)
        cairo_arc(_context, x + width - radius, y + radius, radius, -1.57079628, 0);
    else
        cairo_line_to(_context, x + width, y);

    // Bottom-right
    if (corners & BottomRight)
        cairo_arc(_context, x + width - radius, y + height - radius, radius, 0, 1.57079628);
    else
        cairo_line_to(_context, x + width, y + height);

    // Bottom-left
    if (corners & BottomLeft)
        cairo_arc(_context, x + radius, y + height - radius, radius, 1.57079628, 3.14159265);
    else
        cairo_line_to(_context, x, y + height);

    // Top-left
    if (corners & TopLeft)
        cairo_arc(_context, x + radius, y + radius, radius, 3.14159265, 4.712388975);
    else
        cairo_line_to(_context, x, y);

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
    cairo_move_to(_context, x, y);
    cairo_line_to(_context, x, y);
    cairo_stroke(_context);
}

void
CairoPainter::applyCairoBrush()
{
    if (!(_state & BrushActive) || _brush._modified)
    {
        _brush.applyBrush(_context);
        _state = (PainterFlags) (_state | BrushActive);
    }
}

void
CairoPainter::applyCairoPen()
{
    if ((_state & BrushActive) || _pen._modified)
    {
        _pen.applyPen(_context);
        _state = (PainterFlags) (_state & ~BrushActive);
    }
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
