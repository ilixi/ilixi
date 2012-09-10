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

#include <core/DaleDFB.h>
#include <core/Logger.h>
#include <sys/types.h>
#include <unistd.h>

namespace ilixi
{

IFusionDale* DaleDFB::__dale = NULL;
IComa* DaleDFB::__coma = NULL;
IComaComponent* DaleDFB::__oskComp = NULL;

D_DEBUG_DOMAIN( ILX_DALEDFB, "ilixi/core/DaleDFB", "DaleDFB");

DaleDFB::DaleDFB()
{
}

DaleDFB::~DaleDFB()
{
}

IComa*
DaleDFB::getComa()
{
    return __coma;
}

DFBResult
DaleDFB::comaGetComponent(const char* name, IComaComponent** component)
{
    ILOG_TRACE_F(ILX_DALEDFB);
    if (!__coma)
        return DFB_FAILURE;

    if (__coma->GetComponent(__coma, name, 500, component) != DR_OK)
    {
        ILOG_ERROR( ILX_DALEDFB, "%s( %s ) failed!\n", __FUNCTION__, name);
        return DFB_FAILURE;
    }
    return DFB_OK;
}

DFBResult
DaleDFB::comaGetLocal(unsigned int bytes, void** ret)
{
    ILOG_TRACE_F(ILX_DALEDFB);
    if (!__coma)
        return DFB_FAILURE;

    if (__coma->GetLocal(__coma, bytes, ret) != DR_OK)
    {
        ILOG_ERROR( ILX_DALEDFB, "%s( %u ) failed!\n", __FUNCTION__, bytes);
        return DFB_FAILURE;
    }
    return DFB_OK;
}

DFBResult
DaleDFB::comaCallComponent(IComaComponent* component, ComaMethodID method, void* arg)
{
    ILOG_TRACE_F(ILX_DALEDFB);
    if (!__coma)
        return DFB_FAILURE;

    if (component)
    {
        int ret_val;
        if (component->Call(component, method, arg, &ret_val) != DR_OK)
        {
            ILOG_ERROR( ILX_DALEDFB,
                       "%s( %lu ) failed!\n", __FUNCTION__, method);
            return DFB_FAILURE;
        }
        return DFB_OK;
    }
    return DFB_FAILURE;
}

DFBResult
DaleDFB::showOSK(const Rectangle& rect)
{
    ILOG_TRACE_F(ILX_DALEDFB);

    if (getOSKComp() == DFB_FAILURE)
        return DFB_FAILURE;

    OSKRequest request;
    request.inputRect = rect.dfbRect();
    request.mode = 0;
    request.process = getpid();

    void *ptr;
    __coma->GetLocal(__coma, sizeof(OSKRequest), &ptr);
    OSKRequest* req = (OSKRequest*) ptr;
    *req = request;

    int ret_val;
    DirectResult ret = __oskComp->Call(__oskComp, 0, (void*) req, &ret_val);
    if (ret)
    {
        D_DERROR(ret, "WebKit/DFB: %s() failed!\n", __FUNCTION__);
        return (DFBResult) ret;
    }
    return DFB_OK;
}

DFBResult
DaleDFB::hideOSK()
{
    ILOG_TRACE_F(ILX_DALEDFB);

    if (getOSKComp() == DFB_FAILURE)
        return DFB_FAILURE;

    int ret_val;
    DirectResult ret = __oskComp->Call(__oskComp, 1, NULL, &ret_val);
    if (ret)
    {
        D_DERROR(ret, "WebKit/DFB: %s() failed!\n", __FUNCTION__);
        return (DFBResult) ret;
    }
    return DFB_OK;
}

DFBResult
DaleDFB::initDale(int* argc, char*** argv)
{
    ILOG_TRACE_F(ILX_DALEDFB);
    DirectResult ret = FusionDaleInit(argc, argv);
    if (ret)
    {
        ILOG_THROW(ILX_DALEDFB, "FusionDaleInit() failed!\n");
        return (DFBResult) ret;
    }

    if (!__dale)
    {
        DirectResult ret = FusionDaleCreate(&__dale);
        if (ret)
        {
            ILOG_THROW(ILX_DALEDFB, "FusionDaleCreate() failed!\n");
            return (DFBResult) ret;
        }
    }

    if (!__coma)
    {
        DirectResult ret = __dale->EnterComa(__dale, "directfb.org", &__coma);
        if (ret)
        {
            ILOG_THROW( ILX_DALEDFB,
                       "IFusionDale::EnterComa('directfb.org') failed!\n");
            return (DFBResult) ret;
        }
    }
    return DFB_OK;
}

void
DaleDFB::releaseDale()
{
    ILOG_TRACE_F(ILX_DALEDFB);
    if (__oskComp)
    {
        __oskComp->Release(__oskComp);
        __oskComp = NULL;
    }

    if (__coma)
    {
        __coma->Release(__coma);
        __coma = NULL;
    }

    if (__dale)
    {
        __dale->Release(__dale);
        __dale = NULL;
    }
}

DFBResult
DaleDFB::getOSKComp()
{
    if (!__coma)
        return DFB_FAILURE;

    if (!__oskComp)
    {
        static bool tryOnce = true;
        if (tryOnce)
        {
            tryOnce = false;
            DirectResult ret = __coma->GetComponent(__coma, "OSKComponent", 500,
                                                    &__oskComp);
            if (ret)
            {
                ILOG_ERROR( ILX_DALEDFB, "Cannot get OSK component!\n");
                return DFB_FAILURE;
            }
        } else
            return DFB_FAILURE;
    }
    return DFB_OK;
}

} /* namespace ilixi */
