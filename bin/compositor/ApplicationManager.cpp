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

#include "ApplicationManager.h"
#include <core/Logger.h>
#include <lib/FileSystem.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdexcept>
#include "Compositor.h"

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPLICATIONMANAGER, "ilixi/comp/AppMan",
               "ApplicationManager");

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

ApplicationManager::ApplicationManager(Compositor* compositor)
        : _compositor(compositor)
{
    ILOG_DEBUG(ILX_APPLICATIONMANAGER, "Initialising Application Manager.\n");
    pthread_mutex_init(&_mutex, NULL);
    if (SaWManInit(NULL, NULL) != DR_OK)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to initialise SaWMan!\n");

    if (SaWManCreate(&_saw) != DR_OK)
        ILOG_THROW(ILX_APPLICATIONMANAGER, "Unable to create SaWMan!\n");

    memset(&_callbacks, 0, sizeof(_callbacks));
    _callbacks.Start = start_request;
    _callbacks.Stop = stop_request;
    _callbacks.ProcessAdded = process_added;
    _callbacks.ProcessRemoved = process_removed;
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
        ILOG_THROW(ILX_APPLICATIONMANAGER,
                   "Unable to create SaWMan manager!\n");

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
        return instanceRecord->appInfo();
    return NULL;
}

AppInfo*
ApplicationManager::infoByPID(pid_t pid)
{
    AppInstance* instanceRecord = instanceByPID(pid);
    if (instanceRecord)
        return instanceRecord->appInfo();
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
ApplicationManager::startApplication(const std::string& name, bool autoStart)
{
    ILOG_INFO( ILX_APPLICATIONMANAGER,
              "%s( name %s )\n", __FUNCTION__, name.c_str());

    AppInfo* appInfo = infoByName(name);
    if (!appInfo)
        return DR_ITEMNOTFOUND;

    pid_t pid;

    AppInstance* instance = instanceByAppID(appInfo->appID());

    if (instance && !(appInfo->appFlags() & APP_ALLOW_MULTIPLE) && !waitpid(
            instance->pid(), NULL, WNOHANG))
    {
        ILOG_DEBUG(
                ILX_APPLICATIONMANAGER,
                "  -> Already running '%s' (%d)!\n", name.c_str(), instance->pid());
        if (!autoStart)
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
        for (AppInstanceList::iterator it = _instances.begin();
                it != _instances.end(); ++it)
        {
            if (((AppInstance*) *it)->pid() == pid)
            {
                _instances.erase(it);
                break;
            }
        }

        kill(instance->pid(), SIGKILL);
        delete instance;
        pthread_mutex_unlock(&_mutex);
        return DR_OK;
    } else
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
        AppInfo* info = instance->appInfo();
        ILOG_DEBUG(
                ILX_APPLICATIONMANAGER,
                "  -> Killing %s[%d]...\n", info->name().c_str(), instance->pid());

        kill(instance->pid(), SIGKILL);
        delete instance;
        _instances.pop_front();
    }
    pthread_mutex_unlock(&_mutex);
}

void
ApplicationManager::initStartup()
{
    usleep(10000);
    startApp("StatusBar");
    usleep(10000);
    startApp("Home");
    for (AppInfoList::iterator it = _infos.begin(); it != _infos.end(); ++it)
    {
        if (((AppInfo*) (*it))->appFlags() & APP_AUTO_START)
        {
            usleep(10000);
            startApplication(((AppInfo*) (*it))->name(), true);
        }
    }
}

DirectResult
ApplicationManager::processAdded(SaWManProcess *process)
{
    ILOG_DEBUG( ILX_APPLICATIONMANAGER,
               "%s( process %p )\n", __FUNCTION__, process);
    ILOG_DEBUG(
            ILX_APPLICATIONMANAGER,
            "  -> Process [%d] Fusionee [%lu]\n", process->pid, process->fusion_id);

    if (process->fusion_id == 1)
        return DR_OK;

    AppInstance* instance = instanceByPID(process->pid);
    if (instance)
        return DR_OK;

    ILOG_WARNING(ILX_APPLICATIONMANAGER,
                 "Process[%d] is not recognized!\n", process->pid);
//    kill(process->pid, SIGKILL);
    return DR_ITEMNOTFOUND;
}

DirectResult
ApplicationManager::processRemoved(SaWManProcess *process)
{
    ILOG_DEBUG( ILX_APPLICATIONMANAGER,
               "%s( process %p )\n", __FUNCTION__, process);
    ILOG_DEBUG(
            ILX_APPLICATIONMANAGER,
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

    ILOG_DEBUG(ILX_APPLICATIONMANAGER,
               " -> Process [%d] Window [%lu]\n", process.pid, info->handle);

    AppInstance* instance = instanceByPID(process.pid);
    if (!instance)
        return DR_FAILURE;

    AppInfo* appInfo = instance->appInfo();

    if (appInfo->appFlags() & APP_OSK)
    {
        ILOG_DEBUG(ILX_APPLICATIONMANAGER,
                   " -> setting window config for APP_OSK.\n");
        DFBRectangle r = { 0, 0, _compositor->width(), 400 };
        info->config.bounds = r;
        _manager->SetWindowConfig(_manager, info->handle,
                                  (SaWManWindowConfigFlags) (SWMCF_SIZE),
                                  &info->config);
    } else if (appInfo->appFlags() & APP_STATUSBAR)
    {
        ILOG_DEBUG(ILX_APPLICATIONMANAGER,
                   " -> setting window config for APP_STATUSBAR.\n");
        DFBRectangle r = { 0, 0, _compositor->width(), 55 };
        info->config.bounds = r;
        _manager->SetWindowConfig(
                _manager, info->handle,
                (SaWManWindowConfigFlags) (SWMCF_POSITION | SWMCF_SIZE),
                &info->config);
    } else if (!(appInfo->appFlags() & APP_ALLOW_WINDOW_CONFIG))
    {
        ILOG_DEBUG(ILX_APPLICATIONMANAGER,
                   " -> setting window config for Default.\n");
        DFBRectangle r = { 0, 0, _compositor->width(),
                           _compositor->height() - 50 };
        info->config.bounds = r;
        _manager->SetWindowConfig(_manager, info->handle,
                                  (SaWManWindowConfigFlags) (SWMCF_SIZE),
                                  &info->config);
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

    ILOG_DEBUG(ILX_APPLICATIONMANAGER,
               " -> Process [%d] Window [%lu]\n", process.pid, info->handle);

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
    ILOG_DEBUG(
            ILX_APPLICATIONMANAGER,
            "  -> Window [%lu] Flags [0x%04x]\n", reconfig->handle, reconfig->flags);

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
            ILOG_WARNING(ILX_APPLICATIONMANAGER,
                         "SWMCF_POSITION not allowed!\n");
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

void
ApplicationManager::initApps()
{
    std::vector<std::string> files;
    files = FileSystem::listDirectory(ILIXI_DATADIR"apps");

    for (unsigned int i = 0; i < files.size(); ++i)
    {
        if (files[i].find(".appdef") != std::string::npos)
            parseAppDef(files[i].c_str());
    }
    _infos.sort(app_sort);
}

bool
ApplicationManager::parseAppDef(const char* file)
{
    ILOG_DEBUG(ILX_APPLICATIONMANAGER, "Parsing appdef file: %s\n", file);
    xmlParserCtxtPtr ctxt;
    xmlDocPtr doc;
    std::string filePath = ILIXI_DATADIR"apps/";
    filePath.append(file);

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL)
    {
        ILOG_ERROR(ILX_APPLICATIONMANAGER,
                   "Failed to allocate parser context\n");
        return false;
    }

    doc = xmlCtxtReadFile(
            ctxt,
            filePath.c_str(),
            NULL,
            XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID | XML_PARSE_NOBLANKS);

    if (doc == NULL)
    {
        xmlFreeParserCtxt(ctxt);
        ILOG_ERROR(ILX_APPLICATIONMANAGER,
                   "Failed to parse appdef: %s\n", file);
        return false;
    }

    if (ctxt->valid == 0)
    {
        xmlFreeDoc(doc);
        xmlFreeParserCtxt(ctxt);
        ILOG_ERROR(ILX_APPLICATIONMANAGER,
                   "Failed to validate appdef: %s\n", file);
        return false;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    xmlNodePtr group = root->xmlChildrenNode;

    xmlChar* name;
    xmlChar* author;
    xmlChar* licence;
    xmlChar* category;
    xmlChar* version;
    xmlChar* icon;
    xmlChar* exec;
    xmlChar* args;
    xmlChar* appFlags;
    xmlChar* depFlags;

    while (group != NULL)
    {

        ILOG_DEBUG(ILX_APPLICATIONMANAGER, " Parsing %s...\n", group->name);

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

    ILOG_DEBUG(ILX_APPLICATIONMANAGER, "Parsed appdef file: %s\n", file);

    addApplication((const char*) name, (const char*) author,
                   (const char*) licence, (const char*) category,
                   (const char*) version, (const char*) icon,
                   (const char*) exec, (const char*) args,
                   (const char*) appFlags, (const char*) depFlags);

    xmlFree(name);
    xmlFree(author);
    xmlFree(licence);
    xmlFree(category);
    xmlFree(version);
    xmlFree(icon);
    xmlFree(exec);
    xmlFree(args);
    xmlFree(appFlags);
    xmlFree(depFlags);

    xmlFreeDoc(doc);
    xmlFreeParserCtxt(ctxt);

    return true;
}

void
ApplicationManager::addApplication(const char* name, const char* author, const char* licence, const char* category, const char* version, const char* icon, const char* exec, const char* args, const char* appFlags, const char* depFlags)
{
    if (infoByName(name))
        return;

    std::string str("which ");
    str.append(exec);
    int ret = system(str.c_str());
    if (ret == 256)
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
        app->setPath(exec);
    if (args)
        app->setArgs(args);
    app->setAppFlags(appFlags);
    app->setDepFlags(depFlags);
    _infos.push_back(app);
}

} /* namespace ilixi */
