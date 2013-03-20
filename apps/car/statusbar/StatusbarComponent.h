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
#include <core/ComponentData.h>
#include <directfb.h>

namespace ilixi
{

class ILXStatusBar;

class StatusbarComponent : public ComaComponent
{
public:
    StatusbarComponent(ILXStatusBar* statusbar);

    virtual
    ~StatusbarComponent();

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

    void
    sendNotification(DFBSurfaceID id);

private:
    ILXStatusBar* _statusbar;
};

} /* namespace ilixi */
#endif /* SOUNDCOMPONENT_H_ */
