/*
 TERMS OF USE - EASING EQUATIONS

 The equations are originally written by Robert Penner <info@robertpenner.com>.
 Later implemented in C++ by Jesus Gollonet <jesus@jesusgollonet.com>,
 and minor modifications by Tarik Sekmen <tarik@ilixi.org>.

 Open source under the BSD License.

 Copyright (c) 2001 Robert Penner
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the author nor the names of its contributors may
 be used to endorse or promote products derived from this software without
 specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <lib/Easing.h>
#include <cmath>

// Global constants with internal linkage
namespace ilixi
{
namespace Ease
{
float const Pi = 3.14159265f;
float const PiHalved = 1.57079633f;
float const Pi2 = 6.28318531f;

namespace Back
{
float const s = 1.70158f;
float const s1 = s + 1;
float const sMult = s * 1.525f;
float const sMult1 = sMult + 1;
}
}
}

using namespace ilixi::Ease;

//**********************************************************************
// Back
//**********************************************************************
float
Back::easeIn(float t, float b, float c, float d)
{
    return c * (t /= d) * t * (s1 * t - s) + b;
}

float
Back::easeOut(float t, float b, float c, float d)
{
    return c * ((t = t / d - 1) * t * (s1 * t + s) + 1) + b;
}

float
Back::easeInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
        return c / 2 * (t * t * (sMult1 * t - sMult)) + b;
    return c / 2 * ((t -= 2) * t * (sMult1 * t + sMult) + 2) + b;
}

//**********************************************************************
// Bounce
//**********************************************************************
float
Bounce::easeOut(float t, float b, float c, float d)
{
    if ((t /= d) < .363636364f)
        return c * (7.5625f * t * t) + b;
    else if (t < .727272727f)
        return c * (7.5625f * (t -= .545454545f) * t + .75f) + b;
    else if (t < .909090909f)
        return c * (7.5625f * (t -= .818181818f) * t + .9375f) + b;
    else
        return c * (7.5625f * (t -= .954545455f) * t + .984375f) + b;
}

float
Bounce::easeIn(float t, float b, float c, float d)
{
    return c - Bounce::easeOut(d - t, 0, c, d) + b;
}

float
Bounce::easeInOut(float t, float b, float c, float d)
{
    if (t < d / 2)
        return Bounce::easeIn(t * 2, 0, c, d) * .5f + b;
    else
        return Bounce::easeOut(t * 2 - d, 0, c, d) * .5f + c * .5f + b;
}

//**********************************************************************
// Circle
//**********************************************************************
float
Circle::easeIn(float t, float b, float c, float d)
{
    return c * (1 - (float) sqrt(1 - (t /= d) * t)) + b;
}

float
Circle::easeOut(float t, float b, float c, float d)
{
    t = t / d - 1;
    return (c * (float) sqrt(1 - pow(t, 2)) + b);
}

float
Circle::easeInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
        return c / 2 * (1 - (float) sqrt(1 - t * t)) + b;
    return c / 2 * ((float) sqrt(1 - (t -= 2) * t) + 1) + b;
}

//**********************************************************************
// Cubic
//**********************************************************************
float
Cubic::easeIn(float t, float b, float c, float d)
{
    return c * pow(t / d, 3) + b;
}

float
Cubic::easeOut(float t, float b, float c, float d)
{
    return c * (pow(t / d - 1, 3) + 1) + b;
}

float
Cubic::easeInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
        return c / 2 * pow(t, 3) + b;
    return c / 2 * (pow(t - 2, 3) + 2) + b;
}

//**********************************************************************
// Elastic
//**********************************************************************
float
Elastic::easeIn(float t, float b, float c, float d)
{
    if (t == 0)
        return b;
    else if ((t /= d) == 1)
        return b + c;
    float p = d * .3f;
    return -(c * pow(2, 10 * (t -= 1)) * sin((t * d - p / 4) * Pi2 / p)) + b;
}

float
Elastic::easeOut(float t, float b, float c, float d)
{
    if (t == 0)
        return b;
    if ((t /= d) == 1)
        return b + c;
    float p = d * .3f;
    return (c * pow(2, -10 * t) * sin((t * d - p / 4) * Pi2 / p) + c + b);
}

float
Elastic::easeInOut(float t, float b, float c, float d)
{
    if (t == 0)
        return b;
    if ((t /= d / 2) == 2)
        return b + c;
    float p = d * .45f;
    if (t < 1)
        return -.5f * (c * pow(2, 10 * (t -= 1)) * sin((t * d - p / 4) * Pi2 / p)) + b;
    return c * pow(2, -10 * (t -= 1)) * sin((t * d - p / 4) * Pi2 / p) * .5f + c + b;
}

//**********************************************************************
// Expo
//**********************************************************************
float
Expo::easeIn(float t, float b, float c, float d)
{
    return (t == 0) ? b : c * pow(2, 10 * (t / d - 1)) + b;
}

float
Expo::easeOut(float t, float b, float c, float d)
{
    return (t == d) ? b + c : c * (-pow(2, -10 * t / d) + 1) + b;
}

float
Expo::easeInOut(float t, float b, float c, float d)
{
    if (t == 0)
        return b;
    else if (t == d)
        return b + c;
    else if ((t /= d / 2) < 1)
        return c / 2 * pow(2, 10 * (t - 1)) + b;
    return c / 2 * (-pow(2, -10 * --t) + 2) + b;
}

//**********************************************************************
// Quad
//**********************************************************************
float
Quad::easeIn(float t, float b, float c, float d)
{
    return c * (t /= d) * t + b;
}

float
Quad::easeOut(float t, float b, float c, float d)
{
    return -c * (t /= d) * (t - 2) + b;
}

float
Quad::easeInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
        return ((c / 2) * (t * t)) + b;
    return -c / 2 * (((t - 2) * (--t)) - 1) + b;
}

//**********************************************************************
// Quart
//**********************************************************************
float
Quart::easeIn(float t, float b, float c, float d)
{
    return c * pow(t / d, 4) + b;
}

float
Quart::easeOut(float t, float b, float c, float d)
{
    return -c * (pow(t / d - 1, 4) - 1) + b;
}

float
Quart::easeInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
        return c / 2 * pow(t, 4) + b;
    return -c / 2 * (pow(t - 2, 4) - 2) + b;
}

//**********************************************************************
// Quint
//**********************************************************************
float
Quint::easeIn(float t, float b, float c, float d)
{
    return c * pow(t / d, 5) + b;
}

float
Quint::easeOut(float t, float b, float c, float d)
{
    return c * (pow(t / d - 1, 5) + 1) + b;
}

float
Quint::easeInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
        return c / 2 * pow(t, 5) + b;
    return c / 2 * (pow(t - 2, 5) + 2) + b;
}

//**********************************************************************
// Sine
//**********************************************************************
float
Sine::easeIn(float t, float b, float c, float d)
{
    return c * (1 - cos(t / d * PiHalved)) + b;
}

float
Sine::easeOut(float t, float b, float c, float d)
{
    return c * sin(t / d * PiHalved) + b;
}

float
Sine::easeInOut(float t, float b, float c, float d)
{
    return c / 2 * (1 - cos(Pi * t / d)) + b;
}

//**********************************************************************
// Linear
//**********************************************************************
float
Linear::easeIn(float t, float b, float c, float d)
{
    return c * t / d + b;
}

float
Linear::easeOut(float t, float b, float c, float d)
{
    return c * t / d + b;
}

float
Linear::easeInOut(float t, float b, float c, float d)
{
    return c * t / d + b;
}

