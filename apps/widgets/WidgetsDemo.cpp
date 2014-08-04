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

#include <stdio.h>
#include <stdlib.h>

using namespace ilixi;

WidgetsDemo::WidgetsDemo(int argc, char* argv[])
        : Application(&argc, &argv, (AppOptions) (OptDaleAuto | OptSound))
{
    setMargins(5, 5, 5, 5);
    setLayout(new VBoxLayout());

    // ToolBar
    ToolBar* bar = new ToolBar();
    bar->addWidget(new Label("ilixi_widgets"));
    bar->addWidget(new Spacer(Horizontal));
    ToolBarButton* barButton = new ToolBarButton("Quit");
    barButton->setIcon(StyleHint::Cross);
    barButton->sigClicked.connect(sigc::ptr_fun(WidgetsDemo::quit));
    bar->addWidget(barButton);
    setToolbar(bar);

    // TabPanel
    TabPanel* tab = new TabPanel();
//    tab->surface()->setSurfaceFlag(Surface::SubSurface);
//    tab->surface()->unsetSurfaceFlag(Surface::SharedSurface);
    addWidget(tab);

    // Buttons Tab
    VBoxLayout* vBox = new VBoxLayout();
    tab->addTab(vBox, "Tab 1");

    // Label
    vBox->addWidget(createLabelGroup());
    vBox->addWidget(new LineSeperator());

    // CheckBox and RadioButton
    HBoxLayout* box1 = new HBoxLayout();
    box1->addWidget(createCheckGroup());
    box1->addWidget(createRadioGroup());
    vBox->addWidget(box1);
    vBox->addWidget(new LineSeperator());

    // PushButton
    vBox->addWidget(createPBGroup());
    vBox->addWidget(new LineSeperator());

    // ToolButton
    vBox->addWidget(createTBGroup());
    vBox->addWidget(new LineSeperator());

    // ButtonGroup
    DirectionalButton* db1 = new DirectionalButton("Left");
    DirectionalButton* db2 = new DirectionalButton("1");
    DirectionalButton* db3 = new DirectionalButton("2");
    DirectionalButton* db4 = new DirectionalButton("3");
    DirectionalButton* db5 = new DirectionalButton("Right");

    ButtonGroup* bg = new ButtonGroup(Horizontal);
    bg->addButton(db1);
    bg->addButton(db2);
    bg->addButton(db3);
    bg->addButton(db4);
    bg->addButton(db5);
    vBox->addWidget(bg);

    vBox->addWidget(new Spacer(Vertical));

    // Disabled
    VBoxLayout* vBox4 = new VBoxLayout();
    tab->addTab(vBox4, "Tab 2");
    tab->setTabEnabled(1, false);

    // LineInput Tab
    VBoxLayout* vBox2 = new VBoxLayout();
    tab->addTab(vBox2, "Tab 3");

    LineInput *li1 = new LineInput("123...");
    li1->sigTextEntered.connect(sigc::mem_fun(this, &WidgetsDemo::print));
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
    tab->addTab(vBox3, "Tab 4");

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

LayoutBase*
WidgetsDemo::createLabelGroup()
{
    HBoxLayout* group = new HBoxLayout();

    Label* label1 = new Label("This is a Label");
    group->addWidget(label1);

    Label* label2 = new Label("This is a disabled Label");
    label2->setDisabled();
    group->addWidget(label2);

    return group;
}

LayoutBase*
WidgetsDemo::createCheckGroup()
{
    VBoxLayout* group = new VBoxLayout();

    CheckBox* cb1 = new CheckBox("CheckBox");
    group->addWidget(cb1);

    CheckBox* cb2 = new CheckBox("CheckBox Tri-state");
    cb2->setTriState(true);
    group->addWidget(cb2);

    CheckBox* disabled = new CheckBox("CheckBox Disabled");
    disabled->setDisabled();
    group->addWidget(disabled);

    return group;
}

LayoutBase*
WidgetsDemo::createRadioGroup()
{
    VBoxLayout* group = new VBoxLayout();

    RadioButton* rb1 = new RadioButton("RadioButton 1");
    group->addWidget(rb1);

    RadioButton* rb2 = new RadioButton("RadioButton 2");
    group->addWidget(rb2);

    RadioButton* disabled = new RadioButton("RadioButton Disabled");
    disabled->setDisabled();
    group->addWidget(disabled);

    return group;
}

LayoutBase*
WidgetsDemo::createPBGroup()
{
    HBoxLayout* group = new HBoxLayout();

    PushButton* pb1 = new PushButton("PushButton");
    group->addWidget(pb1);

    PushButton* pb2 = new PushButton("PushButton Disabled");
    pb2->setDisabled();
    group->addWidget(pb2);

    PushButton* pb3 = new PushButton("OK");
    pb3->setPushButtonStyle(PushButton::OK);
    group->addWidget(pb3);

    PushButton* pb4 = new PushButton("CANCEL");
    pb4->setPushButtonStyle(PushButton::CANCEL);
    group->addWidget(pb4);

    return group;
}

ilixi::LayoutBase*
WidgetsDemo::createTBGroup()
{
    HBoxLayout* group = new HBoxLayout();

    ToolButton* tb1 = new ToolButton("ToolButton");
    tb1->setToolButtonStyle(ToolButton::IconBelowText);
    tb1->setIcon(ILIXI_DATADIR"images/default.png");
    tb1->setCheckable(true);
    group->addWidget(tb1);

    ToolButton* tb2 = new ToolButton("IconAboveText");
    tb2->setToolButtonStyle(ToolButton::IconAboveText);
    tb2->setIcon(ILIXI_DATADIR"images/default.png");
    tb2->setCheckable(true);
    group->addWidget(tb2);

    ToolButton* tb3 = new ToolButton("Text");
    tb3->setToolButtonStyle(ToolButton::IconBeforeText);
    tb3->setIcon(ILIXI_DATADIR"images/default.png");
    tb3->setCheckable(true);
    group->addWidget(tb3);

    ToolButton* tb4 = new ToolButton("IconOnly");
    tb4->setToolButtonStyle(ToolButton::IconOnly);
    tb4->setIcon(ILIXI_DATADIR"images/default.png");
    tb4->setCheckable(true);
    group->addWidget(tb4);

    ToolButton* tb5 = new ToolButton("TextOnly");
    tb5->setToolButtonStyle(ToolButton::TextOnly);
    tb5->setIcon(ILIXI_DATADIR"images/default.png");
    tb5->setCheckable(true);
    group->addWidget(tb5);

    ToolButton* tb6 = new ToolButton("Disabled");
    tb6->setChecked(true);
    tb6->setCheckable(true);
    tb6->setDisabled();
    group->addWidget(tb6);

    return group;
}

void
WidgetsDemo::print(const std::string& text)
{
    printf("%s\n", text.c_str());
}

int
main(int argc, char* argv[])
{
    WidgetsDemo app(argc, argv);
    app.exec();
    return 0;
}

