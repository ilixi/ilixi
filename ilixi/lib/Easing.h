/*
 TERMS OF USE - EASING EQUATIONS

 The equations are originally written by Robert Penner <info@robertpenner.com>.
 Later implemented in C++ by Jesus Gollonet <jesus@jesusgollonet.com>,
 Tarik Sekmen <tarik@ilixi.org>.

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
 * Neither the name of the author nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

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

#ifndef ILIXI_EASING_H_
#define ILIXI_EASING_H_

namespace ilixi
{
//! Easing equations.
namespace Ease
{

//! Back easing.
namespace Back
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}

//! Bouncy easing.
namespace Bounce
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}

//! Circular easing.
namespace Circle
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}

//! Cubical easing.
namespace Cubic
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}

//! Elastic easing.
namespace Elastic
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}

//! Exponential easing.
namespace Expo
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}

//! Quadratic easing.
namespace Quad
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}

//! Quartic easing.
namespace Quart
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}

//! Quintic easing.
namespace Quint
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}

//! Sinusoidal easing
namespace Sine
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}

//! Linear easing.
namespace Linear
{
float
easeIn(float t, float b, float c, float d);

float
easeOut(float t, float b, float c, float d);

float
easeInOut(float t, float b, float c, float d);
}
}
} // namespace ilixi

#endif /* ILIXI_EASING_H_ */
