/*
 * ApplicationManager.cpp
 *
 *  Created on: May 1, 2012
 *      Author: tarik
 */

#include "ApplicationManager.h"
#include "core/Logger.h"
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

namespace ilixi
{

  D_DEBUG_DOMAIN( ILX_APPLICATIONMANAGER, "ilixi/AppMan", "ApplicationManager");

  ApplicationManager* ApplicationManager::__instance = NULL;

  ApplicationManager*
  ApplicationManager::instance()
  {
    if (__instance == NULL)
      __instance = new ApplicationManager();
    return __instance;
  }

  ApplicationManager::ApplicationManager()
  {
    initApps();
  }

  ApplicationManager::~ApplicationManager()
  {
  }

  AppInfo*
  ApplicationManager::lookUpAppRecord(const char* name)
  {
    for (AppInfoList::iterator it = _apps.begin(); it != _apps.end(); ++it)
      {
        if (strcmp(((AppInfo*) (*it))->name, name) == 0)
          return ((AppInfo*) (*it));
      }
    return NULL;
  }

  AppInfo*
  ApplicationManager::lookUpAppRecord(unsigned long appID)
  {
    for (AppInfoList::iterator it = _apps.begin(); it != _apps.end(); ++it)
      {
        if (((AppInfo*) (*it))->appID == appID)
          return ((AppInfo*) (*it));
      }
    return NULL;
  }

  AppInfo*
  ApplicationManager::lookUpAppRecord(unsigned int instanceID)
  {
    AppInstanceRecord* instanceRecord = lookUpAppInstanceRecord(instanceID);
    if (instanceRecord)
      return lookUpAppRecord(instanceRecord->appID);
    return NULL;
  }

  AppInfo*
  ApplicationManager::lookUpAppRecord(pid_t pid)
  {
    AppInstanceRecord* instanceRecord = lookUpAppInstanceRecord(pid);
    if (instanceRecord)
      return lookUpAppRecord(instanceRecord->appID);
    return NULL;
  }

  AppInstanceRecord*
  ApplicationManager::lookUpAppInstanceRecord(unsigned long appID)
  {
    for (AppInstanceRecordList::iterator it = _instances.begin();
        it != _instances.end(); ++it)
      {
        if (((AppInstanceRecord*) (*it))->appID == appID)
          return ((AppInstanceRecord*) (*it));
      }
    return NULL;
  }

  AppInstanceRecord*
  ApplicationManager::lookUpAppInstanceRecord(unsigned int instanceID)
  {
    for (AppInstanceRecordList::iterator it = _instances.begin();
        it != _instances.end(); ++it)
      {
        if (((AppInstanceRecord*) (*it))->instanceID == instanceID)
          return ((AppInstanceRecord*) (*it));
      }
    return NULL;
  }

  AppInstanceRecord*
  ApplicationManager::lookUpAppInstanceRecord(pid_t pid)
  {
    for (AppInstanceRecordList::iterator it = _instances.begin();
        it != _instances.end(); ++it)
      {
        if (((AppInstanceRecord*) (*it))->pid == pid)
          return ((AppInstanceRecord*) (*it));
      }
    return NULL;
  }

  AppInfoList
  ApplicationManager::applicationList()
  {
    return _apps;
  }

  void
  ApplicationManager::startApplication(const char* name)
  {
    AppInfo* appInfo = ApplicationManager::instance()->lookUpAppRecord(name);
    if (!appInfo)
      return;

    pid_t pid;

    AppInstanceRecord* appInstanceRecord = lookUpAppInstanceRecord(
        appInfo->appID);

    // TODO check if multiple instances are allowed.
    if (appInstanceRecord && !waitpid(appInstanceRecord->pid, NULL, WNOHANG))
      {
        ILOG_DEBUG(ILX_APPLICATIONMANAGER,
            "Already running '%s' (%d)!\n", name, appInstanceRecord->pid);
        sigViewRequest(appInfo->appID);
        return;
      }

    appInstanceRecord = new AppInstanceRecord;
    _instances.push_back(appInstanceRecord);
    appInstanceRecord->appID = appInfo->appID;
    appInstanceRecord->start_time = direct_clock_get_millis();
    appInstanceRecord->instanceID = 0;

    char str[256];
    int arC = 1;
    char *p;
    int i = 0;
    pid = vfork();
    switch (pid)
      {
    case -1:
      perror("vfork");
      return;

    case 0:
      setsid();

      if (appInfo->args != NULL)
        {
          strncpy(str, appInfo->args, 255);
          p = strtok(str, " ");
          while (p != NULL)
            {
              arC++;
              p = strtok(NULL, " ");
            }

          const char *args[arC + 1];
          args[0] = appInfo->path;

          strncpy(str, appInfo->args, 255);
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
                "ARG: %d - %s\n", i - 1, args[i - 1]);

          execvp(args[0], (char**) args);
        }
      else
        {
          const char* args[2];
          args[0] = appInfo->path;
          args[1] = NULL;
          execvp(args[0], (char**) args);
        }

      perror("execvp");
      _exit(0);
      break;

    default:
      appInstanceRecord->pid = pid;
      ILOG_INFO(ILX_APPLICATIONMANAGER, "Starting %s [%d]\n", name, pid);
      break;
      }
  }

  void
  ApplicationManager::stopApplication(const char* name)
  {
    AppInfo* appInfo = lookUpAppRecord(name);
    if (appInfo)
      stopApplication(appInfo->appID);
    ILOG_ERROR(ILX_APPLICATIONMANAGER, "Record for [%s] not found!", name);
  }

  void
  ApplicationManager::stopApplication(unsigned long appID)
  {
    AppInstanceRecord* appRecord = lookUpAppInstanceRecord(appID);
    if (appRecord)
      kill(appRecord->pid, SIGTERM);
  }

  void
  ApplicationManager::stopAll()
  {
    for (AppInstanceRecordList::iterator it = _instances.begin();
        it != _instances.end(); ++it)
      kill(((AppInstanceRecord*) *it)->pid, SIGKILL);
  }

  void
  ApplicationManager::addApplication(const char* name, const char* path,
      const char* args, const char* icon, const char* licence,
      const char* author, int version, AppFlags appFlags,
      DependencyFlags depFlags)
  {
    if (lookUpAppRecord(name))
      return;

    AppInfo* app = new AppInfo();
    app->appID = _apps.size();
    app->name = name;
    app->path = path;
    app->args = args;
    app->icon = icon;
    app->licence = licence;
    app->author = author;
    app->version = version;
    app->appFlags = appFlags;
    app->depFlags = depFlags;
    _apps.push_back(app);
  }

  void
  ApplicationManager::initApps()
  {
    // dfb-examples
    addApplication("Windows", "df_window", "",
        ILIXI_DATADIR"compositor/input.png", "MIT", "Andreas Shimokawa", 1,
        APP_NO_MAINWINDOW, DEP_MOUSE);

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
        APP_NONE, DEP_NONE);

    // lite
    addApplication("DFBTerm", "dfbterm", NULL,
        ILIXI_DATADIR"compositor/dfbterm.png", "MIT", "Andreas Shimokawa", 1,
        (AppFlags) (APP_LITE | APP_ALLOW_MULTIPLE), DEP_NONE);

    addApplication("ListTest", "lite_listtest", NULL,
        ILIXI_DATADIR"compositor/dfbterm.png", "MIT", "Andreas Shimokawa", 1,
        APP_LITE, DEP_NONE);

    // ilixi
    addApplication("Gallery", "ilixi_demo1", NULL,
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

    addApplication("WebKitDFB", "WebKitDFB", NULL,
        ILIXI_DATADIR"compositor/webkitdfb.png", "LGPLv3", "Andreas Shimokawa",
        1, APP_LITE, DEP_MOUSE);
  }

} /* namespace ilixi */
