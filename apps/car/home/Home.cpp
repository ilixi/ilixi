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

#include "Home.h"
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_HOMEAPP, "ilixi/car/Home", "Home");

Image* Home::_circle = NULL;
Image* Home::_circle_sm = NULL;

void
appStatusChanged(void* ctx, void* arg)
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    Home* home = (Home*) ctx;
    Compositor::VisibilityData notification = *((Compositor::VisibilityData*) arg);
    home->_pages->setAppStatus(notification);
}

void
receiveAppList(void* ctx, void* arg)
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    Home* home = (Home*) ctx;

    int* size = ((int*) arg);
    ILOG_DEBUG(ILX_HOMEAPP, " -> Size: %d\n", *size);
    Compositor::AppData *srcdata = (Compositor::AppData*) (size + 1);
    Home::AppDataVector vec;

    for (int i = 0; i < *size; ++i)
    {
        ILOG_DEBUG(ILX_HOMEAPP, "%s - %s\n", srcdata[i].name, srcdata[i].icon);

        Compositor::AppData data;
        snprintf(data.name, 64, "%s", srcdata[i].name);
        snprintf(data.icon, 128, "%s", srcdata[i].icon);
        vec.push_back(data);
    }
    home->initButtons(vec);
}

Home::Home(int argc, char* argv[])
        : Application(&argc, &argv, OptDaleAuto),
          _compositor(NULL)
{
    ILOG_TRACE_F(ILX_HOMEAPP);

    if (!_circle)
    {
        _circle = new Image(ILIXI_DATADIR"car/home/circle.png", Size(48, 48));
        _circle_sm = new Image(ILIXI_DATADIR"car/home/circle_small.png", Size(38, 38));
    }

    _pages = new NumPages();
    _pages->sigAppStart.connect(sigc::mem_fun(this, &Home::runApp));
    addWidget(_pages);

    DaleDFB::comaGetComponent("Compositor", &_compositor);

    appWindow()->sigGeometryUpdated.connect(sigc::mem_fun(this, &Home::updateHomeGeometry));
    sigVisible.connect(sigc::mem_fun(this, &Home::requestAppList));
}

Home::~Home()
{
    delete _circle;
    delete _circle_sm;
    if (_compositor)
        _compositor->Release(_compositor);
}

void
Home::runApp(const char* name)
{
    if (_compositor)
    {
        void *ptr;
        DaleDFB::comaGetLocal(128, &ptr);
        char* n = (char*) ptr;
        snprintf(n, 128, "%s", name);
        DaleDFB::comaCallComponent(_compositor, Compositor::StartApp, (void*) n);
    }
}

void
Home::initButtons(const Home::AppDataVector& dataVector)
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    for (Home::AppDataVector::const_iterator it = dataVector.begin(); it != dataVector.end(); ++it)
        _pages->addItem(((Compositor::AppData) *it).name, ((Compositor::AppData) *it).icon);
    _pages->initPages();
}

void
Home::requestAppList()
{
    if (_compositor)
    {
        _compositor->Listen(_compositor, Compositor::AppStatus, appStatusChanged, this);
        _compositor->Listen(_compositor, Compositor::SendingAppList, receiveAppList, this);
        DaleDFB::comaCallComponent(_compositor, Compositor::GetAppList, NULL);
    }
}

void
Home::updateHomeGeometry()
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    _pages->setGeometry(0, 100, width(), height() - 200);
}

}

int
main(int argc, char* argv[])
{
    ilixi::Home app(argc, argv);
    app.exec();
    return 0;
}
