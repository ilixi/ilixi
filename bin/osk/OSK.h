/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#ifndef OSK_H_
#define OSK_H_

#include <ui/Application.h>
#include <ui/Frame.h>
#include <ui/ToolButton.h>
#include <lib/Thread.h>
#include <ui/LineInput.h>
#include <vector>
#include "Keyboard.h"

using namespace ilixi;

class OSKButton;

class OSKButtonThread : public Thread
{
public:
    OSKButtonThread(OSKButton* button);

    virtual
    ~OSKButtonThread();

    int
    run();

private:
    OSKButton* _button;
};

class OSKButton : public ToolButton
{
    friend class OSK;
public:
    /*!
     * Constructor
     * @param text
     * @param parent
     */
    OSKButton(std::string text, Widget* parent = 0);

    OSKButton(std::string c1, std::string c2, std::string c3,
              DFBInputDeviceKeySymbol s1, DFBInputDeviceKeySymbol s2,
              DFBInputDeviceKeySymbol s3, Widget* parent = 0);

    /*!
     * Destructor
     */
    virtual
    ~OSKButton();

    void
    pressSlot();

    static LineInput* _target;
    static short _mode;

private:
    const std::string _c1, _c2, _c3;
    DFBInputDeviceKeySymbol _s1, _s2, _s3;
    //  OSKButtonThread* _thread;

};

class OSK : public Application
{
public:
    OSK(int argc, char* argv[]);

    virtual
    ~OSK();

private:
    LineInput* _input;
    Font* _buttonFont;
    OSKButton* _caps;
    OSKButton* _style;
    std::vector<OSKButton*> _keys;
    Keyboard* _keyboard;

    void
    append(DFBInputDeviceKeySymbol key);

    void
    changeCase();

    void
    changeLayout();

    void
    changeSymbols();

    void
    createKeys();

    void
    done();

    void
    compose(const PaintEvent& event);

    void
    setOptimalFontSize();

};

#endif /* OSK_H_ */
