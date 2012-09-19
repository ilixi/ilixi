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

#ifndef ILIXI_DALEDFB_H_
#define ILIXI_DALEDFB_H_

#include <directfb.h>
extern "C"
{
#include <fusiondale.h>
}
#include <lib/Notify.h>
#include <types/Rectangle.h>
#include <list>
namespace ilixi
{

class DaleDFB
{
    friend class AppBase;
    friend class Notify;
public:

    /*!
     * Returns Coma interface.
     */
    static IComa*
    getComa();

    static DFBResult
    comaGetComponent(const char* name, IComaComponent** component);

    static DFBResult
    comaGetLocal(unsigned int bytes, void** ret);

    static DFBResult
    comaCallComponent(IComaComponent* component, ComaMethodID method,
                      void* arg);

    static DFBResult
    showOSK(const Rectangle& rect);

    static DFBResult
    hideOSK();

private:
    static IFusionDale* __dale;
    static IComa* __coma;
    static IComaComponent* __oskComp;
    static IComaComponent* __compComp;

    struct OSKRequest
    {
        DFBRectangle inputRect;
        int mode;
        pid_t process;
    };

    typedef std::list<Notify*> Notifications;
    static Notifications __nots;

    DaleDFB();

    virtual
    ~DaleDFB();

    static DFBResult
    initDale(int* argc, char*** argv);

    static void
    releaseDale();

    static DFBResult
    getOSKComp();

    static DFBResult
    getCompComp();

    static DFBResult
    addNotification(Notify*, void* data);

    static DFBResult
    removeNotification(Notify*);

    static void
    notificationListener(void* ctx, void* arg);

};

} /* namespace ilixi */
#endif /* DALEDFB_H_ */
