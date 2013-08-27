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
#include <ui/VBoxLayout.h>
#include <core/Logger.h>

namespace ilixi
{

Stacker::Stacker(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setBackgroundImage(ILIXI_DATADIR"images/grid.png", true);
    sigVisible.connect(sigc::mem_fun(this, &Stacker::initStacker));
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

void
Stacker::initStacker()
{
    int x = width() * .2;
    int w = width() * .4;
    int h = height() / 2;

    VBoxLayout* left = new VBoxLayout();
    addWidget(left);
    left->setGeometry(0, 0, x, height());

    PushButton* bBack = new PushButton("Send to Back");
    left->addWidget(bBack);
    bBack->sigClicked.connect(sigc::mem_fun(this, &Stacker::sendBack));

    PushButton* bFront = new PushButton("Bring to front");
    left->addWidget(bFront);
    bFront->sigClicked.connect(sigc::mem_fun(this, &Stacker::bringForward));

    PushButton* bRaise = new PushButton("Raise");
    left->addWidget(bRaise);
    bRaise->sigClicked.connect(sigc::mem_fun(this, &Stacker::raise));

    PushButton* bLower = new PushButton("Lower");
    left->addWidget(bLower);
    bLower->sigClicked.connect(sigc::mem_fun(this, &Stacker::lower));

    ToolButton* pb2 = new ToolButton("Button 2");
    pb2->setZ(0);
    addWidget(pb2);
    pb2->setGeometry(x, 0, w, h);

    ToolButton* pb3 = new ToolButton("Button 3");
    pb2->setZ(0);
    addWidget(pb3);
    pb3->setGeometry(x + w, 0, w, h);

    ToolButton* pb4 = new ToolButton("Button 4");
    pb2->setZ(0);
    addWidget(pb4);
    pb4->setGeometry(x, h, w, h);

    ToolButton* pb5 = new ToolButton("Button 5");
    pb2->setZ(0);
    addWidget(pb5);
    pb5->setGeometry(x + w, h, w, h);

    pb1 = new ToolButton("Button 1");
    addWidget(pb1);
    pb1->setGeometry(x + x, h / 2, w, h);
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::Stacker app(argc, argv);
    app.exec();
    return 0;
}
