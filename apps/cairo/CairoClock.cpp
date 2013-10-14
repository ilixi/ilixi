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

#include "CairoClock.h"
#include <graphics/CairoPainter.h>
#include <types/LinearGradient.h>
#include <algorithm>
#include <math.h>

namespace ilixi
{

const double M_D2R = 0.0174532925;  // degrees to radians

CairoClock::CairoClock(Widget* parent)
        : Widget(parent)
{
    setConstraints(MinimumConstraint, MinimumConstraint);
    _timer.sigExec.connect(sigc::mem_fun(this, &CairoClock::updateClock));
    _timer.start(100);
}

CairoClock::~CairoClock()
{
}

Size
CairoClock::preferredSize() const
{
    return Size(150, 150);
}

void
CairoClock::compose(const PaintEvent& event)
{
    int diameter = std::min(width(), height()) - 10;
    int radius = diameter / 2.0 - 4;
    int x = (width() - diameter) / 2.0;
    int y = (height() - diameter) / 2.0;

    int cx = width() / 2;
    int cy = height() / 2;

    Pen pen1;
    pen1.setLineWidth(5);
    pen1.setLineCapStyle(Pen::RoundCap);

    Pen pen2;
    pen2.setLineWidth(10);
    pen2.setLineCapStyle(Pen::RoundCap);

    LinearGradient border(x, y, diameter, diameter);
    border.addStop(Color(128, 128, 128), 0);
    border.addStop(Color(10, 10, 10), 1);
    Brush brush1;
    brush1.setGradient(border);

    CairoPainter p(this);
    p.begin(event);

    // frame
    p.setPen(pen1);
    p.setBrush(brush1);
    p.drawEllipse(x, y, diameter, diameter, CairoPainter::StrokeAndFill);
    p.setBrush(Color(255, 255, 255, 200));
    p.drawEllipse(x + 15, y + 15, diameter - 30, diameter - 30, CairoPainter::StrokeAndFill);

    // ticks
    int inset;
    for (int i = 0; i < 12; i++)
    {
        if (i % 3 != 0)
        {
            inset = 20;
            p.setPen(pen1);
        } else
        {
            inset = 40;
            p.setPen(pen2);
        }

        p.drawLine(cx + radius * cos(i * M_PI / 6), cy + radius * sin(i * M_PI / 6), cx + (radius - inset) * cos(i * M_PI / 6), cy + (radius - inset) * sin(i * M_PI / 6));
    }

    // store the current time
    time_t rawtime;
    time(&rawtime);
    struct tm * timeinfo = localtime(&rawtime);

    // compute the angles of hands
    double hours = timeinfo->tm_hour;
    if (hours >= 12)
        hours -= 12;
    hours = ((hours * 30 + timeinfo->tm_min / 2.0) - 90) * M_D2R;
    double minutes = (timeinfo->tm_min * 6 - 90) * M_D2R;
    double seconds = (timeinfo->tm_sec * 6 - 90) * M_D2R;

    p.setPen(pen2);
    p.drawLine(cx, cy, cx + cos(hours) * (radius * 0.5), cy + sin(hours) * (radius * 0.5));
    p.drawLine(cx, cy, cx + cos(minutes) * (radius * 0.7), cy + sin(minutes) * (radius * 0.7));

    p.setPen(Color(255, 0, 0));
    p.drawLine(cx, cy, cx + cos(seconds) * (radius * 0.7), cy + sin(seconds) * (radius * 0.7));

    p.setBrush(Color(0, 0, 0));
    p.drawEllipse(cx - 10, cy - 10, 20, 20, CairoPainter::FillPath);

    p.end();
}

void
CairoClock::updateClock()
{
    update();
}

} /* namespace ilixi */
