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

#include "ui/ContainerBase.h"
#include "core/EventManager.h"
#include <algorithm>
#include "core/Logger.h"

using namespace ilixi;

ContainerBase::ContainerBase(Widget* parent) :
    Widget(parent), _layout(NULL)
{
  setLayout(new LayoutBase(this));
  setInputMethod(PointerInput);
  sigGeometryUpdated.connect(
      sigc::mem_fun(this, &ContainerBase::updateLayoutGeometry));
}

ContainerBase::~ContainerBase()
{
}

int
ContainerBase::heightForWidth(int width) const
{
  return _layout->heightForWidth(width);
}

Size
ContainerBase::preferredSize() const
{
  return _layout->preferredSize();
}

Rectangle
ContainerBase::childrenRect() const
{
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
  return _layout->addWidget(widget);
}

bool
ContainerBase::removeWidget(Widget* widget)
{
  return _layout->removeWidget(widget);
}

LayoutBase*
ContainerBase::layout()
{
  return _layout;
}

WidgetResizeConstraint
ContainerBase::xConstraint() const
{
  return _layout->xConstraint();
}

WidgetResizeConstraint
ContainerBase::yConstraint() const
{
  return _layout->yConstraint();
}

void
ContainerBase::setLayout(LayoutBase* layout)
{
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
  if (_layout)
    _layout->tile();
  if (parent())
    parent()->doLayout();
}

bool
ContainerBase::widgetToFront(Widget* widget)
{
  return _layout->raiseChildToFront(widget);
}

bool
ContainerBase::widgetToBack(Widget* widget)
{
  return _layout->lowerChildToBottom(widget);
}

bool
ContainerBase::lowerWidget(Widget* widget)
{
  return _layout->lowerChild(widget);
}

bool
ContainerBase::raiseWidget(Widget* widget)
{
  return _layout->raiseChild(widget);
}

void
ContainerBase::updateLayoutGeometry()
{
  _layout->setGeometry(0, 0, width(), height());
}

bool
ContainerBase::consumePointerEvent(const PointerEvent& pointerEvent)
{
  if (_inputMethod & PointerInput)
    {
      if (_frameGeometry.contains(pointerEvent.x, pointerEvent.y, true))
        {
          // priority is given to most recent child.
          for (WidgetListReverseIterator it = _children.rbegin();
              it != _children.rend(); ++it)
            {
              // event is consumed by child.
              if (((Widget*) *it)->acceptsPointerInput()
                  && ((Widget*) *it)->consumePointerEvent(pointerEvent))
                return true;
            }
          if (eventManager())
            eventManager()->setExposedWidget(NULL, pointerEvent);
        }
    }
  return false;
}

