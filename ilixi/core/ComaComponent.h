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

#ifndef ILIXI_COMACOMPONENT_H_
#define ILIXI_COMACOMPONENT_H_

#include <string>
extern "C"
{
#include <fusiondale.h>
}

namespace ilixi
{
//! Helps create a FusionDale Coma component.
class ComaComponent
{
public:
    /*!
     * Constructor.
     *
     * Creates coma component.
     *
     * @param name unique name.
     * @param numNotifications Number of notifications.
     */
    ComaComponent(const std::string& name, int numNotifications = 0);

    /*!
     * Destructor.
     */
    virtual
    ~ComaComponent();

    /*!
     * Returns name of coma component.
     */
    std::string
    name() const;

protected:
    /*!
     * Allocates block of shared memory.
     *
     * @param bytes
     * @param arg
     */
    DirectResult
    allocate(int bytes, void** arg);

    /*!
     * Deallocate block of shard memory.
     *
     * @param ptr
     */
    DirectResult
    deallocate(void* ptr);

    /*!
     * Activates component.
     */
    void
    init();

    /*!
     * Send a notification to all listeners.
     *
     * @param id
     * @param arg
     */
    void
    notify(ComaNotificationID id, void* arg);

    /*!
     * This is executed once a new message arrives.
     *
     * @param method
     * @param arg
     */
    virtual DirectResult
    comaMethod(ComaMethodID method, void* arg);

    /*!
     * Setup a notification.
     *
     * @param id
     * @param func
     * @param flags by default deallocate after notification is dispatched.
     */
    void
    createNotification(ComaNotificationID id, ComaNotifyFunc func, ComaNotificationFlags flags = CNF_DEALLOC_ARG);

private:
    //! This property stores name of component.
    std::string _name;
    //! This property stores FusionDale Coma component.
    IComaComponent* _component;

    friend void
    ComaComponentMethod(void *ctx, ComaMethodID method, void *arg, unsigned int magic);
};

} /* namespace ilixi */
#endif /* ILIXI_COMACOMPONENT_H_ */
