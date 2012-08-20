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
#include <ui/ToolButton.h>
#include <core/Logger.h>
#include <sigc++/bind.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_HOMEAPP, "ilixi/Home", "Home");

void
receiveAppList(void* ctx, void* arg)
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    Home* home = (Home*) ctx;

    int* size = ((int*) arg);
    ILOG_DEBUG(ILX_HOMEAPP, " -> Size: %d\n", *size);
    Home::AppData *srcdata = (Home::AppData*) (size + 1);
    Home::AppDataVector vec;

    for (int i = 0; i < *size; ++i)
    {
        ILOG_DEBUG(ILX_HOMEAPP, "%s - %s\n", srcdata[i].name, srcdata[i].icon);

        Home::AppData data;
        snprintf(data.name, 128, "%s", srcdata[i].name);
        snprintf(data.icon, 128, "%s", srcdata[i].icon);
        vec.push_back(data);
    }
    home->initButtons(vec);
}

Home::Home(int argc, char* argv[])
        : Application(&argc, &argv, OptDale)
{
    ILOG_TRACE_W(ILX_HOMEAPP);
    setTitle("Home");
    setBackgroundImage(ILIXI_DATADIR"compositor/bg.png");
    setPaletteFromFile(ILIXI_DATADIR"statusbar/def_palette.xml");

    _font = new Font("decker");
    _font->setSize(18);
    _font->setStyle(Font::Bold);

    AppBase::comaGetComponent("CompositorComponent", &_compositor);
    _compositor->Listen(_compositor, 7, receiveAppList, this);
    AppBase::comaCallComponent(_compositor, 8, NULL);

    sigGeometryUpdated.connect(sigc::mem_fun(this, &Home::updateHomeGeometry));
}

Home::~Home()
{
    delete _font;
}

void
Home::runApp(const char* name)
{
    void *ptr;
    comaGetLocal(128, &ptr);
    char* n = (char*) ptr;
    snprintf(n, 128, "%s", name);
    AppBase::comaCallComponent(_compositor, 7, (void*) n);
}

void
Home::initButtons(const AppDataVector& dataVector)
{
    ILOG_TRACE_W(ILX_HOMEAPP);
    for (AppDataVector::const_iterator it = dataVector.begin();
            it != dataVector.end(); ++it)
        addButton(((AppData) *it).name, ((AppData) *it).icon);
}

void
Home::addButton(const char* name, const char* icon)
{
    ToolButton* button = new ToolButton(name);
    button->setToolButtonStyle(ToolButton::IconAboveText);
    button->setDrawFrame(false);
    button->setFont(_font);
    button->setIcon(icon, Size(96, 96));
    addChild(button);

    button->sigClicked.connect(
            sigc::bind<const char*>(sigc::mem_fun(this, &Home::runApp), button->text().c_str()));
}

void
Home::updateHomeGeometry()
{
    int hOffset = (height() - 450) / 2;
    int wC = width() / 150.0;
    int xOffset = (width() - (wC * 150)) / 2;
    if (wC)
    {
        int y = -1;
        int i = 0;
        for (WidgetList::iterator it = _children.begin(); it != _children.end();
                ++it)
        {
            ToolButton* button = dynamic_cast<ToolButton*>(*it);
            if (button)
            {
                if (i % wC == 0)
                    y++;

                button->moveTo(xOffset + i % wC * 150, hOffset + y * 150);
                button->setSize(145, 145);
                i++;
            }
        }
    }
}

}

int
main(int argc, char* argv[])
{
    ilixi::Home app(argc, argv);
    app.exec();
    return 0;
}
