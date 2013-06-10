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
#include <ui/GridView.h>
#include <core/DaleDFB.h>
#include <core/ComponentData.h>

namespace ilixi
{

class PAppButton;

class PHome : public Application
{
    typedef std::vector<Compositor::AppData> AppDataVector;
    typedef std::vector<PAppButton*> AppButtonVector;

public:
    PHome(int argc, char* argv[]);

    virtual
    ~PHome();

    void
    runApp(const std::string& name);

protected:
    void
    initButtons(const PHome::AppDataVector& dataVector);

private:
    IComaComponent* _compositor;
    GridView* _view;


    AppButtonVector _buttons;

    void
    requestAppList();

    void
    updateHomeGeometry();

    friend void
    appStatusChanged(void* ctx, void* arg);

    friend void
    receiveAppList(void* ctx, void* arg);

};

}

#endif /* HOME_H_ */
