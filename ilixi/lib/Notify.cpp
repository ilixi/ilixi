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

#include <lib/Notify.h>
#include <core/DaleDFB.h>
#include <core/ComponentData.h>
#include <core/AppBase.h>

namespace ilixi
{

Notify::Notify(const std::string& title, const std::string& body, const std::string& iconPath)
        : _title(title),
          _body(body),
          _icon(iconPath)
{
}

Notify::~Notify()
{
}

void
Notify::setIcon(const std::string& iconPath)
{
    _icon = iconPath;
}

void
Notify::show()
{
    Compositor::NotificationData msg;
    snprintf(msg.body, 128, "%s", _body.c_str());
    snprintf(msg.icon, 128, "%s", _icon.c_str());
    snprintf(msg.title, 128, "%s", _title.c_str());
    msg.client = getpid();

    IComaComponent* comp;
    DaleDFB::comaGetComponent("Compositor", &comp);

    void *ptr;
    DaleDFB::comaGetLocal(sizeof(Compositor::NotificationData), &ptr);
    Compositor::NotificationData* data = (Compositor::NotificationData*) ptr;
    *data = msg;

    DaleDFB::comaCallComponent(comp, 0, (void*) data);

    comp->Release(comp);
}

} /* namespace ilixi */
