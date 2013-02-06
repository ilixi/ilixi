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

#ifndef ILIXI_GROUPBOX_H_
#define ILIXI_GROUPBOX_H_

#include <ui/Frame.h>
#include <ui/Label.h>
#include <ui/Icon.h>

namespace ilixi
{
//! A container widget with a frame and a title.
class GroupBox : public Frame
{
public:
    /*!
     * Constructor.
     * @param title for GroupBox.
     * @param parent widget if any.
     */
    GroupBox(std::string title = "", Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~GroupBox();

    /*!
     * Returns a height for given width.
     */
    virtual int
    heightForWidth(int width) const;

    /*!
     * Calculates the preferred size of contents and returns an updated size
     * using canvas, margins and borders.
     */
    virtual Size
    preferredSize() const;

    /*!
     * Returns title.
     *
     * @return title
     */
    std::string
    title() const;

    /*!
     * Returns size of title tab.
     */
    Size
    titleSize() const;

    /*!
     * Sets title.
     */
    void
    setTitle(std::string title);

    /*!
     * Sets icon shown before title.
     */
    void
    setTitleIcon(StyleHint::PackedIcon icon);

protected:
    //! This image stores the title icon.
    Icon* _titleIcon;
    //! This property stores GroupBox's title.
    Label* _title;
    //! This property stores the size of title tab.
    Size _titleSize;

    /*!
     * Draws groupbox.
     */
    virtual void
    compose(const PaintEvent& event);

    /*!
     * Returns canvas y-coordinate including the top margin and title.
     */
    virtual int
    canvasY() const;

    /*!
     * Returns canvas height excluding margins and title.
     */
    virtual int
    canvasHeight() const;

private:
    virtual void
    updateLayoutGeometry();

    void
    updateTitleSize();
};

}

#endif /* ILIXI_GROUPBOX_H_ */
