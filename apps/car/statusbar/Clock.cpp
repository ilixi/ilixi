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

#include "Clock.h"
#include <graphics/Painter.h>

namespace ilixi
{

const char* days[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const char* months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

Clock::Clock(Widget* parent)
        : Widget(parent),
          _time(NULL),
          _date(NULL),
          _timer(NULL)
{
    setConstraints(FixedConstraint, FixedConstraint);
    _box = new VBoxLayout();
    _box->setSpacing(0);
    _box->setHorizontalAlignment(Alignment::Center);

    _time = new Label("00:00");
    _time->setFont(new Font("decker", 24));
    _time->setLayoutAlignment(TextLayout::Center);

    _date = new Label("Frii181Nov111");
    _date->setSingleLine(true);
    _date->setLayoutAlignment(TextLayout::Center);

    updateTime();
    _box->addWidget(_time);
    _box->addWidget(_date);
    addChild(_box);

    _timer = new Timer();
    _timer->sigExec.connect(sigc::mem_fun(this, &Clock::updateTime));
    _timer->start(10000);

    sigGeometryUpdated.connect(sigc::mem_fun(this, &Clock::onClockGeomUpdate));
}

Clock::~Clock()
{
    delete _timer;
}

Size
Clock::preferredSize() const
{
    Size s = _box->preferredSize();
    return Size(s.width() + 10, s.height());
}

void
Clock::compose(const PaintEvent& event)
{
}

void
Clock::updateTime()
{
    struct timeval tv;
    struct tm* tm;
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    char time[6];
    sprintf(time, "%02d:%02d", tm->tm_hour, tm->tm_min);
    _time->setText(time);

    char date[12];
    sprintf(date, "%s %d %s", days[tm->tm_wday], tm->tm_mday, months[tm->tm_mon]);
    _date->setText(date);
}

void
Clock::onClockGeomUpdate()
{
    _box->setGeometry(0, 0, width(), height());
}

} /* namespace ilixi */
