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

#include <compositor/AppInstance.h>

namespace ilixi
{

unsigned int AppInstance::_instanceCounter = 0;

AppInstance::AppInstance()
        : _instanceID(_instanceCounter++),
          _appInfo(NULL),
          _started(0),
          _pid(0),
          _process(NULL),
          _view(NULL),
          _thumb(NULL)
{
    pthread_mutex_init(&_mutex, NULL);
}

AppInstance::~AppInstance()
{
    pthread_mutex_destroy(&_mutex);
}

bool
AppInstance::ownWindow(const SaWManWindowHandle handle)
{
    for (AppWindowList::iterator it = _windows.begin(); it != _windows.end(); ++it)
    {
        if (*it == handle)
            return true;
    }
    return false;
}

void
AppInstance::addWindow(SaWManWindowHandle handle)
{
    pthread_mutex_lock(&_mutex);
    _windows.push_back(handle);
    pthread_mutex_unlock(&_mutex);
}

void
AppInstance::removeWindow(SaWManWindowHandle handle)
{
    pthread_mutex_lock(&_mutex);
    for (AppWindowList::iterator it = _windows.begin(); it != _windows.end(); ++it)
    {
        if (*it == handle)
        {
            it = _windows.erase(it);
            break;
        }
    }
    pthread_mutex_unlock(&_mutex);
}

AppInfo*
AppInstance::appInfo() const
{
    return _appInfo;
}

AppID
AppInstance::appID() const
{
    return _appInfo->appID();
}

InstanceID
AppInstance::instanceID() const
{
    return _instanceID;
}

pid_t
AppInstance::pid() const
{
    return _pid;
}

SaWManProcess*
AppInstance::process() const
{
    return _process;
}

long long
AppInstance::started() const
{
    return _started;
}

AppThumbnail*
AppInstance::thumb() const
{
    return _thumb;
}

AppView*
AppInstance::view() const
{
    return _view;
}

unsigned int
AppInstance::windowCount() const
{
    return _windows.size();
}

void
AppInstance::setAppInfo(AppInfo* appInfo)
{
    _appInfo = appInfo;
}

void
AppInstance::setPid(pid_t pid)
{
    _pid = pid;
}

void
AppInstance::setProcess(SaWManProcess* process)
{
    _process = process;
}

void
AppInstance::setStarted(long long started)
{
    _started = started;
}

void
AppInstance::setThumb(AppThumbnail* thumb)
{
    _thumb = thumb;
}

void
AppInstance::setView(AppView* view)
{
    _view = view;
}

} /* namespace ilixi */
