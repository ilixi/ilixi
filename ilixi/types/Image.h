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

#ifndef ILIXI_IMAGE_H_
#define ILIXI_IMAGE_H_

#include <directfb.h>
#include <string>
#include <types/Rectangle.h>

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
     * Creates a sub-image using the given image and sourceRect.
     *
     * SourceRect specifies a bounding box, content is copied
     * to create a new image. If sourceRect is null then whole image is used.
     */
    Image(Image* source, const Rectangle& sourceRect);

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
     * Returns actual size of image or an invalid size if image cannot be loaded.
     */
    Size
    preferredSize();

    /*!
     * Returns image description's capabilities.
     */
    DFBImageCapabilities
    getCaps();

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
     *
     * \warning This method does not have any effect if image is a sub-image.
     */
    void
    setImagePath(const std::string& path);

    /*!
     * Sets image size.
     *
     * @param width
     * @param height
     *
     * \warning This method does not have any effect if image is a sub-image.
     */
    void
    setSize(int width, int height);

    /*!
     * Sets image size.
     *
     * @param size
     *
     * \warning This method does not have any effect if image is a sub-image.
     */
    void
    setSize(const Size& size);

private:
    enum ImageFlags
    {
        Initialised = 0x0001,
        Modified = 0x0002,
        NotAvailable = 0x0004,
        Ready = 0x0008,
        SubImage = 0x0010
    };

    //! This property stores the pointer to DirectFB surface.
    IDirectFBSurface* _dfbSurface;
    //! This property stores the image path.
    std::string _imagePath;
    //! This property stores the size of the image.
    Size _size;
    //! This property stores the state of image.
    ImageFlags _state;
    //! This property stores the image capabilities acquired from image provider.
    DFBImageCapabilities _caps;

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

    bool
    loadSubImage(Image* source, const Rectangle& sourceRect);

    friend std::istream&
    operator>>(std::istream& is, Image& obj);

    friend std::ostream&
    operator<<(std::ostream& os, const Image& obj);
};
}

#endif /* ILIXI_IMAGE_H_ */
