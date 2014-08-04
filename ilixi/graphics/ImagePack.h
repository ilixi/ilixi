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

#ifndef ILIXI_IMAGEPACK_H_
#define ILIXI_IMAGEPACK_H_

#include <types/Image.h>
#include <map>
#include <iostream>

namespace ilixi
{
//! Simplifies using sub-images for UI elements
/*!
 * An image pack is useful if an application has lots of graphical elements composed of images.
 * You can group such related images in an image pack as it is usually faster to load a
 * big image instead of loading lots of smaller images.
 *
 * You can access an image as follows:
 * \code
 * ImagePack* imagePack = PlatformManager::instance().getImagePack("my-image-pack");
 * imagePack->getImage("custom-button");
 * \endcode
 */
class ImagePack
{
public:
    /*!
     * Constructor.
     */
    ImagePack(const char* file);

    /*!
     * Destructor.
     */
    virtual
    ~ImagePack();

    /*!
     * If name is found returns a new image, else returns NULL.
     */
    Image*
    getImage(const std::string& name) const;

    /*!
     * Initialise image pack from an XML file.
     *
     * Returns false on error.
     */
    bool
    parsePack(const char* file);

private:
    //! This image stores the pack.
    Image* _pack;

    typedef std::map<std::string, Rectangle> ImageMap;
    //! This map stores position and dimensions of sub-images in pack.
    ImageMap _map;

    //! Release pack image.
    void
    release();

    friend std::istream&
    operator>>(std::istream& is, ImagePack& obj);

    friend std::ostream&
    operator<<(std::ostream& os, const ImagePack& obj);
};

} /* namespace ilixi */
#endif /* ILIXI_IMAGEPACK_H_ */
