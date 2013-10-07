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

#ifndef GLDEMO_H_
#define GLDEMO_H_

#include <core/Application.h>
#include <ui/Slider.h>
#include <lib/FPSCalculator.h>
#include "GLTri.h"

namespace ilixi
{

class GLDemo : public Application
{
public:
    GLDemo(int argc, char* argv[]);

    virtual
    ~GLDemo();

protected:
    void
    compose(const ilixi::PaintEvent& event);

private:
    GLTri* _glesExample;
    Slider* _xSlider;
    Slider* _ySlider;
    FPSCalculator* _fps;
};

} /* namespace ilixi */
#endif /* GLDEMO_H_ */
