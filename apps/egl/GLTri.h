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

#ifndef GLTRI_H_
#define GLTRI_H_

#include <ui/EGLWidget.h>

namespace ilixi
{

class GLTri : public EGLWidget
{
public:
    GLTri(Widget* parent = 0);

    virtual
    ~GLTri();

protected:
    virtual void
    keyDownEvent(const KeyEvent& event);

    virtual void
    renderGL();

private:
    virtual void
    initialiseGL();
};

} /* namespace ilixi */
#endif /* GLTRI_H_ */
