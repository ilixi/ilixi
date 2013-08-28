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

#include <types/Color.h>
#include <stdlib.h>
#include <cmath>

namespace ilixi
{

Color::Color()
        : _red(0),
          _green(0),
          _blue(0),
          _alpha(255)
{
}

Color::Color(const Color& color)
        : _red(color.red()),
          _green(color.green()),
          _blue(color.blue()),
          _alpha(color.alpha())
{
}

Color::Color(u8 r, u8 g, u8 b, u8 a)
        : _red(r),
          _green(g),
          _blue(b),
          _alpha(a)
{
}

Color::Color(const char* hex, u8 alpha)
{
    setFromHex(hex, alpha);
}

Color::~Color()
{
}

void
Color::setAlpha(u8 a)
{
    _alpha = a;
}

void
Color::setBlue(u8 b)
{
    _blue = b;
}

void
Color::setGreen(u8 g)
{
    _green = g;
}

void
Color::setRed(u8 r)
{
    _red = r;
}

void
Color::setBrightness(float brightness)
{
    if (brightness > 1)
        brightness = 1;
    else if (brightness < 0)
        brightness = 0;

    HSL hsl(*this);
    hsl.lightness = brightness;
    setFromHSL(hsl);
}

void
Color::adjustBrightness(float factor)
{
    if (factor > 1)
        factor = 1;
    else if (factor < 0)
        factor = 0;

    HSL hsl(*this);
    hsl.lightness *= factor;
    setFromHSL(hsl);
}

void
Color::setSaturation(float saturation)
{
    if (saturation > 100)
        saturation = 100;
    else if (saturation < 0)
        saturation = 0;

    HSL hsl(*this);
    hsl.saturation = saturation;
    setFromHSL(hsl);
}

void
Color::adjustSaturation(float factor)
{
    if (factor > 1)
        factor = 1;
    else if (factor < 0)
        factor = 0;

    HSL hsl(*this);
    hsl.saturation *= factor;
    setFromHSL(hsl);
}

void
Color::setHue(u16 hue)
{
    if (hue > 100)
        hue = 100;

    HSL hsl(*this);
    hsl.hue = hue;
    setFromHSL(hsl);
}

void
Color::adjustHue(float factor)
{
    if (factor > 1)
        factor = 1;
    else if (factor < 0)
        factor = 0;

    HSL hsl(*this);
    hsl.hue *= factor;
    setFromHSL(hsl);
}

void
Color::setRGBA(u8 red, u8 green, u8 blue, u8 alpha)
{
    setRed(red);
    setGreen(green);
    setBlue(blue);
    setAlpha(alpha);
}

void
Color::setFromHex(const char* hex, u8 a)
{
    char *pEnd;
    unsigned long int color = strtol(hex, &pEnd, 16);
    _red = ((color & 0xFF0000) >> 16);
    _green = ((color & 0x00FF00) >> 8);
    _blue = (color & 0x0000FF);
    _alpha = a;
}

void
Color::setFromHSV(const HSV& hsv)
{
    float c = hsv.value * hsv.saturation;
    float h = hsv.hue / 60.0;
    float x = c * (1 - fabs(fmod(h, 2) - 1.0));
    float m = hsv.value - c;

    if (h >= 0 && h < 1)
    {
        _red = 255 * (c + m);
        _green = 255 * (x + m);
        _blue = 255 * m;
    }
    if (h >= 1 && h < 2)
    {
        _red = 255 * (x + m);
        _green = 255 * (c + m);
        _blue = 255 * m;
    }
    if (h >= 2 && h < 3)
    {
        _red = 255 * m;
        _green = 255 * (c + m);
        _blue = 255 * (x + m);
    }
    if (h >= 3 && h < 4)
    {
        _red = 255 * m;
        _green = 255 * (x + m);
        _blue = 255 * (c + m);
    }
    if (h >= 4 && h < 5)
    {
        _red = 255 * (x + m);
        _green = 255 * m;
        _blue = 255 * (c + m);
    }
    if (h >= 5 && h < 6)
    {
        _red = 255 * (c + m);
        _green = 255 * m;
        _blue = 255 * (x + m);
    }
}

void
Color::setFromHSL(const HSL& hsl)
{
    if (hsl.saturation == 0)
    {
        _red = hsl.lightness * _red;
        _green = hsl.lightness * _green;
        _blue = hsl.lightness * _blue;
        return;
    }

    float c = (1 - fabs(2 * hsl.lightness - 1)) * hsl.saturation;
    float h = hsl.hue / 60;
    float x = c * (1 - fabs(fmod(h, 2) - 1.0));
    float m = hsl.lightness - c / 2.0;

    if (h >= 0 && h < 1)
    {
        _red = 255 * (c + m);
        _green = 255 * (x + m);
        _blue = 255 * m;
    }
    if (h >= 1 && h < 2)
    {
        _red = 255 * (x + m);
        _green = 255 * (c + m);
        _blue = 255 * m;
    }
    if (h >= 2 && h < 3)
    {
        _red = 255 * m;
        _green = 255 * (c + m);
        _blue = 255 * (x + m);
    }
    if (h >= 3 && h < 4)
    {
        _red = 255 * m;
        _green = 255 * (x + m);
        _blue = 255 * (c + m);
    }
    if (h >= 4 && h < 5)
    {
        _red = 255 * (x + m);
        _green = 255 * m;
        _blue = 255 * (c + m);
    }
    if (h >= 5 && h < 6)
    {
        _red = 255 * (c + m);
        _green = 255 * m;
        _blue = 255 * (x + m);
    }
}

void
Color::interpolate(const Color& color, float factor)
{
    HSL c1(*this);
    HSL c2(color);
    HSL r;
    float distCW = (c1.hue >= c2.hue) ? c1.hue - c2.hue : 1 + c1.hue - c2.hue;
    float distCCW = (c1.hue >= c2.hue) ? 1 + c2.hue - c1.hue : c2.hue - c1.hue;
    if (distCW <= distCCW)
        r.hue = c1.hue + (distCW * factor);
    else
        r.hue = c1.hue - (distCCW * factor);

    r.saturation = (1 - factor) * c1.saturation + factor * c2.saturation;
    r.lightness = (1 - factor) * c1.lightness + factor * c2.lightness;

    setFromHSL(r);
}

Color&
Color::operator=(const Color & color)
{
    if (this != &color)
    {
        _red = color.red();
        _green = color.green();
        _blue = color.blue();
        _alpha = color.alpha();
    }
    return *this;
}

bool
Color::operator==(const Color& color) const
{
    return ((red() == color.red()) && (green() == color.green())
            && (blue() == color.blue()) && (alpha() == color.alpha()));
}

bool
Color::operator!=(const Color& color) const
{
    return !(*this == color);
}

DFBColor
Color::dfbColor() const
{
    DFBColor c = { _alpha, _red, _green, _blue };
    return c;
}

/////////////////////////////////////////////
// HSV
/////////////////////////////////////////////
Color::HSV::HSV()
        : hue(0),
          saturation(0),
          value(0)
{
}

Color::HSV::HSV(float h, float s, float v)
        : hue(h > 360 ? 0 : h),
          saturation(s > 1 ? 1 : s < 0 ? 0 : s),
          value(v > 1 ? 1 : v < 0 ? 0 : v)
{
}

Color::HSV::HSV(const Color& color)
{
    float r = color._red / 255.0;
    float g = color._green / 255.0;
    float b = color._blue / 255.0;

    float min = b;
    float max = b;

    if (max < g)
        max = g;
    if (max < r)
        max = r;

    if (min > g)
        min = g;
    if (min > r)
        min = r;

    float chroma = max - min;
    value = max;

    if (chroma == 0)
    {
        hue = 0;
        saturation = 0;
    } else
    {
        saturation = chroma / max;

        if (r == max)
            hue = ((g - b) / (float) chroma) * 60.0;
        else if (g == max)
            hue = (2.0 + ((b - r) / (float) chroma)) * 60.0;
        else if (b == max)
            hue = (4.0 + ((r - g) / (float) chroma)) * 60.0;

        if (hue < 0)
            hue += 360;
        else if (hue > 360)
            hue -= 360;
    }
}

/////////////////////////////////////////////
// HSL
/////////////////////////////////////////////
Color::HSL::HSL()
        : hue(0),
          saturation(0),
          lightness(0)
{
}

Color::HSL::HSL(float h, float s, float l)
        : hue(h > 360 ? 0 : h),
          saturation(s > 1 ? 1 : s < 0 ? 0 : s),
          lightness(l > 1 ? 1 : l < 0 ? 0 : l)
{
}

Color::HSL::HSL(const Color& color)
{
    float r = color._red / 255.0;
    float g = color._green / 255.0;
    float b = color._blue / 255.0;

    float min = b;
    float max = b;

    if (max < g)
        max = g;
    if (max < r)
        max = r;

    if (min > g)
        min = g;
    if (min > r)
        min = r;

    float chroma = max - min;
    lightness = (min + max) / 2.0;

    if (chroma == 0)
    {
        hue = 0; // not undefined.
        saturation = 0;
    } else
    {
        if (lightness < .5)
            saturation = chroma / (float) (min + max);
        else
            saturation = chroma / (float) (2.0 - min - max);

        if (r == max)
            hue = ((g - b) / (float) chroma) * 60.0;
        else if (g == max)
            hue = (2.0 + ((b - r) / (float) chroma)) * 60.0;
        else if (b == max)
            hue = (4.0 + ((r - g) / (float) chroma)) * 60.0;

        if (hue < 0)
            hue += 360;
        else if (hue > 360)
            hue -= 360;
    }
}

} /* namespace ilixi */
