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

#ifndef ILIXI_EVENTMANAGER_H_
#define ILIXI_EVENTMANAGER_H_

#include <list>
#include <string>
#include <types/Event.h>
#include <types/Enums.h>
#include <ilixiConfig.h>

namespace ilixi
{
class Widget;
class WindowWidget;

//! Keeps track of input widgets.
/*!
 * This class is used by window widgets in order to forward key inputs inside application's main loop.
 */
class EventManager
{
    friend class WindowWidget;
public:
    /*!
     * Constructor.
     */
    EventManager(WindowWidget* creator);

    /*!
     * Destructor.
     */
    ~EventManager();

    /*!
     * Returns currently exposed widget inside window.
     */
    Widget*
    exposedWidget() const;

    /*!
     * Returns focused widget inside window.
     */
    Widget*
    focusedWidget() const;

    /*!
     * Returns grabbed widget if any.
     */
    Widget*
    grabbedWidget() const;

    /*!
     * Returns widget which is the target of OSK app.
     */
    Widget*
    oskWidget() const;

    /*!
     * Sets widgets exposed flag and executes onEnter/onLeave methods of widgets.
     *
     * @param widget becomes exposed.
     * @param pointerEvent usually an event of type PointerMotion.
     * @return true if successful.
     */
    bool
    setExposedWidget(Widget* widget, const PointerEvent& pointerEvent = PointerEvent());

    /*!
     * Sets widgets focused flag and executes focusInEvent/focusOutEvent methods of widgets.
     * Shows OSK app, if widget has OSKInput flag set.
     *
     * @param widget becomes focused.
     * @return true if successful.
     */
    bool
    setFocusedWidget(Widget* widget);

    /*!
     * Sets grabbed widget. If there is a grabbed widget PointerMotion events are passed to this
     * widget directly inside the main application loop.
     *
     * @param widget grabs pointer events.
     * @return true if successful.
     */
    bool
    setGrabbedWidget(Widget* widget, const PointerEvent& pointerEvent = PointerEvent());

    /*!
     * Sets the target of OSK app.
     *
     * @param widget becomes the target of OSK app.
     * @return true if successful.
     */
    bool
    setOSKWidget(Widget* widget);

    /*!
     *
     * @param direction
     * @return true if successful.
     */
    bool
    selectNeighbour(Direction direction);

    bool
    selectNext(Widget* target = NULL, Widget* startFrom = NULL, int iter = 0);

    bool
    selectPrevious(Widget* target = NULL, Widget* startFrom = NULL, int iter = 0);

    void
    setOSKWidgetText(const std::string& text);

    void
    clear(Widget* widget);

private:
    bool
    selectNeighbourFromChildren(Widget* target, Direction direction);

    //! Points to currently focused widget.
    Widget* _focusedWidget;
    //! Points to currently exposed widget.
    Widget* _exposedWidget;
    //! Points to currently grabbed widget.
    Widget* _grabbedWidget;
    //! Widget that requested osk input.
    Widget* _oskWidget;
    //! WindowWidget that created this event manager.
    WindowWidget* _creator;

#if ILIXI_HAVE_FUSIONDALE
    struct OSKRequest
    {
        DFBRectangle inputRect;
        int mode;
        pid_t process;
    };
#endif

    void
    clear();
};

}

#endif /* ILIXI_EVENTMANAGER_H_ */
