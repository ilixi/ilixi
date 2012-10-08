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

#ifndef ILIXI_APPINSTANCE_H_
#define ILIXI_APPINSTANCE_H_

#include <sawman/sawman.h>
#include <list>
#include <pthread.h>
#include "types/AppInfo.h"

namespace ilixi
{

class AppView;
class AppThumbnail;

typedef unsigned int InstanceID;

class AppInstance
{
public:
    AppInstance();

    ~AppInstance();

    bool
    hasWindow(const SaWManWindowHandle handle);

    void
    addWindow(SaWManWindowHandle handle);

    void
    removeWindow(SaWManWindowHandle handle);

    AppInfo*
    appInfo() const;

    AppID
    appID() const;

    InstanceID
    instanceID() const;

    pid_t
    pid() const;

    SaWManProcess*
    process() const;

    long long
    started() const;

    AppThumbnail*
    thumb() const;

    AppView*
    view() const;

    unsigned int
    windowCount() const;

    void
    setAppInfo(AppInfo* appInfo);

    void
    setPid(pid_t pid);

    void
    setProcess(SaWManProcess* process);

    void
    setStarted(long long started);

    void
    setThumb(AppThumbnail* thumb);

    void
    setView(AppView* view);

private:
    typedef std::list<SaWManWindowHandle> AppWindowList;

    InstanceID _instanceID;
    AppInfo* _appInfo;
    long long _started;
    pid_t _pid;
    SaWManProcess* _process;
    AppView* _view;
    AppThumbnail* _thumb;
    pthread_mutex_t _mutex;
    AppWindowList _windows;

    static unsigned int _instanceCounter;
};

} /* namespace ilixi */
#endif /* ILIXI_APPINSTANCE_H_ */
