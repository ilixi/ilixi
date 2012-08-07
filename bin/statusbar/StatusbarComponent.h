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

#ifndef SOUNDCOMPONENT_H_
#define SOUNDCOMPONENT_H_

#include <core/ComaComponent.h>
#include <directfb.h>

namespace ilixi
{

class StatusBar;

class StatusbarComponent : public ComaComponent
{
public:
    enum StatCompMethods
    {
        GetOptimalSize, AddContent, RemoveContent
    };

    enum StatCompNotifications
    {
        ContentAdded, ContentRemoved, StatCompNumNotifications
    };

    StatusbarComponent(StatusBar* statusbar);

    virtual
    ~StatusbarComponent();

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

    void
    sendNotification(DFBSurfaceID id);

private:
    StatusBar* _statusbar;

    void
    addContentDispatchCallback(void *ctx, ComaNotificationID notification,
                               void *arg);

    void
    removeContentDispatchCallback(void *ctx, ComaNotificationID notification,
                                  void *arg);

};

} /* namespace ilixi */
#endif /* SOUNDCOMPONENT_H_ */
