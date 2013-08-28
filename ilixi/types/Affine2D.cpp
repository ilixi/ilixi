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

#include <types/Affine2D.h>
#include <cmath>

using namespace std;

namespace ilixi
{

#define DOUBLE_TO_FIXED(x)      ((long)(x * 65536.0 + 0.5))

Affine2D::Affine2D()
{
    clear();
}

Affine2D::Affine2D(double m11, double m12, double m13, double m21, double m22,
                   double m23)
{
    _m[0] = m11;
    _m[1] = m21;
    _m[2] = m12;
    _m[3] = m22;
    _m[4] = m13;
    _m[5] = m23;
}

Affine2D::Affine2D(const Affine2D& other)
{
    _m[0] = other._m[0];
    _m[1] = other._m[1];
    _m[2] = other._m[2];
    _m[3] = other._m[3];
    _m[4] = other._m[4];
    _m[5] = other._m[5];
}

Affine2D::~Affine2D()
{
}

int32_t*
Affine2D::m() const
{
    int32_t* m3 = new int32_t[9];
    m3[0] = DOUBLE_TO_FIXED(_m[0]);
    m3[1] = DOUBLE_TO_FIXED(_m[2]);
    m3[2] = DOUBLE_TO_FIXED(_m[4]);
    m3[3] = DOUBLE_TO_FIXED(_m[1]);
    m3[4] = DOUBLE_TO_FIXED(_m[3]);
    m3[5] = DOUBLE_TO_FIXED(_m[5]);
    m3[6] = DOUBLE_TO_FIXED(0);
    m3[7] = DOUBLE_TO_FIXED(0);
    m3[8] = DOUBLE_TO_FIXED(1);
    return m3;
}

Affine2D&
Affine2D::clear()
{
    _m[0] = _m[3] = 1.0;
    _m[1] = _m[2] = _m[4] = _m[5] = 0.0;
    return *this;
}

double
Affine2D::determinant() const
{
    return 1.0 / (_m[0] * _m[3] - _m[1] * _m[2]);
}

Affine2D&
Affine2D::invert()
{
    double d = determinant();

    double t0 = _m[3] * d;
    _m[3] = _m[0] * d;
    _m[1] = -_m[1] * d;
    _m[2] = -_m[2] * d;

    double t4 = -_m[4] * t0 - _m[5] * _m[2];
    _m[5] = -_m[4] * _m[1] - _m[5] * _m[3];

    _m[0] = t0;
    _m[4] = t4;
    return *this;
}

Affine2D&
Affine2D::rotate(double d)
{
    double r = d * M_PI / 180;
    Affine2D tmp(cos(r), -sin(r), 0, sin(r), cos(r), 0);
    multiply(tmp);
    return *this;
}

Affine2D&
Affine2D::scale(float sx, float sy)
{
    Affine2D tmp(sx, 0, 0, 0, sy, 0);
    multiply(tmp);
    return *this;
}

Affine2D&
Affine2D::shearX(float x)
{
    Affine2D tmp(1, x, 0, 0, 1, 0);
    multiply(tmp);
    return *this;
}

Affine2D&
Affine2D::shearY(float y)
{
    Affine2D tmp(1, 0, 0, y, 1, 0);
    multiply(tmp);
    return *this;
}

Affine2D&
Affine2D::skew(double sx, double sy)
{
    double rx = sx * M_PI / 180.0;
    double ry = sy * M_PI / 180.0;
    Affine2D tmp(1, tan(rx), 0, tan(ry), 1, 0);
    multiply(tmp);
    return *this;
}

Affine2D&
Affine2D::translate(double dx, double dy)
{
    Affine2D tmp(1, 0, dx, 0, 1, dy);
    multiply(tmp);
    return *this;
}

Affine2D&
Affine2D::operator =(const Affine2D& other)
{
    if (this != &other)
    {
        _m[0] = other._m[0];
        _m[1] = other._m[1];
        _m[2] = other._m[2];
        _m[3] = other._m[3];
        _m[4] = other._m[4];
        _m[5] = other._m[5];
    }
    return *this;
}

Affine2D&
Affine2D::operator *=(const Affine2D& other)
{
    return multiply(other);
}

Affine2D
Affine2D::operator ~() const
{
    Affine2D tmp = *this;
    return tmp.invert();

}

Affine2D&
Affine2D::multiply(const Affine2D& o)
{
    double t0 = _m[0] * o._m[0] + _m[1] * o._m[2];
    double t2 = _m[2] * o._m[0] + _m[3] * o._m[2];
    double t4 = _m[4] * o._m[0] + _m[5] * o._m[2] + o._m[4];
    _m[1] = _m[0] * o._m[1] + _m[1] * o._m[3];
    _m[3] = _m[2] * o._m[1] + _m[3] * o._m[3];
    _m[5] = _m[4] * o._m[1] + _m[5] * o._m[3] + o._m[5];
    _m[0] = t0;
    _m[2] = t2;
    _m[4] = t4;
    return *this;
}

} /* namespace ilixi */
