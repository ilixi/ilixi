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

#include "graphics/Surface.h"
#include "core/AppBase.h"
#include "core/Logger.h"

using namespace ilixi;

D_DEBUG_DOMAIN( ILX_SURFACE, "ilixi/graphics/Surface", "Surface");

Surface::Surface() :
    _dfbSurface(NULL), _parentSurface(NULL)
{
  pthread_mutex_init(&_surfaceLock, NULL);
}

Surface::~Surface()
{
  release();
  pthread_mutex_destroy(&_surfaceLock);
}

bool
Surface::createDFBSurface(int width, int height)
{
  release();
  DFBSurfaceDescription desc;
  desc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_WIDTH | DSDESC_HEIGHT
      | DSDESC_PIXELFORMAT | DSDESC_CAPS);
  desc.width = width;
  desc.height = height;
  desc.pixelformat = DSPF_ARGB;
  desc.caps = DSCAPS_DOUBLE;
  desc.hints = DSHF_FONT;
  DFBResult ret = AppBase::getDFB()->CreateSurface(AppBase::getDFB(), &desc,
      &_dfbSurface);
  if (ret)
    {
      ILOG_ERROR(
          ILX_SURFACE, "Cannot create surface: %s", DirectFBErrorString(ret));
      return false;
    }
  _dfbSurface->SetBlittingFlags(_dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);
  return true;
}

bool
Surface::createDFBSubSurface(const Rectangle& geometry,
    IDirectFBSurface* parent)
{
  release();
  DFBRectangle r = geometry.dfbRect();
  _parentSurface = parent;
  DFBResult ret = _parentSurface->GetSubSurface(_parentSurface, &r,
      &_dfbSurface);
  if (ret)
    {
      ILOG_ERROR(
          ILX_SURFACE, "Cannot get sub-surface: %s", DirectFBErrorString(ret));
      return false;
    }
  _dfbSurface->SetBlittingFlags(_dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);
  return true;
}

IDirectFBSurface*
Surface::DFBSurface()
{
  return _dfbSurface;
}

DFBSurfaceID
Surface::DFBSurfaceId() const
{
  unsigned int id = 0;
  if (_dfbSurface)
    _dfbSurface->GetID(_dfbSurface, &id);
  else
    ILOG_ERROR(ILX_SURFACE, "Cannot get surface id!");
  return id;
}

void
Surface::setGeometry(const Rectangle& geometry)
{
  setGeometry(geometry.x(), geometry.y(), geometry.width(), geometry.height());
}

void
Surface::setGeometry(int x, int y, int width, int height)
{
  DFBRectangle r =
    { x, y, width, height };
  if (_parentSurface)
    {
      DFBResult ret = _dfbSurface->MakeSubSurface(_dfbSurface, _parentSurface,
          &r);
      if (ret)
        ILOG_ERROR(
            ILX_SURFACE, "Cannot set geometry: %s", DirectFBErrorString(ret));
    }
  else
    ILOG_ERROR(ILX_SURFACE, "No Parent Surface, need to create surface again!");
}

void
Surface::flip()
{
  DFBResult ret = _dfbSurface->Flip(_dfbSurface, NULL, DSFLIP_FLUSH);
  if (ret)
    ILOG_ERROR(ILX_SURFACE, "Flip error: %s", DirectFBErrorString(ret));
}

void
Surface::flip(const Rectangle& rect)
{
  DFBRegion r = rect.dfbRegion();
  DFBResult ret = _dfbSurface->Flip(_dfbSurface, &r, DSFLIP_FLUSH);
  if (ret)
    ILOG_ERROR(ILX_SURFACE, "Flip error: %s", DirectFBErrorString(ret));
}

void
Surface::lock()
{
  int rc = pthread_mutex_lock(&_surfaceLock);
  if (rc != 0)
    ILOG_ERROR(ILX_SURFACE, "Error while locking surface!");
}

void
Surface::unlock()
{
  int rc = pthread_mutex_unlock(&_surfaceLock);
  if (rc != 0)
    ILOG_ERROR(ILX_SURFACE, "Error while unlocking surface!");
}

void
Surface::clear()
{
  DFBResult ret = _dfbSurface->Clear(_dfbSurface, 0, 0, 0, 0);
  if (ret)
    ILOG_ERROR(ILX_SURFACE, "Clear error: %s", DirectFBErrorString(ret));
}

void
Surface::clear(const Rectangle& rect)
{
  _dfbSurface->SetDrawingFlags(_dfbSurface, DSDRAW_NOFX);
  _dfbSurface->SetColor(_dfbSurface, 0, 0, 0, 0);
  _dfbSurface->FillRectangle(_dfbSurface, rect.x(), rect.y(), rect.width(),
      rect.height());
}

void
Surface::clip(const Rectangle& rect)
{
  DFBRegion r = rect.dfbRegion();
  _dfbSurface->SetClip(_dfbSurface, &r);
}

void
Surface::resetClip()
{
  _dfbSurface->SetClip(_dfbSurface, NULL);
}

void
Surface::blit(IDirectFBSurface* source, const Rectangle& crop, int x, int y)
{
  if (source)
    {
      DFBRectangle r = crop.dfbRect();
      DFBResult ret = _dfbSurface->Blit(_dfbSurface, source, &r, x, y);
      if (ret)
        ILOG_ERROR(ILX_SURFACE, "Blit error: %s", DirectFBErrorString(ret));
    }
}

void
Surface::blit(IDirectFBSurface* source, int x, int y)
{
  if (source)
    {
      DFBResult ret = _dfbSurface->Blit(_dfbSurface, source, NULL, x, y);
      if (ret)
        ILOG_ERROR(ILX_SURFACE, "Blit error: %s", DirectFBErrorString(ret));
    }
}

void
Surface::blit(Surface* source, const Rectangle& crop, int x, int y)
{
  if (source)
    blit(source->DFBSurface(), crop, x, y);
}

void
Surface::blit(Surface* source, int x, int y)
{
  if (source)
    {
      DFBResult ret = _dfbSurface->Blit(_dfbSurface, source->DFBSurface(), NULL,
          x, y);
      if (ret)
        ILOG_ERROR(ILX_SURFACE, "Blit error: %s", DirectFBErrorString(ret));
    }
}

void
Surface::release()
{
  lock();
  if (_dfbSurface)
    {
      _dfbSurface->Release(_dfbSurface);
      _dfbSurface = NULL;
    }
  unlock();
}
