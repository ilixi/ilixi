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

#include "CPUMonitor.h"
#include <cstdlib>
#include <cstring>
#include <fstream>

CPUMonitor::CPUMonitor()
{
    _cpuCores = 0;
    readCpuInfo();
}

CPUMonitor::~CPUMonitor()
{
}

void
CPUMonitor::refresh()
{
    readTicks();
    readLoadAverages();
}

void
CPUMonitor::readTicks()
{
    if ((fd = fopen("/proc/stat", "r")) == 0)
    {
        printf("Unable to open file: /proc/stat\n");
        return;
    }
    if (!all.parse(fd))
    {
        printf("Parse failed\n");
        return;
    }

    for (unsigned int i = 0; i < _cpuCores; ++i)
        _cores[i].parse(fd);

    fclose(fd); // proc/stat
}

void
CPUMonitor::readCpuInfo()
{
    std::ifstream infile;
    infile.open("/proc/cpuinfo", std::ifstream::in);
    std::string line;
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
                i += 2;
            }
            if (readingItem)
                item.append(1, line[i]);
        }

        if (tagbuffer.find("vendor_id", 0) != std::string::npos)
            _vendor = item;

        else if (tagbuffer.find("model name", 0) != std::string::npos)
            _model = item;

        else if (tagbuffer.find("cpu MHz", 0) != std::string::npos)
            sscanf(item.c_str(), "%f", &_cpuMHz);

        else if (tagbuffer.find("cache size", 0) != std::string::npos)
            sscanf(item.c_str(), "%d", &_cacheSize);

        else if (tagbuffer.find("siblings", 0) != std::string::npos)
        {
            sscanf(item.c_str(), "%u", &_cpuCores);
            _cores.assign(_cpuCores, CPU());
            break;
        }
    }
    infile.close();

    if (_cpuCores == 0)
    {
        infile.open("/proc/stat", std::ifstream::in);
        while (infile.good())
        {
            std::string tagbuffer;
            getline(infile, line);

            if (strncmp(line.c_str(), "cpu", 3) == 0)
                ++_cpuCores;
        }
        if (_cpuCores--)
            _cores.assign(_cpuCores, CPU());
        infile.close();
    }
}

void
CPUMonitor::readLoadAverages()
{
    std::string line;
    std::ifstream infile;
    infile.open("/proc/loadavg", std::ifstream::in);
    if (infile.good())
    {
        char processBuffer[20];
        getline(infile, line);
        sscanf(line.c_str(), "%f%f%f%s%*s", &_loadAvg1, &_loadAvg5, &_loadAvg10, processBuffer);
        _processes = processBuffer;
    }
    infile.close();
}

int
CPUMonitor::getCacheSize() const
{
    return _cacheSize;
}

unsigned int
CPUMonitor::getCpuCores() const
{
    return _cpuCores;
}

float
CPUMonitor::getCpuMHz() const
{
    return _cpuMHz;
}

std::string
CPUMonitor::getModel() const
{
    return _model;
}

std::string
CPUMonitor::getVendor() const
{
    return _vendor;
}

float
CPUMonitor::getLoadAvg1() const
{
    return _loadAvg1;
}

float
CPUMonitor::getLoadAvg10() const
{
    return _loadAvg10;
}

float
CPUMonitor::getLoadAvg5() const
{
    return _loadAvg5;
}

std::string
CPUMonitor::getProcesses() const
{
    return _processes;
}

const CPUMonitor::CPU&
CPUMonitor::getCpu(unsigned int index) const
{
    if (index == 0)
        return all;
    else
    {
        if (index > _cores.size())
            index = _cores.size();
        return _cores.at(index - 1);
    }
}

CPUMonitor::CPU::Ticks::Ticks()
        : U(0),
          S(0),
          N(0),
          I(0)
{
}

bool
CPUMonitor::CPU::parse(FILE* fd)
{
    Ticks n; // new ticks
    if (fscanf(fd, "%*s%d%d%d%d", &n.U, &n.N, &n.S, &n.I) != 4)
    {
        printf("Parse failure\n");
        return false;
    }

    // shift readings
    previous = current;
    current = n;

    // ignore the rest of the line
    int ch;
    do
    {
        ch = getc(fd);
    } while (ch != '\n' && ch != EOF);
    return true;
}

int
CPUMonitor::CPU::getIdle() const
{
    int tot = current.elapsed() - previous.elapsed();
    return (tot > 0 ? (100 * (current.I - previous.I)) / tot : 0);
}

int
CPUMonitor::CPU::getUser() const
{
    int tot = current.elapsed() - previous.elapsed();
    return (tot > 0 ? (100 * (current.user() - previous.user())) / tot : 0);
}

int
CPUMonitor::CPU::getSystem() const
{
    int tot = current.elapsed() - previous.elapsed();
    return (tot > 0 ? (100 * (current.system() - previous.system())) / tot : 0);
}

int
CPUMonitor::CPU::getUsage() const
{
    int tot = current.elapsed() - previous.elapsed();
    return (tot > 0 ? (100 * (current.total() - previous.total())) / tot : 0);
}
