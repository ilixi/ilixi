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

PAppButton::PAppButton(const std::string& text, Widget* parent)
        : ToolButton(text, parent),
          _colorize(true)
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    _timer.sigExec.connect(sigc::mem_fun(this, &PAppButton::timerSlot));
    sigGeometryUpdated.connect(sigc::mem_fun(this, &PAppButton::updateIconGeometry));
    setConstraints(MinimumConstraint, FixedConstraint);
    setDrawFrame(false);
    _textColor = stylist()->palette()->getGroup(FocusedState).text;
}

PAppButton::~PAppButton()
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
}

Size
PAppButton::preferredSize() const
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    return Size(150, 150);
}

void
PAppButton::setAppVisible(bool visible)
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    _timer.stop();
    if (visible)
        _textColor = Color(0, 255, 0);
    update();
}

void
PAppButton::appStarting()
{
    ILOG_TRACE_W(ILX_HOMEAPPBUTTON);
    _timer.start(400);
}

void
PAppButton::appQuit()
{
    _timer.stop();
    _textColor = stylist()->palette()->getGroup(DefaultState).text;
    update();
}

void
PAppButton::compose(const PaintEvent& event)
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
PAppButton::updateIconGeometry()
{
    _iconPos = _icon->surfaceGeometry().topLeft();
}

void
PAppButton::timerSlot()
{
    if (_colorize)
        _textColor = Color(0, 0, 255);
    else
        _textColor = stylist()->palette()->getGroup(DefaultState).text;
    _colorize = !_colorize;
    update();
}

} /* namespace ilixi */
