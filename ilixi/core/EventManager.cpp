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

#include <core/EventManager.h>
#include <ui/Widget.h>
#include <ui/WindowWidget.h>
#include <core/Logger.h>
#if ILIXI_HAVE_FUSIONDALE
#include <core/DaleDFB.h>
#endif

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_EVENTMANAGER, "ilixi/core/EventManager", "EventManager");

EventManager::EventManager(WindowWidget* creator)
        : _focusedWidget(0),
          _exposedWidget(0),
          _grabbedWidget(0),
          _oskWidget(0),
          _creator(creator)
{
    ILOG_TRACE_F(ILX_EVENTMANAGER);
}

EventManager::~EventManager()
{
    ILOG_TRACE_F(ILX_EVENTMANAGER);
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
    ILOG_TRACE_F(ILX_EVENTMANAGER);
    ILOG_DEBUG(ILX_EVENTMANAGER, " -> widget: %p\n", widget);

    if (widget == _exposedWidget)
        return false;

    if (_exposedWidget)
    {
        _exposedWidget->_state = (WidgetState) (_exposedWidget->_state & ~ExposedState);
        _exposedWidget->_state = (WidgetState) (_exposedWidget->_state & ~PressedState);
        _exposedWidget->sigStateChanged(_exposedWidget, _exposedWidget->_state);
        _exposedWidget->leaveEvent(pointerEvent);
    }

    _exposedWidget = widget;
    if (_exposedWidget)
    {
        _exposedWidget->_state = (WidgetState) (_exposedWidget->_state | ExposedState);
        _exposedWidget->sigStateChanged(_exposedWidget, _exposedWidget->_state);
        _exposedWidget->enterEvent(pointerEvent);
    }

    return true;
}

bool
EventManager::setFocusedWidget(Widget* widget)
{
    ILOG_TRACE_F(ILX_EVENTMANAGER);
    ILOG_DEBUG(ILX_EVENTMANAGER, " -> widget: %p\n", widget);

    if (widget && !(widget->acceptsKeyInput()))
        return false;

    if (widget == _focusedWidget)
        return false;

    if (_focusedWidget)
    {
        _focusedWidget->_state = (WidgetState) (_focusedWidget->_state & ~FocusedState);
        _focusedWidget->sigStateChanged(_focusedWidget, _focusedWidget->_state);
        _focusedWidget->focusOutEvent();
    }

    _focusedWidget = widget;

    if (widget != NULL)
    {
        if (widget->_parent)
            widget->_parent->_preSelectedWidget = _focusedWidget;

        _focusedWidget->_state = (WidgetState) (_focusedWidget->_state | FocusedState);
        _focusedWidget->sigStateChanged(_focusedWidget, _focusedWidget->_state);
        _focusedWidget->focusInEvent();
    }

    setOSKWidget(widget);

    return true;
}

bool
EventManager::setGrabbedWidget(Widget* widget, const PointerEvent& pointerEvent)
{
    ILOG_TRACE_F(ILX_EVENTMANAGER);
    ILOG_DEBUG(ILX_EVENTMANAGER, " -> widget: %p\n", widget);
    if (widget && !(widget->inputMethod() & PointerGrabbing))
        return false;

    if (_grabbedWidget != widget)
    {
        if (_grabbedWidget)
        {
            _grabbedWidget->_state = (WidgetState) (_grabbedWidget->_state & ~(GrabbedState | PressedState));
            _grabbedWidget->sigStateChanged(_grabbedWidget, _grabbedWidget->_state);
            _grabbedWidget->pointerReleaseEvent(pointerEvent);
            _grabbedWidget->update();
        }
        _grabbedWidget = widget;
        if (_grabbedWidget)
        {
            _grabbedWidget->_state = (WidgetState) (_grabbedWidget->_state | GrabbedState | PressedState);
            _grabbedWidget->sigStateChanged(_grabbedWidget, _grabbedWidget->_state);
            _grabbedWidget->pointerGrabEvent(pointerEvent);
            _grabbedWidget->update();
        }
        ILOG_DEBUG( ILX_EVENTMANAGER, "setGrabbedWidget ( widget %p, event.x %d, event.y %d )\n", widget, pointerEvent.x, pointerEvent.y);
        return true;
    }
    return false;
}

bool
EventManager::setOSKWidget(Widget* widget)
{
    if (widget == _oskWidget)
        return false;

    if (widget == NULL || (!(widget->inputMethod() & OSKInput) && _oskWidget))
    {
        _oskWidget = NULL;
#if ILIXI_HAVE_FUSIONDALE
        DaleDFB::hideOSK();
#endif
        return false;
    } else if (widget == _oskWidget)
        return false;
    else if (widget->inputMethod() & OSKInput)
    {
#if ILIXI_HAVE_FUSIONDALE
        Rectangle r = widget->frameGeometry();
        Point p = _creator->_window->windowPosition();
        r.translate(p.x(), p.y());
        DaleDFB::showOSK(r);
#endif
        _oskWidget = widget;
        return true;
    }
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
        if (target == _creator || target == NULL)
        {
            if (selectNeighbourFromChildren(_creator, direction))
                return true;
        }

        if (!target->acceptsKeyInput() && target->_children.size() == 0)
            target = target->getNeighbour(direction);

        // Should terminate if we can not find anything.
        if (step || target == NULL)
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
        for (Widget::WidgetListReverseIterator it = target->_children.rbegin(), end = target->_children.rend();
                it != end; ++it)
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
        for (Widget::WidgetListIterator it = target->_children.begin(), end = target->_children.end(); it != end; ++it)
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
EventManager::selectNext(Widget* target, Widget* startFrom, int iter)
{
    ILOG_TRACE_F(ILX_EVENTMANAGER);
    ILOG_DEBUG(ILX_EVENTMANAGER, " -> target: %p\n", target);
    ILOG_DEBUG(ILX_EVENTMANAGER, " -> startFrom: %p\n", startFrom);
    ILOG_DEBUG(ILX_EVENTMANAGER, " -> iter: %d\n", iter);

    if (iter)
        return true;

    if (target == NULL && startFrom == _creator)
        ++iter;

    if (startFrom && target == _creator)
    {
        startFrom = NULL;
        iter++;
    } else if (!target)
    {
        if (_focusedWidget)
            target = _focusedWidget;
        else
            target = _creator;
    }

    if (setFocusedWidget(target))
        return true;

    if (target->_children.size())
    {
        Widget* targetChild;
        for (Widget::WidgetListIterator it = target->_children.begin(), end = target->_children.end(); it != end; ++it)
        {
            targetChild = (Widget*) *it;

            if (startFrom)
            {
                if (startFrom == targetChild)
                    startFrom = NULL;
            } else
            {
                if (targetChild->_children.size())
                {
                    if (selectNext(targetChild, NULL, iter))
                        return true;
                } else if (setFocusedWidget(targetChild))
                    return true;
            }
        }
        return selectNext(target->_parent, target, iter);
    } else if (target != _creator)
        return selectNext(target->_parent, target, iter);
    return false;
}

bool
EventManager::selectPrevious(Widget* target, Widget* startFrom, int iter)
{
    if (iter)
        return true;

    if (startFrom && target == _creator)
    {
        startFrom = NULL;
        iter++;
    } else if (!target)
    {
        if (_focusedWidget)
            target = _focusedWidget;
        else
            target = _creator;
    }

    if (setFocusedWidget(target))
        return true;

    if (target->_children.size())
    {
        Widget* targetChild;
        for (Widget::WidgetListReverseIterator it = target->_children.rbegin(), end = target->_children.rend();
                it != end; ++it)
        {
            targetChild = (Widget*) *it;

            if (startFrom)
            {
                if (startFrom == targetChild)
                    startFrom = NULL;
            } else
            {
                if (targetChild->_children.size())
                {
                    if (selectPrevious(targetChild, NULL, iter))
                        return true;
                } else if (setFocusedWidget(targetChild))
                    return true;
            }
        }
        return selectPrevious(target->_parent, target, iter);
    } else if (target != _creator)
        return selectPrevious(target->_parent, target, iter);
    return false;
}

void
EventManager::setOSKWidgetText(const std::string& text)
{
    // TODO implement OSK
}

void
EventManager::clear(Widget* widget)
{
    ILOG_TRACE_F(ILX_EVENTMANAGER);
    ILOG_DEBUG(ILX_EVENTMANAGER, " -> widget: %p\n", widget);
    if (widget == NULL)
        return;
    if (_focusedWidget && _focusedWidget == widget)
        _focusedWidget = NULL;
    if (_grabbedWidget && _grabbedWidget == widget)
        _grabbedWidget = NULL;
    if (_exposedWidget && _exposedWidget == widget)
        _exposedWidget = NULL;
}

void
EventManager::clear()
{
    ILOG_TRACE_F(ILX_EVENTMANAGER);
    if (_focusedWidget)
    {
        _focusedWidget->_state = (WidgetState) (_focusedWidget->_state & ~FocusedState);
        _focusedWidget->focusOutEvent();
        _focusedWidget->sigStateChanged(_focusedWidget, _focusedWidget->_state);
        _focusedWidget = NULL;
    }

    if (_exposedWidget)
    {
        _exposedWidget->_state = (WidgetState) (_exposedWidget->_state & ~ExposedState);
        _exposedWidget->_state = (WidgetState) (_exposedWidget->_state & ~PressedState);
        _exposedWidget->leaveEvent(PointerEvent());
        _exposedWidget->sigStateChanged(_exposedWidget, _exposedWidget->_state);
        _exposedWidget = NULL;
    }

    if (_grabbedWidget)
    {
        _grabbedWidget->pointerReleaseEvent(PointerEvent());
        _grabbedWidget = NULL;
    }

}

} /* namespace ilixi */
