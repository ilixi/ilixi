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

#ifndef HOME_H_
#define HOME_H_

#include <core/Application.h>
#include <core/DaleDFB.h>
#include "NumPages.h"

namespace ilixi
{

class Home : public Application
{
    typedef std::vector<Compositor::AppData> AppDataVector;

public:
    Home(int argc, char* argv[]);

    virtual
    ~Home();

    void
    runApp(const char* name);

    static Image* _circle;
    static Image* _circle_sm;

protected:
    void
    initButtons(const Home::AppDataVector& dataVector);

private:
    IComaComponent* _compositor;
    NumPages* _pages;

    void
    requestAppList();

    void
    updateHomeGeometry();

    friend void
    appStatusChanged(void* ctx, void* arg);

    friend void
    appStarting(void* ctx, void* arg);

    friend void
    receiveAppList(void* ctx, void* arg);

};

}

#endif /* HOME_H_ */
