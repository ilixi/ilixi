/*
 Copyright 2011 Tarik Sekmen.

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

#ifndef ILIXI_BORDERBASE_H_
#define ILIXI_BORDERBASE_H_

#include "ui/Widget.h"

namespace ilixi
{
  //! Abstract base class for widgets with rectangular borders.
  class BorderBase : virtual public Widget
  {
  public:
    /*!
     * Constructor.
     *
     * @param parent
     */
    BorderBase(Widget* parent = 0);

    /*!
     * Copy constructor.
     */
    BorderBase(const BorderBase& borderbase);

    /*!
     * Destructor.
     */
    virtual
    ~BorderBase();

    /*!
     * Returns widget's border style.
     */
    WidgetBorderStyle
    borderStyle() const;

    /*!
     * Returns the border width of the widget.
     *
     * The width depends on border drawing style and is determined as follows:
     * - If border drawing style is NoBorder, 0 is returned.
     * - If border drawing style is StyledBorder, value is acquired from current Designer.
     * - Otherwise previously set borderWidth is returned.
     */
    int
    borderWidth() const;

    /*!
     * Returns the border width in horizontal direction while compensating for border radius.
     *
     * This method is used for determining an appropriate canvas X-coordinate where value of border radius would
     * affect the result of drawing operations.
     */
    int
    borderOffset() const;

    /*!
     * Sets widget's border style.
     *
     * @param style
     */
    void
    setBorderStyle(WidgetBorderStyle style);

    /*!
     * Set widget's border width.
     *
     * @param borderWidth
     */
    void
    setBorderWidth(int borderWidth);

  protected:
    //! This property holds widget's border drawing style.
    WidgetBorderStyle _borderStyle;

    //! This property holds widget's border width.
    int _borderWidth;
  };
}

#endif /* ILIXI_BORDERBASE_H_ */
