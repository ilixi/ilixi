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

#ifndef ILIXI_MEMORYMONITOR_H_
#define ILIXI_MEMORYMONITOR_H_

#include <lib/Timer.h>
#include <sigc++/signal.h>

namespace ilixi
{

/*!
 *
 */
class ApplicationManager;

class MemoryMonitor
{
public:
    enum MemoryState
    {
        Critical,
        Low,
        Normal,
    };

    MemoryMonitor(ApplicationManager* manager, float memCritical, float memLow, long unsigned int pgCritical, long unsigned int pgLow);

    virtual
    ~MemoryMonitor();

    sigc::signal<void, MemoryState> sigStateChanged;

private:
    ApplicationManager* _manager;
    float _memCritical;
    float _memLow;
    long unsigned int _pgCritical;
    long unsigned int _pgLow;

    MemoryState _state;
    Timer _timer;
    unsigned int _total;
    unsigned int _free;
    unsigned int _cached;
    unsigned int _buffers;

    void
    refresh();

    void
    calcMemoryUsed();

    void
    calcPageFaults();
};

} /* namespace ilixi */
#endif /* ILIXI_MEMORYMONITOR_H_ */
