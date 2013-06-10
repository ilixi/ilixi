/*
 Copyright 2010, 2011 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MONITOR_H_
#define MONITOR_H_

#include <core/Application.h>
#include <ui/ProgressBar.h>
#include <ui/Label.h>
#include <lib/Timer.h>

#include "CPUMonitor.h"
#include "FSMonitor.h"
#include "MEMMonitor.h"
#include "NETMonitor.h"
#include "OSMonitor.h"
#include "BarChart.h"

using namespace ilixi;

class Monitor : public Application
{
public:
    Monitor(int argc, char* argv[]);

    virtual
    ~Monitor();

protected:
    void
    updateMonitor();

private:

    Timer* _monitorTimer;

    CPUMonitor* _cpuMon;
    Label* _uptime;
    ProgressBar* _cpuIdle;
    BarChart* _cpuChart;

    FSMonitor* _fsMon;
    ProgressBar* _fsUsage;

    MEMMonitor* _memMon;
    ProgressBar* _memUsage;

    NETMonitor* _netMon;
    Label* _netRXC;
    Label* _netTXC;
    Label* _netRXT;
    Label* _netTXT;

    OSMonitor* _osMon;
};

#endif /* MONITOR_H_ */
