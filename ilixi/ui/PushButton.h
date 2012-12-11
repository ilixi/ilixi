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

#ifndef ILIXI_PUSHBUTTON_H_
#define ILIXI_PUSHBUTTON_H_

#include <ui/Button.h>

namespace ilixi
{

class PushButton : public Button
{
public:
    PushButton(const std::string& text = "Untitled", Widget* parent = 0);

    virtual
    ~PushButton();

    virtual Size
    preferredSize() const;

    PushButtonStyle
    getPushButtonStyle() const;

    void
    setPushButtonStyle(PushButtonStyle pbStyle);

private:
    //! This property is used to draw PushButton in three styles.
    PushButtonStyle _pbStyle;

    /*!
     * Just updates button.
     */
    virtual void
    focusInEvent();

    /*!
     * Just updates button.
     */
    virtual void
    focusOutEvent();

    /*!
     * Paints PushButton on its surface using current designer.
     * @param rect Clipped rectangle to paint.
     */
    void
    compose(const PaintEvent& event);

    /*!
     * Sets position of text and icon if any.
     */
    virtual void
    updateTextBaseGeometry();

};

}

#endif /* ILIXI_PUSHBUTTON_H_ */
