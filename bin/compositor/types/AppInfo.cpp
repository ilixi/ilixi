/*
 Copyright 2012 Tarik Sekmen.

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

#include "types/AppInfo.h"

namespace ilixi
{

  unsigned int AppInfo::__appCounter = 0;

  AppInfo::AppInfo() :
      _appID(__appCounter++), _name(""), _path(""), _args(""), _icon(""), _licence(
          ""), _author(""), _version(0), _appFlags(APP_NONE), _depFlags(
          DEP_NONE)
  {
    gettimeofday(&_installTime, NULL);
  }

  AppInfo::~AppInfo()
  {
  }

  AppFlags
  AppInfo::appFlags() const
  {
    return _appFlags;
  }

  AppID
  AppInfo::appID() const
  {
    return _appID;
  }

  std::string
  AppInfo::args() const
  {
    return _args;
  }

  std::string
  AppInfo::author() const
  {
    return _author;
  }

  DependencyFlags
  AppInfo::depFlags() const
  {
    return _depFlags;
  }

  std::string
  AppInfo::icon() const
  {
    return _icon;
  }

  timeval
  AppInfo::installTime() const
  {
    return _installTime;
  }

  std::string
  AppInfo::licence() const
  {
    return _licence;
  }

  std::string
  AppInfo::name() const
  {
    return _name;
  }

  std::string
  AppInfo::path() const
  {
    return _path;
  }

  int
  AppInfo::version() const
  {
    return _version;
  }

  void
  AppInfo::setAppFlags(AppFlags appFlags)
  {
    _appFlags = appFlags;
  }

  void
  AppInfo::setArgs(const std::string& args)
  {
    _args = args;
  }

  void
  AppInfo::setAuthor(const std::string& author)
  {
    _author = author;
  }

  void
  AppInfo::setDepFlags(DependencyFlags depFlags)
  {
    _depFlags = depFlags;
  }

  void
  AppInfo::setIcon(const std::string& icon)
  {
    _icon = icon;
  }

  void
  AppInfo::setLicence(const std::string& licence)
  {
    _licence = licence;
  }

  void
  AppInfo::setName(const std::string& name)
  {
    _name = name;
  }

  void
  AppInfo::setPath(const std::string& path)
  {
    _path = path;
  }

  void
  AppInfo::setVersion(int version)
  {
    _version = version;
  }

} /* namespace ilixi */
