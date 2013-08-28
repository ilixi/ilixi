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

#include <ui/ContainerBase.h>
#include <core/EventManager.h>
#include <algorithm>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_CONTAINER, "ilixi/ui/ContainerBase", "ContainerBase");

ContainerBase::ContainerBase(Widget* parent)
        : Widget(parent),
          _layout(NULL)
{
    setLayout(new LayoutBase(this));
    setInputMethod(PointerPassthrough);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &ContainerBase::updateLayoutGeometry));
}

ContainerBase::~ContainerBase()
{
    ILOG_TRACE_W(ILX_CONTAINER);
}

int
ContainerBase::heightForWidth(int width) const
{
    ILOG_TRACE_W(ILX_CONTAINER);
    return _layout->heightForWidth(width);
}

Size
ContainerBase::preferredSize() const
{
    ILOG_TRACE_W(ILX_CONTAINER);
    return _layout->preferredSize();
}

Rectangle
ContainerBase::childrenRect() const
{
    ILOG_TRACE_W(ILX_CONTAINER);
    // TODO return childrenRect()
    return Rectangle();
}

unsigned int
ContainerBase::spacing() const
{
    return _layout->spacing();
}

bool
ContainerBase::addWidget(Widget* widget)
{
    ILOG_TRACE_W(ILX_CONTAINER);
    return _layout->addWidget(widget);
}

bool
ContainerBase::removeWidget(Widget* widget)
{
    ILOG_TRACE_W(ILX_CONTAINER);
    return _layout->removeWidget(widget);
}

LayoutBase*
ContainerBase::layout()
{
    return _layout;
}

void
ContainerBase::setLayout(LayoutBase* layout)
{
    ILOG_TRACE_W(ILX_CONTAINER);
    if (layout)
    {
        removeChild(_layout);
        _layout = layout;
        addChild(_layout);
        raiseChildToFront(_layout);
    }
}

void
ContainerBase::setSpacing(unsigned int spacing)
{
    _layout->setSpacing(spacing);
}

void
ContainerBase::doLayout()
{
    ILOG_TRACE_W(ILX_CONTAINER);
//  if (_layout)
//    _layout->tile();
    if (parent())
        parent()->doLayout();
}

void
ContainerBase::updateLayoutGeometry()
{
    ILOG_TRACE_W(ILX_CONTAINER);
    _layout->setGeometry(0, 0, width(), height());
}

bool
ContainerBase::consumePointerEvent(const PointerEvent& pointerEvent)
{
    ILOG_TRACE_W(ILX_CONTAINER);
    // priority is given to child on top.
    if (_frameGeometry.contains(pointerEvent.x, pointerEvent.y, true))
        for (WidgetListReverseIterator it = _children.rbegin(); it != _children.rend(); ++it)
            if (((Widget*) *it)->consumePointerEvent(pointerEvent))
                return true;
    return false;
}

} /* namespace ilixi */
