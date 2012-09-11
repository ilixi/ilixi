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

#include "AppThumbnail.h"
#include "Compositor.h"
#include <graphics/Painter.h>
#include <sigc++/bind.h>

namespace ilixi
{

AppThumbnail::AppThumbnail(ILXCompositor* compositor, AppInstance* instance, Widget* parent)
        : AppCompositor(compositor, instance, parent),
          _close(NULL)
{
    setInputMethod(KeyAndPointerInput);
    setConstraints(FixedConstraint, FixedConstraint);
    _ani.setDuration(300);
    _opacityTween = new Tween(Tween::SINE, Tween::EASE_OUT, 128, 255);
    _ani.addTween(_opacityTween);
    _ani.sigExec.connect(sigc::mem_fun(this, &AppThumbnail::tweenSlot));
    if (!(instance->appInfo()->appFlags() & APP_AUTO_START))
    {
        _close = new ToolButton("");
        _close->setToolButtonStyle(ToolButton::IconOnly);
        _close->setIcon(ILIXI_DATADIR"compositor/close.png", Size(32, 32));
        _close->setDrawFrame(false);
        _close->setGeometry(164, 0, 32, 32);
        _close->sigPressed.connect(
                sigc::bind<AppInstance*>(
                        sigc::mem_fun(_compositor, &ILXCompositor::killApp),
                        _instance));
        addChild(_close);
    }

    setVisible(false);
}

AppThumbnail::~AppThumbnail()
{
}

Size
AppThumbnail::preferredSize() const
{
    return Size(196, 156);
}

void
AppThumbnail::compose(const PaintEvent& event)
{
}

void
AppThumbnail::pointerButtonUpEvent(const PointerEvent& pointerEvent)
{
    sigFocused(this);
    sigSelected(_instance);
}

void
AppThumbnail::keyUpEvent(const KeyEvent& keyEvent)
{
    if (keyEvent.keySymbol == DIKS_SPACE)
        sigSelected(_instance);
}

void
AppThumbnail::focusInEvent()
{
    sigFocused(this);
//    _ani.stop();
//    _opacityTween->setInitialValue(128);
//    _opacityTween->setEndValue(255);
//    _ani.start();
}

void
AppThumbnail::focusOutEvent()
{
//    _ani.stop();
//    _opacityTween->setInitialValue(255);
//    _opacityTween->setEndValue(128);
//    _ani.start();
}

void
AppThumbnail::tweenSlot()
{
//    setOpacity(_opacityTween->value());
    update();
}

} /* namespace ilixi */
