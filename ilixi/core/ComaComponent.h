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

#ifndef ILIXI_COMACOMPONENT_H_
#define ILIXI_COMACOMPONENT_H_

#include <string>
extern "C"
{
#include <fusiondale.h>
}

namespace ilixi
{

class ComaComponent
{
public:
    ComaComponent(const std::string& name, int numNotifications = 0);

    virtual
    ~ComaComponent();

    std::string
    name() const;

protected:
    DirectResult
    allocate(int bytes, void** arg);

    DirectResult
    deallocate(void* ptr);

    void
    init();

    void
    notify(ComaNotificationID id, void* arg);

    virtual DirectResult
    comaMethod(ComaMethodID method, void* arg);

    void
    createNotification(ComaNotificationID id, ComaNotifyFunc func);

private:
    std::string _name;
    IComaComponent* _component;

    friend void
    ComaComponentMethod(void *ctx, ComaMethodID method, void *arg,
                        unsigned int magic);
};

} /* namespace ilixi */
#endif /* ILIXI_COMACOMPONENT_H_ */
