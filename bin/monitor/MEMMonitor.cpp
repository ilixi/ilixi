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

#include "MEMMonitor.h"
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

MEMMonitor::MEMMonitor()
{
  refresh();
}

MEMMonitor::~MEMMonitor()
{
}

void
MEMMonitor::refresh()
{
  readMemInfo();
}

void
MEMMonitor::readMemInfo()
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
}

float
MEMMonitor::getBuffers() const
{
  return _buffers / 1024.0;
}

float
MEMMonitor::getCached() const
{
  return _cached / 1024.0;
}

float
MEMMonitor::getFree() const
{
  return _free / 1024.0;
}

float
MEMMonitor::getTotal() const
{
  return _total / 1024.0;
}

int
MEMMonitor::getUsage() const
{
  return 100 * (_cached + 0.0) / _total;
}
