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

#include "Demo4.h"
#include "VKMenu.h"
#include "ui/PushButton.h"
#include "ui/HBoxLayout.h"
#include "ui/VBoxLayout.h"
#include "ui/Spacer.h"
#include <sigc++/bind.h>

using namespace ilixi;

Demo4::Demo4(int argc, char* argv[]) :
    Application(&argc, &argv)
{
  setMargin(0);
//  setBackgroundFilled(false);
//  setBackgroundImage(ILIXI_DATADIR"images/ilixi_bg.jpg");
  setLayout(new VBoxLayout());

  addWidget(new Spacer(Vertical));

  VKMenu* menu = new VKMenu();
  addWidget(menu);
}

Demo4::~Demo4()
{
}

int
main(int argc, char* argv[])
{
  Demo4 app(argc, argv);
  app.exec();
  return 0;
}
