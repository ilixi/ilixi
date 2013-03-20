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

#include "Stacker.h"
#include <ui/PushButton.h>
#include <core/Logger.h>

namespace ilixi
{

Stacker::Stacker(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setBackgroundImage(ILIXI_DATADIR"images/grid.png");

    PushButton* cont1 = new PushButton("Send to Back");
    addWidget(cont1);
    cont1->setZ(10);
    cont1->sigClicked.connect(sigc::mem_fun(this, &Stacker::sendBack));
    cont1->moveTo(10, 0);

    PushButton* cont2 = new PushButton("Bring to front");
    addWidget(cont2);
    cont2->setZ(10);
    cont2->sigClicked.connect(sigc::mem_fun(this, &Stacker::bringForward));
    cont2->moveTo(10, 50);

    PushButton* cont3 = new PushButton("Raise");
    addWidget(cont3);
    cont3->sigClicked.connect(sigc::mem_fun(this, &Stacker::raise));
    cont3->moveTo(0, 100);

    PushButton* cont4 = new PushButton("Lower");
    addWidget(cont4);
    cont4->sigClicked.connect(sigc::mem_fun(this, &Stacker::lower));
    cont4->moveTo(0, 150);

    ToolButton* pb2 = new ToolButton("Button 2");
    pb2->setZ(0);
    addWidget(pb2);
    pb2->setGeometry(160, 50, 200, 50);

    ToolButton* pb3 = new ToolButton("Button 3");
    pb2->setZ(0);
    addWidget(pb3);
    pb3->setGeometry(340, 50, 200, 50);

    ToolButton* pb4 = new ToolButton("Button 4");
    pb2->setZ(0);
    addWidget(pb4);
    pb4->setGeometry(160, 160, 200, 50);

    ToolButton* pb5 = new ToolButton("Button 5");
    pb2->setZ(0);
    addWidget(pb5);
    pb5->setGeometry(340, 160, 200, 50);

    pb1 = new ToolButton("Button 1");
    addWidget(pb1);
    pb1->setGeometry(200, 100, 200, 50);
}

Stacker::~Stacker()
{
}

void
Stacker::sendBack()
{
    pb1->setZ(-1);
    ILOG_INFO(ILX, "Z: %d\n", pb1->z());
    update();
}

void
Stacker::bringForward()
{
    pb1->setZ(1);
    ILOG_INFO(ILX, "Z: %d\n", pb1->z());
    update();
}

void
Stacker::raise()
{
    pb1->setZ(pb1->z() + 1);
    ILOG_INFO(ILX, "Z: %d\n", pb1->z());
    update();
}

void
Stacker::lower()
{
    pb1->setZ(pb1->z() - 1);
    ILOG_INFO(ILX, "Z: %d\n", pb1->z());
    update();
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::Stacker app(argc, argv);
    app.exec();
    return 0;
}
