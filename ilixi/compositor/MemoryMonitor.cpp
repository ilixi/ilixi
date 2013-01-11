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

#include <compositor/MemoryMonitor.h>
#include <compositor/ApplicationManager.h>
#include <lib/Util.h>
#include <fstream>

namespace ilixi
{

MemoryMonitor::MemoryMonitor(ApplicationManager* manager, float memCritical, float memLow, long unsigned int pgCritical, long unsigned int pgLow)
        : _manager(manager),
          _memCritical(memCritical),
          _memLow(memLow),
          _pgCritical(pgCritical),
          _pgLow(pgLow),
          _state(Normal)
{
    _timer.start(4000);
    _timer.sigExec.connect(sigc::mem_fun(this, &MemoryMonitor::refresh));
}

MemoryMonitor::~MemoryMonitor()
{
}

void
MemoryMonitor::refresh()
{
    calcMemoryUsed();
    calcPageFaults();
    if (_state != Normal)
    {
        if (_state == Critical)
            _timer.setInterval(500);
        else if (_state == Low)
            _timer.setInterval(1000);
        sigStateChanged(_state);
        _timer.restart();
        _state = Normal;
    } else
        _timer.setInterval(4000);

}

void
MemoryMonitor::calcMemoryUsed()
{
    std::string line;
    std::ifstream infile;
    infile.open("/proc/meminfo", std::ifstream::in);
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
            sscanf(item.c_str(), "%d %*s", &_total);

        else if (tagbuffer.find("MemFree", 0) != std::string::npos)
            sscanf(item.c_str(), "%d %*s", &_free);

        else if (tagbuffer.find("Buffers", 0) != std::string::npos)
            sscanf(item.c_str(), "%d %*s", &_buffers);

        else if (tagbuffer.find("Cached", 0) != std::string::npos)
        {
            sscanf(item.c_str(), "%d %*s", &_cached);
            break;
        }

    }
    infile.close();

    unsigned int sum = _free + _buffers + _cached;
    if (sum > _total * _memCritical)
        _state = Critical;
    else if (sum > _total * _memLow)
        _state = Low;
}

void
MemoryMonitor::calcPageFaults()
{
    std::string line;
    std::ifstream infile;
    long unsigned int faults;
    long unsigned int avg = 0;
    for (AppInstanceList::iterator it = _manager->_instances.begin(); it != _manager->_instances.end(); ++it)
    {
        infile.open(PrintF("/proc/%d/stat", ((AppInstance*) *it)->pid()).c_str(), std::ifstream::in);
        getline(infile, line);
        sscanf(line.c_str(), "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %lu %*s", &faults);
        avg += faults;
        infile.close();
    }
    avg /= _manager->_instances.size();
    static long unsigned int previousAvg = avg;
    long unsigned int dif = previousAvg - avg;
    if (dif > _pgCritical)
        _state = Critical;
    else if (dif > _pgLow)
        _state = Low;
    previousAvg = avg;
}

} /* namespace ilixi */
