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

#ifndef ILIXI_POPUPCOMPONENT_H_
#define ILIXI_POPUPCOMPONENT_H_

#include <core/ComaComponent.h>
#include "AppInstance.h"

#include <libxml/tree.h>

namespace ilixi
{

class ILXCompositor;
class NotificationManager;

class CompositorComponent : public ilixi::ComaComponent
{
public:
    CompositorComponent(ILXCompositor* compositor);

    virtual
    ~CompositorComponent();

    void
    signalInstanceChanged(AppInstance* current, AppInstance* previous);

    void
    signalSwitcher(bool showing);

    void
    sendAppList();

    void
    signalAppStart(AppInstance* instance);

    void
    signalBack(bool showing);

    void
    notifyVisibility(AppInstance* instance, bool visible);

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

private:
    ILXCompositor* _compositor;
    NotificationManager* _notificationMan;

    void
    parseOptions(xmlDocPtr doc);
};

} /* namespace ilixi */
#endif /* ILIXI_POPUPCOMPONENT_H_ */
