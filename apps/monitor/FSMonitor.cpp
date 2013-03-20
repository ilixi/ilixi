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

#include "FSMonitor.h"
#include <sys/vfs.h>

FSMonitor::FSMonitor()
{
  setPath("/");
  refresh();
}

FSMonitor::~FSMonitor()
{
}

void
FSMonitor::setPath(const char* path)
{
  _path = path;
}

void
FSMonitor::refresh()
{
  struct statfs buf;
  statfs(_path, &buf);
  _free = buf.f_bfree;
  _bsize = buf.f_bsize;
  _available = buf.f_bavail;
  _files = buf.f_files;
  _total = buf.f_blocks;
}

float
FSMonitor::getTotal() const
{
  return _total * _bsize / 1073741824.0;
}

float
FSMonitor::getFree() const
{
  return _available * _bsize / 1073741824.0;
}

float
FSMonitor::getUsed() const
{
  return (_total - _free) * _bsize / 1073741824.0;
}

float
FSMonitor::getUsage() const
{
  return 100.0 * (_total - _free) / _total;
}
