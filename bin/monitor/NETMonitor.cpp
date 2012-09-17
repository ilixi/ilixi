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

#include "NETMonitor.h"
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>

NETMonitor::NETMonitor()
{
  refresh();
  readInterfaces();
}

NETMonitor::~NETMonitor()
{
}

void
NETMonitor::refresh()
{
  readInterfaceInfo();
}

void
NETMonitor::readInterfaces()
{
  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST];
  if (getifaddrs(&ifaddr) == -1)
    perror("networkMonitor");

  interfaces.clear();
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
      family = ifa->ifa_addr->sa_family;

      if ((strcmp(ifa->ifa_name, "lo")) && (family == AF_INET))
        {
          s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
              NI_MAXHOST, NULL, 0, 1);
          interface c;
          c.name = std::string(ifa->ifa_name);
          c.address = std::string(host);
          interfaces.push_back(c);
        }
    }
  freeifaddrs(ifaddr);
}

void
NETMonitor::readInterfaceInfo()
{
  std::string line;
  std::ifstream infile;
  infile.open("/proc/net/dev", std::ifstream::in);
  getline(infile, line);
  getline(infile, line);
  while (infile.good())
    {
      std::string interface;
      std::string item;
      bool readingItem = false;
      getline(infile, line);
      for (unsigned int i = 0; i < line.length(); i++)
        {
          if (line[i] != ':' && readingItem == false)
            interface.append(1, line[i]);
          else if (line[i] == ':')
            {
              readingItem = true;
              i++;
            }
          if (readingItem)
            item.append(1, line[i]);
        }

      for (unsigned int i = 0; i < interfaces.size(); i++)
        if (interface.find(interfaces[i].name, 0) != std::string::npos)
          interfaces[i].parse(item.c_str());
    }
  infile.close();
}

std::string
NETMonitor::formatBytes(unsigned int value)
{
  float bytes = value;
  std::stringstream str;
  str.precision(3);
  if (bytes < 1024)
    str << bytes << "B";
  else if (bytes >= 1024 && bytes < 1048576)
    str << bytes / 1024 << "KB";
  else if (bytes >= 1048576 && bytes < 1073741824)
    str << bytes / 1048576 << "MB";
  else
    str << bytes / 1073741824 << "GB";
  return str.str();
}

std::string
NETMonitor::getConnection(unsigned int index)
{
  if (index >= interfaces.size())
    index = interfaces.size() - 1;
  std::string str;
  str.append(interfaces[index].name).append(":").append(
      interfaces[index].address);
  return str;
}

std::string
NETMonitor::getTotalReceived()
{
  unsigned int totalBytes = 0;
  for (unsigned int i = 0; i < interfaces.size(); i++)
    totalBytes += interfaces[i].current.bytesReceived;
  return formatBytes(totalBytes);
}

std::string
NETMonitor::getTotalTransmitted()
{
  unsigned int totalBytes = 0;
  for (unsigned int i = 0; i < interfaces.size(); i++)
    totalBytes += interfaces[i].current.bytesTransmitted;
  return formatBytes(totalBytes);
}

std::string
NETMonitor::getReceiving()
{
  int totalBytes = 0;
  for (unsigned int i = 0; i < interfaces.size(); i++)
    totalBytes += interfaces[i].current.bytesReceived
        - interfaces[i].previous.bytesReceived;
  return formatBytes(totalBytes).append("/s");
}

std::string
NETMonitor::getTransmitting()
{
  int totalBytes = 0;
  for (unsigned int i = 0; i < interfaces.size(); i++)
    totalBytes += interfaces[i].current.bytesTransmitted
        - interfaces[i].previous.bytesTransmitted;

  return formatBytes(totalBytes).append("/s");
}

NETMonitor::interface::info::info() :
  bytesReceived(0), packetsReceived(0), errsReceived(0), dropReceived(0),
      bytesTransmitted(0), packetsTransmitted(0), errsTransmitted(0),
      dropTransmitted(0)
{
}

void
NETMonitor::interface::parse(const char* item)
{
  info c;

  sscanf(item, "%d %d %d %d %*d %*d %*d %*d %d %d %d %d %*d %*d %*d %*d",
      &c.bytesReceived, &c.packetsReceived, &c.errsReceived, &c.dropReceived,
      &c.bytesTransmitted, &c.packetsTransmitted, &c.errsTransmitted,
      &c.dropTransmitted);

  previous = current;
  current = c;
}
