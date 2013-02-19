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

#include "OSMonitor.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/utsname.h>

OSMonitor::OSMonitor()
{
  utsname _test;
  if (uname(&_test) == 0)
    {
      _sysName = _test.sysname;
      _nodeName = _test.nodename;
      _release = _test.release;
      _version = _test.version;
      _machine = _test.machine;
    }
}

OSMonitor::~OSMonitor()
{
}

void
OSMonitor::refresh()
{
  readUptime();
}

std::string
OSMonitor::getMachine() const
{
  return _machine;
}

std::string
OSMonitor::getNodeName() const
{
  return _nodeName;
}

std::string
OSMonitor::getRelease() const
{
  return _release;
}

std::string
OSMonitor::getSysName() const
{
  return _sysName;
}

std::string
OSMonitor::getVersion() const
{
  return _version;
}

std::string
OSMonitor::getUptime()
{
  return timeFormatting(_uptime);
}

std::string
OSMonitor::getUptimeIdle()
{
  return timeFormatting(_uptimeIdle);
}

std::string
OSMonitor::formatting(unsigned long value)
{
  std::stringstream str;
  if (value == 0)
    str << "00";
  else if (value < 10)
    str << "0" << value;
  else
    str << value;
  return str.str();
}

std::string
OSMonitor::timeFormatting(unsigned long seconds)
{
  std::stringstream str;
  if (seconds > 0)
    {
      str << formatting((unsigned long) (seconds / 3600)) << ":";
      str << formatting((unsigned long) ((seconds % 3600) / 60)) << ":";
      str << formatting((unsigned long) ((seconds % 3600) % 60));
      return str.str();
    }
  else
    return "00:00:00";
}

void
OSMonitor::readUptime()
{
  std::string line;
  std::ifstream infile;
  infile.open("/proc/uptime", std::ifstream::in);
  if (infile.good())
    {
      getline(infile, line);
      sscanf(line.c_str(), "%lu%lu", &_uptime, &_uptimeIdle);
    }
  infile.close();
}
