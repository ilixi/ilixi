/*
 Copyright 2011 Tarik Sekmen.

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

#include "Demo3.h"
#include "LineInput.h"
#include "ui/VBoxLayout.h"
#include "ui/Label.h"
#include "ui/Spacer.h"
#include "ui/CheckBox.h"
#include "ui/RadioButton.h"
#include "ui/ComboBox.h"
#include "graphics/Painter.h"

#include <stdio.h>
#include <stdlib.h>

using namespace ilixi;

Demo3::Demo3(int argc, char* argv[]) :
    Application(argc, argv)
{
  setTitle("Forms Demo");
  setBackgroundFilled(true);
  setLayout(new VBoxLayout());

  Label *lipLabel = new Label("LineInput:");
  addWidget(lipLabel);

  LineInput *lip = new LineInput("Line input has some text...");
  lip->setMaximumSize(120, 100);
//  lip->setMaxLength(15);
  addWidget(lip);

  CheckBox* cb1 = new CheckBox("Check 1");
  addWidget(cb1);

  CheckBox* cb2 = new CheckBox("Check 2");
  cb2->setTriState(true);
  addWidget(cb2);

  RadioButton* rb1 = new RadioButton("Radio 1");
  addWidget(rb1);

  RadioButton* rb2 = new RadioButton("Radio 1");
  addWidget(rb2);

  ComboBox::StringList list;

  char str[5];
  for (int i = 2005; i > 1900; --i)
    {
      sprintf(str, "%d", i);
      list.push_back(str);
    }

  ComboBox* co1 = new ComboBox("Select your date of birth:");
  co1->addItems(list);
  addWidget(co1);

  addWidget(new Spacer(Vertical));
}

Demo3::~Demo3()
{
}

void
Demo3::compose(const Rectangle& rect)
{
  Painter p(this);
  p.begin(rect);

  p.setBrush(Color(128, 128, 128));

  p.fillRectangle(0, 0, width(), height());

  p.end();
}

int
main(int argc, char* argv[])
{
  Demo3 app(argc, argv);
  app.exec();
  return 0;
}
