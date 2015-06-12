/*
 Copyright 2010-2015 Tarik Sekmen.

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

#ifndef GESTUREDEMO_H_
#define GESTUREDEMO_H_

#include <core/Application.h>
#include <lib/Gesture.h>
#include <types/Image.h>

namespace ilixi
{

class GestureDemo : public Application
{
public:
    GestureDemo(int argc, char* argv[]);

    virtual
    ~GestureDemo();

protected:
    void
    compose(const ilixi::PaintEvent& event);

private:
    GestureRecognizer _gestures;
    Image* _legend;
    std::string _match;

    void
    matched(const std::string& gesture);

    void
    moving();
};

} /* namespace ilixi */
#endif /* GESTUREDEMO_H_ */
