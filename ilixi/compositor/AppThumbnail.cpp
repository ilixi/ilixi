/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include <compositor/AppThumbnail.h>
#include <compositor/Compositor.h>
#include <core/Logger.h>
#include <graphics/Painter.h>
#include <sigc++/bind.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPTHUMB, "ilixi/compositor/AppThumbnail", "AppThumbnail");

AppThumbnail::AppThumbnail(ILXCompositor* compositor, AppInstance* instance, Widget* parent)
        : AppCompositor(compositor, instance, parent),
          _close(NULL)
{
    ILOG_TRACE_W(ILX_APPTHUMB);
    setInputMethod(KeyPointer);
    setConstraints(FixedConstraint, FixedConstraint);

    if (!(instance->appInfo()->appFlags() & APP_AUTO_START))
    {
        _close = new ToolButton("");
        _close->setToolButtonStyle(ToolButton::IconOnly);
        _close->setIcon(ILIXI_DATADIR"images/close.png", Size(32, 32));
        _close->setDrawFrame(false);
        _close->setGeometry(164, 0, 32, 32);
        _close->sigClicked.connect(sigc::bind<AppInstance*>(sigc::mem_fun(_compositor, &ILXCompositor::killApp), _instance));
        addChild(_close);
    }
    sigGeometryUpdated.connect(sigc::mem_fun(this, &AppThumbnail::updateThumbGeometry));
    sigRestacked.connect(sigc::mem_fun(this, &AppThumbnail::handleRestack));
    setVisible(false);
}

AppThumbnail::~AppThumbnail()
{
    ILOG_TRACE_W(ILX_APPTHUMB);
}

Size
AppThumbnail::preferredSize() const
{
    return Size(196, 156);
}

void
AppThumbnail::addWindow(IDirectFBWindow* window, bool eventHandling, bool blocking)
{
    ILOG_TRACE_W(ILX_APPTHUMB);
    AppCompositor::addWindow(window, eventHandling, blocking);
    _close->bringToFront();
}

void
AppThumbnail::setCloseVisible(bool visible)
{
    ILOG_TRACE_W(ILX_APPTHUMB);
    if (_close)
        _close->setVisible(visible);
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
}

void
AppThumbnail::focusOutEvent()
{
}

void
AppThumbnail::updateThumbGeometry()
{
    ILOG_TRACE_W(ILX_APPTHUMB);
    _close->setGeometry(width() - 32, 0, 32, 32);
}

void
AppThumbnail::handleRestack()
{
    _close->bringToFront();
}

} /* namespace ilixi */
