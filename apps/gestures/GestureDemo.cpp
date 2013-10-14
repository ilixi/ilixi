/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include "GestureDemo.h"
#include <graphics/Painter.h>

namespace ilixi
{

GestureDemo::GestureDemo(int argc, char* argv[])
        : Application(&argc, &argv)
{
    appWindow()->setCustomCompose(true);

    _gestures.addGesture("left", "444444");
    _gestures.addGesture("right", "000000");
    _gestures.addGesture("up", "666666");
    _gestures.addGesture("down", "222222");

    _gestures.addGesture("check", "117777");
    _gestures.addGesture("cancel", "441144");

    _gestures.addGesture("home", "777111");
    _gestures.addGesture("open", "2210766");
    _gestures.addGesture("close", "6670122");

    _gestures.addGesture("reload", "432107654");
    _gestures.addGesture("enter", "222444");

    _gestures.sigGestureMatched.connect(sigc::mem_fun(this, &GestureDemo::matched));
    _gestures.sigMovement.connect(sigc::mem_fun(this, &GestureDemo::moving));

    appWindow()->setEventFilter(&_gestures);

    _legend = new Image(ILIXI_DATADIR"gestures/gestures.png");
}

GestureDemo::~GestureDemo()
{
    delete _legend;
}

void
GestureDemo::compose(const PaintEvent& event)
{
    Painter p(appWindow());
    p.begin(event);
    p.fillRectangle(0, 0, width(), height());

    p.drawImage(_legend, 10, 10);

    if (_gestures.path().size() > 1)
    {
        std::string moves = PrintF("moves: %d", _gestures.moves()[0]);

        p.setPen(Color("FF4040"));
        for (int i = 1; i < _gestures.path().size(); ++i)
        {
            p.drawLine(_gestures.path()[i - 1], _gestures.path()[i]);
            moves.append(PrintF("%d", _gestures.moves()[i]));
        }

        p.setBrush(Color("FF4040"));
        Size s = appWindow()->stylist()->defaultFont(StyleHint::DefaultFont)->extents(moves);
        p.drawText(moves, (width() - s.width()) / 2, height() - s.height());
    }

    if (!_match.empty())
    {
        p.setBrush(Color("6FAF53"));
        Size s = appWindow()->stylist()->defaultFont(StyleHint::TitleFont)->extents(_match);
        p.setFont(*appWindow()->stylist()->defaultFont(StyleHint::TitleFont));
        p.drawText(_match, (width() - s.width()) / 2, (height() - s.height()) / 2);
    }
}

void
GestureDemo::matched(const std::string& gesture)
{
    _match = gesture;
    update();
}

void
GestureDemo::moving()
{
    _match = "";
    update();
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::GestureDemo app(argc, argv);
    app.exec();
    return 0;
}
