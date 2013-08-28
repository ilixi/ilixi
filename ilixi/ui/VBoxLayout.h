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

#ifndef ILIXI_VBOXLAYOUT_H_
#define ILIXI_VBOXLAYOUT_H_

#include <ui/LayoutBase.h>

namespace ilixi
{
//! Vertical box layout
/*!
 * In this layout widgets are aligned vertically on a single line.
 *
 * Widgets with ignored constraints on y axis are ignored by this layout.
 */
class VBoxLayout : public LayoutBase
{
public:
    /*!
     * Constructor.
     */
    VBoxLayout(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~VBoxLayout();

    /*!
     * Returns horizontal alignment of widgets inside layout.
     */
    Alignment::Horizontal
    geHorizontalAlignment() const;

    /*!
     * Sets horizontal alignment of widgets inside layout.
     */
    void
    setHorizontalAlignment(Alignment::Horizontal alignment);

    /*!
     * Inserts widget to layout at given row.
     */
    bool
    insertWidget(unsigned int row, Widget* widget);

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
     * Tiles widgets vertically on a single line.
     */
    void
    tile();

private:
    //! This property defines how widgets are placed on x axis.
    Alignment::Horizontal _alignment;
};

}

#endif /* ILIXI_VBOXLAYOUT_H_ */
