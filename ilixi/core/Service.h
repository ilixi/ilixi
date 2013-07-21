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

#ifndef ILIXI_SERVICE_H_
#define ILIXI_SERVICE_H_

#include <types/Enums.h>

#include <directfb.h>

namespace ilixi
{
//! Helper ilixi process running along windowed applications.
class Service
{
public:
    Service(int* argc, char*** argv, AppOptions opts = OptNone);

    virtual
    ~Service();

    /*!
     * You can reimplement this method to create your own custom main loop.
     */
    void
    exec();

    /*!
     * Terminates application.
     */
    void
    quit();

protected:
    /*!
     * Custom user events are handled in this function.
     * \sa postUserEvent
     */
    virtual void
    handleUserEvent(const DFBUserEvent& event);

    /*!
     * All events are handled using this function.
     *
     * @param timeout in ms.
     * @param forceWait causes to wait even if there is a pending UI update
     */
    void
    handleEvents(int32_t timeout, bool forceWait = false);

private:
    //! Service instance.
    static Service* __instance;
};

} /* namespace ilixi */
#endif /* ILIXI_SERVICE_H_ */
