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

#ifndef OSMONITOR_H_
#define OSMONITOR_H_

#include <string>

class OSMonitor
{
public:
  OSMonitor();

  virtual
  ~OSMonitor();

  //! Refresh readings
  void
  refresh();

  std::string
  getMachine() const;

  std::string
  getNodeName() const;

  std::string
  getRelease() const;

  std::string
  getSysName() const;

  std::string
  getVersion() const;

  //! Returns formatted uptime.
  std::string
  getUptime();

  //! Returns formatted amount of time spent in idle process.
  std::string
  getUptimeIdle();

private:

  //! The uptime of the system (seconds)
  unsigned long _uptime;

  //! The amount of time spent in idle process (seconds)
  unsigned long _uptimeIdle;

  std::string _sysName;

  std::string _nodeName;

  std::string _release;

  std::string _version;

  std::string _machine;

  void
  readUptime();

  std::string
  formatting(unsigned long value);

  std::string
  timeFormatting(unsigned long seconds);
};

#endif /* OSMONITOR_H_ */
