/*
 Copyright 2010-2012 Tarik Sekmen.

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

#ifndef ILIXI_APPINFO_H_
#define ILIXI_APPINFO_H_

#include <string>
#include <sys/time.h>

namespace ilixi
{

typedef unsigned long AppID;

enum AppFlags
{
    APP_NONE = 0x000000, //!<

    // ui toolkits
    APP_LITE = 0x000001, //!<
    APP_ILIXI = 0x000002, //!<
    APP_QT = 0x000004, //!<
    APP_ANDROID = 0x000008, //!<
    APP_FLASH = 0x000010,
    APP_WEB = 0x000020,
    APP_FLTK = 0x000040, //!<
    APP_GTK = 0x000080, //!<
    APP_X11 = 0x000100,

    // capabilities
    APP_NO_MAINWINDOW = 0x001000,
    APP_SPLASH_WINDOW = 0x002000,
    APP_ALLOW_WINDOW_CONFIG = 0x004000,
    APP_SURFACE_DONTBLOCK = 0x008000,

    // constraints
    APP_ALLOW_MULTIPLE = 0x010000,  //!< Application can have multiple instances.
    APP_NEEDS_CLEAR = 0x020000,     //!< Background will be cleared with each update.
    APP_NEEDS_BLENDING = 0x040000,  //!< AppCompositors will set blending mode for each SurfaceView.
    APP_AUTO_START = 0x080000,      //!< Starts application when compositor is initialised.

    // special
    APP_STATUSBAR = 0x100000,
    APP_OSK = 0x200000,
    APP_HOME = 0x400000,
    APP_SYSTEM = 0x800000
};

enum DependencyFlags
{
    DEP_NONE = 0x0000,

    // Requirements
    DEP_3D = 0x0001, //!<
    DEP_RC = 0x0002, //!<
    DEP_TOUCH = 0x0004, //!<
    DEP_MOUSE = 0x0008 //!<

// Recommended
};

class AppInfo
{
public:
    AppInfo();

    ~AppInfo();

    AppFlags
    appFlags() const;

    AppID
    appID() const;

    std::string
    args() const;

    std::string
    author() const;

    std::string
    category() const;

    DependencyFlags
    depFlags() const;

    std::string
    icon() const;

    timeval
    installTime() const;

    std::string
    licence() const;

    std::string
    name() const;

    std::string
    path() const;

    int
    version() const;

    void
    setArgs(const std::string& args);

    void
    setAuthor(const std::string& author);

    void
    setAppFlags(AppFlags appFlags);

    void
    setAppFlags(const std::string& appFlags);

    void
    setDepFlags(DependencyFlags depFlags);

    void
    setDepFlags(const std::string& depFlags);

    void
    setIcon(const std::string& icon);

    void
    setLicence(const std::string& licence);

    void
    setName(const std::string& name);

    void
    setPath(const std::string& path);

    void
    setVersion(int version);

    void
    setVersion(const std::string& version);

    void
    setCategory(const std::string& category);

private:
    AppID _appID;
    std::string _name;
    std::string _path;
    std::string _args;
    std::string _icon;
    std::string _licence;
    std::string _author;
    std::string _category;
    int _version;
    AppFlags _appFlags;
    DependencyFlags _depFlags;
    timeval _installTime;

    static unsigned int __appCounter;
};

} /* namespace ilixi */
#endif /* ILIXI_APPINFO_H_ */
