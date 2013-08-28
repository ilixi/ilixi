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

#ifndef ILIXI_HBOXLAYOUT_H_
#define ILIXI_HBOXLAYOUT_H_

#include <ui/LayoutBase.h>

namespace ilixi
{
//! Horizontal box layout.
/*!
 * In this layout widgets are aligned horizontally on a single line.
 *
 * Widgets with ignored constraints on x axis are ignored by this layout.
 */
class HBoxLayout : public LayoutBase
{
public:
    /*!
     * Constructor.
     */
    HBoxLayout(Widget* parent = 0);

    /*!
     * Destructor.
     */
    ~HBoxLayout();

    /*!
     * Returns vertical alignment of widgets inside layout.
     */
    Alignment::Vertical
    getVerticalAlignment() const;

    /*!
     * Sets vertical alignment of widgets inside layout.
     */
    void
    setVerticalAlignment(Alignment::Vertical alignment);

    /*!
     * Inserts widget to layout at given column.
     */
    bool
    insertWidget(unsigned int column, Widget* widget);

    /*!
     * Calculates total height for given width.
     */
    virtual int
    heightForWidth(int width) const;

    /*!
     * Calculates optimal dimension of this layout.
     */
    virtual Size
    preferredSize() const;

    /*!
     * Tiles widgets horizontally on a single line.
     */
    void
    tile();

private:
    //! This property defines how widgets are placed on y axis.
    Alignment::Vertical _alignment;
};
}

#endif /* ILIXI_HBOXLAYOUT_H_ */
