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

#include <compositor/MemoryMonitor.h>
#include <compositor/ApplicationManager.h>
#include <lib/Util.h>
#include <core/Logger.h>
#include <fstream>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_MEMORYMONITOR, "ilixi/compositor/MemMon", "MemoryMonitor");

MemoryMonitor::MemoryMonitor(ApplicationManager* manager, float memCritical, float memLow, long unsigned int pgCritical, long unsigned int pgLow)
        : _manager(manager),
          _memCritical(memCritical),
          _memLow(memLow),
          _pgCritical(pgCritical),
          _pgLow(pgLow),
          _pgPre(0),
          _state(Normal)
{
    ILOG_TRACE_F(ILX_MEMORYMONITOR);
    _timer.start(4000);
    _timer.sigExec.connect(sigc::mem_fun(this, &MemoryMonitor::refresh));
}

MemoryMonitor::~MemoryMonitor()
{
    ILOG_TRACE_F(ILX_MEMORYMONITOR);
}

float
MemoryMonitor::getMemCritical() const
{
    return _memCritical;
}

void
MemoryMonitor::setMemCritical(float memCritical)
{
    _memCritical = memCritical;
}

float
MemoryMonitor::getMemLow() const
{
    return _memLow;
}

void
MemoryMonitor::setMemLow(float memLow)
{
    _memLow = memLow;
}

int
MemoryMonitor::getPgCritical() const
{
    return _pgCritical;
}

void
MemoryMonitor::setPgCritical(int pgCritical)
{
    _pgCritical = pgCritical;
}

int
MemoryMonitor::getPgLow() const
{
    return _pgLow;
}

void
MemoryMonitor::setPgLow(int pgLow)
{
    _pgLow = pgLow;
}

MemoryMonitor::MemoryState
MemoryMonitor::getState() const
{
    return _state;
}

void
MemoryMonitor::refresh()
{
    ILOG_TRACE_F(ILX_MEMORYMONITOR);
    calcMemoryUsed();
    calcPageFaults();
    if (_state != Normal)
    {
        if (_state == Critical)
            _timer.setInterval(2000);
        else if (_state == Low)
            _timer.setInterval(3000);
        sigStateChanged(_state);
        _timer.restart();
        _state = Normal;
    } else
        _timer.setInterval(4000);
}

void
MemoryMonitor::calcMemoryUsed()
{
    ILOG_TRACE_F(ILX_MEMORYMONITOR);
    std::string line;
    std::ifstream infile;
    infile.open("/proc/meminfo", std::ifstream::in);
    unsigned int total;
    unsigned int free;
    unsigned int cached;
    unsigned int buffers;

    while (infile.good())
    {
        std::string tagbuffer;
        std::string item;
        bool readingItem = false;
        getline(infile, line);
        for (unsigned int i = 0; i < line.length(); i++)
        {
            if (line[i] != ':' && readingItem == false)
                tagbuffer.append(1, line[i]);
            else if (line[i] == ':')
            {
                readingItem = true;
                i++;
            }
            if (readingItem)
                item.append(1, line[i]);
        }

        if (tagbuffer.find("MemTotal", 0) != std::string::npos)
            sscanf(item.c_str(), "%u %*s", &total);

        else if (tagbuffer.find("MemFree", 0) != std::string::npos)
            sscanf(item.c_str(), "%u %*s", &free);

        else if (tagbuffer.find("Buffers", 0) != std::string::npos)
            sscanf(item.c_str(), "%u %*s", &buffers);

        else if (tagbuffer.find("Cached", 0) != std::string::npos)
        {
            sscanf(item.c_str(), "%u %*s", &cached);
            break;
        }

    }
    infile.close();

    double used = (buffers + cached + .0) / total;
    ILOG_DEBUG(ILX_MEMORYMONITOR, " -> Used: %f\n", used);
    if (used > _memCritical)
        _state = Critical;
    else if (used > _memLow)
        _state = Low;
}

void
MemoryMonitor::calcPageFaults()
{
    ILOG_TRACE_F(ILX_MEMORYMONITOR);
    std::string line;
    std::ifstream infile;
    int faults;
    int sum = 0;

    for (AppInstanceList::iterator it = _manager->_instances.begin(); it != _manager->_instances.end(); ++it)
    {
        infile.open(PrintF("/proc/%d/stat", ((AppInstance*) *it)->pid()).c_str(), std::ifstream::in);
        getline(infile, line);
        sscanf(line.c_str(), "%*d %*s %*c %*d %*d %*d %*d %*d %*d %*d %*d %d %*s", &faults);
        sum += faults;
        infile.close();
        ILOG_DEBUG(ILX_MEMORYMONITOR, "   -> %s [%d] faults: %d\n", ((AppInstance*) *it)->appInfo()->name().c_str(), ((AppInstance*) *it)->pid(), faults);
    }

    ILOG_DEBUG(ILX_MEMORYMONITOR, " -> sum: %d\n", sum);

    int dif = sum - _pgPre;
    if (dif > _pgCritical)
        _state = Critical;
    else if (dif > _pgLow)
        _state = Low;
    ILOG_DEBUG(ILX_MEMORYMONITOR, " -> dif. page_faults: %d\n", dif);
    _pgPre = sum;
}

} /* namespace ilixi */
