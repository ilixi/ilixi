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

#ifndef NETMONITOR_H_
#define NETMONITOR_H_

#include <string>
#include <vector>

//! networkMonitor class parses network related /proc/net/dev filesystem file
/*!
 This monitor implements singleton pattern, therefore you should use its Instance() method for access.
 e.g. networkMonitor::Instance()->getUsage(); returns network usage in percentage
 */
class NETMonitor
{
public:
  NETMonitor();

  virtual
  ~NETMonitor();

  //! Returns difference of received bytes since last call in kilobytes
  std::string
  getReceiving();

  //! Returns difference of transmitted bytes since last call in kilobytes
  std::string
  getTransmitting();

  //! Returns total number of received bytes by all interfaces in megabytes
  std::string
  getTotalReceived();

  //! Returns total number of transmitted bytes by all interfaces in megabytes
  std::string
  getTotalTransmitted();

  //! Returns interface name and address as a formatted string
  std::string
  getConnection(unsigned int index = 0);

  //! Refresh readings
  void
  refresh();

private:
  struct interface
  {
    std::string name;
    std::string address;
    struct info
    {
      int bytesReceived;
      int packetsReceived;
      int errsReceived;
      int dropReceived;
      int bytesTransmitted;
      int packetsTransmitted;
      int errsTransmitted;
      int dropTransmitted;
      info();
    };
    info previous;
    info current;
    void
    parse(const char* line);
  };

  std::vector<interface> interfaces;

  void
  readInterfaces();

  void
  readInterfaceInfo();

  std::string
  formatBytes(unsigned int bytes);
};

#endif /* NETMONITOR_H_ */
