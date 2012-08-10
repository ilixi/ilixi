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

#ifndef ILIXI_IMAGE_H_
#define ILIXI_IMAGE_H_

#include <directfb.h>
#include <string>
#include <types/Size.h>

namespace ilixi
{
//! Static images.
/*!
 * This class is used to load images using DFBImageProvider interface. Surface data of all images is
 * permanently stored in system memory and it has premultiplied alpha flag set in its surface description.
 *
 * Note that images are loaded before accessing their surface using getDFBSurface() method for the first time.
 */
class Image
{
public:
    /*!
     * Creates an empty image so you can later set an image path.
     *
     * \sa setImagePath();
     */
    Image();

    /*!
     * Creates an image using given path.
     */
    Image(const std::string& path);

    /*!
     * Creates an image using given path where image surface is
     * constrained by width and height parameters.
     */
    Image(const std::string& path, int width, int height);

    /*!
     * Creates an image using given path where image surface is
     * constrained by size parameter.
     */
    Image(const std::string& path, const Size& size);

    /*!
     * Copy constructor.
     */
    Image(const Image& img);

    /*!
     * Destructor.
     */
    ~Image();

    /*!
     * Returns height of image.
     */
    int
    height() const;

    /*!
     * Returns width of image.
     */
    int
    width() const;

    /*!
     * Returns size of image.
     */
    Size
    size();

    /*!
     * Returns actual size of image.
     */
    Size
    preferredSize() const;

    /*!
     * Returns a pointer to DirectFB surface.
     */
    IDirectFBSurface*
    getDFBSurface();

    /*!
     * Returns image path.
     */
    std::string
    getImagePath() const;

    /*!
     * Sets image path.
     */
    void
    setImagePath(const std::string& path);

    /*!
     * Sets image size.
     * @param width
     * @param height
     */
    void
    setSize(int width, int height);

    /*!
     * Sets image size.
     * @param size
     */
    void
    setSize(const Size& size);

private:
    //! This property stores the pointer to DirectFB surface.
    IDirectFBSurface* _dfbSurface;
    //! This property stores the image path.
    std::string _imagePath;
    //! This property stores the size of the image.
    Size _size;

    /*!
     * Release surface and set it to NULL.
     */
    void
    invalidateSurface();

    /*!
     * Loads image. Returns true if successful.
     *
     * @param path image path.
     */
    bool
    loadImage();
};
}

#endif /* ILIXI_IMAGE_H_ */
