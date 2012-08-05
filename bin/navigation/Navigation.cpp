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

#include "Navigation.h"
#include "ui/PushButton.h"
#include "ui/HBoxLayout.h"
#include "ui/VBoxLayout.h"
#include "core/Logger.h"

namespace ilixi
{

  Navigation::Navigation(int argc, char* argv[]) :
      Application(&argc, &argv)
  {
    setTitle("Navigation");

    setLayout(new HBoxLayout());

    VBoxLayout* vb1 = new VBoxLayout();
    addWidget(vb1);

    PushButton* pb1 = new PushButton("PushButton 1");
    vb1->addWidget(pb1);

    PushButton* pb2 = new PushButton("PushButton 2");
    vb1->addWidget(pb2);

    PushButton* pb3 = new PushButton("PushButton 3");
    vb1->addWidget(pb3);

    PushButton* pb4 = new PushButton("PushButton 4");
    vb1->addWidget(pb4);

    PushButton* pb5 = new PushButton("PushButton 5");
    vb1->addWidget(pb5);

    VBoxLayout* vb2 = new VBoxLayout();
    addWidget(vb2);

    PushButton* pb6 = new PushButton("PushButton 6");
    vb2->addWidget(pb6);

    PushButton* pb7 = new PushButton("PushButton 7");
    vb2->addWidget(pb7);

    PushButton* pb8 = new PushButton("PushButton 8");
    vb2->addWidget(pb8);

    PushButton* pb9 = new PushButton("PushButton 9");
    vb2->addWidget(pb9);

    PushButton* pb10 = new PushButton("PushButton 10");
    vb2->addWidget(pb10);

    VBoxLayout* vb3 = new VBoxLayout();
    addWidget(vb3);

    PushButton* pb11 = new PushButton("PushButton 11");
    vb3->addWidget(pb11);

    PushButton* pb12 = new PushButton("PushButton 12");
    vb3->addWidget(pb12);

    PushButton* pb13 = new PushButton("PushButton 13");
    vb3->addWidget(pb13);

    PushButton* pb14 = new PushButton("PushButton 14");
    vb3->addWidget(pb14);

  }

  Navigation::~Navigation()
  {
  }

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
  ilixi::Navigation app(argc, argv);
  app.exec();
  return 0;
}
