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

#ifndef ILIXI_APPINFO_H_
#define ILIXI_APPINFO_H_

#include <string>
#include <sys/time.h>

namespace ilixi
{

typedef unsigned long AppID;

/*!
 * This enum specifies modes and behaviour for an application.
 */
enum AppFlags
{
    APP_NONE = 0x000000,                //!<
    APP_NO_MAINWINDOW = 0x000001,       //!< Application uses a surface instead.
    APP_SPLASH_WINDOW = 0x000002,       //!< Application has a splash window.
    APP_ALLOW_WINDOW_CONFIG = 0x000004, //!< Application can modify its windows.
    APP_SURFACE_DONTBLOCK = 0x000008,   //!< Application is not blocked when hidden.
    APP_ALLOW_MULTIPLE = 0x000100,      //!< Application can have multiple instances.
    APP_NEEDS_CLEAR = 0x000200,         //!< Background will be cleared with each update.
    APP_NEEDS_BLENDING = 0x000400,      //!< AppCompositors will set blending mode for each SurfaceView.
    APP_AUTO_START = 0x000800,          //!< Starts application when compositor is initialised.
    APP_USE_BACK = 0x001000,            //!< Application can use a Back button.
    APP_VIS_NOTIFY = 0x002000,          //!< Application requires a visibility feedback from compositor.
    APP_STATUSBAR = 0x010000,           //!< Statusbar application.
    APP_OSK = 0x020000,                 //!< OSK application.
    APP_HOME = 0x040000,                //!< Home application.
    APP_SYSTEM = 0x080000               //!< System application (not shown on home).
};

/*!
 * This enum specifies required hardware and features for an application.
 */
enum DependencyFlags
{
    DEP_NONE = 0x0000,      //!< DEP_NONE
    DEP_3D = 0x0001,        //!< Application requires stereoscopy support.
    DEP_RC = 0x0002,        //!< Application requires RC navigation.
    DEP_TOUCH = 0x0004,     //!< Application requires touch or pointer input.
    DEP_NETWORK = 0x0008    //!< Application needs network access.

};

//! Provides information about an application.
class AppInfo
{
public:
    /*!
     * Constructor.
     */
    AppInfo();

    /*!
     * Destructor.
     */
    ~AppInfo();

    /*!
     * Returns application flags.
     */
    AppFlags
    appFlags() const;

    /*!
     * Returns unique id of application.
     */
    AppID
    appID() const;

    /*!
     * Returns arguments for application.
     */
    std::string
    args() const;

    /*!
     * Returns author of application.
     */
    std::string
    author() const;

    /*!
     * Returns category of application.
     */
    std::string
    category() const;

    /*!
     * Returns application's requirements.
     */
    DependencyFlags
    depFlags() const;

    /*!
     * Returns path to application icon.
     */
    std::string
    icon() const;

    /*!
     * Returns licence of application.
     */
    std::string
    licence() const;

    /*!
     * Returns name of application.
     */
    std::string
    name() const;

    /*!
     * Returns path to application executable.
     */
    std::string
    path() const;

    /*!
     * Returns application version.
     */
    int
    version() const;

    /*!
     * Set application arguments.
     */
    void
    setArgs(const std::string& args);

    /*!
     * Set application author.
     */
    void
    setAuthor(const std::string& author);

    /*!
     * Set application flags.
     */
    void
    setAppFlags(AppFlags appFlags);

    /*!
     * Set application flags using a comma separated string.
     */
    void
    setAppFlags(const std::string& appFlags);

    /*!
     * Set application dependency flags.
     */
    void
    setDepFlags(DependencyFlags depFlags);

    /*!
     * Set application dependency flags using a comma separated string.
     */
    void
    setDepFlags(const std::string& depFlags);

    /*!
     * Set path to application icon.
     */
    void
    setIcon(const std::string& icon);

    /*!
     * Set application licence text.
     */
    void
    setLicence(const std::string& licence);

    /*!
     * Set application name.
     */
    void
    setName(const std::string& name);

    /*!
     * Set application executable path.
     */
    void
    setPath(const std::string& path);

    /*!
     * Set application version number.
     */
    void
    setVersion(int version);

    /*!
     * Set application version number.
     */
    void
    setVersion(const std::string& version);

    /*!
     * Set application category.
     */
    void
    setCategory(const std::string& category);

private:
    //! Unique application ID.
    AppID _appID;
    //! This property stores application name.
    std::string _name;
    //! This property stores path to application executable.
    std::string _path;
    //! This property stores application arguments.
    std::string _args;
    //! This property stores path to application icon.
    std::string _icon;
    //! This property stores the licence text of application.
    std::string _licence;
    //! This property stores the author.
    std::string _author;
    //! This property stores the application category.
    std::string _category;
    //! This property stores the version number.
    int _version;
    //! This property stores application flags.
    AppFlags _appFlags;
    //! This property stores dependency flags.
    DependencyFlags _depFlags;

    //! Counter is incremented for each AppInfo.
    static unsigned int __appCounter;

    // TODO Add Category map.
};

} /* namespace ilixi */
#endif /* ILIXI_APPINFO_H_ */
