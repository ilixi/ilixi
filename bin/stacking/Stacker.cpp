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
#include "core/Logger.h"

namespace ilixi
{

  Stacker::Stacker(int argc, char* argv[]) :
      Application(&argc, &argv)
  {
    setTitle("Stacking");
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

    PushButton* pb2 = new PushButton("Button 2");
    addWidget(pb2);
    pb2->setGeometry(160, 50, 100, 100);

    PushButton* pb3 = new PushButton("Button 3");
    addWidget(pb3);
    pb3->setGeometry(340, 50, 100, 100);

    PushButton* pb4 = new PushButton("Button 4");
    addWidget(pb4);
    pb4->setGeometry(160, 160, 100, 100);

    PushButton* pb5 = new PushButton("Button 5");
    addWidget(pb5);
    pb5->setGeometry(340, 160, 100, 100);

    pb1 = new PushButton("Button 1");
    addWidget(pb1);
    pb1->setGeometry(200, 100, 100, 100);
  }

  Stacker::~Stacker()
  {
  }

  void
  Stacker::sendBack()
  {
    ILOG_DEBUG(ILX, "back: %d\n", widgetToBack(pb1));
    update();
  }

  void
  Stacker::bringForward()
  {
    ILOG_DEBUG(ILX, "front: %d\n", widgetToFront(pb1));
    update();
  }

  void
  Stacker::raise()
  {
    pb1->setZ(pb1->z() + 1);
    ILOG_INFO(ILX, "Z: %d\n", pb1->z());
//    ILOG_DEBUG(ILX, "raise: %d\n", raiseWidget(pb1));
    update();
  }

  void
  Stacker::lower()
  {
    pb1->setZ(pb1->z() - 1);
    ILOG_INFO(ILX, "Z: %d\n", pb1->z());
//    ILOG_DEBUG(ILX, "lower: %d\n", lowerWidget(pb1));
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
