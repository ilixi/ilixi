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

#ifdef ILIXI_STEREO_OUTPUT
Surface::Surface() :
    _dfbSurface(NULL), _parentSurface(NULL), _rightSurface(NULL), _eye(
        PaintEvent::LeftEye)
{
  pthread_mutex_init(&_surfaceLock, NULL);
  ILOG_TRACE(ILX_SURFACE);
}
#else
Surface::Surface() :
_dfbSurface(NULL), _parentSurface(NULL)
  {
    pthread_mutex_init(&_surfaceLock, NULL);
    ILOG_TRACE(ILX_SURFACE);
  }
#endif

Surface::~Surface()
{
  ILOG_TRACE(ILX_SURFACE);
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
      ILOG_ERROR( ILX_SURFACE,
          "Cannot create surface: %s\n", DirectFBErrorString(ret));
      return false;
    }
  _dfbSurface->SetBlittingFlags(_dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);
  ILOG_DEBUG(ILX_SURFACE,
      "[%p] %s(width %d, height %d)\n", this, __FUNCTION__, width, height);
  ILOG_DEBUG(ILX_SURFACE, "  -> Created dfb surface %p\n", _dfbSurface);
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
      ILOG_ERROR( ILX_SURFACE,
          "Cannot get sub-surface: %s", DirectFBErrorString(ret));
      return false;
    }
  ret = _dfbSurface->SetBlittingFlags(_dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);
  if (ret)
    {
      ILOG_ERROR( ILX_SURFACE,
          "Cannot set blitting flags of sub-surface: %s", DirectFBErrorString(ret));
      return false;
    }
  ILOG_DEBUG(ILX_SURFACE,
      "[%p] %s(x %d, y %d, w %d, h %d, parent %p)\n", this, __FUNCTION__, geometry.x(), geometry.y(), geometry.width(), geometry.height(), parent);
  ILOG_DEBUG(ILX_SURFACE, "  -> Created dfb surface %p\n", _dfbSurface);
  return true;
}

IDirectFBSurface*
Surface::dfbSurface()
{
#ifdef ILIXI_STEREO_OUTPUT
  if (_eye == PaintEvent::LeftEye)
    return _dfbSurface;
  return _rightSurface;
#else
  return _dfbSurface;
#endif
}

DFBSurfaceID
Surface::dfbSurfaceId() const
{
  unsigned int id = 0;
  if (_dfbSurface)
    _dfbSurface->GetID(_dfbSurface, &id);
  else
    ILOG_ERROR(ILX_SURFACE, "Cannot get surface id!\n");
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
        ILOG_ERROR( ILX_SURFACE,
            "Cannot set geometry: %s\n", DirectFBErrorString(ret));
    }
  else
    ILOG_ERROR( ILX_SURFACE,
        "No Parent Surface, need to create surface again!\n");
}

void
Surface::flip()
{
  DFBResult ret = _dfbSurface->Flip(_dfbSurface, NULL, DSFLIP_WAITFORSYNC);
  if (ret)
    ILOG_ERROR(ILX_SURFACE, "Flip error: %s\n", DirectFBErrorString(ret));
  else
    ILOG_DEBUG(ILX_SURFACE, "[%p] %s\n", this, __FUNCTION__);
}

void
Surface::flip(const Rectangle& rect)
{
  DFBRegion r = rect.dfbRegion();
  DFBResult ret = _dfbSurface->Flip(_dfbSurface, &r, DSFLIP_BLIT);
  if (ret)
    ILOG_ERROR(ILX_SURFACE, "Flip error: %s\n", DirectFBErrorString(ret));
  else
    ILOG_DEBUG(ILX_SURFACE,
        "[%p] %s Rect(%d, %d, %d, %d)\n", this, __FUNCTION__, rect.x(), rect.y(), rect.width(), rect.height());
}

void
Surface::lock()
{
  int rc = pthread_mutex_lock(&_surfaceLock);
  if (rc != 0)
    ILOG_ERROR(ILX_SURFACE, "Error while locking surface!\n");
}

void
Surface::unlock()
{
  int rc = pthread_mutex_unlock(&_surfaceLock);
  if (rc != 0)
    ILOG_ERROR(ILX_SURFACE, "Error while unlocking surface!\n");
}

void
Surface::clear()
{
  DFBResult ret = _dfbSurface->Clear(_dfbSurface, 0, 0, 0, 0);
  if (ret)
    ILOG_ERROR(ILX_SURFACE, "Clear error: %s\n", DirectFBErrorString(ret));
  else
    ILOG_DEBUG(ILX_SURFACE, "Clear\n");
}

void
Surface::clear(const Rectangle& rect)
{
#ifdef ILIXI_STEREO_OUTPUT
  if (_eye == PaintEvent::LeftEye || _parentSurface == NULL)
    {
#endif
      _dfbSurface->SetDrawingFlags(_dfbSurface, DSDRAW_NOFX);
      _dfbSurface->SetColor(_dfbSurface, 0, 0, 0, 0);
      _dfbSurface->FillRectangle(_dfbSurface, rect.x(), rect.y(), rect.width(),
          rect.height());
      ILOG_DEBUG( ILX_SURFACE,
          "Clear left (%d, %d, %d, %d)\n", rect.x(), rect.y(), rect.width(), rect.height());
#ifdef ILIXI_STEREO_OUTPUT
    }
  else
    {
      _rightSurface->SetDrawingFlags(_rightSurface, DSDRAW_NOFX);
      _rightSurface->SetColor(_rightSurface, 0, 0, 0, 0);
      _rightSurface->FillRectangle(_rightSurface, rect.x(), rect.y(),
          rect.width(), rect.height());
      ILOG_DEBUG( ILX_SURFACE,
          "Clear right (%d, %d, %d, %d)\n", rect.x(), rect.y(), rect.width(), rect.height());
    }
#endif
}

void
Surface::clip(const Rectangle& rect)
{
  DFBRegion r = rect.dfbRegion();
#ifdef ILIXI_STEREO_OUTPUT
  if (_eye == PaintEvent::LeftEye || _parentSurface == NULL)
    {
#endif
      int x, y;
      _dfbSurface->GetPosition(_dfbSurface, &x, &y);
      _dfbSurface->SetClip(_dfbSurface, &r);
      ILOG_DEBUG(ILX_SURFACE,
          "[%p] %s at (%d, %d) left Rect(%d, %d, %d, %d)\n", this, __FUNCTION__, x, y, rect.x(), rect.y(), rect.width(), rect.height());
#ifdef ILIXI_STEREO_OUTPUT
    }
  else
    {
      int x, y;
      _rightSurface->GetPosition(_rightSurface, &x, &y);
      _rightSurface->SetClip(_rightSurface, &r);
      ILOG_DEBUG(ILX_SURFACE,
          "[%p] %s at (%d, %d) right Rect(%d, %d, %d, %d)\n", this, __FUNCTION__, x, y, rect.x(), rect.y(), rect.width(), rect.height());
    }
#endif
}

void
Surface::resetClip()
{
#ifdef ILIXI_STEREO_OUTPUT
  if (_eye == PaintEvent::LeftEye || _parentSurface == NULL)
#endif
    _dfbSurface->SetClip(_dfbSurface, NULL);
#ifdef ILIXI_STEREO_OUTPUT
  else
    _rightSurface->SetClip(_rightSurface, NULL);
#endif
}

void
Surface::blit(IDirectFBSurface* source, const Rectangle& crop, int x, int y)
{
  if (source)
    {
      DFBRectangle r = crop.dfbRect();
      DFBResult ret = _dfbSurface->Blit(_dfbSurface, source, &r, x, y);
      if (ret)
        ILOG_ERROR(ILX_SURFACE, "Blit error: %s\n", DirectFBErrorString(ret));
      else
        ILOG_DEBUG(ILX_SURFACE,
            "[%p] %s Rect(%d, %d, %d, %d) P(%d, %d)\n", this, __FUNCTION__, crop.x(), crop.y(), crop.width(), crop.height(), x, y);
    }
}

void
Surface::blit(IDirectFBSurface* source, int x, int y)
{
  if (source)
    {
      DFBResult ret = _dfbSurface->Blit(_dfbSurface, source, NULL, x, y);
      if (ret)
        ILOG_ERROR(ILX_SURFACE, "Blit error: %s\n", DirectFBErrorString(ret));
      else
        ILOG_DEBUG(ILX_SURFACE,
            "[%p] %s P(%d, %d)\n", this, __FUNCTION__, x, y);
    }
}

void
Surface::blit(Surface* source, const Rectangle& crop, int x, int y)
{
  if (source)
    blit(source->dfbSurface(), crop, x, y);
}

void
Surface::blit(Surface* source, int x, int y)
{
  if (source)
    {
      DFBResult ret = _dfbSurface->Blit(_dfbSurface, source->dfbSurface(), NULL,
          x, y);
      if (ret)
        ILOG_ERROR(ILX_SURFACE, "Blit error: %s\n", DirectFBErrorString(ret));
      else
        ILOG_DEBUG(ILX_SURFACE,
            "[%p] %s P(%d, %d)\n", this, __FUNCTION__, x, y);
    }
}

void
Surface::setOpacity(u8 opacity)
{
  if (_dfbSurface && opacity != 255)
    {
      _dfbSurface->SetBlittingFlags(_dfbSurface,
          (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL
              | DSBLIT_BLEND_COLORALPHA));
      _dfbSurface->SetColor(_dfbSurface, 0, 0, 0, opacity);
      ILOG_DEBUG(ILX_SURFACE, "[%p] %s %u\n", this, __FUNCTION__, opacity);
    }
}

#ifdef ILIXI_STEREO_OUTPUT
bool
Surface::createDFBSubSurfaceStereo(const Rectangle& geometry,
    IDirectFBSurface* parent, int zIndex)
{
  release();
  DFBRectangle r = geometry.dfbRect();
  _parentSurface = parent;

  r.x += zIndex;
  DFBResult ret = _parentSurface->GetSubSurface(_parentSurface, &r,
      &_dfbSurface);
  if (ret)
    {
      ILOG_ERROR( ILX_SURFACE,
          "Cannot get sub-surface left: %s", DirectFBErrorString(ret));
      return false;
    }
  ret = _dfbSurface->SetBlittingFlags(_dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);

  r.x -= zIndex + zIndex;
  ret = _parentSurface->GetSubSurface(_parentSurface, &r, &_rightSurface);
  if (ret)
    {
      ILOG_ERROR( ILX_SURFACE,
          "Cannot get sub-surface right: %s", DirectFBErrorString(ret));
      return false;
    }
  ret = _rightSurface->SetBlittingFlags(_rightSurface,
      DSBLIT_BLEND_ALPHACHANNEL);

  return true;
}

void
Surface::setStereoGeometry(const Rectangle& geometry, int zIndex)
{
  setStereoGeometry(geometry.x(), geometry.y(), geometry.width(),
      geometry.height(), zIndex);
}

void
Surface::setStereoGeometry(int x, int y, int width, int height, int zIndex)
{
  DFBRectangle r =
    { x, y, width, height };
  if (_parentSurface)
    {

      r.x += zIndex;
      DFBResult ret = _dfbSurface->MakeSubSurface(_dfbSurface, _parentSurface,
          &r);
      if (ret)
        ILOG_ERROR( ILX_SURFACE,
            "Cannot set left geometry: %s\n", DirectFBErrorString(ret));

      r.x -= zIndex + zIndex;
      ret = _rightSurface->MakeSubSurface(_rightSurface, _parentSurface, &r);
      if (ret)
        ILOG_ERROR( ILX_SURFACE,
            "Cannot set right geometry: %s\n", DirectFBErrorString(ret));
    }
  else
    ILOG_ERROR(ILX_SURFACE,
        "No Parent Surface, need to create surface again!\n");
}

PaintEvent::PaintEventEye
Surface::stereoEye() const
{
  return _eye;
}

void
Surface::setStereoEye(PaintEvent::PaintEventEye eye)
{
  _eye = eye;
  ILOG_DEBUG(ILX_SURFACE,
      "[%p] %s(%s)\n", this, __FUNCTION__, eye == PaintEvent::LeftEye ? "Left" : "Right");
}

IDirectFBSurface*
Surface::getStereoSurface(PaintEvent::PaintEventEye eye)
{
  if (eye == PaintEvent::LeftEye)
    return _dfbSurface;
  return _rightSurface;
}

void
Surface::flipStereo(const Rectangle& left, const Rectangle& right)
{
  DFBRegion l = left.dfbRegion();
  DFBRegion r = right.dfbRegion();
  ILOG_DEBUG(ILX_SURFACE,
      "[%p] %s Left(%d,%d,%d,%d) Right(%d,%d,%d,%d)\n", this, __FUNCTION__,
      l.x1, l.y1, l.x2, l.y2,
      r.x1, r.y1, r.x2, r.y2);
  DFBResult ret = _dfbSurface->FlipStereo(_dfbSurface, &l, &r,
      DSFLIP_WAITFORSYNC);
  if (ret)
    ILOG_ERROR(ILX_SURFACE, "Flip error: %s\n", DirectFBErrorString(ret));
}

#endif

void
Surface::release()
{
  ILOG_TRACE(ILX_SURFACE);
  lock();
#ifdef ILIXI_STEREO_OUTPUT
  if (_rightSurface)
    {
      _rightSurface->Release(_rightSurface);
      _rightSurface = NULL;
    }
#endif
  if (_dfbSurface)
    {
      _dfbSurface->Release(_dfbSurface);
      _dfbSurface = NULL;
    }
  unlock();
}
