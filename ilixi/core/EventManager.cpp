/*
 Copyright 2011 Tarik Sekmen.

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

#include "core/EventManager.h"
#include "core/AppBase.h"
#include "ui/Widget.h"
#include "ui/WindowWidget.h"
#include "core/Logger.h"
#include <algorithm>

using namespace ilixi;
using namespace IMaestro;

EventManager::EventManager(Window* root) :
    _focusedWidget(0), _exposedWidget(0), _grabbedWidget(0), _oskWidget(0)
{
}

EventManager::~EventManager()
{
}

Widget*
EventManager::exposedWidget() const
{
  return _exposedWidget;
}

Widget*
EventManager::focusedWidget() const
{
  return _focusedWidget;
}

Widget*
EventManager::grabbedWidget() const
{
  return _grabbedWidget;
}

Widget*
EventManager::oskWidget() const
{
  return _oskWidget;
}

void
EventManager::reset()
{
  widgets.clear();
  _oskWidget = 0;
  _grabbedWidget = 0;
  _focusedWidget = 0;
  _exposedWidget = 0;
}

void
EventManager::addWidget(Widget* widget)
{
  if (!widget)
    return;
  widgets.push_back(widget);
}

void
EventManager::removeWidget(Widget* widget)
{
  if (!widget || widgets.size() <= 0)
    return;

  widgetListIterator it = std::find(widgets.begin(), widgets.end(), widget);
  if (widget == *it)
    {
      if (widget == _focusedWidget)
        _focusedWidget = NULL;
      widgets.erase(it);
    }
}

bool
EventManager::setExposedWidget(Widget* widget, const PointerEvent& pointerEvent)
{
  if (widget == _exposedWidget)
    return false;

  if (_exposedWidget)
    {
      _exposedWidget->_state = (WidgetState) (_exposedWidget->_state
          & ~ExposedState);
      _exposedWidget->_state = (WidgetState) (_exposedWidget->_state
          & ~PressedState);
      _exposedWidget->leaveEvent(pointerEvent);
    }

  _exposedWidget = widget;
  if (_exposedWidget)
    {
      _exposedWidget->_state = (WidgetState) (_exposedWidget->_state
          | ExposedState);
      _exposedWidget->enterEvent(pointerEvent);
    }

  return true;
}

bool
EventManager::setFocusedWidget(Widget* widget)
{
  if (widget == _focusedWidget || !(widget->acceptsKeyInput()))
    return false;

  if (_focusedWidget)
    {
      _focusedWidget->_state = (WidgetState) (_focusedWidget->_state
          & ~FocusedState);
      _focusedWidget->focusOutEvent();
#if ILIXI_MULTI_ENABLED
      if (_oskWidget)
        {
          AppBase::__appInstance->callMaestro(OSKEvent, Hidden);
          _oskWidget = NULL;
        }
#endif
    }

  _focusedWidget = widget;

  if (widget != NULL)
    {

      if (widget->_parent)
        widget->_parent->_preSelectedWidget = _focusedWidget;

      _focusedWidget->_state = (WidgetState) (_focusedWidget->_state
          | FocusedState);
      _focusedWidget->focusInEvent();
      setOSKWidget(widget);
    }
  return true;
}

bool
EventManager::setGrabbedWidget(Widget* widget, const PointerEvent& pointerEvent)
{
  if (_grabbedWidget != widget)
    {
      if (_grabbedWidget)
        _grabbedWidget->pointerReleaseEvent(pointerEvent);
      _grabbedWidget = widget;
      if (_grabbedWidget)
        _grabbedWidget->pointerGrabEvent(pointerEvent);
      return true;
    }
  return false;
}

bool
EventManager::setOSKWidget(Widget* widget)
{
  if (widget == _oskWidget || widget == NULL
      || !(widget->inputMethod() & OSKInput))
    return false;
  //  TextLayout* tw = dynamic_cast<TextLayout*> (_focusedWidget);
  //  if (tw)
  //    {
  //      _oskWidget = tw;
  //      AppBase::__appInstance->setOSKText(tw->text());
  //      AppBase::__appInstance->callMaestro(OSKEvent, Visible);
  //      return true;
  //    }
  return false;
}

bool
EventManager::selectNext(bool found, bool iter)
{
  for (widgetListIterator it = widgets.begin(); it != widgets.end(); ++it)
    {
      if (found && setFocusedWidget(*it))
        return true;

      else if (*it == _focusedWidget)
        found = true;
    }

  if (iter)
    return false;

  selectNext(true, true);
}

bool
EventManager::selectPrevious(bool found, bool iter)
{
  for (widgetListReverseIterator it = widgets.rbegin(); it != widgets.rend();
      ++it)
    {
      if (found && setFocusedWidget(*it))
        return true;

      else if (*it == _focusedWidget)
        found = true;
    }

  if (iter)
    return false;

  selectPrevious(true, true);
}

bool
EventManager::selectNeighbour(Direction direction)
{
  if (!_focusedWidget)
    return false;

  Widget* target = _focusedWidget->getNeighbour(direction);
  Window* root = _focusedWidget->_rootWindow;
  bool found = false;
  int step = 0;
  while (!found)
    {
      // Should terminate if we can not find anything.
      if (step)
        return false;

      // TODO key navi. does not work atm.
//      if (target != root) // Target is not a window.
//        {
//          // See if direct neighbour can get focus...
//          if (setFocusedWidget(target))
//            return true;
//        }

      // See if there is a preselected child widget...
      if (target->_preSelectedWidget)
        {
          if (setFocusedWidget(target->_preSelectedWidget))
            return true;
        }

      // See if children can receive focus...
      else if (target->_children.size())
        {
          if (selectNeighbourFromChildren(target, direction))
            return true;
        }
      // there are no focusable children so check neighbour in current direction...
      else
        target = target->getNeighbour(direction);
      ++step;
    }
  return false;
}

bool
EventManager::selectNeighbourFromChildren(Widget* target, Direction direction)
{
  Widget* targetChild;
  if (direction == Left || direction == Up)
    for (Widget::WidgetListReverseIterator it = target->_children.rbegin(),
        end = target->_children.rend(); it != end; ++it)
      {
        targetChild = (Widget*) *it;
        // check children of child recursively...
        if (targetChild->_children.size())
          if (selectNeighbourFromChildren(targetChild, direction))
            return true;
        // now see if child can receive focus.
        if (setFocusedWidget(targetChild))
          return true;
      }
  else
    for (Widget::WidgetListIterator it = target->_children.begin(), end =
        target->_children.end(); it != end; ++it)
      {
        targetChild = (Widget*) *it;
        // check children of child recursively...
        if (targetChild->_children.size())
          if (selectNeighbourFromChildren(targetChild, direction))
            return true;
        // now see if child can receive focus.
        if (setFocusedWidget(targetChild))
          return true;
      }
  return false;
}

void
EventManager::setOSKWidgetText(const std::string& text)
{
  //  if (_oskWidget)
  //    {
  //      TextLayout* tw = dynamic_cast<TextLayout*> (_focusedWidget);
  //      if (tw)
  //        {
  //          tw->setText(text);
  //          _oskWidget = NULL;
  //        }
  //    }
}
