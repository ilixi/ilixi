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

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <core/Application.h>
#include <ilixiGUI.h>
#include <core/DaleDFB.h>

namespace ilixi
{

class Settings : public Application
{
public:
    Settings(int argc, char* argv[]);

    virtual
    ~Settings();

private:
    IComaComponent* _comp;
    CheckBox* _clickToFocus;
    CheckBox* _enableAnimations;
    CheckBox* _showZoom;
    CheckBox* _showOpacity;
    SpinBox* _showDuration;
    CheckBox* _hideZoom;
    CheckBox* _hideOpacity;
    SpinBox* _hideDuration;

    ComboBox* _showZoomTransition;
    ComboBox* _showOpacityTransition;

    Slider* _brightness;

    PushButton* _apply;
    PushButton* _reset;

    void
    setDefaults();

    void
    sendSettings();
};

} /* namespace ilixi */
#endif /* SETTINGS_H_ */
