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

#ifndef STATUSBAR_H_
#define STATUSBAR_H_

#include <core/Application.h>
#include <ui/Label.h>
#include <ui/ListBox.h>
#include <core/DaleDFB.h>
#include <lib/Timer.h>

#include "CPUMonitor.h"
#include "BarChart.h"

#include "NotificationIcon.h"

namespace ilixi
{

class PStatusBar : public Application
{
public:
    PStatusBar(int argc, char* argv[]);

    virtual
    ~PStatusBar();

protected:
    void
    onShow();

    void
    showApp(const std::string& title);

    void
    compose(const PaintEvent& event);

    void
    timerSlot();

private:
    Image* _bg;
    Image* _listBG;
    Font* _listFont;

    NotificationIcon* _headset;
    NotificationIcon* _mic;
    NotificationIcon* _vol;

    ListBox* _list;

    IComaComponent* _soundComponent;
    IComaComponent* _compComponent;

    Timer* _timer;
    CPUMonitor* _cpuMon;
    BarChart* _cpuChart;

    Label* _fpsLabel;

    void
    showSwitcher();

    friend void
    volumeListener(void* ctx, void* arg);

    friend void
    appStatusChanged(void* ctx, void* arg);
};

}
#endif /* STATUSBAR_H_ */
