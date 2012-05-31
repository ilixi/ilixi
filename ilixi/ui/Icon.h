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

#ifndef ILIXI_ICON_H_
#define ILIXI_ICON_H_

#include "ui/Widget.h"
#include "types/Image.h"

namespace ilixi
{

  class Icon : public Widget
  {
  public:
    Icon(Widget* parent = 0);

    Icon(const std::string& path, Widget* parent = 0);

    Icon(const Image& image, Widget* parent = 0);

    virtual
    ~Icon();

    virtual Size
    preferredSize() const;

    Image*
    image();

    Image*
    image(WidgetState state);

    void
    setDefaultImage(const std::string& path);

    void
    setDisabledImage(const std::string& path);

    void
    setExposedImage(const std::string& path);

    void
    setFocusedImage(const std::string& path);

  private:
    //! Image to show if icon is in default state.
    Image* _default;
    //! Image to show if icon is in disabled state.
    Image* _disabled;
    //! Image to show if icon is in focused state.
    Image* _focused;
    //! Image to show if icon is in exposed state.
    Image* _exposed;

    void
    updateImageSize();

    void
    compose(const PaintEvent& event);

  };

}

#endif /* ILIXI_ICON_H_ */
