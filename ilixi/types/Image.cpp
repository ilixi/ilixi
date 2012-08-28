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

#include <types/Image.h>
#include <core/AppBase.h>
#include <core/Logger.h>
#include <graphics/Stylist.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_IMAGE, "ilixi/types/Image", "Image");

Image::Image()
        : _dfbSurface(NULL),
          _imagePath(""),
          _size()
{
    ILOG_TRACE(ILX_IMAGE);
}

Image::Image(const std::string& path)
        : _dfbSurface(NULL),
          _imagePath(path),
          _size()
{
    ILOG_TRACE(ILX_IMAGE);
}

Image::Image(const std::string& path, int width, int height)
        : _dfbSurface(NULL),
          _imagePath(path),
          _size(width, height)
{
    ILOG_TRACE(ILX_IMAGE);
}

Image::Image(const std::string& path, const Size& size)
        : _dfbSurface(NULL),
          _imagePath(path),
          _size(size)
{
    ILOG_TRACE(ILX_IMAGE);
}

Image::Image(const Image& img)
        : _dfbSurface(NULL),
          _imagePath(img._imagePath),
          _size(img._size)
{
    ILOG_TRACE(ILX_IMAGE);
}

Image::~Image()
{
    ILOG_TRACE(ILX_IMAGE);
    invalidateSurface();
}

int
Image::height() const
{
    return _size.height();
}

int
Image::width() const
{
    return _size.width();
}

Size
Image::size()
{
    if (_size.isValid())
        return _size;
    else if (loadImage())
    {
        int w, h;
        _dfbSurface->GetSize(_dfbSurface, &w, &h);
        return Size(w, h);
    } else
        return _size;

}

IDirectFBSurface*
Image::getDFBSurface()
{
    if (loadImage())
        return _dfbSurface;
    else
        return Stylist::_noImage->getDFBSurface();
}

std::string
Image::getImagePath() const
{
    return _imagePath;
}

void
Image::setImagePath(const std::string& path)
{
    if (path != _imagePath)
    {
        invalidateSurface();
        _imagePath = path;
    }
}

void
Image::setSize(int width, int height)
{
    setSize(Size(width, height));
}

void
Image::setSize(const Size& s)
{
    if (s.isValid() && s != _size)
    {
        invalidateSurface();
        _size = s;
    }
}

void
Image::invalidateSurface()
{
    if (_dfbSurface)
    {
        _dfbSurface->ReleaseSource(_dfbSurface);
        _dfbSurface->Release(_dfbSurface);
        _dfbSurface = NULL;
        ILOG_TRACE(ILX_IMAGE);
    }
}

bool
Image::loadImage()
{
    if (_dfbSurface)
        return true;

    if (_imagePath == "")
    {
        ILOG_ERROR(ILX_IMAGE, "Image path is empty");
        return false;
    }

    if (access(_imagePath.c_str(), F_OK) != 0)
    {
        ILOG_ERROR( ILX_IMAGE,
                   "File (%s) is not accessible!\n", _imagePath.c_str());
        return false;
    }

    invalidateSurface();

    DFBSurfaceDescription desc;

    IDirectFBImageProvider* provider;
    if (AppBase::getDFB()->CreateImageProvider(AppBase::getDFB(),
                                               _imagePath.c_str(), &provider)
            != DFB_OK)
    {
        invalidateSurface();
        ILOG_ERROR(ILX_IMAGE, "Cannot create image provider!\n");
        return false;
    }

    if (provider->GetSurfaceDescription(provider, &desc) != DFB_OK)
        ILOG_ERROR(ILX_IMAGE, "Cannot get surface description!\n");

    desc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_CAPS | DSDESC_WIDTH
            | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
    desc.caps = DSCAPS_PREMULTIPLIED;
    desc.pixelformat = DSPF_ARGB;

    if (width() > 0)
        desc.width = width();

    if (height() > 0)
        desc.height = height();

    DFBResult ret = AppBase::getDFB()->CreateSurface(AppBase::getDFB(), &desc,
                                                     &_dfbSurface);

    if (ret)
    {
        invalidateSurface();
        ILOG_ERROR(
                ILX_IMAGE,
                "Cannot create surface for %s - %s\n", _imagePath.c_str(), DirectFBErrorString(ret));
        return false;
    } else
    {
        if (provider->RenderTo(provider, _dfbSurface, NULL) != DFB_OK)
        {
            invalidateSurface();
            ILOG_ERROR(ILX_IMAGE, "Cannot render image to surface!\n");
            return false;
        }
        provider->Release(provider);
        ILOG_DEBUG(ILX_IMAGE, "Image [%s] is loaded.\n", _imagePath.c_str());
        return true;
    }
}

} /* namespace ilixi */
