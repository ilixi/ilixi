/*
 Copyright 2012 Tarik Sekmen.

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

#include "ui/SurfaceView.h"
#include "core/AppBase.h"
#include "core/Logger.h"

namespace ilixi
{

  D_DEBUG_DOMAIN( ILX_SURFACEVIEW, "ilixi/ui/SurfaceView", "SurfaceView");

  SurfaceView::SurfaceView(DFBSurfaceID id, Widget* parent) :
      SurfaceEventListener(id), Widget(parent)
  {
    setSourceFromSurfaceID(id);
    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &SurfaceView::onSVGeomUpdate));
  }

  SurfaceView::~SurfaceView()
  {
    detachEB();
  }

  Size
  SurfaceView::preferredSize() const
  {
    // FIXME default size for surface view.
    if (_source)
      {
        int width, height;
        _source->GetSize(_source, &width, &height);
        return Size(width, height);
      }
    return Size(50, 50);
  }

  DFBSurfaceID
  SurfaceView::sourceID() const
  {
    return _id;
  }

  void
  SurfaceView::setSourceFromSurfaceID(DFBSurfaceID id)
  {
    if (id)
      {
        detachEB();

        DFBResult ret = AppBase::getDFB()->GetSurface(AppBase::getDFB(), id,
            &_source);
        if (ret)
          {
            ILOG_ERROR(
                ILX_SURFACEVIEW, "Error! GetSurface: %s", DirectFBErrorString(ret));
            _source = NULL;
            _id = 0;
          }
        else
          {
            _source->GetID(_source, &_id);
            attachEB();
          }
      }
  }

  void
  SurfaceView::setSourceFromSurface(IDirectFBSurface* source)
  {
    if (source)
      {
        detachEB();

        _source = source;
        _source->GetID(_source, &_id);

        attachEB();
      }
  }

  void
  SurfaceView::setSourceFromWindow(IDirectFBWindow* window)
  {
    if (window)
      {
        detachEB();

        DFBResult ret = window->GetSurface(window, &_source);

        if (ret)
          {
            ILOG_ERROR(
                ILX_SURFACEVIEW, "Error! GetSurface: %s", DirectFBErrorString(ret));
            _source = NULL;
          }
        else
          {
            _window = window;
            _source->GetID(_source, &_id);
            attachEB();
          }
      }
  }

  void
  SurfaceView::paint(const Rectangle& rect)
  {
    if (visible())
      {
        updateSurface();
        Rectangle intersect = _frameGeometry.intersected(rect);
        if (intersect.isValid())
          {
            compose();
            renderSource();
            paintChildren(intersect);
          }
      }
  }

  bool
  SurfaceView::consumeWindowEvent(DFBWindowEvent event)
  {
    if (_window && event.type != DWET_UPDATE)
      {
        event.x *= _wScale;
        event.y *= _hScale;
        _window->SendEvent(_window, &event);
        return true;
      }
    return false;
  }

  void
  SurfaceView::compose()
  {
  }

  void
  SurfaceView::renderSource()
  {
    if (_source)
      {
        DFBRectangle rect =
          { 0, 0, width(), height() };
        IDirectFBSurface* dfbSurface = surface()->DFBSurface();
        dfbSurface->StretchBlit(dfbSurface, _source, NULL, &rect);
      }
  }

  void
  SurfaceView::onSourceUpdate(const DFBSurfaceEvent& event)
  {
    // TODO scale region and use rectangle for update.
    Rectangle rect(Point(event.update.x1, event.update.y1),
        Point(event.update.x2, event.update.y2));
    rect.setWidth(width() * _wScale);
    rect.setHeight(height() * _hScale);
    _source->FrameAck(_source, event.flip_count);
    update();
  }

  void
  SurfaceView::onSourceDestroyed(const DFBSurfaceEvent& event)
  {
    _source = NULL;
    _id = 0;
  }

  void
  SurfaceView::onSVGeomUpdate()
  {
    if (_source)
      {
        int w, h;
        _source->GetSize(_source, &w, &h);
        _wScale = (float) (0.0 + w) / width();
        _hScale = (float) (0.0 + h) / height();
      }
  }

} /* namespace ilixi */
