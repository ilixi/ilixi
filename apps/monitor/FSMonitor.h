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

#ifndef HDDMONITOR_H_
#define HDDMONITOR_H_

//! This class provides simple statistics using statvfs
class FSMonitor
{
public:
  FSMonitor();

  virtual
  ~FSMonitor();

  //! Returns hdd free space in gigabytes.
  float
  getFree() const;

  //! Returns hdd used space in gigabytes.
  float
  getUsed() const;

  //! Returns total available hdd space in gigabytes.
  float
  getTotal() const;

  //! Returns hdd usage in percentage.
  float
  getUsage() const;

  //! Sets filesystem path. e.g. /
  void
  setPath(const char* path);

  //! Refresh readings
  void
  refresh();

private:
  const char* _path;
  unsigned long long _total; /* total data blocks in file system */
  unsigned long long _free; /* free blocks in fs */
  unsigned long long _files; /* total file nodes in file system */
  unsigned long long _available; /* free blocks avail to non-superuser */
  unsigned long long _bsize; /* optimal transfer block size */

};

#endif /* HDDMONITOR_H_ */
