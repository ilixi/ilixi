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

#include <compositor/CompositorComponent.h>
#include <compositor/Compositor.h>
#include <compositor/NotificationManager.h>
#include <compositor/ApplicationManager.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_COMPCOMP, "ilixi/Coma/CompositorComponent", "CompositorComponent");

CompositorComponent::CompositorComponent(ILXCompositor* compositor)
        : ComaComponent("Compositor", Compositor::CNumNotifications),
          _compositor(compositor),
          _notificationMan(NULL)
{
    init();

    createNotification(Compositor::AppStatus, NULL);

    createNotification(Compositor::BackKeyVisible, NULL, CNF_NONE);
    createNotification(Compositor::BackKeyHidden, NULL, CNF_NONE);

    createNotification(Compositor::NotificationAck, NULL);

    createNotification(Compositor::SendingAppList, NULL);

    createNotification(Compositor::SwitcherHidden, NULL, CNF_NONE);
    createNotification(Compositor::SwitcherVisible, NULL, CNF_NONE);

    _notificationMan = new NotificationManager(compositor);
}

CompositorComponent::~CompositorComponent()
{
    delete _notificationMan;
}

void
CompositorComponent::signalInstanceChanged(AppInstance* cur, AppInstance* pre)
{
    if (cur)
        notifyVisibility(cur, true);

    if (pre)
        notifyVisibility(pre, false);
}

void
CompositorComponent::signalSwitcher(bool showing)
{
    if (showing)
        notify(Compositor::SwitcherVisible, NULL);
    else
        notify(Compositor::SwitcherHidden, NULL);
}

void
CompositorComponent::sendAppList()
{
    ILOG_TRACE_F(ILX_COMPCOMP);
    AppInfoList list = _compositor->appMan()->applicationList();
    std::vector<Compositor::AppData> dataVector;

    for (AppInfoList::iterator it = list.begin(); it != list.end(); ++it)
    {
        if (!(((AppInfo*) *it)->appFlags() & APP_SYSTEM))
        {
            Compositor::AppData data;
            snprintf(data.name, 64, "%s", ((AppInfo*) *it)->name().c_str());
            snprintf(data.icon, 256, "%s", ((AppInfo*) *it)->icon().c_str());
            dataVector.push_back(data);
        }
    }

    int* vec;
    allocate(sizeof(int) + dataVector.size() * sizeof(Compositor::AppData), (void**) &vec);
    *vec = dataVector.size();
    memcpy(vec + 1, &dataVector[0], dataVector.size() * sizeof(Compositor::AppData));
    notify(Compositor::SendingAppList, vec);
    ILOG_DEBUG(ILX_COMPCOMP, "Sent application vector.\n");
}

void
CompositorComponent::signalAppStart(AppInstance* instance)
{
    Compositor::VisibilityData vNo;
    snprintf(vNo.name, 64, "%s", instance->appInfo()->name().c_str());
    vNo.pid = instance->pid();
    vNo.status = (Compositor::AppStatusFlags) (Compositor::AppStarting | (instance->appInfo()->appFlags() & APP_ALLOW_MULTIPLE));

    Compositor::VisibilityData* tPid;
    allocate(sizeof(Compositor::VisibilityData), (void**) &tPid);
    *tPid = vNo;

    notify(Compositor::AppStatus, tPid);
}

void
CompositorComponent::signalAppQuit(AppInstance* instance)
{
    Compositor::VisibilityData vNo;
    snprintf(vNo.name, 64, "%s", instance->appInfo()->name().c_str());
    vNo.pid = instance->pid();
    vNo.status = (Compositor::AppStatusFlags) (Compositor::AppQuit | (instance->appInfo()->appFlags() & APP_ALLOW_MULTIPLE));

    Compositor::VisibilityData* tPid;
    allocate(sizeof(Compositor::VisibilityData), (void**) &tPid);
    *tPid = vNo;

    notify(Compositor::AppStatus, tPid);
}

void
CompositorComponent::signalBack(bool showing)
{
    if (showing)
        notify(Compositor::BackKeyVisible, NULL);
    else
        notify(Compositor::BackKeyHidden, NULL);
}

DirectResult
CompositorComponent::comaMethod(ComaMethodID method, void *arg)
{
    DirectResult ret = DR_OK;
    switch (method)
    {
    case Compositor::AddNotification:
        {
            Compositor::NotificationData data = *((Compositor::NotificationData*) arg);
            ILOG_DEBUG(ILX_COMPCOMP, "AddNotification request from PID[%d] for UUID: %s\n", data.client, data.uuid);
            _notificationMan->addNotification(data);
            break;
        }

    case Compositor::ShowHome:
        _compositor->toggleLauncher(true);
        break;

    case Compositor::ShowSwitcher:
        _compositor->toggleSwitcher(true);
        break;

    case Compositor::HideHome:
        _compositor->toggleLauncher(false);
        break;

    case Compositor::HideSwitcher:
        _compositor->toggleSwitcher(false);
        break;

    case Compositor::StartApp:
        {
            char name[64];
            snprintf(name, 64, "%s", ((char*) arg));
            _compositor->appMan()->startApplication(name);
        }
        break;

    case Compositor::GetFPS:
        {
            *((float*) arg) = _compositor->_fps->fps();
        }
        break;

    case Compositor::GetAppList:
        ILOG_TRACE_F(ILX_COMPCOMP);
        sendAppList();
        break;

    case Compositor::SendBackKey:
        ILOG_TRACE_F(ILX_COMPCOMP);
        ILOG_DEBUG(ILX_COMPCOMP, "SendBackKey\n");
        _compositor->sendOSKInput(DIKS_BACK);
        break;

    case Compositor::SetOptions:
        {
            const char* opts = ((const char*) arg);

            xmlDocPtr doc = xmlReadDoc((xmlChar*) opts, "noname.xml", NULL, 0);
            if (doc == NULL)
            {
                ILOG_ERROR(ILX_COMPCOMP, "Failed to parse options\n");
                return DR_FAILURE;
            }
            parseOptions(doc);
            xmlFreeDoc(doc);

            break;
        }

    default:
        ret = DR_NOIMPL;
        break;
    }
    return ret;
}

void
CompositorComponent::notifyVisibility(AppInstance* instance, bool visible)
{
    if (instance->view() && !instance->view()->visible())
        return;

    Compositor::VisibilityData vNo;
    snprintf(vNo.name, 64, "%s", instance->appInfo()->name().c_str());
    vNo.pid = instance->pid();
    vNo.status = (Compositor::AppStatusFlags) ((visible ? Compositor::AppVisible : Compositor::AppHidden) | (instance->appInfo()->appFlags() & APP_ALLOW_MULTIPLE));

    Compositor::VisibilityData* tPid;
    allocate(sizeof(Compositor::VisibilityData), (void**) &tPid);
    *tPid = vNo;

    notify(Compositor::AppStatus, tPid);
    ILOG_DEBUG(ILX_COMPCOMP, "%s is now %s!\n", instance->appInfo()->name().c_str(), visible ? "visible": "hidden");
}

void
CompositorComponent::signalNotificationAck(int method, char* uuid, pid_t client)
{

    Compositor::NotificationAckData* data;
    allocate(sizeof(Compositor::NotificationAckData), (void**) &data);
    data->method = (Compositor::NotificationMethod) method;
    snprintf(data->uuid, 37, "%s", uuid);
    data->client = client;
    notify(Compositor::NotificationAck, data);
    ILOG_DEBUG(ILX_COMPCOMP, "Sent NotificationAck[%d] to %d for UUID: %s\n", method, client, uuid);
}

void
CompositorComponent::parseOptions(xmlDocPtr doc)
{
    xmlNodePtr root = xmlDocGetRootElement(doc);
    xmlNodePtr child = root->xmlChildrenNode;
    xmlNodePtr element = NULL;
    while (child != NULL)
    {
        ILOG_DEBUG(ILX_COMPCOMP, " Parsing %s...\n", child->name);

        if (xmlStrcmp(child->name, (xmlChar*) "ClickToFocus") == 0)
        {
            xmlChar* enabled = xmlGetProp(child, (xmlChar*) "enabled");
            _compositor->settings.clickFocus = atoi((const char*) enabled);
            xmlFree(enabled);
        } else if (xmlStrcmp(child->name, (xmlChar*) "Animations") == 0)
        {
            xmlChar* enabled = xmlGetProp(child, (xmlChar*) "enabled");
            _compositor->settings.animations = atoi((const char*) enabled);
            xmlFree(enabled);
            element = child->children;
            while (element != NULL)
            {
                if (xmlStrcmp(element->name, (xmlChar*) "Showing") == 0)
                {
                    xmlChar* duration = xmlGetProp(element, (xmlChar*) "duration");
                    _compositor->settings.durationShow = atoi((const char*) duration);
                    xmlFree(duration);

                    xmlChar* zoom = xmlGetProp(element, (xmlChar*) "zoom");
                    bool zoomF = atoi((const char*) zoom);
                    if (zoomF)
                        _compositor->settings.showAnimProps = AppView::AnimatedProperty(_compositor->settings.showAnimProps | AppView::Zoom);
                    else
                        _compositor->settings.showAnimProps = AppView::AnimatedProperty(_compositor->settings.showAnimProps & ~AppView::Zoom);
                    xmlFree(zoom);

                    xmlChar* opacity = xmlGetProp(element, (xmlChar*) "opacity");
                    bool opacityF = atoi((const char*) opacity);
                    if (opacityF)
                        _compositor->settings.showAnimProps = AppView::AnimatedProperty(_compositor->settings.showAnimProps | AppView::Opacity);
                    else
                        _compositor->settings.showAnimProps = AppView::AnimatedProperty(_compositor->settings.showAnimProps & ~AppView::Opacity);
                    xmlFree(opacity);
                } else if (xmlStrcmp(element->name, (xmlChar*) "Hiding") == 0)
                {
                    xmlChar* duration = xmlGetProp(element, (xmlChar*) "duration");
                    _compositor->settings.durationHide = atoi((const char*) duration);
                    xmlFree(duration);

                    // Zoom flag
                    xmlChar* zoom = xmlGetProp(element, (xmlChar*) "zoom");
                    bool zoomF = atoi((const char*) zoom);
                    if (zoomF)
                        _compositor->settings.hideAnimProps = AppView::AnimatedProperty(_compositor->settings.hideAnimProps | AppView::Zoom);
                    else
                        _compositor->settings.hideAnimProps = AppView::AnimatedProperty(_compositor->settings.hideAnimProps & ~AppView::Zoom);
                    xmlFree(zoom);

                    // Opacity flag
                    xmlChar* opacity = xmlGetProp(element, (xmlChar*) "opacity");
                    bool opacityF = atoi((const char*) opacity);
                    if (opacityF)
                        _compositor->settings.hideAnimProps = AppView::AnimatedProperty(_compositor->settings.hideAnimProps | AppView::Opacity);
                    else
                        _compositor->settings.hideAnimProps = AppView::AnimatedProperty(_compositor->settings.hideAnimProps & ~AppView::Opacity);
                    xmlFree(opacity);
                }

                element = element->next;
            }

        } else if (xmlStrcmp(child->name, (xmlChar*) "Brightness") == 0)
        {
            xmlChar* value = xmlGetProp(child, (xmlChar*) "value");
            _compositor->setLayerOpacity(atoi((const char*) value));
            xmlFree(value);
        }

        child = child->next;
    }

}

} /* namespace ilixi */
