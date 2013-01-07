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

D_DEBUG_DOMAIN( ILX_HOMEAPPBUTTON, "ilixi/phone/Home/AppButton", "AppButton");

AppButton::AppButton(const std::string& text, Widget* parent)
        : ToolButton(text, parent),
          _appVisible(0)
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    _anim = new TweenAnimation();
    _anim->setDuration(500);
    _anim->setLoops(0);
    _tween = new Tween(Tween::LINEAR, Tween::EASE_IN, 0.5, 1);
    _anim->addTween(_tween);
    _anim->sigExec.connect(sigc::mem_fun(this, &AppButton::tweenSlot));
    sigGeometryUpdated.connect(sigc::mem_fun(this, &AppButton::updateIconGeometry));
    setConstraints(MinimumConstraint, FixedConstraint);
    setDrawFrame(false);
    _textColor = stylist()->palette()->getGroup(FocusedState).text;
}

AppButton::~AppButton()
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    delete _anim;
}

Size
AppButton::preferredSize() const
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
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
    _anim->stop();
    if (_appVisible)
        _textColor = stylist()->palette()->getGroup(DefaultState).text;
    else
        _textColor = stylist()->palette()->getGroup(DisabledState).text;

    update();
}

void
AppButton::appStarting()
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    _anim->start();
}

void
AppButton::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    if (state() & FocusedState)
    {
        p.setBrush(stylist()->palette()->focus);
        p.fillRectangle(0, 0, width(), height());
    }

    p.setFont(*font());
    p.setBrush(_textColor);
    p.drawLayout(layout(), 0, 0);
}

void
AppButton::updateIconGeometry()
{
    _iconPos = _icon->surfaceGeometry().topLeft();
}

void
AppButton::tweenSlot()
{
    _textColor = stylist()->palette()->getGroup(DefaultState).text;
    _textColor.setBrightness(_tween->value());
    update();
}

} /* namespace ilixi */
