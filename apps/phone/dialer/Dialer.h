/*
 Copyright 2010-2012 Tarik Sekmen.

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

#ifndef DIALER_H_
#define DIALER_H_

#include <core/Application.h>
#include <ui/LineInput.h>
#include <ui/PushButton.h>

namespace ilixi
{

class Dialer : public Application
{
public:
    Dialer(int argc, char* argv[]);

    virtual
    ~Dialer();

private:
    Font* _font;
    LineInput* _input;

    PushButton* _key1;
    PushButton* _key2;
    PushButton* _key3;

    PushButton* _key4;
    PushButton* _key5;
    PushButton* _key6;

    PushButton* _key7;
    PushButton* _key8;
    PushButton* _key9;

    PushButton* _keyStar;
    PushButton* _key0;
    PushButton* _keyHash;

    PushButton* _keyCall;
    PushButton* _keyCLR;
    PushButton* _keyEnd;

    virtual bool
    windowPreEventFilter(const DFBWindowEvent& event);
};

} /* namespace ilixi */
#endif /* DIALER_H_ */
