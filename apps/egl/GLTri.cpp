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
#include "es2tri.h"

namespace ilixi
{

GLTri::GLTri(Widget* parent)
        : EGLWidget(parent)
{
    setInputMethod(KeyInput);
    setFocus();
}

GLTri::~GLTri()
{
}

void
GLTri::keyDownEvent(const KeyEvent& event)
{
    switch (event.keySymbol)
    {
    case DIKS_SMALL_A:
    case DIKS_CAPITAL_A:
        view_roty += 5.0;
        update();
        break;
    case DIKS_SMALL_D:
    case DIKS_CAPITAL_D:
        view_roty -= 5.0;
        update();
        break;
    case DIKS_SMALL_W:
    case DIKS_CAPITAL_W:
        view_rotx += 5.0;
        update();
        break;
    case DIKS_SMALL_S:
    case DIKS_CAPITAL_S:
        view_rotx -= 5.0;
        update();
        break;
    default:
        break;
    }
}

void
GLTri::renderGL()
{
    draw();
}

void
GLTri::initialiseGL()
{
    create_shaders();
}

} /* namespace ilixi */
