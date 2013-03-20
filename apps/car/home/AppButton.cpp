/*
 * AppButton.cpp
 *
 *  Created on: Aug 29, 2012
 *      Author: tarik
 */

#include "AppButton.h"
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_HOMEAPPBUTTON, "ilixi/car/Home/AppButton", "AppButton");

AppButton::AppButton(const std::string& text, Widget* parent)
        : ToolButton(text, parent),
          _appVisible(0)
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    _anim = new TweenAnimation();
    _anim->setDuration(500);
    _anim->setLoops(0);
    _tween = new Tween(Tween::LINEAR, Tween::EASE_IN, 0, 20);
    _anim->addTween(_tween);
    _anim->sigExec.connect(sigc::mem_fun(this, &AppButton::tweenSlot));
    sigGeometryUpdated.connect(
            sigc::mem_fun(this, &AppButton::updateIconGeometry));
    setConstraints(MinimumConstraint, FixedConstraint);
}

AppButton::~AppButton()
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    delete _anim;
}

Size
AppButton::preferredSize() const
{
    return Size(150, 150);
}

bool
AppButton::appVisible() const
{
    return _appVisible;
}

void
AppButton::setAppVisible(bool visible)
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    _appVisible = visible;
    if (_appVisible)
    {
        _anim->stop();
        _icon->moveTo(
                (width() - 96) / 2,
                stylist()->defaultParameter(StyleHint::ToolButtonTop) + 1);
        _icon->setSize(96, 96);
        update();
    }
}

void
AppButton::appStarting()
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    _anim->start();
}

void
AppButton::updateIconGeometry()
{
    _iconPos = _icon->surfaceGeometry().topLeft();
}

void
AppButton::tweenSlot()
{
    int temp;
    int delta;
    if (_tween->value() <= 10)
    {
        delta = -_tween->value() / 2;
        temp = 96 + _tween->value();
    } else if (_tween->value() <= 20)
    {
        delta = _tween->value() / 2 - 10;
        temp = 116 - _tween->value();
    }

    _icon->moveTo(_iconPos.x() + delta, _iconPos.y() + delta);
    _icon->setSize(temp, temp);

    update();
}

} /* namespace ilixi */
