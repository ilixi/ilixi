/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#include "StatusBar.h"
#include <ui/HBoxLayout.h>
#include <ui/VBoxLayout.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <sigc++/sigc++.h>
#include <string.h>

D_DEBUG_DOMAIN( ILX_STATUSBAR, "ilixi/StatusBar", "StatusBar");

const char* days[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const char* months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
        "Aug", "Sep", "Oct", "Nov", "Dec" };

DateThread::DateThread(StatusBar* parent)
        : Thread(), _parent(parent)
{
}

DateThread::~DateThread()
{
}

int
DateThread::run()
{
    while (1)
    {
        pthread_testcancel();
        _parent->updateTime();
        sleep(1);
    }
    return 0;
}

//*********************************************************

void
volumeListener(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    char volText[3];
    sprintf(volText, "%d", *((int*) arg));
    bar->_volume->setText(volText);
    ILOG_DEBUG(ILX_STATUSBAR, "Volume %d\n", *((int*) arg));
}

StatusBar::StatusBar(int argc, char* argv[])
        : Application(&argc, &argv, OptStatusBar), _dateThread(NULL)
{
    setTitle("StatusBar");
    setBackgroundFilled(true);
    setMargins(5, 5, 10, 5);
    HBoxLayout* mainLayout = new HBoxLayout();
    mainLayout->setVerticalAlignment(Alignment::Middle);
    setLayout(mainLayout);

    _app = new Label("dfb_ilixi");
    _app->setLayoutAlignment(TextLayout::Left);
    _app->setXConstraint(ExpandingConstraint);
    _app->setFont(new Font(ILIXI_DATADIR"fonts/decker.ttf", 36));
    addWidget(_app);

    _volume = new Label("  0  ");
    _volume->setXConstraint(ExpandingConstraint);
    _volume->setLayoutAlignment(TextLayout::Left);
    _volume->setFont(new Font(ILIXI_DATADIR"fonts/decker.ttf", 36));
    addWidget(_volume);

    _time = new Label("00:00:00");
    _date = new Label("Fri 18 Nove");

    VBoxLayout* dateBox = new VBoxLayout();
    dateBox->addWidget(_time);
    dateBox->addWidget(_date);
    addWidget(dateBox);

    sigVisible.connect(sigc::mem_fun(this, &StatusBar::onShow));
    sigHidden.connect(sigc::mem_fun(this, &StatusBar::onHide));

    _dateThread = new DateThread(this);
}

StatusBar::~StatusBar()
{
    delete _dateThread;
}

void
StatusBar::onHide()
{
    _dateThread->cancel();
    _soundComponent->Release(_soundComponent);
}

void
StatusBar::onShow()
{
    _dateThread->start();
    comaGetComponent("SoundComponent", &_soundComponent);
    _soundComponent->Listen(_soundComponent, 0, volumeListener, this);
}

void
StatusBar::updateTime()
{
    struct timeval tv;
    struct tm* tm;
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    char time[9];
    sprintf(time, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
    _time->setText(time);

    char date[12];
    sprintf(date, "%s %d %s ", days[tm->tm_wday], tm->tm_mday,
            months[tm->tm_mon]);
    _date->setText(date);
}

void
StatusBar::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
    painter.setBrush(Color(60, 60, 60, 127));
    painter.fillRectangle(0, 0, width(), height());
    painter.setPen(Color(0, 0, 0));
    painter.drawRectangle(0, 0, width(), height());
    painter.end();
}

int
main(int argc, char* argv[])
{
    StatusBar app(argc, argv);
    app.exec();
    return 0;
}
