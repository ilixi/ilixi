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

#include "GLTri.h"
#include "es2gears.h"

namespace ilixi
{

GLTri::GLTri(Widget* parent)
        : EGLWidget(parent),
          _idleTime(direct_clock_get_millis())
{
    setInputMethod(KeyInput);
    setFocus();
    _timer.sigExec.connect(sigc::mem_fun(this, &GLTri::timerSlot));
    _timer.start(10);
}

GLTri::~GLTri()
{
}

void
GLTri::rotateX(int amount)
{
    view_rot[1] += amount;
    update();
}

void
GLTri::rotateY(int amount)
{
    view_rot[0] += amount;
    update();
}

void
GLTri::setX(int value)
{
    view_rot[1] = value;
    update();
}

void
GLTri::setY(int value)
{
    view_rot[0] = value;
    update();
}

void
GLTri::keyDownEvent(const KeyEvent& event)
{
    switch (event.keySymbol)
    {
    case DIKS_SMALL_A:
    case DIKS_CAPITAL_A:
        rotateX(5);
        break;
    case DIKS_SMALL_D:
    case DIKS_CAPITAL_D:
        rotateX(-5);
        break;
    case DIKS_SMALL_W:
    case DIKS_CAPITAL_W:
        rotateY(5);
        break;
    case DIKS_SMALL_S:
    case DIKS_CAPITAL_S:
        rotateY(-5);
        break;
    default:
        break;
    }
}

void
GLTri::timerSlot()
{
    long long diff;
    long long now = direct_clock_get_millis();

    diff = now - _idleTime;
    if (diff >= 1000)
        _idleTime = now;

    gears_idle(diff);
    update();
}

void
GLTri::renderGL(const PaintEvent& event)
{
    gears_draw();
}

void
GLTri::initialiseGL()
{
    gears_init();
    gears_reshape(width(), height());
    view_rot[1] = 0;
    view_rot[0] = 0;
}

} /* namespace ilixi */
