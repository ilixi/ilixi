/*
 * Spiral.cpp
 *
 *  Created on: Nov 8, 2012
 *      Author: tarik
 */

#include "Spiral.h"
#include <graphics/Painter.h>
#include <sstream>
#include <cmath>

using namespace ilixi;

Spiral::Spiral(Widget* parent)
        : Widget(parent),
          _temp(0),
          _image(NULL)
{
    _image = new Image(ILIXI_DATADIR"images/circle.png");
}

Spiral::~Spiral()
{
    delete _image;
}

void
Spiral::setTemp(int temp)
{
    _temp = temp;
    if (_temp < 0)
        _temp = 0;
}

void
Spiral::compose(const ilixi::PaintEvent& event)
{
    float off = direct_clock_get_millis() * 0.5;

    Painter p(this);
    p.begin(event);

    int centerX = width() / 2;
    int centerY = height() / 2;

    for (int i = 12; i >= 0; --i)
    {
        int angle = i * 30 + off;
        int x = centerX + i * 5 * cos(angle * M_PI / 180);
        int y = centerY + i * 5 * sin(angle * M_PI / 180);
        p.setBrush(Color(0, 0, 0, _temp - i * 2));
        p.stretchImage(_image, x - i * 2, y - i * 2, i * 4, i * 4, (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA));
    }

    p.end();
}
