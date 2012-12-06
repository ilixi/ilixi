/*
 * AppButton.h
 *
 *  Created on: Aug 29, 2012
 *      Author: tarik
 */

#ifndef APPBUTTON_H_
#define APPBUTTON_H_

#include <ui/ToolButton.h>
#include <lib/TweenAnimation.h>
#include <lib/Tween.h>

namespace ilixi
{

class AppButton : public ilixi::ToolButton
{
public:
    AppButton(const std::string& text, Widget* parent = 0);

    virtual
    ~AppButton();

    Size
    preferredSize() const;

    bool
    appVisible() const;

    void
    setAppVisible(bool visible);

    void
    appStarting();

private:
    bool _appVisible;
    Point _iconPos;

    TweenAnimation* _anim;
    Tween* _tween;

    void
    updateIconGeometry();

    void
    tweenSlot();

};

} /* namespace ilixi */
#endif /* APPBUTTON_H_ */
