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

#include <lib/Notify.h>
#include <core/DaleDFB.h>
#include <core/ComponentData.h>
#include <core/Logger.h>
#include <lib/Util.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_NOTIFY, "ilixi/lib/Notify", "Notify");

Notify::Notify(const std::string& title, const std::string& body, const std::string& iconPath, const std::string& tag)
        : _title(title),
          _body(body),
          _icon(iconPath),
          _tag(tag)
{
    snprintf(_uuid, 37, "%s", createUUID().c_str());
    ILOG_TRACE_F(ILX_NOTIFY);
    ILOG_DEBUG(ILX_NOTIFY, " -> title: %s\n", title.c_str());
    ILOG_DEBUG(ILX_NOTIFY, " -> body: %s\n", body.c_str());
    ILOG_DEBUG(ILX_NOTIFY, " -> iconPath: %s\n", iconPath.c_str());
    ILOG_DEBUG(ILX_NOTIFY, " -> UUID: %s\n", _uuid);
}

Notify::~Notify()
{
    DaleDFB::removeNotification(this);
}

const std::string&
Notify::body() const
{
    return _body;
}

const std::string&
Notify::icon() const
{
    return _icon;
}

const std::string&
Notify::tag() const
{
    return _tag;
}

const std::string&
Notify::title() const
{
    return _title;
}

const char*
Notify::uuid() const
{
    return _uuid;
}

void
Notify::setBody(const std::string& body)
{
    _body = body;
}

void
Notify::setIcon(const std::string& iconPath)
{
    _icon = iconPath;
}

void
Notify::setTag(const std::string& tag)
{
    _tag = tag;
}

void
Notify::setTitle(const std::string& title)
{
    _title = title;
}

void
Notify::show()
{
    ILOG_TRACE_F(ILX_NOTIFY);

    void *ptr;
    DaleDFB::comaGetLocal(sizeof(Compositor::NotificationData), &ptr);
    Compositor::NotificationData* data = (Compositor::NotificationData*) ptr;

    snprintf(data->body, 256, "%s", _body.c_str());
    snprintf(data->icon, 256, "%s", _icon.c_str());
    snprintf(data->origin, 256, "");
    snprintf(data->tag, 128, "%s", _tag.c_str());
    snprintf(data->title, 128, "%s", _title.c_str());
    snprintf(data->uuid, 37, "%s", _uuid);

    data->client = getpid();

    DaleDFB::addNotification(this, (void*) data);
}

} /* namespace ilixi */
