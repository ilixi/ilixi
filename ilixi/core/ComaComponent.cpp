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

#include <core/ComaComponent.h>
#include <core/Logger.h>
#include <core/DaleDFB.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_COMACOMP, "ilixi/core/ComaComponent", "ComaComponent");

void
ComaComponentMethod(void *ctx, ComaMethodID method, void *arg, unsigned int magic)
{
    ILOG_TRACE_F(ILX_COMACOMP);
    ComaComponent* comp = (ComaComponent*) ctx;
    DirectResult ret = comp->comaMethod(method, arg);
    comp->_component->Return(comp->_component, ret, magic);
}

ComaComponent::ComaComponent(const std::string& name, int numNotifications)
        : _name(name)
{
    if (DaleDFB::getComa()->CreateComponent(DaleDFB::getComa(), name.c_str(), ComaComponentMethod, numNotifications, this, &_component) != DR_OK)
        ILOG_THROW(ILX_COMACOMP, "CreateComponent failed!\n");
}

ComaComponent::~ComaComponent()
{
    _component->Release(_component);
}

std::string
ComaComponent::name() const
{
    return _name;
}

void
ComaComponent::init()
{
    ILOG_TRACE_F(ILX_COMACOMP);
    _component->Activate(_component);
}

void
ComaComponent::notify(ComaNotificationID id, void* arg)
{
    ILOG_TRACE_F(ILX_COMACOMP);
    _component->Notify(_component, id, arg);
}

DirectResult
ComaComponent::allocate(int bytes, void** arg)
{
    ILOG_TRACE_F(ILX_COMACOMP);
    return DaleDFB::getComa()->Allocate(DaleDFB::getComa(), bytes, arg);
}

DirectResult
ComaComponent::deallocate(void* ptr)
{
    ILOG_TRACE_F(ILX_COMACOMP);
    return DaleDFB::getComa()->Deallocate(DaleDFB::getComa(), ptr);
}

DirectResult
ComaComponent::comaMethod(ComaMethodID method, void* arg)
{
    return DR_NOIMPL;
}

void
ComaComponent::createNotification(ComaNotificationID id, ComaNotifyFunc func, ComaNotificationFlags flags)
{
    ILOG_TRACE_F(ILX_COMACOMP);
    _component->InitNotification(_component, id, func, this, flags);

}

} /* namespace ilixi */
