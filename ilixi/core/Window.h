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

#ifndef ILIXI_WINDOW_H_
#define ILIXI_WINDOW_H_

#include <types/Size.h>
#include <types/Point.h>

namespace ilixi
{
//! Creates a DirectFB window.
/*!
 * Each application has at least one window and this class creates and maintains a list of DirectFB windows.
 * Windows also store a list of child widgets using UIManager which helps to transfer input events.
 */
class Window
{
    friend class Application;
    friend class WindowWidget;
public:

    /*!
     * Creates a window with its own event manager.
     */
    Window();

    /*!
     * Destructor.
     */
    virtual
    ~Window();

    /*!
     * Returns the unique DirectFB window ID.
     */
    DFBWindowID
    windowID() const;

    /*!
     * Returns DirectFB window interface.
     */
    IDirectFBSurface*
    dfbSurface() const;

    /*!
     * Returns the size of window.
     */
    Size
    windowSize() const;

    /*!
     * Returns the position of window.
     */
    Point
    windowPosition() const;

    /*!
     *  Makes the window visible and starts input event thread.
     */
    void
    showWindow();

    /*!
     * Hides the window and terminates input event thread.
     */
    void
    hideWindow();

protected:
    //! This property stores an interface to DirectFB window.
    IDirectFBWindow* _dfbWindow;
    //! This property stores an interface to window's surface.
    IDirectFBSurface* _windowSurface;

    /*!
     * Creates a DFBWindow and acquires an interface to its surface.
     */
    bool
    initDFBWindow(const Size& size);

    /*!
     * Release DirectFB window interfaces.
     */
    void
    releaseDFBWindow();
};

}

#endif /* ILIXI_WINDOW_H_ */
