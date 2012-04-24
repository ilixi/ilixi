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
#include "ui/Widget.h"
#include "ui/WindowWidget.h"
#include "core/Logger.h"

using namespace ilixi;

EventManager::EventManager(WindowWidget* creator) :
    _focusedWidget(0), _exposedWidget(0), _grabbedWidget(0), _oskWidget(0), _creator(
        creator)
{
}

EventManager::~EventManager()
{
  ILOG_DEBUG(ILX, "~EventManager %p\n", this);
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
EventManager::selectNeighbour(Direction direction)
{
  if (!_focusedWidget)
    return false;

  Widget* target = _focusedWidget->getNeighbour(direction);
  bool found = false;
  int step = 0;
  while (!found)
    {
//      ILOG_DEBUG(
//          "%s(%p, %d) - step: %d", __FUNCTION__, target, direction, step);
//      ILOG_DEBUG(
//          " %p %p %p %p", _focusedWidget->getNeighbour(Up), _focusedWidget->getNeighbour(Down), _focusedWidget->getNeighbour(Left), _focusedWidget->getNeighbour(Right));

      if (target == _creator || target == NULL)
        {
          if (selectNeighbourFromChildren(_creator, direction))
            return true;
        }

      // Should terminate if we can not find anything.
      if (step)
        return false;

      if (target != _creator) // Target is not a window.
        {
          // See if direct neighbour can get focus...
          if (setFocusedWidget(target))
            return true;
        }

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
  if (target == NULL)
    return false;

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

bool
EventManager::selectNext(Widget* target, Widget* startFrom)
{
  return false;
  if (!target)
    {
      if (!_focusedWidget)
        target = _creator;
      else
        target = _focusedWidget;
    }

  ILOG_DEBUG(ILX, "Target %p\n", target);
  if (target->_children.size())
    {
      Widget* targetChild;
      int i = 1;
      for (Widget::WidgetListIterator it = target->_children.begin(), end =
          target->_children.end(); it != end; ++it, ++i)
        {
          targetChild = (Widget*) *it;

          if (startFrom == targetChild)
            {
              startFrom = NULL;
//              continue;
            }

          else if (!startFrom)
            {
              ILOG_DEBUG(ILX, "TargetChild %p\n", targetChild);
              // check children of child recursively...
              if (targetChild->_children.size())
                {
                  if (selectNext(targetChild))
                    return true;
                }
              else // get next child from parent.
                {
                  ILOG_DEBUG(ILX, " No more children\n");
                }

              // now see if child can receive focus.
              if (setFocusedWidget(targetChild))
                {
                  ILOG_DEBUG(ILX, "Focused TargetChild %p\n", targetChild);
                  return true;
                }

            }
        }

      selectNext(target->_parent, target);
    }
  else
    selectNext(target->_parent, target);
  return false;
}

bool
EventManager::selectPrevious(Widget* target, Widget* start)
{
  return false;
}

void
EventManager::setOSKWidgetText(const std::string& text)
{
  // TODO implement OSK
}
