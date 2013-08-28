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

#ifndef ILIXI_ICON_H_
#define ILIXI_ICON_H_

#include <ui/Widget.h>
#include <types/Image.h>

namespace ilixi
{
//! A widget for displaying static images.
/*!
 * Icon is used for rendering images especially with different states.
 *
 * Source for an icon can be a file or a packed icon. Note that original
 * image will be scaled if icon has a different size.
 */
class Icon : public Widget
{
public:
    /*!
     * Constructor creates a new icon with no image.
     *
     * @param parent widget.
     */
    Icon(Widget* parent = 0);

    /*!
     * Constructor creates a new icon using given path.
     *
     * @param path to an image file.
     * @param parent widget.
     */
    Icon(const std::string& path, Widget* parent = 0);

    /*!
     * Constructor creates a new icon using given image.
     *
     * @param image will be copied.
     * @param parent widget.
     */
    Icon(const Image& image, Widget* parent = 0);

    /*!
     * Constructor creates a new icon using given packed icon.
     *
     * @param packedIcon an enum specifying a default icon.
     * @param parent widget.
     */
    Icon(StyleHint::PackedIcon packedIcon, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~Icon();

    /*!
     * Returns image size.
     */
    virtual Size
    preferredSize() const;

    /*!
     * Returns whether image is colorized using default text color.
     */
    bool
    colorize() const;

    /*!
     * Returns image.
     */
    Image* const
    image();

    /*!
     * Sets whether image is colorized using default text color.
     */
    void
    setColorize(bool colorize);

    /*!
     * Sets image using given path.
     *
     * @param path to an image file.
     */
    void
    setImage(const std::string& path);

    /*!
     * Sets the state of icon.
     */
    void
    setState(WidgetState state);

private:
    //! This property specifies whether icon is colorized before painted by stylist.
    bool _colorize;
    //! Image to show if icon is in default state.
    Image* _image;

    //! Draws icon using Stylist.
    virtual void
    compose(const PaintEvent& event);
};

}

#endif /* ILIXI_ICON_H_ */
