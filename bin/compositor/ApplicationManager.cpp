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

#include "ApplicationManager.h"
#include "core/Logger.h"
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdexcept>
#include "Compositor.h"

namespace ilixi
{

  D_DEBUG_DOMAIN( ILX_APPLICATIONMANAGER, "ilixi/compositor/AppMan",
      "ApplicationManager");

  //*********************************************************************

  DirectResult
  start_request(void *context, const char *name, pid_t *ret_pid)
  {
    ApplicationManager* appMan = (ApplicationManager*) context;
    return appMan->startApplication(name);
  }

  DirectResult
  stop_request(void *context, pid_t pid, FusionID caller)
  {
    ILOG_DEBUG(ILX_APPLICATIONMANAGER,
        "Stop request from Fusion ID 0x%lx for pid %d!\n", caller, pid);
    ApplicationManager* appMan = (ApplicationManager*) context;
    return appMan->stopApplication(pid);
  }

  DirectResult
  process_added(void *context, SaWManProcess *process)
  {
    ApplicationManager* appMan = (ApplicationManager*) context;
    return appMan->processAdded(process);
  }

  DirectResult
  process_removed(void *context, SaWManProcess *process)
  {
    ApplicationManager* appMan = (ApplicationManager*) context;
    return appMan->processRemoved(process);
  }

  DirectResult
  window_added(void *context, SaWManWindowInfo *info)
  {
    ApplicationManager* appMan = (ApplicationManager*) context;
    return appMan->windowAdded(info);
  }

  DirectResult
  window_removed(void *context, SaWManWindowInfo *info)
  {
    ApplicationManager* appMan = (ApplicationManager*) context;
    return appMan->windowRemoved(info);
  }

  DirectResult
  window_reconfig(void *context, SaWManWindowReconfig *reconfig)
  {
    ApplicationManager* appMan = (ApplicationManager*) context;
    return appMan->windowReconfig(reconfig);
  }

  //*********************************************************************

  ApplicationManager::ApplicationManager(Compositor* compositor) :
      _compositor(compositor)
  {
    ILOG_DEBUG(ILX_APPLICATIONMANAGER, "Initialising Application Manager.\n");
    pthread_mutex_init(&_mutex, NULL);
    if (SaWManInit(NULL, NULL) != DR_OK)
      ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to initialise SaWMan!\n");

    if (SaWManCreate(&_saw) != DR_OK)
      ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create SaWMan!\n");

    _callbacks.Start = start_request;
    _callbacks.Stop = stop_request;
    _callbacks.ProcessAdded = process_added;
    _callbacks.ProcessRemoved = process_removed;
    _callbacks.InputFilter = NULL;
    _callbacks.WindowPreConfig = NULL;
    _callbacks.WindowAdded = window_added;
    _callbacks.WindowRemoved = window_removed;
    _callbacks.WindowReconfig = window_reconfig;
    _callbacks.WindowRestack = NULL;
    _callbacks.StackResized = NULL;
    _callbacks.SwitchFocus = NULL;
    _callbacks.LayerReconfig = NULL;
    _callbacks.ApplicationIDChanged = NULL;

    ILOG_DEBUG(ILX_APPLICATIONMANAGER, "Creating SaWManager.\n");
    if (_saw->CreateManager(_saw, &_callbacks, this, &_manager) != DR_OK)
      ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create SaWMan manager!\n");

    initApps();
  }

  ApplicationManager::~ApplicationManager()
  {
    stopAll();

    if (_manager)
      _manager->Release(_manager);

    if (_saw)
      _saw->Release(_saw);

    for (AppInfoList::iterator it = _infos.begin(); it != _infos.end(); ++it)
      delete *it;

    for (AppInstanceList::iterator it = _instances.begin();
        it != _instances.end(); ++it)
      delete *it;

    pthread_mutex_destroy(&_mutex);
  }

  AppInfo*
  ApplicationManager::infoByName(const std::string& name)
  {
    for (AppInfoList::iterator it = _infos.begin(); it != _infos.end(); ++it)
      {
        if (((AppInfo*) (*it))->name() == name)
          return ((AppInfo*) (*it));
      }
    return NULL;
  }

  AppInfo*
  ApplicationManager::infoByAppID(unsigned long appID)
  {
    for (AppInfoList::iterator it = _infos.begin(); it != _infos.end(); ++it)
      {
        if (((AppInfo*) (*it))->appID() == appID)
          return ((AppInfo*) (*it));
      }
    return NULL;
  }

  AppInfo*
  ApplicationManager::infoByInstanceID(unsigned int instanceID)
  {
    AppInstance* instanceRecord = instanceByInstanceID(instanceID);
    if (instanceRecord)
      return infoByAppID(instanceRecord->appID());
    return NULL;
  }

  AppInfo*
  ApplicationManager::infoByPID(pid_t pid)
  {
    AppInstance* instanceRecord = instanceByPID(pid);
    if (instanceRecord)
      return infoByAppID(instanceRecord->appID());
    return NULL;
  }

  AppInstance*
  ApplicationManager::instanceByAppID(unsigned long appID)
  {
    for (AppInstanceList::iterator it = _instances.begin();
        it != _instances.end(); ++it)
      {
        if (((AppInstance*) (*it))->appID() == appID)
          return ((AppInstance*) (*it));
      }
    return NULL;
  }

  AppInstance*
  ApplicationManager::instanceByInstanceID(unsigned int instanceID)
  {
    for (AppInstanceList::iterator it = _instances.begin();
        it != _instances.end(); ++it)
      {
        if (((AppInstance*) (*it))->instanceID() == instanceID)
          return ((AppInstance*) (*it));
      }
    return NULL;
  }

  AppInstance*
  ApplicationManager::instanceByPID(pid_t pid)
  {
    for (AppInstanceList::iterator it = _instances.begin();
        it != _instances.end(); ++it)
      {
        if (((AppInstance*) (*it))->pid() == pid)
          return ((AppInstance*) (*it));
      }
    return NULL;
  }

  AppInfoList
  ApplicationManager::applicationList()
  {
    return _infos;
  }

  unsigned int
  ApplicationManager::instanceCount() const
  {
    return _instances.size();
  }

  void
  ApplicationManager::startApp(const std::string& name)
  {
    startApplication(name);
  }

  DirectResult
  ApplicationManager::startApplication(const std::string& name)
  {
    ILOG_DEBUG( ILX_APPLICATIONMANAGER,
        "%s( name %s )\n", __FUNCTION__, name.c_str());

    AppInfo* appInfo = infoByName(name);
    if (!appInfo)
      return DR_ITEMNOTFOUND;

    pid_t pid;

    AppInstance* instance = instanceByAppID(appInfo->appID());

    if (instance && !(appInfo->appFlags() & APP_ALLOW_MULTIPLE)
        && !waitpid(instance->pid(), NULL, WNOHANG))
      {
        ILOG_DEBUG(ILX_APPLICATIONMANAGER,
            "  -> Already running '%s' (%d)!\n", name.c_str(), instance->pid());
        _compositor->handleViewRequest(instance);
        return DR_BUSY;
      }

    char str[256];
    int arC = 1;
    char *p;
    int i = 0;
    pid = vfork();
    switch (pid)
      {
    case -1:
      perror("vfork");
      return DR_FAILURE;

    case 0:
      setsid();

      if (appInfo->args() != "")
        {
          strncpy(str, appInfo->args().c_str(), 255);
          p = strtok(str, " ");
          while (p != NULL)
            {
              arC++;
              p = strtok(NULL, " ");
            }

          const char *args[arC + 1];
          args[0] = appInfo->path().c_str();

          strncpy(str, appInfo->args().c_str(), 255);
          arC = 1;
          p = strtok(str, " ");
          while (p != NULL)
            {
              args[arC++] = p;
              p = strtok(NULL, " ");
            }
          args[arC++] = NULL;

          while (i++ < arC)
            ILOG_DEBUG(ILX_APPLICATIONMANAGER,
                "  -> ARG: %d - %s\n", i - 1, args[i - 1]);

          execvp(args[0], (char**) args);
        }
      else
        {
          const char* args[2];
          args[0] = appInfo->path().c_str();
          args[1] = NULL;
          execvp(args[0], (char**) args);
        }

      perror("execvp");
      _exit(0);
      break;

    default:
      pthread_mutex_lock(&_mutex);
      instance = new AppInstance();
      instance->setAppID(appInfo->appID());
      instance->setStarted(direct_clock_get_millis());
      instance->setPid(pid);
      _instances.push_back(instance);
      pthread_mutex_unlock(&_mutex);
      break;
      }
    return DR_OK;
  }

  DirectResult
  ApplicationManager::stopApplication(pid_t pid)
  {
    ILOG_DEBUG( ILX_APPLICATIONMANAGER, "%s( pid %d)\n", __FUNCTION__, pid);

    AppInstance* instance = instanceByPID(pid);
    if (instance)
      {
        pthread_mutex_lock(&_mutex);
        for (AppInstanceList::iterator it = _instances.begin();
            it != _instances.end(); ++it)
          {
            if (((AppInstance*) *it)->pid() == pid)
              {
                _instances.erase(it);
                break;
              }
          }

        kill(instance->pid(), SIGTERM);
        delete instance;
        pthread_mutex_unlock(&_mutex);
        return DR_OK;
      }
    else
      {
        ILOG_ERROR(ILX_APPLICATIONMANAGER,
            "Could not find an application with pid[%d]!", pid);
        return DR_ITEMNOTFOUND;
      }
  }

  void
  ApplicationManager::stopAll()
  {
    ILOG_DEBUG( ILX_APPLICATIONMANAGER, "%s()\n", __FUNCTION__);
    pthread_mutex_lock(&_mutex);
    while (!_instances.empty())
      {
        AppInstance* instance = _instances.front();
        AppInfo* info = infoByAppID(instance->appID());
        ILOG_DEBUG( ILX_APPLICATIONMANAGER,
            "  -> Killing %s[%d]...\n", info->name().c_str(), instance->pid());

        kill(instance->pid(), SIGKILL);
        delete instance;
        _instances.pop_front();
      }
    pthread_mutex_unlock(&_mutex);
  }

  DirectResult
  ApplicationManager::processAdded(SaWManProcess *process)
  {
    ILOG_DEBUG( ILX_APPLICATIONMANAGER,
        "%s( process %p )\n", __FUNCTION__, process);
    ILOG_DEBUG(ILX_APPLICATIONMANAGER,
        "  -> Process [%d] Fusionee [%lu]\n", process->pid, process->fusion_id);

    if (process->fusion_id == 1)
      return DR_OK;

    AppInstance* instance = instanceByPID(process->pid);
    if (instance)
      return DR_OK;

    ILOG_WARNING(ILX_APPLICATIONMANAGER,
        "Process[%d] is not recognized!\n", process->pid);
    kill(process->pid, SIGKILL);
    return DR_ITEMNOTFOUND;
  }

  DirectResult
  ApplicationManager::processRemoved(SaWManProcess *process)
  {
    ILOG_DEBUG( ILX_APPLICATIONMANAGER,
        "%s( process %p )\n", __FUNCTION__, process);
    ILOG_DEBUG(ILX_APPLICATIONMANAGER,
        "  -> Process [%d] Fusionee [%lu]\n", process->pid, process->fusion_id);

    pthread_mutex_lock(&_mutex);
    bool found = false;
    AppInstance* instance;
    for (AppInstanceList::iterator it = _instances.begin();
        it != _instances.end(); ++it)
      {
        instance = ((AppInstance*) *it);
        if (instance->pid() == process->pid)
          {
            found = true;
            it = _instances.erase(it);
            break;
          }
      }
    pthread_mutex_unlock(&_mutex);
    if (found)
      {
        _compositor->processRemoved(instance);
        delete instance;
        return DR_OK;
      }
    return DR_ITEMNOTFOUND;
  }

  DirectResult
  ApplicationManager::windowAdded(SaWManWindowInfo *info)
  {
    ILOG_DEBUG( ILX_APPLICATIONMANAGER, "%s( info %p )\n", __FUNCTION__, info);

    SaWManProcess process;
    _manager->GetProcessInfo(_manager, info->handle, &process);

    ILOG_DEBUG(ILX_APPLICATIONMANAGER,
        "  -> Process [%d] Window [%lu]\n", process.pid, info->handle);

    AppInstance* instance = instanceByPID(process.pid);
    if (!instance)
      return DR_FAILURE;

    _manager->Lock(_manager);
    instance->addWindow(info->handle);
    _compositor->addWindow(instance, info);
    _manager->Unlock(_manager);
    return DR_OK;
  }

  DirectResult
  ApplicationManager::windowRemoved(SaWManWindowInfo *info)
  {
    ILOG_DEBUG( ILX_APPLICATIONMANAGER, "%s( info %p )\n", __FUNCTION__, info);

    SaWManProcess process;
    _manager->GetProcessInfo(_manager, info->handle, &process);

    ILOG_DEBUG(ILX_APPLICATIONMANAGER,
        "  -> Process [%d] Window [%lu]\n", process.pid, info->handle);

    AppInstance* instance = instanceByPID(process.pid);
    if (instance)
      {
        _manager->Lock(_manager);
        instance->removeWindow(info->handle);
        _compositor->removeWindow(instance, info);
        _manager->Unlock(_manager);
      }
    return DR_OK;
  }

  DirectResult
  ApplicationManager::windowReconfig(SaWManWindowReconfig *reconfig)
  {
    ILOG_DEBUG( ILX_APPLICATIONMANAGER,
        "%s( reconfig %p )\n", __FUNCTION__, reconfig);
    ILOG_DEBUG(ILX_APPLICATIONMANAGER,
        "  -> Window [%lu] Flags [0x%04x]\n", reconfig->handle, reconfig->flags);

    if (reconfig->flags & SWMCF_POSITION)
      {
        ILOG_WARNING(ILX_APPLICATIONMANAGER, "SWMCF_POSITION not allowed!\n");
        reconfig->flags = (SaWManWindowConfigFlags) (reconfig->flags
            & ~SWMCF_POSITION);
      }

    SaWManProcess process;
    _manager->GetProcessInfo(_manager, reconfig->handle, &process);

    SaWManWindowInfo winInfo;
    _manager->GetWindowInfo(_manager, reconfig->handle, &winInfo);

    AppInstance* instance = instanceByPID(process.pid);
    if (instance)
      {
        _manager->Lock(_manager);
        _compositor->configWindow(instance, reconfig, &winInfo);
        _manager->Unlock(_manager);
      }
    return DR_OK;
  }

  void
  ApplicationManager::addApplication(const char* name, const char* path,
      const char* args, const char* icon, const char* licence,
      const char* author, int version, AppFlags appFlags,
      DependencyFlags depFlags)
  {
    if (infoByName(name))
      return;

    AppInfo* app = new AppInfo();
    app->setAppFlags(appFlags);
    app->setArgs(args);
    app->setAuthor(author);
    app->setDepFlags(depFlags);
    app->setIcon(icon);
    app->setLicence(licence);
    app->setName(name);
    app->setPath(path);
    app->setVersion(version);

    _infos.push_back(app);
  }

  void
  ApplicationManager::initApps()
  {
    // dfb-examples
    addApplication("Windows", "df_window", "",
        ILIXI_DATADIR"compositor/input.png", "MIT", "Andreas Shimokawa", 1,
        (AppFlags) (APP_NO_MAINWINDOW | APP_NEEDS_CLEAR
            | APP_ALLOW_WINDOW_CONFIG), DEP_MOUSE);

    addApplication("Fire", "df_fire", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/input.png", "MIT", "Andreas Shimokawa", 1,
        APP_NEEDS_CLEAR, DEP_MOUSE);

    addApplication("Penguins", "df_andi", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/andi.png", "MIT", "Andreas Shimokawa", 1,
        APP_NONE, DEP_NONE);

    addApplication("Neo", "df_neo", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/neo.png", "MIT", "Andreas Shimokawa", 1,
        APP_NONE, DEP_NONE);

    addApplication("Inputs", "df_input", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/input.png", "MIT", "Andreas Shimokawa", 1,
        APP_NONE, DEP_NONE);

    addApplication("Matrix", "df_matrix", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/matrix.png", "MIT", "Andreas Shimokawa", 1,
        APP_NONE, DEP_NONE);

    addApplication("Knuckles", "df_knuckles", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/knuckles.png", "MIT", "Andreas Shimokawa", 1,
        APP_NONE, DEP_NONE);

    addApplication("Particle", "df_particle", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/particle.png", "MIT", "Andreas Shimokawa", 1,
        APP_NONE, DEP_NONE);

    addApplication("Texture", "df_texture", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/texture.png", "MIT", "Andreas Shimokawa", 1,
        APP_NEEDS_CLEAR, DEP_NONE);

    // lite
    addApplication("DFBTerm", "dfbterm", "",
        ILIXI_DATADIR"compositor/dfbterm.png", "MIT", "Andreas Shimokawa", 1,
        (AppFlags) (APP_LITE | APP_ALLOW_MULTIPLE | APP_NEEDS_CLEAR), DEP_NONE);

    addApplication("ListTest", "lite_listtest", "",
        ILIXI_DATADIR"compositor/dfbterm.png", "MIT", "Andreas Shimokawa", 1,
        APP_LITE, DEP_NONE);

    // ilixi
    addApplication("Gallery", "ilixi_demo1", "",
        ILIXI_DATADIR"compositor/gallery.png", "LGPLv3", "Andreas Shimokawa", 1,
        APP_ILIXI, DEP_MOUSE);

    // others
    addApplication("Video Player", "dfbtest_video",
        ILIXI_DATADIR"compositor/demo.mp4 -f RGB32 -l --dfb:force-windowed",
        ILIXI_DATADIR"compositor/player.png", "LGPLv3", "Andreas Shimokawa", 1,
        APP_ILIXI, DEP_MOUSE);

    addApplication("ClanBomber 2", "clanbomber2", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/clanbomber2.png", "LGPLv3",
        "Andreas Shimokawa", 1, APP_NONE, DEP_MOUSE);

    // Check if installed
    addApplication("QML Viewer", "qmlviewer", "-qws -display directfb",
        ILIXI_DATADIR"compositor/qt-qml.png", "GPL", "Andreas Shimokawa", 1,
        APP_QT, DEP_MOUSE);

    addApplication("WebKitDFB", "WebKitDFB", "",
        ILIXI_DATADIR"compositor/webkitdfb.png", "LGPLv3", "Andreas Shimokawa",
        1, APP_LITE, DEP_MOUSE);
  }

} /* namespace ilixi */
