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

#include <compositor/ApplicationManager.h>
#include <compositor/Compositor.h>
#include <core/Logger.h>
#include <lib/FileSystem.h>
#include <lib/Notify.h>
#include <lib/XMLReader.h>

#include <string.h>
#include <signal.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPLICATIONMANAGER, "ilixi/compositor/AppMan", "ApplicationManager");

bool
app_sort(AppInfo* app1, AppInfo* app2)
{
    unsigned int i = 0;
    while ((i < app1->name().length()) && (i < app2->name().length()))
    {
        if (tolower(app1->name()[i]) < tolower(app2->name()[i]))
            return true;
        else if (tolower(app1->name()[i]) > tolower(app2->name()[i]))
            return false;
        ++i;
    }
    if (app1->name().length() < app2->name().length())
        return true;
    else
        return false;
}

//*********************************************************************
static ApplicationManager* __appMan = NULL;

void
sigchild_handler(int sig, siginfo_t *siginfo, void *context)
{
    ILOG_TRACE_F(ILX_APPLICATIONMANAGER);
    ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> si_code: %d\n", siginfo->si_code);
    if (__appMan != NULL)
    {
        if (siginfo->si_code == CLD_DUMPED || siginfo->si_code == CLD_KILLED)
        {
            AppInstance* instance = __appMan->instanceByPID(siginfo->si_pid);
            ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> pid: %d instance: %p (CLD_KILLED || CLD_DUMPED)\n", siginfo->si_pid, instance);
            if (!instance)
                return;

            if (instance->view())
                return;

            __appMan->processTerminated(instance);
        } else if (siginfo->si_code == CLD_EXITED)
        {
            AppInstance* instance = __appMan->instanceByPID(siginfo->si_pid);
            ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> pid: %d instance: %p (CLD_EXITED)\n", siginfo->si_pid, instance);

            if (!instance)
                return;

            __appMan->processRemoved(instance);
        }
    }
}

DirectResult
start_request(void *context, const char *name, pid_t *ret_pid)
{
    ApplicationManager* appMan = (ApplicationManager*) context;
    return appMan->startApplication(name);
}

DirectResult
stop_request(void *context, pid_t pid, FusionID caller)
{
    ILOG_DEBUG(ILX_APPLICATIONMANAGER, "Stop request from Fusion ID 0x%lx for pid %d!\n", caller, pid);
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
window_preconfig(void *context, SaWManWindowConfig *config)
{
    ApplicationManager* appMan = (ApplicationManager*) context;
    return appMan->windowPreconfig(config);
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

DirectResult
window_restack(void *context, SaWManWindowHandle handle, SaWManWindowHandle relative, SaWManWindowRelation relation)
{
    ApplicationManager* appMan = (ApplicationManager*) context;
    return appMan->windowRestack(handle, relative, relation);
}

//*********************************************************************

ApplicationManager::ApplicationManager(ILXCompositor* compositor)
        : _compositor(compositor)
{
    ILOG_TRACE_F(ILX_APPLICATIONMANAGER);
    pthread_mutex_init(&_mutex, NULL);

    __appMan = this;

    memset(&_act, 0, sizeof(_act));
    _act.sa_sigaction = &sigchild_handler;
    _act.sa_flags = SA_SIGINFO | SA_NOCLDWAIT;

    if (sigaction(SIGCHLD, &_act, NULL) == -1)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create signal handler!\n");
    if (sigaction(SIGSEGV, &_act, NULL) == -1)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create signal handler!\n");
    if (sigaction(SIGBUS, &_act, NULL) == -1)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create signal handler!\n");
    if (sigaction(SIGILL, &_act, NULL) == -1)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create signal handler!\n");
    if (sigaction(SIGFPE, &_act, NULL) == -1)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create signal handler!\n");
    if (sigaction(SIGABRT, &_act, NULL) == -1)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create signal handler!\n");
    if (sigaction(SIGTERM, &_act, NULL) == -1)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create signal handler!\n");

    if (SaWManInit(NULL, NULL) != DR_OK)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to initialise SaWMan!\n");

    if (SaWManCreate(&_saw) != DR_OK)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create SaWMan!\n");

    memset(&_callbacks, 0, sizeof(_callbacks));
    _callbacks.Start = start_request;
    _callbacks.Stop = stop_request;
    _callbacks.ProcessAdded = process_added;
    _callbacks.ProcessRemoved = NULL;
    _callbacks.InputFilter = NULL;
    _callbacks.WindowPreConfig = window_preconfig;
    _callbacks.WindowAdded = window_added;
    _callbacks.WindowRemoved = window_removed;
    _callbacks.WindowReconfig = window_reconfig;
    _callbacks.WindowRestack = window_restack;
    _callbacks.StackResized = NULL;
    _callbacks.SwitchFocus = NULL;
    _callbacks.LayerReconfig = NULL;
    _callbacks.ApplicationIDChanged = NULL;

    ILOG_DEBUG(ILX_APPLICATIONMANAGER, "Creating SaWManager.\n");
    if (_saw->CreateManager(_saw, &_callbacks, this, &_manager) != DR_OK)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create SaWMan manager!\n");

    if (_compositor->settings.memMonitor)
    {
        _monitor = new MemoryMonitor(this, _compositor->settings.memCritical, _compositor->settings.memLow, _compositor->settings.pgCritical, _compositor->settings.pgLow);
        _monitor->sigStateChanged.connect(sigc::mem_fun(this, &ApplicationManager::handleMemoryState));
    }

    std::string pidFile = PrintF("%silx_compositor.pid", FileSystem::ilxDirectory().c_str());
    FILE* _pidFile = fopen(pidFile.c_str(), "w");
    if (!_pidFile)
        ILOG_ERROR(ILX_APPLICATIONMANAGER, "Could not open %s for writing!\n", pidFile.c_str());
    else
    {
        fprintf(_pidFile, "%d\n", getpid());
        fclose(_pidFile);
    }
}

ApplicationManager::~ApplicationManager()
{
    ILOG_TRACE_F(ILX_APPLICATIONMANAGER);
    delete _monitor;
    __appMan = NULL;
    stopAll();

    if (_manager)
        _manager->Release(_manager);

    if (_saw)
        _saw->Release(_saw);

    for (AppInfoList::iterator it = _infos.begin(); it != _infos.end(); ++it)
        delete *it;

    for (AppInstanceList::iterator it = _instances.begin(); it != _instances.end(); ++it)
        delete *it;

    pthread_mutex_destroy(&_mutex);
    FileSystem::deleteFile(FileSystem::homeDirectory().append("/ilx_compositor.pid"));
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
        return instanceRecord->appInfo();
    return NULL;
}

AppInfo*
ApplicationManager::infoByPID(const pid_t pid)
{
    AppInstance* instanceRecord = instanceByPID(pid);
    if (instanceRecord)
        return instanceRecord->appInfo();
    return NULL;
}

AppInstance*
ApplicationManager::instanceByAppID(unsigned long appID)
{
    for (AppInstanceList::iterator it = _instances.begin(); it != _instances.end(); ++it)
    {
        if (((AppInstance*) (*it))->appID() == appID)
            return ((AppInstance*) (*it));
    }
    return NULL;
}

AppInstance*
ApplicationManager::instanceByInstanceID(unsigned int instanceID)
{
    for (AppInstanceList::iterator it = _instances.begin(); it != _instances.end(); ++it)
    {
        if (((AppInstance*) (*it))->instanceID() == instanceID)
            return ((AppInstance*) (*it));
    }
    return NULL;
}

AppInstance*
ApplicationManager::instanceByPID(const pid_t pid)
{
    pid_t p = pid;
    do
    {
        for (AppInstanceList::iterator it = _instances.begin(); it != _instances.end(); ++it)
        {
            if (((AppInstance*) (*it))->pid() == p)
                return ((AppInstance*) (*it));
        }

        p = getParentPID(p);

        if (p == -1)
            return NULL;

    } while (p != 0);

    return NULL;
}

pid_t
ApplicationManager::getParentPID(const pid_t pid)
{
    std::string stat = PrintF("/proc/%d/stat", pid);

    if (!FileSystem::fileExists(stat))
    {
        ILOG_DEBUG(ILX_APPLICATIONMANAGER, "%s does not exist.\n", stat.c_str());
        return -1;
    }

    FILE *f = fopen(stat.c_str(), "r");

    if (!f)
    {
        ILOG_ERROR(ILX_APPLICATIONMANAGER, "Failed to open '%s' (%s)!\n", stat.c_str(), strerror(errno));
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to read parent PID!\n");
        return 0;
    }

    pid_t p;

    while (true)
    {
        int c = fgetc(f);

        if (c == ')')
            break;

        if (c == EOF)
        {
            ILOG_ERROR(ILX_APPLICATIONMANAGER, "Failed to parse PID from '%s'!\n", stat.c_str());
            fclose(f);
            ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to read parent PID!\n");
        }
    }

    fgetc(f);
    fgetc(f);
    fgetc(f);

    if (fscanf(f, "%d", &p) != 1)
    {
        ILOG_ERROR(ILX_APPLICATIONMANAGER, "Failed to parse PID from '%s'!\n", stat.c_str());
        fclose(f);
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to read parent PID!\n");
    }

    fclose(f);

    return p;
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

DirectResult
ApplicationManager::startApplication(const std::string& name, bool showInstance)
{
    ILOG_INFO( ILX_APPLICATIONMANAGER, "%s( name %s )\n", __FUNCTION__, name.c_str());

    AppInfo* appInfo = infoByName(name);
    if (!appInfo)
        return DR_ITEMNOTFOUND;

    pid_t pid;

    AppInstance* instance = instanceByAppID(appInfo->appID());

    if (instance && !(appInfo->appFlags() & APP_ALLOW_MULTIPLE) && !waitpid(instance->pid(), NULL, WNOHANG))
    {
        ILOG_DEBUG( ILX_APPLICATIONMANAGER, "  -> Already running '%s' (%d)!\n", name.c_str(), instance->pid());
        if (showInstance)
            _compositor->showInstance(instance);
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
        ILOG_ERROR(ILX_APPLICATIONMANAGER, " -> Error vfork!");
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
                ILOG_DEBUG(ILX_APPLICATIONMANAGER, "  -> ARG: %d - %s\n", i - 1, args[i - 1]);

            execvp(args[0], (char**) args);
        } else
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
        instance->setAppInfo(appInfo);
        instance->setStarted(direct_clock_get_millis());
        instance->setPid(pid);
        _instances.push_back(instance);
        pthread_mutex_unlock(&_mutex);
        _compositor->_compComp->signalAppStart(instance);
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
        kill(instance->pid(), SIGKILL);
        for (AppInstanceList::iterator it = _instances.begin(); it != _instances.end(); ++it)
        {
            if (((AppInstance*) *it)->pid() == pid)
            {
                _instances.erase(it);
                break;
            }
        }

        delete instance;
        ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> Application is killed and instance is removed.\n");
        pthread_mutex_unlock(&_mutex);
        return DR_OK;
    } else
    {
        ILOG_ERROR(ILX_APPLICATIONMANAGER, "Could not find an application with pid[%d]!", pid);
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
        AppInfo* info = instance->appInfo();
        ILOG_DEBUG( ILX_APPLICATIONMANAGER, "  -> Killing %s[%d]...\n", info->name().c_str(), instance->pid());

        kill(instance->pid(), SIGKILL);
        delete instance;
        _instances.pop_front();
    }
    pthread_mutex_unlock(&_mutex);
}

void
ApplicationManager::initStartup()
{
    ILOG_TRACE_F(ILX_APPLICATIONMANAGER);
    parseFolder(ILIXI_DATADIR"apps");
    usleep(10000);
    startApplication("StatusBar");
    for (AppInfoList::iterator it = _infos.begin(); it != _infos.end(); ++it)
    {
        if (((AppInfo*) (*it))->appFlags() & APP_AUTO_START)
        {
            usleep(10000);
            startApplication(((AppInfo*) (*it))->name(), false);
        }
    }
    usleep(500000);
    startApplication("Home");
}

void
ApplicationManager::parseFolder(const std::string& folder)
{
    ILOG_TRACE_F(ILX_APPLICATIONMANAGER);
    std::vector<std::string> files;
    files = FileSystem::listDirectory(folder);

    for (unsigned int i = 0; i < files.size(); ++i)
    {
        if (files[i].find(".appdef") != std::string::npos)
            parseAppDef(folder, files[i]);
    }
    _infos.sort(app_sort);
}

DirectResult
ApplicationManager::processAdded(SaWManProcess *process)
{
    ILOG_DEBUG( ILX_APPLICATIONMANAGER, "%s( process %p )\n", __FUNCTION__, process);
    ILOG_DEBUG( ILX_APPLICATIONMANAGER, "  -> Process [%d] Fusionee [%lu]\n", process->pid, process->fusion_id);

    if (process->fusion_id == 1)
        return DR_OK;

    AppInstance* instance = instanceByPID(process->pid);
    if (instance)
        return DR_OK;

    ILOG_WARNING(ILX_APPLICATIONMANAGER, "Process[%d] is not recognized!\n", process->pid);
    return DR_ITEMNOTFOUND;
}

DirectResult
ApplicationManager::processRemoved(AppInstance* instance)
{
    ILOG_TRACE_F(ILX_APPLICATIONMANAGER);
    _compositor->processRemoved(instance);
    AppInstance* ins;
    pthread_mutex_lock(&_mutex);
    for (AppInstanceList::iterator it = _instances.begin(); it != _instances.end(); ++it)
    {
        ins = ((AppInstance*) *it);
        if (instance == ins)
        {
            it = _instances.erase(it);
            break;
        }
    }
    pthread_mutex_unlock(&_mutex);
    return DR_OK;
}

DirectResult
ApplicationManager::processTerminated(AppInstance* instance)
{
    ILOG_TRACE_F(ILX_APPLICATIONMANAGER);
    _compositor->processTerminated(instance);
    AppInstance* ins;
    pthread_mutex_lock(&_mutex);
    for (AppInstanceList::iterator it = _instances.begin(); it != _instances.end(); ++it)
    {
        ins = ((AppInstance*) *it);
        if (instance == ins)
        {
            it = _instances.erase(it);
            break;
        }
    }
    pthread_mutex_unlock(&_mutex);
    return DR_OK;
}

DirectResult
ApplicationManager::windowPreconfig(SaWManWindowConfig *config)
{
    return DR_OK;
}

DirectResult
ApplicationManager::windowAdded(SaWManWindowInfo *info)
{
    ILOG_DEBUG( ILX_APPLICATIONMANAGER, "%s( info %p )\n", __FUNCTION__, info);

    SaWManProcess process;
    _manager->GetProcessInfo(_manager, info->handle, &process);

    ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> Process [%d] Window [%lu]\n", process.pid, info->handle);

    AppInstance* instance = instanceByPID(process.pid);
    if (!instance)
        return DR_FAILURE;

    AppInfo* appInfo = instance->appInfo();

    ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> App: %s Window: %u\n", appInfo->name().c_str(), instance->windowCount());

    if ((info->config.options & DWOP_KEEP_SIZE) || (appInfo->appFlags() & APP_ALLOW_WINDOW_CONFIG))
    {
        ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> keeping window size.\n");
    } else if (appInfo->appFlags() & APP_OSK)
    {
        if (!instance->view())
        {
            ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> setting window config for APP_OSK.\n");
            DFBRectangle r = _compositor->_oskGeometry.dfbRect();
            info->config.bounds = r;
            _manager->SetWindowConfig(_manager, info->handle, (SaWManWindowConfigFlags) (SWMCF_SIZE), &info->config);
        }
    } else if (appInfo->appFlags() & APP_STATUSBAR)
    {
        ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> setting window config for APP_STATUSBAR.\n");
        DFBRectangle r = _compositor->_barGeometry.dfbRect();
        info->config.bounds = r;
        _manager->SetWindowConfig(_manager, info->handle, (SaWManWindowConfigFlags) (SWMCF_POSITION | SWMCF_SIZE), &info->config);
    } else if (instance->windowCount() == 0)
    {
        ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> setting window config for Default.\n");
        DFBRectangle r = _compositor->_appGeometry.dfbRect();
        info->config.bounds = r;
        _manager->SetWindowConfig(_manager, info->handle, (SaWManWindowConfigFlags) (SWMCF_POSITION | SWMCF_SIZE), &info->config);
    }

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

    ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> Process [%d] Window [%lu]\n", process.pid, info->handle);

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
    ILOG_DEBUG( ILX_APPLICATIONMANAGER, "%s( reconfig %p )\n", __FUNCTION__, reconfig);
    ILOG_DEBUG( ILX_APPLICATIONMANAGER, "  -> Window [%lu] Flags [0x%04x]\n", reconfig->handle, reconfig->flags);

    SaWManProcess process;
    _manager->GetProcessInfo(_manager, reconfig->handle, &process);

    SaWManWindowInfo winInfo;
    _manager->GetWindowInfo(_manager, reconfig->handle, &winInfo);

    AppInstance* instance = instanceByPID(process.pid);
    if (instance)
    {
        AppInfo* info = instance->appInfo();

        if (!(info->appFlags() & APP_ALLOW_WINDOW_CONFIG) && (reconfig->flags & SWMCF_POSITION))
        {
            ILOG_WARNING(ILX_APPLICATIONMANAGER, "SWMCF_POSITION not allowed!\n");
            reconfig->flags = (SaWManWindowConfigFlags) (reconfig->flags & ~SWMCF_POSITION);
        }

        _manager->Lock(_manager);
        _compositor->configWindow(instance, reconfig, &winInfo);
        _manager->Unlock(_manager);
    }
    return DR_OK;
}

DirectResult
ApplicationManager::windowRestack(SaWManWindowHandle handle, SaWManWindowHandle relative, SaWManWindowRelation relation)
{
    SaWManProcess process;
    _manager->GetProcessInfo(_manager, handle, &process);

    SaWManWindowInfo winInfo;
    _manager->GetWindowInfo(_manager, handle, &winInfo);

    DFBWindowID related = 0;
    if (relative)
    {
        SaWManWindowInfo relativeInfo;
        _manager->GetWindowInfo(_manager, relative, &relativeInfo);
        related = relativeInfo.win_id;
    }

    AppInstance* instance = instanceByPID(process.pid);

    if (instance)
    {
        _manager->Lock(_manager);
        _compositor->restackWindow(instance, &winInfo, relation, related);
        _manager->Unlock(_manager);
    }

    return DR_OK;
}

bool
ApplicationManager::parseAppDef(const std::string& folder, const std::string& file)
{
    ILOG_DEBUG(ILX_APPLICATIONMANAGER, "parseAppDef ( %s )\n", file.c_str());
    xmlParserCtxtPtr ctxt;
    xmlDocPtr doc;
    std::string filePath = folder + "/" + file;

    XMLReader xml;
    if (xml.loadFile(filePath) == false)
    {
        ILOG_WARNING(ILX_APPLICATIONMANAGER, "Could not parse!\n");
        return false;
    }

    xmlNodePtr group = xml.currentNode();

    xmlChar* name;
    xmlChar* author;
    xmlChar* licence;
    xmlChar* category;
    xmlChar* version;
    xmlChar* icon;
    xmlChar* exec;
    xmlChar* args = NULL;
    xmlChar* appFlags = NULL;
    xmlChar* depFlags = NULL;

    while (group != NULL)
    {

        ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> Parsing %s...\n", group->name);

        if (xmlStrcmp(group->name, (xmlChar*) "name") == 0)
            name = xmlNodeGetContent(group->children);

        else if (xmlStrcmp(group->name, (xmlChar*) "author") == 0)
            author = xmlNodeGetContent(group->children);

        else if (xmlStrcmp(group->name, (xmlChar*) "licence") == 0)
            licence = xmlNodeGetContent(group->children);

        else if (xmlStrcmp(group->name, (xmlChar*) "category") == 0)
            category = xmlNodeGetContent(group->children);

        else if (xmlStrcmp(group->name, (xmlChar*) "version") == 0)
            version = xmlNodeGetContent(group->children);

        else if (xmlStrcmp(group->name, (xmlChar*) "icon") == 0)
            icon = xmlNodeGetContent(group->children);

        else if (xmlStrcmp(group->name, (xmlChar*) "exec") == 0)
            exec = xmlNodeGetContent(group->children);

        else if (xmlStrcmp(group->name, (xmlChar*) "args") == 0)
            args = xmlNodeGetContent(group->children);

        else if (xmlStrcmp(group->name, (xmlChar*) "flags") == 0)
            appFlags = xmlNodeGetContent(group->children);

        else if (xmlStrcmp(group->name, (xmlChar*) "deps") == 0)
            depFlags = xmlNodeGetContent(group->children);

        group = group->next;
    }

    ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> done.\n", file.c_str());

    addApplication((const char*) name, (const char*) author, (const char*) licence, (const char*) category, (const char*) version, (const char*) icon, (const char*) exec, (const char*) args, (const char*) appFlags, (const char*) depFlags);

    xmlFree(name);
    xmlFree(author);
    xmlFree(licence);
    xmlFree(category);
    xmlFree(version);
    xmlFree(icon);
    xmlFree(exec);
    if (args)
        xmlFree(args);
    if (appFlags)
        xmlFree(appFlags);
    if (depFlags)
        xmlFree(depFlags);

    return true;
}

void
ApplicationManager::addApplication(const char* name, const char* author, const char* licence, const char* category, const char* version, const char* icon, const char* exec, const char* args, const char* appFlags, const char* depFlags)
{
    if (infoByName(name))
        return;

    std::string path;
    if (!searchExec(exec, path))
        return;

    AppInfo* app = new AppInfo();
    app->setName(name);

    app->setAuthor(author);
    app->setLicence(licence);
    app->setCategory(category);
    app->setVersion(version);
    if (icon)
        app->setIcon(icon);
    if (exec)
        app->setPath(path);
    if (args)
        app->setArgs(args);
    if (appFlags)
        app->setAppFlags(appFlags);
    if (depFlags)
        app->setDepFlags(depFlags);
    _infos.push_back(app);
}

bool
ApplicationManager::searchExec(const char* exec, std::string& execPath)
{
    ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> exec: %s\n", exec);

    if (exec[0] == '/')
    {
        if ((access(exec, X_OK) == 0))
            return true;
        else
            return false;
    } else if (exec[0] == '$')
    {
        std::string file;
        char* execCp = strdup(exec);
        char* env = strtok(execCp, "$");
        char* path = NULL;
        while (env != NULL)
        {
            if (!path)
                path = env;
            else
            {
                char* var = getenv(path);
                if (!var)
                    break;
                file = var;
                file.append("/").append(env);
                if (access(file.c_str(), X_OK) == 0)
                {
                    free(execCp);
                    execPath = file;
                    return true;
                }
            }
            env = strtok(NULL, "$");
        }
        free(execCp);
        return false;
    } else
    {
        ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> using $PATH\n");
        char* path = getenv("PATH");
        char* pathCp = strdup(path);
        char* dir = strtok(pathCp, ":");

        std::string file;
        while (dir != NULL)
        {
            file = dir;
            file.append("/").append(exec);
            ILOG_DEBUG(ILX_APPLICATIONMANAGER, "   -> checking: %s\n", dir);
            if (access(file.c_str(), X_OK) == 0)
            {
                free(pathCp);
                execPath = file;
                return true;
            }
            dir = strtok(NULL, ":");
        }
        free(pathCp);

        file = ILIXI_BINDIR;
        ILOG_DEBUG(ILX_APPLICATIONMANAGER, "   -> checking: %s\n", dir);
        file.append(exec);
        if (access(file.c_str(), X_OK) == 0)
        {
            execPath = file;
            return true;
        }

        ILOG_DEBUG(ILX_APPLICATIONMANAGER, " -> not found: %s\n", exec);
    }
    return false;
}

void
ApplicationManager::handleMemoryState(MemoryMonitor::MemoryState state)
{
    switch (state)
    {
    case MemoryMonitor::Critical:
        {
            // kill a non-system app.
            ILOG_WARNING(ILX_APPLICATIONMANAGER, "MemoryMonitor reports Critical.\n");
            AppInstance* instance = NULL;
            AppInstance* match = NULL;
            AppInfo* info;
            for (AppInstanceList::iterator it = _instances.begin(); it != _instances.end(); ++it)
            {
                instance = (AppInstance*) *it;
                info = instance->appInfo();
                if (!(info->appFlags() & APP_SYSTEM))
                {
                    match = instance;
                    info = match->appInfo();
                    break;
                }
            }
            if (!match)
                break;
            ILOG_WARNING(ILX_APPLICATIONMANAGER, " -> Stopping %s\n", info->name().c_str());
            std::stringstream ss;
            ss << info->name() << " is terminated automatically.";
            Notify notify("Memory is critically low!", ss.str());
            notify.setIcon(ILIXI_DATADIR"images/default.png");
            notify.show();
            _compositor->killApp(match);
        }
        break;

    case MemoryMonitor::Low:
        {
            ILOG_WARNING(ILX_APPLICATIONMANAGER, "MemoryMonitor reports Low.\n");
            // kill an invisible and non-system app.
            AppInstance* instance = NULL;
            AppInstance* match = NULL;
            AppInfo* info;
            for (AppInstanceList::iterator it = _instances.begin(); it != _instances.end(); ++it)
            {
                instance = (AppInstance*) *it;
                info = instance->appInfo();
                if (!(info->appFlags() & APP_SYSTEM) && instance->view() && !instance->view()->visible())
                {
                    match = instance;
                    info = match->appInfo();
                    break;
                }
            }
            if (!match)
                break;
            ILOG_WARNING(ILX_APPLICATIONMANAGER, " -> Stopping %s\n", info->name().c_str());
            std::stringstream ss;
            ss << info->name() << " is terminated automatically.";
            Notify notify("Memory is low!", ss.str());
            notify.setIcon(ILIXI_DATADIR"images/default.png");
            notify.show();
            _compositor->killApp(match);
        }
        break;

    case MemoryMonitor::Normal:
        ILOG_INFO(ILX_APPLICATIONMANAGER, "MemoryMonitor reports Normal.\n");
        break;

    default:
        break;
    }
}

} /* namespace ilixi */
