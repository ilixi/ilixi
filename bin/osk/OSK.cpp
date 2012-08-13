/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#include "OSK.h"
#include <graphics/Painter.h>
#include <sigc++/bind.h>
#include <core/Logger.h>
#include <ui/VBoxLayout.h>
#include <ilixiConfig.h>

OSK::OSK(int argc, char* argv[])
        : Application(&argc, &argv, OptDale),
          _buttonFont(NULL)
{
    setTitle("OSK");
    setBackgroundFilled(true);
    setMargin(5);
    setLayout(new VBoxLayout());

    sigGeometryUpdated.connect(sigc::mem_fun(this, &OSK::setOptimalFontSize));

    _keyboard = new Keyboard(this);
    _keyboard->parseLayoutFile(ILIXI_DATADIR"osk/keyboard.xml");
    addWidget(_keyboard);
}

OSK::~OSK()
{
    delete _buttonFont;
}

void
OSK::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
    painter.setBrush(Color(0, 0, 0, 128));
    painter.fillRectangle(0, 0, width(), height());
    painter.end();
}

void
OSK::setOptimalFontSize()
{
    _keyboard->setGeometry(0, 0, width(), height());

//    _buttonFont = new Font("decker", (width() - 250) / 10);
//    _buttonFont->setStyle(Font::Bold);
//    _buttonFont->dfbFont();
//    for (int i = 0; i < _keys.size(); i++)
//        _keys[i]->setFont(_buttonFont);
//    update();
}

int
main(int argc, char* argv[])
{
    OSK app(argc, argv);
    app.exec();
    return 0;
}
