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

#ifndef ILIXI_POPUPCOMPONENT_H_
#define ILIXI_POPUPCOMPONENT_H_

#include <core/ComaComponent.h>
#include <compositor/AppInstance.h>

#include <libxml/tree.h>

namespace ilixi
{

class ILXCompositor;
class NotificationManager;

//! Compositor's ComaComponent.
/*!
 * Provides signals and notifications for various compositor events.
 */
class CompositorComponent : public ilixi::ComaComponent
{
public:
    /*!
     * Constructor.
     */
    CompositorComponent(ILXCompositor* compositor);

    /*!
     * Destructor.
     */
    virtual
    ~CompositorComponent();

    /*!
     * Used for sending two visibilty notifications.
     * One for new foreground application and another for previous foreground application.
     *
     * @param current instance.
     * @param previous instance.
     */
    void
    signalInstanceChanged(AppInstance* current, AppInstance* previous);

    /*!
     * Used for sending a notification when switcher becomes visible or hidden.
     *
     * @param showing true if switcher is visible.
     */
    void
    signalSwitcher(bool showing);

    /*!
     * Sends the list of registered applications.
     */
    void
    sendAppList();

    /*!
     * Used for sending a notification when an application starts running.
     *
     * If application is already running, you should send a visibility notification.
     */
    void
    signalAppStart(AppInstance* instance);

    /*!
     * Used for sending a notification when an application quits or crashed.
     */
    void
    signalAppQuit(AppInstance* instance);

    /*!
     * Used for sending a notification if the foreground requires a BACK button.
     *
     * @param showing true if foreground application uses BACK button.
     */
    void
    signalBack(bool showing);

    /*!
     * Used for sending a notification when an application becomes visible of hidden.
     *
     * @param instance application instance.
     * @param visible true if application is visible.
     */
    void
    notifyVisibility(AppInstance* instance, bool visible);

    /*!
     * Sends an acknowledgement message when a notification is shown, closed or clicked.
     *
     * @param method @see NotificationMethod
     * @param uuid notification UUID
     * @param client process ID
     */
    void
    signalNotificationAck(int method, char* uuid, pid_t client);

protected:
    /*!
     * This method handles incoming IPC requests.
     *
     * @param method @see CompositorMethodID
     * @param arg custom argument
     */
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

private:
    //! Owner.
    ILXCompositor* _compositor;
    //! Used for managing and displaying incoming notifications.
    NotificationManager* _notificationMan;

    //! Parses compositor options.
    void
    parseOptions(xmlDocPtr doc);
};

} /* namespace ilixi */
#endif /* ILIXI_POPUPCOMPONENT_H_ */
