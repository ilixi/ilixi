/*
 * ComaComponent.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: tarik
 */

#include "core/ComaComponent.h"
#include "core/AppBase.h"
#include "core/Logger.h"

D_DEBUG_DOMAIN( ILX_COMACOMP, "ilixi/core/ComaComponent", "ComaComponent");

namespace ilixi
{

void
ComaComponentMethod(void *ctx, ComaMethodID method, void *arg,
        unsigned int magic)
{
    ILOG_TRACE_F(ILX_COMACOMP);
    ComaComponent* comp = (ComaComponent*) ctx;
    DirectResult ret = comp->comaMethod(method, arg);
    comp->_component->Return(comp->_component, ret, magic);
}

ComaComponent::ComaComponent(const std::string& name, int numNotifications)
        : _name(name)
{
    if (AppBase::getComa()->CreateComponent(AppBase::getComa(), name.c_str(),
            ComaComponentMethod, numNotifications, this, &_component) != DR_OK)
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
    return AppBase::getComa()->Allocate(AppBase::getComa(), bytes, arg);
}

DirectResult
ComaComponent::deallocate(void* ptr)
{
    ILOG_TRACE_F(ILX_COMACOMP);
    return AppBase::getComa()->Deallocate(AppBase::getComa(), ptr);
}

DirectResult
ComaComponent::comaMethod(ComaMethodID method, void* arg)
{
    return DR_NOIMPL;
}

void
ComaComponent::createNotification(ComaNotificationID id, ComaNotifyFunc func)
{
    ILOG_TRACE_F(ILX_COMACOMP);
    _component->InitNotification(_component, id, func, this, CNF_DEALLOC_ARG);

}

} /* namespace ilixi */
