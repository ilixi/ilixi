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

#include "BarChart.h"
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_BARCHART, "ilixi/monitor/BarChart", "BarChart");

BarChart::BarChart(unsigned int size, Widget* parent)
        : Widget(parent),
          _drawBackground(true),
          _size(size),
          _major(20)
{
    ILOG_TRACE_W(ILX_BARCHART);
    setConstraints(ExpandingConstraint, ExpandingConstraint);
}

BarChart::~BarChart()
{
    ILOG_TRACE_W(ILX_BARCHART);
}

void
BarChart::addBar(const std::string& name, const Color& color)
{
    ILOG_TRACE_W(ILX_BARCHART);
    BarData bar;
    bar.name = name;
    bar.values = BarList(_size, 0);
    bar.color = color;
    _data.push_back(bar);
}

void
BarChart::addValue(unsigned int bar, float value)
{
    ILOG_TRACE_W(ILX_BARCHART);
    if (bar < _data.size())
    {
        if (_data[bar].values.size() == _size)
            _data[bar].values.pop_back();
        _data[bar].values.push_front(value);
        ILOG_DEBUG(ILX_BARCHART, " -> bar %u value %f\n", bar, value);
        update();
    }
}

void
BarChart::setDrawBG(bool drawBackground)
{
    _drawBackground = drawBackground;
}

void
BarChart::setMajorTicks(unsigned int major)
{
    _major = major;
}

void
BarChart::compose(const PaintEvent& event)
{
    if (!_data.size())
        return;

    Painter p(this);
    p.begin(event);
    if (_drawBackground)
    {
        p.setBrush(Color(0, 0, 0, 128));
        p.fillRectangle(0, 0, width(), height());
    }
    float w;
    if (_data.size() == 1)
        w = (width() - _size * 1.0) / _size;
    else
        w = width() / (_size * _data.size() + .0);
    float vCoef = height() / 100.0;

    if (_major)
    {
        for (int i = 0; i < 100 / _major; ++i)
        {
            p.setBrush(Color(255, 255, 255, 20));
            p.fillRectangle(0, _major * i * vCoef, width(), 1, (DFBSurfaceDrawingFlags) (DSDRAW_SRC_PREMULTIPLY | DSDRAW_BLEND));
        }
    }

    ILOG_TRACE_W(ILX_BARCHART);
    float x = 0;
    int h = 0;
    for (unsigned int i = 0; i < _data.size(); ++i)
    {
        x = i * w;
        p.setBrush(_data[i].color);
        for (BarList::reverse_iterator it = _data[i].values.rbegin(); it != _data[i].values.rend(); ++it)
        {
            h = *it * vCoef;
            if (h > 0)
                p.fillRectangle(x, height() - h, w, h, (DFBSurfaceDrawingFlags) (DSDRAW_SRC_PREMULTIPLY | DSDRAW_BLEND));
            if (_data.size() == 1)
                x += w + 1;
            else
                x += w * _data.size();
        }

    }
}

} /* namespace ilixi */
