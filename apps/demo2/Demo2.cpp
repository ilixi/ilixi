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

#include "Demo2.h"
#include "ScrollItem.h"
#include <ui/HBoxLayout.h>
#include <ui/ListBox.h>

using namespace ilixi;

Demo2::Demo2(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setLayout(new HBoxLayout());

    _icon = new Icon();
    _icon->setMaximumSize(300, 300);
    addWidget(_icon);

    ListBox* box = new ListBox();
    box->setDrawFrame(true);
    addWidget(box);

    ScrollItem* item;
    char file[128];
    for (int i = 0; i < 40; ++i)
    {
        sprintf(file, "%sgallery/%d.jpg", ILIXI_DATADIR, i % 5);
        item = new ScrollItem(file);
        item->setImage(new Image(file, 50, 50));
        box->addItem(item);
        item->sigFocused.connect(sigc::mem_fun(this, &Demo2::showImage));
    }
}

Demo2::~Demo2()
{
}

void
Demo2::showImage(const std::string& file)
{
    _icon->setImage(file);
    _icon->update();
}

int
main(int argc, char* argv[])
{
    Demo2 app(argc, argv);
    app.exec();
    return 0;
}
