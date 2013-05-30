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

#ifndef ILIXI_LINESEPERATOR_H_
#define ILIXI_LINESEPERATOR_H_

#include <ui/Widget.h>

namespace ilixi
{
//! A separator widget which draws a line in vertical or horizontal direction.
class LineSeperator : public ilixi::Widget
{
public:
    /*!
     * Constructor
     *
     * @param orientation specifies orientation of line
     * @param parent
     */
    LineSeperator(Orientation orientation = Horizontal, Widget* parent = 0);

    /*!
     * Destructor
     */
    virtual
    ~LineSeperator();

    virtual Size
    preferredSize() const;

    /*!
     * Returns orientation of line seperator.
     */
    Orientation
    orientation() const;

    /*!
     * Sets orientation of line seperator.
     */
    void
    setOrientation(Orientation orientation);

protected:
    virtual void
    compose(const ilixi::PaintEvent& event);

private:
    //! This property stores orientation of button group.
    Orientation _orientation;
};

} /* namespace ilixi */
#endif /* ILIXI_LINESEPERATOR_H_ */
