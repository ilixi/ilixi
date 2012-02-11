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

#ifndef ILIXI_BOXLAYOUT_H_
#define ILIXI_BOXLAYOUT_H_

#include "types/WidgetLayout.h"

namespace ilixi
{

  class BoxLayout : public WidgetLayout
  {
  public:
    BoxLayout(Widget* parent = NULL);

    virtual
    ~BoxLayout();

    virtual int
    heightForWidth(int width);

    virtual Size
    preferredSize();

    Alignment::Vertical
    getVerticalAlignment() const;

    void
    setVerticalAlignment(Alignment::Vertical alignment);

    virtual void
    tile();

  private:
    //! This property defines how widgets are placed on y axis.
    Alignment::Vertical _alignment;
  };

}

#endif /* ILIXI_BOXLAYOUT_H_ */
