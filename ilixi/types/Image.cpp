/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#include "types/Image.h"
#include "core/AppBase.h"
#include "core/Logger.h"
#include "graphics/Stylist.h"

using namespace ilixi;

Image::Image() :
    _dfbSurface(NULL), _imagePath(""), _size()
{
}

Image::Image(const std::string& path) :
    _dfbSurface(NULL), _imagePath(path), _size()
{
}

Image::Image(const std::string& path, int width, int height) :
    _dfbSurface(NULL), _imagePath(path), _size(width, height)
{

}

Image::Image(const std::string& path, const Size& size) :
    _dfbSurface(NULL), _imagePath(path), _size(size)
{
}

Image::Image(const Image& img) :
    _dfbSurface(NULL), _imagePath(img._imagePath), _size(img._size)
{
}

Image::~Image()
{
  invalidateSurface();
  ILOG_DEBUG(ILX_IMAGE, "~Image %p\n", this);
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
Image::size() const
{
  return _size;
}

IDirectFBSurface*
Image::getDFBSurface()
{
  if (_dfbSurface)
    return _dfbSurface;
  else if (loadImage())
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
    }
}

bool
Image::loadImage()
{
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
      _imagePath.c_str(), &provider) != DFB_OK)
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
      ILOG_ERROR( ILX_IMAGE,
          "Cannot create surface for %s - %s\n", _imagePath.c_str(), DirectFBErrorString(ret));
      return false;
    }
  else
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
