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

#ifndef ILIXI_TOOLBAR_H_
#define ILIXI_TOOLBAR_H_

#include <ui/Frame.h>
#include <ui/Button.h>

namespace ilixi
{

//! A button for ToolBar.
/*!
 * Use this class if you need to place buttons on a ToolBar.
 */
class ToolBarButton : public Button
{
public:
    /*!
     * Constructor.
     *
     * @param text button text if any
     * @param parent
     */
    ToolBarButton(const std::string& text = "", Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~ToolBarButton();

    Size
    preferredSize() const;

    /*!
     * Sets button icon using an image.
     *
     * @param path to an image file.
     */
    void
    setIcon(const std::string& path);

    /*!
     * Sets icon using stock icons.
     */
    void
    setIcon(StyleHint::PackedIcon icon);

protected:
    //! This property holds button's icon.
    Icon* _icon;

    /*!
     * Paints ToolBarButton.
     */
    virtual void
    compose(const PaintEvent& event);

private:
    virtual void
    updateTextBaseGeometry();
};

//! A horizontal bar which can contain buttons and/or other widgets.
/*!
 * A ToolBar can be set using Applicaton::setToolBar() method.
 */
class ToolBar : public Frame
{
public:
    /*!
     * Constructor.
     */
    ToolBar(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~ToolBar();

    Size
    preferredSize() const;

protected:
    virtual void
    compose(const PaintEvent& event);
};

} /* namespace ilixi */
#endif /* ILIXI_TOOLBAR_H_ */
