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

#ifndef ILIXI_APPINSTANCE_H_
#define ILIXI_APPINSTANCE_H_

#include <compositor/AppInfo.h>
//extern "C" {
#include <sawman.h>

#include <list>
#include <pthread.h>

namespace ilixi
{

class AppView;
class AppThumbnail;

typedef unsigned int InstanceID;

//! Provides information about a running application instance.
class AppInstance
{
public:
    /*!
     * Constructor.
     */
    AppInstance();

    /*!
     * Destructor.
     */
    ~AppInstance();

    /*!
     * Returns true if application owns given window.
     */
    bool
    ownWindow(const SaWManWindowHandle handle);

    /*!
     * Registers a window with instance.
     */
    void
    addWindow(SaWManWindowHandle handle);

    /*!
     * Removes a window from instance.
     */
    void
    removeWindow(SaWManWindowHandle handle);

    /*!
     * Returns AppInfo for this instance.
     */
    AppInfo*
    appInfo() const;

    /*!
     * Returns unique application ID.
     */
    AppID
    appID() const;

    /*!
     * Returns unique instance ID.
     */
    InstanceID
    instanceID() const;

    /*!
     * Returns process ID.
     */
    pid_t
    pid() const;

    /*!
     * Returns SaWMan process handle.
     */
    SaWManProcess*
    process() const;

    /*!
     * Returns milliseconds since application started.
     */
    long long
    started() const;

    /*!
     * Returns pointer to thumbnail if any.
     */
    AppThumbnail*
    thumb() const;

    /*!
     * Returns pointer to view if any.
     */
    AppView*
    view() const;

    /*!
     * Returns number of windows registered with this instance.
     */
    unsigned int
    windowCount() const;

    /*!
     * Set application info for this instance.
     */
    void
    setAppInfo(AppInfo* appInfo);

    /*!
     * Set process ID.
     */
    void
    setPid(pid_t pid);

    /*!
     * Set SaWMan process handle.
     */
    void
    setProcess(SaWManProcess* process);

    /*!
     * Set application start time in milliseconds.
     */
    void
    setStarted(long long started);

    /*!
     * Set an application thumbnail widget for this instance.
     */
    void
    setThumb(AppThumbnail* thumb);

    /*!
     * Set an application view widget for this instance.
     */
    void
    setView(AppView* view);

private:
    typedef std::list<SaWManWindowHandle> AppWindowList;

    //! This property stores unique instance ID.
    InstanceID _instanceID;
    //! This property points to application info.
    AppInfo* _appInfo;
    //! This properts stores milliseconds since application has started.
    long long _started;
    //! This property stores process ID.
    pid_t _pid;
    //! This property stores SaWMan process handle.
    SaWManProcess* _process;
    //! This property stores application view widget.
    AppView* _view;
    //! This property stores application thumbnail widget.
    AppThumbnail* _thumb;
    //! This mutex locks window list.
    pthread_mutex_t _mutex;
    //! This property stores list of registered windows.
    AppWindowList _windows;

    //! Counter is incremented for each AppInstance.
    static unsigned int _instanceCounter;
};

} /* namespace ilixi */
#endif /* ILIXI_APPINSTANCE_H_ */
