/*
 * Settings.h
 *
 *  Created on: Sep 6, 2012
 *      Author: tarik
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

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
