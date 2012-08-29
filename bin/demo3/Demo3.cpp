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

#include "Demo3.h"

#include <ui/GridLayout.h>
#include <ui/HBoxLayout.h>
#include <ui/VBoxLayout.h>
#include <ui/Spacer.h>
#include <ilixiGUI.h>
#include <graphics/Painter.h>

#include <stdio.h>
#include <stdlib.h>

using namespace ilixi;

Demo3::Demo3(int argc, char* argv[])
        : Application(&argc, &argv, OptDale)
{
    setTitle("Forms Demo");
    setMargins(5, 5, 5, 5);
//    setBackgroundFilled(true);
    setLayout(new VBoxLayout());

//    addWidget(new Spacer(Vertical));

    GroupBox* frame = new GroupBox("Test");
    addWidget(frame);

    //    GridLayout* grid = new GridLayout(4, 4);
    VBoxLayout* grid = new VBoxLayout();
    frame->setLayout(grid);

    CheckBox* cb1 = new CheckBox("Check 1");
    frame->addWidget(cb1);

    CheckBox* cb2 = new CheckBox("Check 2");
    cb2->setTriState(true);
    frame->addWidget(cb2);

    LineInput *li1 = new LineInput("Line input has some text...");
    frame->addWidget(li1);

    RadioButton* rb1 = new RadioButton("Radio 1");
    frame->addWidget(rb1);

    RadioButton* rb2 = new RadioButton("Radio 1");
    frame->addWidget(rb2);

    ProgressBar* bar1 = new ProgressBar();
    bar1->setValue(5);
    frame->addWidget(bar1);

    Slider* slider1 = new Slider();
//    slider1->setInverted(true);
    slider1->setRange(-30 , 30);
    slider1->sigValueChanged.connect(
            sigc::mem_fun(bar1, &ProgressBar::setValue));
    frame->addWidget(slider1);

    Slider* slider2 = new Slider();
    slider2->setRange(-30 , 30);
    slider2->setOrientation(Vertical);
    slider2->sigValueChanged.connect(
            sigc::mem_fun(bar1, &ProgressBar::setValue));
    frame->addWidget(slider2);

    LineInput *li2 = new LineInput("Line input has some text...");
    frame->addWidget(li2);

    PushButton* pb1 = new PushButton("PushButton 1");
    frame->addWidget(pb1);

    PushButton* pb2 = new PushButton("PushButton 1");
    frame->addWidget(pb2);

    Label *lipLabel = new Label("LineInput:");
    frame->addWidget(lipLabel);

    LineInput *li3 = new LineInput("Line input has some text...");
    frame->addWidget(li3);

    ToolButton* tb = new ToolButton("ToolButton");
    tb->setToolButtonStyle(ToolButton::TextOnly);
    addWidget(tb);

    LineInput *li4 = new LineInput("Line input has some text...");
    addWidget(li4);

//    ComboBox::StringList list;
//
//    char str[5];
//    for (int i = 2005; i > 1900; --i)
//    {
//        sprintf(str, "%d", i);
//        list.push_back(str);
//    }
//
//    ComboBox* co1 = new ComboBox("Select your date of birth:");
//    co1->addItems(list);
//    frame->addWidget(co1);

//    addWidget(new Spacer(Vertical));
}

Demo3::~Demo3()
{
}

int
main(int argc, char* argv[])
{
    Demo3 app(argc, argv);
    app.exec();
    return 0;
}
