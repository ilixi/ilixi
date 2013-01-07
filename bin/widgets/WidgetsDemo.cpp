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

#include "WidgetsDemo.h"

#include <ui/GridLayout.h>
#include <ui/HBoxLayout.h>
#include <ui/VBoxLayout.h>
#include <ui/Spacer.h>
#include <ilixiGUI.h>
#include <graphics/Painter.h>

#include <stdio.h>
#include <stdlib.h>

using namespace ilixi;

WidgetsDemo::WidgetsDemo(int argc, char* argv[])
        : Application(&argc, &argv, OptDale)
{
    setMargins(5, 5, 5, 5);
    setLayout(new VBoxLayout());

    TabPanel* tab = new TabPanel();

//    GroupBox* frame = new GroupBox("Test");
    addWidget(tab);

    VBoxLayout* vBox = new VBoxLayout();
    tab->addPage(vBox, "Buttons");

    CheckBox* cb1 = new CheckBox("CheckBox");
    vBox->addWidget(cb1);

    CheckBox* cb2 = new CheckBox("Tri-state checkBox");
    cb2->setTriState(true);
    vBox->addWidget(cb2);

    RadioButton* rb1 = new RadioButton("RadioButton 1");
    vBox->addWidget(rb1);

    RadioButton* rb2 = new RadioButton("RadioButton 2");
    vBox->addWidget(rb2);

    PushButton* pb1 = new PushButton("PushButton 1");
    vBox->addWidget(pb1);

    PushButton* pb2 = new PushButton("PushButton Disabled");
    pb2->setDisabled();
    vBox->addWidget(pb2);

    ToolButton* tb1 = new ToolButton("ToolButton");
    vBox->addWidget(tb1);

    ToolButton* tb2 = new ToolButton("ToolButton Disabled");
    tb2->setDisabled();
    vBox->addWidget(tb2);

    VBoxLayout* vBox2 = new VBoxLayout();
    tab->addPage(vBox2, "LineInputs");

    LineInput *li1 = new LineInput("Line input has some text...");
    vBox2->addWidget(li1);
    vBox2->addWidget(new Spacer(Vertical));

    LineInput *li2 = new LineInput("Line input has some text...");
    vBox2->addWidget(li2);
    vBox2->addWidget(new Spacer(Vertical));

    LineInput *li3 = new LineInput("Line input has some text...");
    vBox2->addWidget(li3);
    vBox2->addWidget(new Spacer(Vertical));

    LineInput *li4 = new LineInput("Line input has some text...");
    vBox2->addWidget(li4);
    li4->setDisabled();
    vBox2->addWidget(new Spacer(Vertical));

    LineInput *li5 = new LineInput("Line input has some text...");
    vBox2->addWidget(li5);

    VBoxLayout* vBox3 = new VBoxLayout();
    tab->addPage(vBox3, "Bars and Sliders");

    ProgressBar* bar1 = new ProgressBar();
    bar1->setValue(5);
    vBox3->addWidget(bar1);

    Slider* slider1 = new Slider();
    slider1->sigValueChanged.connect(sigc::mem_fun(bar1, &ProgressBar::setValue));
    vBox3->addWidget(slider1);

    Slider* slider2 = new Slider();
    slider2->setInverted(true);
    slider2->sigValueChanged.connect(sigc::mem_fun(bar1, &ProgressBar::setValue));
    vBox3->addWidget(slider2);


    HBoxLayout* vSliderLayout = new HBoxLayout();
    vBox3->addWidget(vSliderLayout);

    Slider* slider3 = new Slider();
    slider3->setOrientation(Vertical);
    slider3->sigValueChanged.connect(sigc::mem_fun(bar1, &ProgressBar::setValue));
    vSliderLayout->addWidget(slider3);

    Slider* slider4 = new Slider();
    slider4->setOrientation(Vertical);
    slider4->setInverted(true);
    slider4->sigValueChanged.connect(sigc::mem_fun(bar1, &ProgressBar::setValue));
    vSliderLayout->addWidget(slider4);

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
//    vBox3->addWidget(co1);

    SpinBox* spin1 = new SpinBox(5);
    vBox3->addWidget(spin1);

}

WidgetsDemo::~WidgetsDemo()
{
}

int
main(int argc, char* argv[])
{
    WidgetsDemo app(argc, argv);
    app.exec();
    return 0;
}
