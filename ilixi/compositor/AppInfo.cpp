/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include <compositor/AppInfo.h>
#include <core/Logger.h>
#include <ilixiConfig.h>
#include <stdlib.h>
#include <string.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPINFO, "ilixi/compositor/AppInfo", "AppInfo");

unsigned int AppInfo::__appCounter = 0;

AppInfo::AppInfo()
        : _appID(__appCounter++),
          _name(""),
          _path(""),
          _args(""),
          _icon(""),
          _licence(""),
          _author(""),
          _category("default"),
          _version(0),
          _appFlags(APP_NONE),
          _depFlags(DEP_NONE)
{
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

std::string
AppInfo::category() const
{
    return _category;
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
AppInfo::setAppFlags(const std::string& appFlags)
{
    ILOG_DEBUG(ILX_APPINFO, "%s\n", appFlags.c_str());
    char* pch = strtok(const_cast<char*>(appFlags.c_str()), " ,");
    while (pch != NULL)
    {
        ILOG_DEBUG(ILX_APPINFO, " Token: %s\n", pch);
        if (strcmp(pch, "APP_NONE") == 0)
            _appFlags = APP_NONE;
        else if (strcmp(pch, "APP_NO_MAINWINDOW") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_NO_MAINWINDOW);
        else if (strcmp(pch, "APP_SPLASH_WINDOW") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_SPLASH_WINDOW);
        else if (strcmp(pch, "APP_ALLOW_WINDOW_CONFIG") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_ALLOW_WINDOW_CONFIG);
        else if (strcmp(pch, "APP_SURFACE_DONTBLOCK") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_SURFACE_DONTBLOCK);
        else if (strcmp(pch, "APP_ALLOW_MULTIPLE") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_ALLOW_MULTIPLE);
        else if (strcmp(pch, "APP_NEEDS_CLEAR") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_NEEDS_CLEAR);
        else if (strcmp(pch, "APP_NEEDS_BLENDING") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_NEEDS_BLENDING);
        else if (strcmp(pch, "APP_AUTO_START") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_AUTO_START);
        else if (strcmp(pch, "APP_USE_BACK") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_USE_BACK);
        else if (strcmp(pch, "APP_VIS_NOTIFY") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_VIS_NOTIFY);
        else if (strcmp(pch, "APP_STATUSBAR") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_STATUSBAR | APP_SYSTEM);
        else if (strcmp(pch, "APP_OSK") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_OSK);
        else if (strcmp(pch, "APP_HOME") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_HOME | APP_SYSTEM);
        else if (strcmp(pch, "APP_SYSTEM") == 0)
            _appFlags = (AppFlags) (_appFlags | APP_SYSTEM);
        pch = strtok(NULL, " ,");
    }
    ILOG_DEBUG(ILX_APPINFO, " _appFlags: %x\n", _appFlags);
}

void
AppInfo::setArgs(const std::string& args)
{
    if (args.empty())
        return;

    ILOG_DEBUG(ILX_APPINFO, "setArgs( %s )\n", args.c_str());
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
AppInfo::setDepFlags(const std::string& depFlags)
{
    ILOG_DEBUG(ILX_APPINFO, "%s\n", depFlags.c_str());
    char* pch = strtok(const_cast<char*>(depFlags.c_str()), " ,");
    while (pch != NULL)
    {
        if (strcmp(pch, "DEP_NONE") == 0)
            _depFlags = DEP_NONE;
        else if (strcmp(pch, "DEP_3D") == 0)
            _depFlags = (DependencyFlags) (_depFlags | DEP_3D);
        else if (strcmp(pch, "DEP_RC") == 0)
            _depFlags = (DependencyFlags) (_depFlags | DEP_RC);
        else if (strcmp(pch, "DEP_TOUCH") == 0)
            _depFlags = (DependencyFlags) (_depFlags | DEP_TOUCH);
        else if (strcmp(pch, "DEP_NETWORK") == 0)
            _depFlags = (DependencyFlags) (_depFlags | DEP_NETWORK);
        pch = strtok(NULL, " ,");
    }
    ILOG_DEBUG(ILX_APPINFO, " _depFlags: %x\n", _depFlags);
}

void
AppInfo::setIcon(const std::string& icon)
{
    if (icon.empty())
        return;

    ILOG_DEBUG(ILX_APPINFO, "setIcon( %s )\n", icon.c_str());
    size_t found = icon.find("@DATADIR:");
    if (found != std::string::npos)
    {
        _icon = ILIXI_DATADIR"apps/icons/";
        _icon.append(icon.substr(found + 9, std::string::npos));
    } else
        _icon = icon;
}

void
AppInfo::setLicence(const std::string& licence)
{
    if (licence.empty())
        return;

    ILOG_DEBUG(ILX_APPINFO, "setLicence( %s )\n", licence.c_str());
    _licence = licence;
}

void
AppInfo::setName(const std::string& name)
{
    if (name.empty())
        return;

    ILOG_DEBUG(ILX_APPINFO, "setName( %s )\n", name.c_str());
    _name = name;
}

void
AppInfo::setPath(const std::string& path)
{
    if (path.empty())
        return;

    ILOG_DEBUG(ILX_APPINFO, "setPath( %s )\n", path.c_str());
    _path = path;
}

void
AppInfo::setVersion(int version)
{
    _version = version;
}

void
AppInfo::setVersion(const std::string& version)
{
    if (version.empty())
        return;

    ILOG_DEBUG(ILX_APPINFO, "setVersion( %s )\n", version.c_str());
    _version = atoi(version.c_str());
}

void
AppInfo::setCategory(const std::string& category)
{
    if (category.empty())
        return;

    ILOG_DEBUG(ILX_APPINFO, "setCategory( %s )\n", category.c_str());
    _category = category;
}

} /* namespace ilixi */
