/*
 Copyright 2010-2012 Tarik Sekmen.

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

#ifndef ILIXI_TOOLBUTTON_H_
#define ILIXI_TOOLBUTTON_H_

#include <ui/Button.h>

namespace ilixi
{
//! A tool button with text label and icon.
class ToolButton : public Button
{
public:
    /*!
     * This enum controls toolbutton's drawing style.
     */
    enum ToolButtonStyle
    {
        IconOnly, //!< Only icon is drawn.
        TextOnly, //!< Only text is drawn.
        IconBeforeText, //!< Icon is placed before text.
        IconBelowText, //!< Icon is placed below text and centered.
        IconAboveText
    //!< Icon is placed above text and centered.
    };

    /*!
     * Constructor
     * @param text
     * @param parent
     */
    ToolButton(std::string text, Widget* parent = 0);

    /*!
     * Destructor
     */
    virtual
    ~ToolButton();

    Size
    preferredSize() const;

    /*!
     * Returns button icon, if any.
     */
    const Icon*
    icon() const;

    /*!
     * Returns tool button's style.
     */
    ToolButtonStyle
    toolButtonStyle() const;

    /*!
     * Set tool button's style.
     * @param style
     */
    void
    setToolButtonStyle(ToolButtonStyle style);

    /*!
     * Sets button icon.
     */
    void
    setIcon(Icon* icon);

    /*!
     * Sets button icon by automatically creating an icon.
     *
     * @param path to an image file.
     * @param size in pixels.
     */
    void
    setIcon(const std::string& path, const Size& size = Size(48, 48));

    /*!
     * Sets the size of button icon.
     */
    void
    setIconSize(const Size& size);

private:
    //    ! This property holds tool button's style.
    ToolButtonStyle _toolButtonStyle;
    //! This property holds button's icon.
    Icon* _icon;

    /*!
     * Paints ToolButton on its surface using current designer.
     * @param rect Clipped rectangle to paint.
     */
    void
    compose(const PaintEvent& event);

    virtual void
    updateTextBaseGeometry();
};
}

#endif /* ILIXI_TOOLBUTTON_H_ */
