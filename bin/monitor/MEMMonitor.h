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

#ifndef MEMMONITOR_H_
#define MEMMONITOR_H_

//! memMonitor class parses memory related /proc/meminfo filesystem file
class MEMMonitor
{
public:
  MEMMonitor();

  virtual
  ~MEMMonitor();

  //! Returns amount of physical RAM, in megabytes, used for file memBuffers
  float
  getBuffers() const;

  //! Returns amount of physical RAM, in megabytes, used as cache memory.
  float
  getCached() const;

  //! Returns amount of physical RAM, in megabytes, left unused by the system.
  float
  getFree() const;

  //! Returns total amount of physical RAM, in megabytes.
  float
  getTotal() const;

  //! Returns memory usage in percentage.
  int
  getUsage() const;

  //! Refresh readings
  void
  refresh();

private:
  int _total; // Total amount of physical RAM, in kilobytes.
  int _free; // The amount of physical RAM, in kilobytes, left unused by the system.
  int _buffers; // The amount of physical RAM, in kilobytes, used for file memBuffers.
  int _cached; // The amount of physical RAM, in kilobytes, used as cache memory.

  void
  readMemInfo();
};

#endif /* MEMMONITOR_H_ */
