/*
 Copyright 2010-2015 Tarik Sekmen.

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

#ifndef ILIXI_APPWINDOW_H_
#define ILIXI_APPWINDOW_H_

#include <ui/WindowWidget.h>

namespace ilixi
{

class Application;

//! Provides a main window for an application.
/*!
 * A main window is automatically instantiated during construction of an application to act as a container for widgets.
 * It can also have a ToolBar to add a set of controls.
 *
 * Main window draws a background image by default; but you can also use a custom method, see setCustomCompose().
 */
class AppWindow : public WindowWidget
{
public:
    /*!
     * Constructor.
     */
    AppWindow(Application* owner);

    /*!
     * Destructor.
     */
    virtual
    ~AppWindow();

    /*!
     * Returns background image.
     */
    Image*
    background() const;

    /*!
     * Returns whether background image is tiled.
     */
    bool
    backgroundTiled() const;

    /*!
     * Returns frame's canvas x-coordinate including the left margin.
     */
    virtual int
    canvasX() const;

    /*!
     * Returns frame's canvas y-coordinate including the top margin.
     */
    virtual int
    canvasY() const;

    /*!
     * Returns frame's canvas height excluding margins.
     */
    virtual int
    canvasHeight() const;

    /*!
     * Returns frame's canvas width excluding margins.
     */
    virtual int
    canvasWidth() const;

    /*!
     * Returns current toolbar if any.
     */
    const ToolBar*
    toolbar() const;

    /*!
     * Sets a background image.
     *
     * @param imagePath pointing to image file.
     * @param tile If true image is tiled.
     */
    void
    setBackgroundImage(const std::string& imagePath, bool tile = false);

    /*!
     * Adds a toolbar to application.
     *
     * @param toolbar pointer to ToolBar widget.
     * @param positionNorth if true places toolbar at top, else at bottom.
     */
    bool
    setToolbar(ToolBar* toolbar, bool positionNorth = true);

    /*!
     * This method is used to bypass default compose method and instead use Application::compose().
     */
    void
    setCustomCompose(bool useCustomCompose);

protected:
    /*!
     * Updates layout and toolbar geometry
     */
    virtual void
    updateLayoutGeometry();

    /*!
     * This method is used if application has backgroundFilled set to true.
     * Default implementation uses a Stylist to tile the background image or
     * fill background with a colour from palette.
     *
     * \sa setBackgroundFilled()
     */
    virtual void
    compose(const PaintEvent& event);

private:
    //! Background image of application.
    Image* _backgroundImage;
    //! This property stores whether background image is tiled or not.
    bool _backgroundImageTiled;
    //! AppWindow toolbar if any.
    ToolBar* _toolbar;
    //! This property stores toolbar position.
    bool _toolbarNorth;
    //! This flag specifies if window should use a custom compose method.
    bool _customCompose;
    Application* _owner;

    friend class Application;

};
}
#endif /* ILIXI_APPWINDOW_H_ */
