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

#ifndef ILIXI_SPACER_H_
#define ILIXI_SPACER_H_

#include <ui/Widget.h>

namespace ilixi
{
//! An empty widget which extends in one direction, Horizontal or Vertical.
/*!
 * You can use spacers to align widgets inside Horizontal or Vertical box layouts.
 */
class Spacer : public Widget
{
public:
    /*!
     * Constructor.
     * @param orientation
     * @param parent
     */
    Spacer(Orientation orientation = Horizontal, Widget* parent = 0);

    /*!
     * Destructor.
     */
    ~Spacer();

    virtual Size
    preferredSize() const;

    /*!
     * Sets orientation of spacer.
     */
    void
    setOrientation(Orientation orientation);

    /*!
     * Overloaded.
     *
     * Spacer will not allocate any surface resources or paint its children.
     */
    virtual void
    paint(const PaintEvent& event);

protected:
    virtual void
    compose(const PaintEvent& event);

private:
    //! This property stores orientation of spacer.
    Orientation _orientation;
};

}

#endif /* ILIXI_SPACER_H_ */
