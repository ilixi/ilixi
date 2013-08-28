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

#include <types/Image.h>
#include <core/PlatformManager.h>
#include <core/Logger.h>
#include <graphics/Stylist.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_IMAGE, "ilixi/types/Image", "Image");

Image::Image()
        : _dfbSurface(NULL),
          _imagePath(""),
          _size(),
          _state(Initialised),
          _caps(DICAPS_NONE)
{
    ILOG_TRACE(ILX_IMAGE);
}

Image::Image(const std::string& path)
        : _dfbSurface(NULL),
          _imagePath(path),
          _size(),
          _state(Initialised),
          _caps(DICAPS_NONE)
{
    ILOG_TRACE(ILX_IMAGE);
    ILOG_DEBUG(ILX_IMAGE, " -> path: %s\n", _imagePath.c_str());
}

Image::Image(const std::string& path, int width, int height)
        : _dfbSurface(NULL),
          _imagePath(path),
          _size(width, height),
          _state(Initialised),
          _caps(DICAPS_NONE)
{
    ILOG_TRACE(ILX_IMAGE);
    ILOG_DEBUG(ILX_IMAGE, " -> path: %s - width: %d height: %d\n", _imagePath.c_str(), _size.width(), _size.height());
}

Image::Image(const std::string& path, const Size& size)
        : _dfbSurface(NULL),
          _imagePath(path),
          _size(size),
          _state(Initialised),
          _caps(DICAPS_NONE)
{
    ILOG_TRACE(ILX_IMAGE);
    ILOG_DEBUG(ILX_IMAGE, " -> path: %s - size: %d, %d\n", _imagePath.c_str(), _size.width(), _size.height());
}

Image::Image(Image* source, const Rectangle& sourceRect)
        : _dfbSurface(NULL),
          _imagePath(""),
          _size(sourceRect.size()),
          _state((ImageFlags) (Initialised | SubImage)),
          _caps(source->_caps)
{
    ILOG_TRACE(ILX_IMAGE);
    ILOG_DEBUG(ILX_IMAGE, " -> SubImage of %p - size: %d, %d\n", source, _size.width(), _size.height());
    ILOG_DEBUG(ILX_IMAGE, " -> Rect: %d, %d, %d, %d\n", sourceRect.x(), sourceRect.y(), sourceRect.width(), sourceRect.height());
    loadSubImage(source, sourceRect);
}

Image::Image(const Image& img)
        : _dfbSurface(NULL),
          _imagePath(img._imagePath),
          _size(img._size),
          _state(_state),
          _caps(img._caps)
{
    ILOG_TRACE(ILX_IMAGE);
    if (_state & SubImage)
    {
        _dfbSurface = img._dfbSurface;
        _dfbSurface->AddRef(_dfbSurface);
    }
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
    ILOG_TRACE(ILX_IMAGE);
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

Size
Image::preferredSize()
{
    ILOG_TRACE(ILX_IMAGE);
    if (loadImage())
    {
        int w, h;
        _dfbSurface->GetSize(_dfbSurface, &w, &h);
        return Size(w, h);
    }
    return Size();
}

IDirectFBSurface*
Image::getDFBSurface()
{
    ILOG_TRACE(ILX_IMAGE);
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
    ILOG_TRACE(ILX_IMAGE);
    if (path != _imagePath)
    {
        ILOG_DEBUG(ILX_IMAGE, " -> Path: %s\n", path.c_str());
        _imagePath = path;
        _state = Initialised;
        invalidateSurface();
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
    if (_state & SubImage)
        return;

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
        ILOG_TRACE(ILX_IMAGE);
        _dfbSurface->Release(_dfbSurface);
        _dfbSurface = NULL;
        if (_state & SubImage)
            _state = (ImageFlags) (Initialised | SubImage);
        else
            _state = Initialised;
        ILOG_DEBUG(ILX_IMAGE, " -> State: %x\n", _state);
    }
}

bool
Image::loadImage()
{
    ILOG_TRACE(ILX_IMAGE);
    if (_dfbSurface)
        return true;

    if ((_state & NotAvailable) || (_state & SubImage))
    {
        ILOG_DEBUG(ILX_IMAGE, " -> NotAvailable or SubImage.\n");
        return false;
    }

    if (_imagePath == "")
    {
        ILOG_ERROR(ILX_IMAGE, "Image path is empty!\n");
        _state = (ImageFlags) (_state | NotAvailable);
        return false;
    }

    ILOG_DEBUG(ILX_IMAGE, " -> Loading image: %s\n", _imagePath.c_str());
    DFBSurfaceDescription desc;

    IDirectFBImageProvider* provider;
    DFBResult ret = PlatformManager::instance().getDFB()->CreateImageProvider(PlatformManager::instance().getDFB(), _imagePath.c_str(), &provider);
    if (ret)
    {
        ILOG_ERROR(ILX_IMAGE, "Cannot create image provider! %s\n", DirectFBErrorString(ret));
        _state = (ImageFlags) (_state | NotAvailable);
        return false;
    }

    invalidateSurface();

    if (provider->GetSurfaceDescription(provider, &desc) != DFB_OK)
        ILOG_ERROR(ILX_IMAGE, "Cannot get surface description!\n");

    DFBImageDescription iDesc;
    if (provider->GetImageDescription(provider, &iDesc) == DFB_OK)
        _caps = iDesc.caps;

    if (PlatformManager::instance().forcedPixelFormat() != DSPF_UNKNOWN)
    {
        desc.flags = (DFBSurfaceDescriptionFlags) (desc.flags | DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
        desc.pixelformat = PlatformManager::instance().forcedPixelFormat();
    } else
        desc.flags = (DFBSurfaceDescriptionFlags) (desc.flags | DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT);
    desc.caps = DSCAPS_PREMULTIPLIED;

    if (width() > 0)
        desc.width = width();

    if (height() > 0)
        desc.height = height();

    ret = PlatformManager::instance().getDFB()->CreateSurface(PlatformManager::instance().getDFB(), &desc, &_dfbSurface);

    if (ret != DFB_OK)
    {
        invalidateSurface();
        ILOG_ERROR( ILX_IMAGE, "Cannot create surface for %s - %s\n", _imagePath.c_str(), DirectFBErrorString(ret));
        _state = (ImageFlags) (_state | NotAvailable);
        return false;
    } else
    {
        ret = provider->RenderTo(provider, _dfbSurface, NULL);
        if (ret != DFB_OK)
        {
            invalidateSurface();
            ILOG_ERROR(ILX_IMAGE, "Cannot render image to surface! %s\n", DirectFBErrorString(ret));
            _state = (ImageFlags) (_state | NotAvailable);
            return false;
        }
        provider->Release(provider);
        ILOG_DEBUG(ILX_IMAGE, " -> Image is loaded.\n");
        _state = (ImageFlags) (_state | Ready);
        return true;
    }
}

DFBImageCapabilities
Image::getCaps()
{
    ILOG_TRACE(ILX_IMAGE);
    if (!_dfbSurface)
        loadImage();
    return _caps;
}

bool
Image::loadSubImage(Image* source, const Rectangle& sourceRect)
{
    ILOG_TRACE(ILX_IMAGE);

    IDirectFBSurface* src = source->getDFBSurface();
    DFBRectangle srcRect = sourceRect.dfbRect();
    if (src->GetSubSurface(src, &srcRect, &_dfbSurface) != DFB_OK)
    {
        _state = (ImageFlags) (_state | NotAvailable);
        return false;
    }

    _caps = source->getCaps();
    _state = (ImageFlags) (_state | Ready);
    return true;
}

std::istream&
operator>>(std::istream& is, Image& obj)
{
    obj.invalidateSurface();
    std::string name;
    int w, h;
    std::getline(is, name, ',');
    is >> w;
    is.ignore(1);
    is >> h;
    obj._imagePath = name;
    obj._size = Size(w, h);
    return is;
}

std::ostream&
operator<<(std::ostream& os, const Image& obj)
{
    return os << obj._imagePath << ", " << obj._size.width() << ',' << obj._size.height();
}

} /* namespace ilixi */
