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
#include <core/AppBase.h>
#include <ui/Widget.h>

namespace ilixi
{

Notify::Notify(const std::string& title, const std::string& text,
               const std::string& iconPath)
        : _title(title),
          _text(text),
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
    NotifyData msg;
    snprintf(msg.path, 128, "%s", _icon.c_str());
    snprintf(msg.sender, 128, "%s", AppBase::title().c_str());
    snprintf(msg.text, 128, "%s", _text.c_str());
    snprintf(msg.title, 128, "%s", _title.c_str());

    IComaComponent* comp;
    AppBase::comaGetComponent("CompositorComponent", &comp);

    void *ptr;
    AppBase::comaGetLocal(sizeof(NotifyData), &ptr);
    NotifyData* data = (NotifyData*) ptr;
    *data = msg;

    AppBase::comaCallComponent(comp, 0, (void*) data);

    comp->Release(comp);
}

} /* namespace ilixi */
