/*
 * ApplicationManager.h
 *
 *  Created on: May 1, 2012
 *      Author: tarik
 */

#ifndef ILIXI_APPLICATIONMANAGER_H_
#define ILIXI_APPLICATIONMANAGER_H_

#include <sys/types.h>
#include <vector>
#include <sigc++/signal.h>

namespace ilixi
{
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

    // window managers
    APP_WM_DEFAULT = 0x001000,
    APP_WM_SAWMAN = 0x002000,

    // capabilities
    APP_NO_MAINWINDOW = 0x010000,

    // constraints
    APP_ALLOW_MULTIPLE = 0x100000, //!<
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

  struct AppInfo
  {
    unsigned long appID;
    const char* name;
    const char* path;
    const char* args;
    const char* icon;
    const char* licence;
    const char* author;
    int version;
    long long installTime;
    AppFlags appFlags;
    DependencyFlags depFlags;
  };

  struct AppInstanceRecord
  {
    unsigned long appID; // AppInfo::appID
    unsigned int instanceID;
    pid_t pid;
    long long start_time;
  };

  typedef std::vector<AppInfo*> AppInfoList;
  typedef std::vector<AppInstanceRecord*> AppInstanceRecordList;

  // Singleton
  class ApplicationManager : virtual public sigc::trackable
  {

  public:
    static ApplicationManager*
    instance();

    virtual
    ~ApplicationManager();

    AppInfo*
    lookUpAppRecord(const char* name);

    AppInfo*
    lookUpAppRecord(unsigned long appID);

    AppInfo*
    lookUpAppRecord(unsigned int instanceID);

    AppInfo*
    lookUpAppRecord(pid_t pid);

    AppInstanceRecord*
    lookUpAppInstanceRecord(unsigned long appID);

    AppInstanceRecord*
    lookUpAppInstanceRecord(unsigned int instanceID);

    AppInstanceRecord*
    lookUpAppInstanceRecord(pid_t pid);

    AppInfoList
    applicationList();

    void
    startApplication(const char* name);

    void
    stopApplication(const char* name);

    void
    stopApplication(unsigned long appID);

    void
    stopAll();

    sigc::signal<void, unsigned long> sigViewRequest;
    sigc::signal<void, AppInfo*> sigAppStart;

  private:
    static ApplicationManager* __instance;
    AppInfoList _apps;
    AppInstanceRecordList _instances;

    ApplicationManager();

    void
    initApps();

    void
    addApplication(const char* name, const char* path, const char* args,
        const char* icon, const char* licence, const char* author, int version,
        AppFlags appFlags, DependencyFlags depFlags = DEP_NONE);
  };

} /* namespace ilixi */
#endif /* ILIXI_APPLICATIONMANAGER_H_ */
