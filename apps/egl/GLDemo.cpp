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

#include "GLDemo.h"

#include <ui/Frame.h>
#include <ui/HBoxLayout.h>
#include <ui/VBoxLayout.h>
#include <graphics/Painter.h>

namespace ilixi
{

GLDemo::GLDemo(int argc, char* argv[])
        : Application(&argc, &argv)
{
    appWindow()->setCustomCompose(true);
    setLayout(new VBoxLayout());

    _glesExample = new GLTri();
    _glesExample->setConstraints(ExpandingConstraint, ExpandingConstraint);
    addWidget(_glesExample);

    Frame* controls = new Frame();
    controls->setConstraints(ExpandingConstraint, MinimumConstraint);
    addWidget(controls);
    controls->setLayout(new HBoxLayout());

    _xSlider = new Slider();
    controls->addWidget(_xSlider);
    _xSlider->setRange(0, 360);
    _xSlider->sigValueChanged.connect(sigc::mem_fun(_glesExample, &GLTri::setX));

    _ySlider = new Slider();
    controls->addWidget(_ySlider);
    _ySlider->setRange(0, 360);
    _ySlider->sigValueChanged.connect(sigc::mem_fun(_glesExample, &GLTri::setY));

    _fps = new FPSCalculator();
}

GLDemo::~GLDemo()
{
}

void
GLDemo::compose(const ilixi::PaintEvent& event)
{
    Painter p(appWindow());
    p.begin(event);
    p.fillRectangle(0, 0, width(), height());
    p.setBrush(Color("6FAF53"));

    _fps->funck();
    std::string text = PrintF("FPS: %.1f", _fps->fps());

    Size s = appWindow()->stylist()->defaultFont()->extents(text);
    p.drawText(text, width() - s.width() - 10, 10);
    p.end();
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::GLDemo app(argc, argv);
    app.exec();
    return 0;
}
