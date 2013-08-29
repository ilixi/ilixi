/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#ifndef OSK_H_
#define OSK_H_

#include <core/Application.h>
#include "Keyboard.h"

using namespace ilixi;

class ILXOSK : public Application
{
public:
    ILXOSK(int argc, char* argv[]);

    virtual
    ~ILXOSK();

    void
    forwardString(const std::string& str);

#if ILIXI_HAVE_LIBWNN
    /*!
     * This method gets window events first.
     */
    virtual bool
    windowPreEventFilter(const DFBWindowEvent &event);
#endif

private:
    OSKHelper* _helper;
    Keyboard* _keyboard;
};

#endif /* OSK_H_ */
