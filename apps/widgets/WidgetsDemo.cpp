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

#include <ilixiGUI.h>
#include <graphics/Painter.h>

#include <stdio.h>
#include <stdlib.h>

using namespace ilixi;

WidgetsDemo::WidgetsDemo(int argc, char* argv[])
        : Application(&argc, &argv, (AppOptions) (OptDaleAuto | OptSound))
{
    setMargins(5, 5, 5, 5);
    setLayout(new VBoxLayout());

    ToolBar* bar = new ToolBar();

    ToolBarButton* barB1 = new ToolBarButton();
    barB1->setIcon(StyleHint::Tick);
    bar->addWidget(barB1);

    ToolBarButton* barB2 = new ToolBarButton();
    barB2->setIcon(StyleHint::Movie);
    bar->addWidget(barB2);

    ToolBarButton* barB3 = new ToolBarButton("test3");
    barB3->setIcon(StyleHint::Plus);
    bar->addWidget(barB3);
    setToolbar(bar);

    TabPanel* tab = new TabPanel();

//    GroupBox* frame = new GroupBox("Test");
    addWidget(tab);

    VBoxLayout* vBox = new VBoxLayout();
    tab->addTab(vBox, "Buttons");

    CheckBox* cb1 = new CheckBox("CheckBox");
    vBox->addWidget(cb1);

    CheckBox* cb2 = new CheckBox("Tri-state checkBox");
    cb2->setTriState(true);
    vBox->addWidget(cb2);

    CheckBox* cb3 = new CheckBox("CheckBox Disabled");
    cb3->setDisabled();
    vBox->addWidget(cb3);

    RadioButton* rb1 = new RadioButton("RadioButton 1");
    vBox->addWidget(rb1);

    RadioButton* rb2 = new RadioButton("RadioButton 2");
    vBox->addWidget(rb2);

    RadioButton* rb3 = new RadioButton("RadioButton 3");
    rb3->setDisabled();
    vBox->addWidget(rb3);

    vBox->addWidget(new LineSeperator());

    PushButton* pb1 = new PushButton("PushButton 1");
    vBox->addWidget(pb1);

    PushButton* pb2 = new PushButton("PushButton Disabled");
    pb2->setDisabled();
    vBox->addWidget(pb2);

    ToolButton* tb1 = new ToolButton("ToolButton");
//    tb1->setToolButtonStyle(ToolButton::IconBelowText);
    tb1->setCheckable(true);
    vBox->addWidget(tb1);

    ToolButton* tb2 = new ToolButton("ToolButton Disabled");
    tb2->setDisabled();
    vBox->addWidget(tb2);

    DirectionalButton* db1 = new DirectionalButton("Left");
    DirectionalButton* db2 = new DirectionalButton("1");
    DirectionalButton* db3 = new DirectionalButton("2");
    DirectionalButton* db4 = new DirectionalButton("Right");

    ButtonGroup* bg = new ButtonGroup(Vertical);
//    bg->setOrientation(Horizontal);
    bg->addButton(db1);
    bg->addButton(db2);
    bg->addButton(db3);
    bg->addButton(db4);
    vBox->addWidget(bg);

    VBoxLayout* vBox2 = new VBoxLayout();
    tab->addTab(vBox2, "LineInputs");

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
    tab->addTab(vBox3, "Bars and Sliders");

    ProgressBar* bar1 = new ProgressBar();
    bar1->setValue(5);
    vBox3->addWidget(bar1);

    ProgressBar* bar1d = new ProgressBar();
    bar1d->setValue(5);
    bar1d->setDisabled();
    vBox3->addWidget(bar1d);

    Slider* slider1 = new Slider();
    slider1->sigValueChanged.connect(sigc::mem_fun(bar1, &ProgressBar::setValue));
    vBox3->addWidget(slider1);

    Slider* slider11 = new Slider();
    slider11->setValue(48);
    slider11->setDisabled();
    vBox3->addWidget(slider11);

    Slider* slider2 = new Slider();
    slider2->setInverted(true);
    slider2->sigValueChanged.connect(sigc::mem_fun(bar1, &ProgressBar::setValue));
    vBox3->addWidget(slider2);

    ScrollBar* sb1 = new ScrollBar(Horizontal);
    sb1->setRange(0, 10);
    vBox3->addWidget(sb1);

    ScrollBar* sb12 = new ScrollBar(Horizontal);
    sb12->setDisabled();
    sb12->setRange(0, 10);
    sb12->setValue(5);
    vBox3->addWidget(sb12);

    // Verticals
    HBoxLayout* vSliderLayout = new HBoxLayout();
    vBox3->addWidget(vSliderLayout);

    ProgressBar* bar2 = new ProgressBar();
    bar2->setValue(5);
    bar2->setOrientation(Vertical);
    vSliderLayout->addWidget(bar2);

    Slider* slider3 = new Slider();
    slider3->setOrientation(Vertical);
    slider3->sigValueChanged.connect(sigc::mem_fun(bar2, &ProgressBar::setValue));
    vSliderLayout->addWidget(slider3);

    Slider* slider4 = new Slider();
    slider4->setOrientation(Vertical);
    slider4->setInverted(true);
    slider4->sigValueChanged.connect(sigc::mem_fun(bar2, &ProgressBar::setValue));
    vSliderLayout->addWidget(slider4);

    ScrollBar* sb2 = new ScrollBar(Vertical);
    sb2->setRange(0, 10);
    vSliderLayout->addWidget(sb2);

    ComboBox::StringList list;

    char str[5];
    for (int i = 2005; i > 1900; --i)
    {
        sprintf(str, "%d", i);
        list.push_back(str);
    }

    ComboBox* co1 = new ComboBox("Select your date of birth:");
    co1->addItems(list);
    vBox3->addWidget(co1);

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

