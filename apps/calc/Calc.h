/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
            Sonat Deniz <sonat@ilixi.org>.

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

#ifndef CALC_H_
#define CALC_H_

#include <core/Application.h>
#include <ui/LineInput.h>
#include <ui/ToolButton.h>
#include <ui/Dialog.h>
#include <ui/Label.h>

namespace ilixi
{

class Calc : public Application
{
public:
    Calc(int argc, char* argv[]);

    virtual
    ~Calc();

private:
    Font* _font;
    LineInput* _input;

    ToolButton* _keyCLR;
    ToolButton* _keyPo;
    ToolButton* _keyPc;
    ToolButton* _keyDiv;
    ToolButton* _keyBack;

    ToolButton* _key7;
    ToolButton* _key8;
    ToolButton* _key9;
    ToolButton* _keyMult;
    ToolButton* _keyMod;

    ToolButton* _key4;
    ToolButton* _key5;
    ToolButton* _key6;
    ToolButton* _keyMin;
    ToolButton* _keyPow;

    ToolButton* _key1;
    ToolButton* _key2;
    ToolButton* _key3;
    ToolButton* _keyPlus;
    ToolButton* _keySqrt;

    ToolButton* _key0;
    ToolButton* _keyDot;
    ToolButton* _keyEq;

    Dialog* _dialog;
    Label* _text;
    Label* _errorText;

    void
    calculate();

    virtual bool
    windowPreEventFilter(const DFBWindowEvent& event);
};

} /* namespace ilixi */
#endif /* CALC_H_ */
