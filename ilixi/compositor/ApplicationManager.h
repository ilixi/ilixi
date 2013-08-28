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

#ifndef ILIXI_APPLICATIONMANAGER_H_
#define ILIXI_APPLICATIONMANAGER_H_

#include <compositor/AppInfo.h>
#include <compositor/AppInstance.h>
#include <compositor/MemoryMonitor.h>
#include <sys/types.h>

namespace ilixi
{
class ILXCompositor;

typedef std::list<AppInfo*> AppInfoList;
typedef std::list<AppInstance*> AppInstanceList;

//! Manages registered applications.
/*!
 * Application manager keeps a track of installed applications and their running instances.
 */
class ApplicationManager
{

public:
    /*!
     * Constructor.
     *
     * Initialises SaWMan callbacks, signal handler and memory monitor.
     */
    ApplicationManager(ILXCompositor* compositor);

    /*!
     * Destructor.
     */
    virtual
    ~ApplicationManager();

    /*!
     * Returns an AppInfo given an application name.
     *
     * @param name of application.
     * @return NULL if an AppInfo is not found with given name.
     */
    AppInfo*
    infoByName(const std::string& name);

    /*!
     * Returns an AppInfo given an application ID.
     *
     * @param appID unique application ID.
     * @return NULL if an AppInfo is not found with given ID.
     */
    AppInfo*
    infoByAppID(AppID appID);

    /*!
     * Returns an AppInfo given an instance ID.
     *
     * @param instanceID unique instance ID.
     * @return NULL if an AppInfo is not found with given instance ID.
     */
    AppInfo*
    infoByInstanceID(InstanceID instanceID);

    /*!
     * Returns an AppInfo given a process ID.
     *
     * @param pid Process ID.
     * @return NULL if an AppInfo is not found with given process ID.
     */
    AppInfo*
    infoByPID(const pid_t pid);

    /*!
     * Returns an AppInstance given an application ID.
     *
     * @param appID unique application ID.
     * @return NULL if an AppInstance is not found with given application ID.
     */
    AppInstance*
    instanceByAppID(AppID appID);

    /*!
     * Returns an AppInstance given an instance ID.
     *
     * @param instanceID unique instance ID.
     * @return NULL if an AppInstance is not found with given instance ID.
     */
    AppInstance*
    instanceByInstanceID(InstanceID instanceID);

    /*!
     * Returns an AppInstance given a process ID.
     *
     * @param pid Process ID.
     * @return NULL if an AppInstance is not found with given process ID.
     */
    AppInstance*
    instanceByPID(const pid_t pid);

    /*!
     * Returns a process' parent PID.
     *
     * @param pid Process ID.
     * @return Process ID of parent.
     */
    pid_t
    getParentPID(const pid_t pid);

    /*!
     * Returns list of registered applications.
     */
    AppInfoList
    applicationList();

    /*!
     * Returns number of running application instances.
     */
    unsigned int
    instanceCount() const;

    /*!
     * Starts an application.
     *
     * If application is not running, this method will fork and exec.
     * Otherwise, it will show first application instance.
     *
     * @param name registered application name.
     * @param showInstance if false will not switch to application.
     * @return DR_OK if successful.
     */
    DirectResult
    startApplication(const std::string& name, bool showInstance = true);

    /*!
     * Stops a running application.
     *
     * @param pid process ID.
     * @return DR_OK if successful.
     */
    DirectResult
    stopApplication(pid_t pid);

    /*!
     * Stops all running applications.
     */
    void
    stopAll();

    /*!
     * Starts home, statusbar and all startup applications.
     * @see APP_AUTO_START
     */
    void
    initStartup();

    /*!
     * Parses given folder and adds all application definitions (*.appdef).
     *
     * It will ignore applications with same name.
     */
    void
    parseFolder(const std::string& folder);

protected:
    /*!
     * Called when a DirectFB process starts.
     */
    virtual DirectResult
    processAdded(SaWManProcess *process);

    /*!
     * Called when a DirectFB process exits.
     */
    virtual DirectResult
    processRemoved(AppInstance* instance);

    /*!
     * Called when a child process is killed.
     */
    virtual DirectResult
    processTerminated(AppInstance* instance);

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
    windowRestack(SaWManWindowHandle handle, SaWManWindowHandle relative, SaWManWindowRelation relation);

private:
    //! Owner.
    ILXCompositor* _compositor;
    //! List of registered applications.
    AppInfoList _infos;
    //! List of running application instances.
    AppInstanceList _instances;

    //! SaWMan interface.
    ISaWMan *_saw;
    //! SaWMan manager interface.
    ISaWManManager *_manager;
    //! Registered SaWMan callbacks.
    SaWManCallbacks _callbacks;

    //! Memory Monitor.
    MemoryMonitor* _monitor;

    //! This locks application instance list.
    pthread_mutex_t _mutex;
    //! Registers signal handler.
    struct sigaction _act;

    //! Parses an appdef file.
    bool
    parseAppDef(const std::string& folder, const std::string& file);

    //! Add application to list.
    void
    addApplication(const char* name, const char* author, const char* licence, const char* category, const char* version, const char* icon, const char* exec, const char* args, const char* appFlags, const char* depFlags);

    //! Searches for executable. Returns true if found.
    bool
    searchExec(const char* exec, std::string& path);

    //! Slot, handles a memory state change.
    void
    handleMemoryState(MemoryMonitor::MemoryState state);

    friend void
    sigchild_handler(int sig, siginfo_t *siginfo, void *context);

    // SaWMan callbacks...
    friend DirectResult
    start_request(void *context, const char *name, pid_t *ret_pid);

    friend DirectResult
    stop_request(void *context, pid_t pid, FusionID caller);

    friend DirectResult
    process_added(void *context, SaWManProcess *process);

    friend DirectResult
    window_preconfig(void *context, SaWManWindowConfig *config);

    friend DirectResult
    window_added(void *context, SaWManWindowInfo *info);

    friend DirectResult
    window_removed(void *context, SaWManWindowInfo *info);

    friend DirectResult
    window_reconfig(void *context, SaWManWindowReconfig *reconfig);

    friend DirectResult
    window_restack(void *context, SaWManWindowHandle handle, SaWManWindowHandle relative, SaWManWindowRelation relation);

    friend class MemoryMonitor;
};

} /* namespace ilixi */
#endif /* ILIXI_APPLICATIONMANAGER_H_ */
