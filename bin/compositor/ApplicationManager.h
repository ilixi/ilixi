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

#ifndef ILIXI_APPLICATIONMANAGER_H_
#define ILIXI_APPLICATIONMANAGER_H_

#include "types/AppInfo.h"
#include "types/AppInstance.h"
#include <sys/types.h>

namespace ilixi
{
class Compositor;

typedef std::list<AppInfo*> AppInfoList;
typedef std::list<AppInstance*> AppInstanceList;

class ApplicationManager
{

public:
    ApplicationManager(Compositor* compositor);

    virtual
    ~ApplicationManager();

    AppInfo*
    infoByName(const std::string& name);

    AppInfo*
    infoByAppID(AppID appID);

    AppInfo*
    infoByInstanceID(InstanceID instanceID);

    AppInfo*
    infoByPID(pid_t pid);

    AppInstance*
    instanceByAppID(AppID appID);

    AppInstance*
    instanceByInstanceID(InstanceID instanceID);

    AppInstance*
    instanceByPID(pid_t pid);

    AppInfoList
    applicationList();

    unsigned int
    instanceCount() const;

    void
    startApp(const std::string& name);

    DirectResult
    startApplication(const std::string& name);

    DirectResult
    stopApplication(pid_t pid);

    void
    stopAll();

    void
    initStartup();

protected:

    /*!
     * Called when a DirectFB process starts.
     */
    virtual DirectResult
    processAdded(SaWManProcess *process);

    /*!
     * Called when a DirectFB process stops.
     */
    virtual DirectResult
    processRemoved(SaWManProcess *process);

    /*!
     * Called right before a window is created.
     */
    virtual DirectResult
    windowPreconfig(SaWManWindowConfig *config);

    /*!
     * Called when a window is added to stack.
     */
    virtual DirectResult
    windowAdded(SaWManWindowInfo *info);

    /*!
     * Called when a window is removed from stack.
     */
    virtual DirectResult
    windowRemoved(SaWManWindowInfo *info);

    /*!
     * Called when a window is configured.
     */
    virtual DirectResult
    windowReconfig(SaWManWindowReconfig *reconfig);

    /*!
     * Called when window is restacked.
     */
    virtual DirectResult
    windowRestack(SaWManWindowHandle handle, SaWManWindowHandle relative,
                  SaWManWindowRelation relation);

private:
    Compositor* _compositor;
    AppInfoList _infos;
    AppInstanceList _instances;

    ISaWMan *_saw;
    ISaWManManager *_manager;
    SaWManCallbacks _callbacks;

    pthread_mutex_t _mutex;

    void
    initApps();

    bool
    parseAppDef(const char* file);

    void
    addApplication(const char* name, const char* author, const char* licence,
                   const char* category, const char* version, const char* icon,
                   const char* exec, const char* args, const char* appFlags,
                   const char* depFlags);

    // SaWMan callbacks...
    friend DirectResult
    start_request(void *context, const char *name, pid_t *ret_pid);

    friend DirectResult
    stop_request(void *context, pid_t pid, FusionID caller);

    friend DirectResult
    process_added(void *context, SaWManProcess *process);

    friend DirectResult
    process_removed(void *context, SaWManProcess *process);

    friend DirectResult
    window_preconfig(void *context, SaWManWindowConfig *config);

    friend DirectResult
    window_added(void *context, SaWManWindowInfo *info);

    friend DirectResult
    window_removed(void *context, SaWManWindowInfo *info);

    friend DirectResult
    window_reconfig(void *context, SaWManWindowReconfig *reconfig);

    friend DirectResult
    window_restack(void *context, SaWManWindowHandle handle,
                   SaWManWindowHandle relative, SaWManWindowRelation relation);
};

} /* namespace ilixi */
#endif /* ILIXI_APPLICATIONMANAGER_H_ */
