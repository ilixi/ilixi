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

#include <graphics/Surface.h>
#include <ui/Widget.h>
#include <core/PlatformManager.h>
#include <core/Logger.h>
#include <ui/WindowWidget.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_SURFACE, "ilixi/graphics/Surface", "Surface");

bool
compareZ(Widget* first, Widget* second)
{
    if (first->_z < second->_z)
        return true;
    return false;
}

#ifdef ILIXI_STEREO_OUTPUT
Surface::Surface(Widget* owner)
        : _owner(owner),
          _dfbSurface(NULL),
          _parentSurface(NULL),
          _flags((SurfaceFlags) DefaultDescription),
          _rightSurface(NULL),
          _eye(PaintEvent::LeftEye)
{
    pthread_mutex_init(&_surfaceLock, NULL);
    ILOG_TRACE(ILX_SURFACE);
}
#else
Surface::Surface(Widget* owner)
: _owner(owner),
_dfbSurface(NULL),
_parentSurface(NULL),
_flags((SurfaceFlags) DefaultDescription)
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
Surface::createDFBSurface(int width, int height, DFBSurfaceCapabilities caps)
{
    ILOG_TRACE(ILX_SURFACE);
    release();
    DFBSurfaceDescription desc;
    desc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS);
    desc.width = width;
    desc.height = height;
    desc.pixelformat = DSPF_ARGB;
    desc.caps = caps;
    desc.hints = DSHF_FONT;
    DFBResult ret = PlatformManager::instance().getDFB()->CreateSurface(PlatformManager::instance().getDFB(), &desc, &_dfbSurface);
    if (ret)
    {
        ILOG_ERROR(ILX_SURFACE, "Cannot create surface: %s\n", DirectFBErrorString(ret));
        return false;
    }
    _dfbSurface->SetBlittingFlags(_dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);
    ILOG_DEBUG(ILX_SURFACE, "[%p] %s(width %d, height %d)\n", this, __FUNCTION__, width, height);
    ILOG_DEBUG(ILX_SURFACE, "  -> Created dfb surface %p\n", _dfbSurface);
    return true;
}

bool
Surface::createDFBSubSurface(const Rectangle& geometry, IDirectFBSurface* parent)
{
    ILOG_TRACE(ILX_SURFACE);
    release();
    DFBRectangle r = geometry.dfbRect();
    _parentSurface = parent;
    DFBResult ret = _parentSurface->GetSubSurface(_parentSurface, &r, &_dfbSurface);
    if (ret)
    {
        ILOG_ERROR(ILX_SURFACE, "Cannot get sub-surface: %s", DirectFBErrorString(ret));
        return false;
    }
    ret = _dfbSurface->SetBlittingFlags(_dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);
    if (ret)
    {
        ILOG_ERROR(ILX_SURFACE, "Cannot set blitting flags of sub-surface: %s", DirectFBErrorString(ret));
        return false;
    }
    ILOG_DEBUG(ILX_SURFACE, "[%p] %s(x %d, y %d, w %d, h %d, parent %p)\n", this, __FUNCTION__, geometry.x(), geometry.y(), geometry.width(), geometry.height(), parent);
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
#if ILIXI_DFB_SURFACE_EVENTS
    if (_dfbSurface)
    _dfbSurface->GetID(_dfbSurface, &id);
    else
#endif
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
    ILOG_TRACE(ILX_SURFACE);
    DFBRectangle r = { x, y, width, height };
    if (_parentSurface)
    {
        DFBResult ret = _dfbSurface->MakeSubSurface(_dfbSurface, _parentSurface, &r);
        if (ret)
            ILOG_ERROR(ILX_SURFACE, "Cannot set geometry: %s\n", DirectFBErrorString(ret));
    } else
        ILOG_ERROR(ILX_SURFACE, "Cannot set geometry without a parent surface!\n");
}

void
Surface::flip()
{
    ILOG_TRACE(ILX_SURFACE);
    DFBResult ret;
    switch (PlatformManager::instance().getLayerFlipMode(_owner->_rootWindow->layerName()))
    {
    case FlipNone:
        ret = _dfbSurface->Flip(_dfbSurface, NULL, DSFLIP_NONE);
        break;
    case FlipOnSync:
        ret = _dfbSurface->Flip(_dfbSurface, NULL, DSFLIP_ONSYNC);
        break;
    case FlipWaitForSync:
        ret = _dfbSurface->Flip(_dfbSurface, NULL, DSFLIP_WAITFORSYNC);
        break;
    case FlipNew:
        ret = _dfbSurface->Flip(_dfbSurface, NULL, DSFLIP_ONSYNC);
        break;

    default:
        ret = _dfbSurface->Flip(_dfbSurface, NULL, DSFLIP_NONE);
        break;
    }

    if (ret)
        ILOG_ERROR(ILX_SURFACE, " -> error: %s\n", DirectFBErrorString(ret));
}

void
Surface::flip(const Rectangle& rect)
{
    ILOG_TRACE(ILX_SURFACE);
    DFBResult ret;
    DFBRegion r = rect.dfbRegion();
    switch (PlatformManager::instance().getLayerFlipMode(_owner->_rootWindow->layerName()))
    {
    case FlipNone:
        ret = _dfbSurface->Flip(_dfbSurface, &r, DSFLIP_NONE);
        break;
    case FlipOnSync:
        ret = _dfbSurface->Flip(_dfbSurface, &r, DSFLIP_ONSYNC);
        break;
    case FlipWaitForSync:
        ret = _dfbSurface->Flip(_dfbSurface, &r, DSFLIP_WAITFORSYNC);
        break;
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,7,0)
    case FlipNew:
        {
            int w, h;
            _dfbSurface->GetSize(_dfbSurface, &w, &h);
            _dfbSurface->SetClip(_dfbSurface, NULL);
            _dfbSurface->SetBlittingFlags(_dfbSurface, DSBLIT_NOFX);

            if (r.y1)
            {
                DFBRectangle rect = { 0, 0, w, r.y1 };
                _dfbSurface->Blit(_dfbSurface, _dfbSurface, &rect, rect.x, rect.y);
            }

            if (r.y2 < h - 1)
            {
                DFBRectangle rect = { 0, r.y2 + 1, w, h - r.y2 - 1 };
                _dfbSurface->Blit(_dfbSurface, _dfbSurface, &rect, rect.x, rect.y);
            }

            if (r.x1)
            {
                DFBRectangle rect = { 0, r.y1, r.x1, r.y2 - r.y1 + 1 };
                _dfbSurface->Blit(_dfbSurface, _dfbSurface, &rect, rect.x, rect.y);
            }

            if (r.x2 < w - 1)
            {
                DFBRectangle rect = { r.x2 + 1, r.y1, w - r.x2 - 1, r.y2 - r.y1 + 1 };
                _dfbSurface->Blit(_dfbSurface, _dfbSurface, &rect, rect.x, rect.y);
            }

            ret = _dfbSurface->Flip(_dfbSurface, &r, (DFBSurfaceFlipFlags) (DSFLIP_SWAP | DSFLIP_ONSYNC));
        }
        break;
#endif

    default:
        ret = _dfbSurface->Flip(_dfbSurface, &r, DSFLIP_NONE);
        break;
    }

    if (ret)
        ILOG_ERROR(ILX_SURFACE, " -> error: %s\n", DirectFBErrorString(ret));
    else
        ILOG_DEBUG(ILX_SURFACE, " -> Rect(%d, %d, %d, %d)\n", rect.x(), rect.y(), rect.width(), rect.height());
}

void
Surface::lock()
{
    ILOG_TRACE(ILX_SURFACE);
    int rc = pthread_mutex_lock(&_surfaceLock);
    if (rc != 0)
        ILOG_ERROR(ILX_SURFACE, "Error while locking surface!\n");
}

void
Surface::unlock()
{
    ILOG_TRACE(ILX_SURFACE);
    int rc = pthread_mutex_unlock(&_surfaceLock);
    if (rc != 0)
        ILOG_ERROR(ILX_SURFACE, "Error while unlocking surface!\n");
}

void
Surface::clear()
{
    ILOG_TRACE(ILX_SURFACE);
    DFBResult ret = _dfbSurface->Clear(_dfbSurface, 0, 0, 0, 0);
    if (ret)
        ILOG_ERROR(ILX_SURFACE, "Clear error: %s\n", DirectFBErrorString(ret));
    else
        ILOG_DEBUG(ILX_SURFACE, "Clear\n");
}

void
Surface::clear(const Rectangle& rect)
{
    ILOG_TRACE(ILX_SURFACE);
#ifdef ILIXI_STEREO_OUTPUT
    if (_eye == PaintEvent::LeftEye)
    {
#endif
        _dfbSurface->SetDrawingFlags(_dfbSurface, DSDRAW_NOFX);
        _dfbSurface->SetColor(_dfbSurface, 0, 0, 0, 0);
        _dfbSurface->FillRectangle(_dfbSurface, rect.x(), rect.y(), rect.width(), rect.height());
        ILOG_DEBUG(ILX_SURFACE, " -> left (%d, %d, %d, %d)\n", rect.x(), rect.y(), rect.width(), rect.height());
#ifdef ILIXI_STEREO_OUTPUT
    } else
    {
        _rightSurface->SetDrawingFlags(_rightSurface, DSDRAW_NOFX);
        _rightSurface->SetColor(_rightSurface, 0, 0, 0, 0);
        _rightSurface->FillRectangle(_rightSurface, rect.x(), rect.y(), rect.width(), rect.height());
        ILOG_DEBUG(ILX_SURFACE, " -> right (%d, %d, %d, %d)\n", rect.x(), rect.y(), rect.width(), rect.height());
    }
#endif
}

void
Surface::clip(const Rectangle& rect)
{
    ILOG_TRACE(ILX_SURFACE);
    DFBRegion r = rect.dfbRegion();
#ifdef ILIXI_STEREO_OUTPUT
    if (_eye == PaintEvent::LeftEye)
    {
#endif
        int x, y;
        _dfbSurface->GetPosition(_dfbSurface, &x, &y);
        _dfbSurface->SetClip(_dfbSurface, &r);
        ILOG_DEBUG(ILX_SURFACE, " -> LEFT at (%d, %d) left Rect(%d, %d, %d, %d)\n", x, y, rect.x(), rect.y(), rect.width(), rect.height());
#ifdef ILIXI_STEREO_OUTPUT
    } else
    {
        int x, y;
        _rightSurface->GetPosition(_rightSurface, &x, &y);
        _rightSurface->SetClip(_rightSurface, &r);
        ILOG_DEBUG(ILX_SURFACE, " -> RIGHT at (%d, %d) right Rect(%d, %d, %d, %d)\n", x, y, rect.x(), rect.y(), rect.width(), rect.height());
    }
#endif
}

void
Surface::resetClip()
{
    ILOG_TRACE(ILX_SURFACE);
#ifdef ILIXI_STEREO_OUTPUT
    if (_eye == PaintEvent::LeftEye)
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
        DFBResult ret;
#ifdef ILIXI_STEREO_OUTPUT
        if (_eye == PaintEvent::LeftEye)
#endif
            ret = _dfbSurface->Blit(_dfbSurface, source, &r, x, y);
#ifdef ILIXI_STEREO_OUTPUT
        else
            ret = _rightSurface->Blit(_rightSurface, source, &r, x, y);
#endif
        if (ret)
            ILOG_ERROR(ILX_SURFACE, "Blit error: %s\n", DirectFBErrorString(ret));
        else
            ILOG_DEBUG(ILX_SURFACE, "[%p] %s Rect(%d, %d, %d, %d) P(%d, %d)\n", this, __FUNCTION__, crop.x(), crop.y(), crop.width(), crop.height(), x, y);
    }
}

void
Surface::blit(IDirectFBSurface* source, int x, int y)
{
    if (source)
    {
        DFBResult ret;
#ifdef ILIXI_STEREO_OUTPUT
        if (_eye == PaintEvent::LeftEye)
#endif
            ret = _dfbSurface->Blit(_dfbSurface, source, NULL, x, y);
#ifdef ILIXI_STEREO_OUTPUT
        else
            ret = _rightSurface->Blit(_rightSurface, source, NULL, x, y);
#endif
        if (ret)
            ILOG_ERROR(ILX_SURFACE, "Blit error: %s\n", DirectFBErrorString(ret));
        else
            ILOG_DEBUG(ILX_SURFACE, "[%p] %s P(%d, %d)\n", this, __FUNCTION__, x, y);
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
        blit(source->dfbSurface(), x, y);
}

void
Surface::setOpacity(u8 opacity)
{
#ifdef ILIXI_STEREO_OUTPUT
    if (_eye == PaintEvent::LeftEye)
    {
#endif
        if (_dfbSurface && opacity != 255)
        {
            _dfbSurface->SetBlittingFlags(_dfbSurface, (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA));
            _dfbSurface->SetColor(_dfbSurface, 0, 0, 0, opacity);
            ILOG_DEBUG(ILX_SURFACE, "[%p] %s %u\n", this, __FUNCTION__, opacity);
        }
#ifdef ILIXI_STEREO_OUTPUT
    } else
    {
        if (_rightSurface && opacity != 255)
        {
            _rightSurface->SetBlittingFlags(_rightSurface, (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA));
            _rightSurface->SetColor(_rightSurface, 0, 0, 0, opacity);
            ILOG_DEBUG(ILX_SURFACE, "[%p] %s %u\n", this, __FUNCTION__, opacity);
        }
    }
#endif
}

#ifdef ILIXI_STEREO_OUTPUT
bool
Surface::createDFBSubSurfaceStereo(const Rectangle& geometry, IDirectFBSurface* parent, int zIndex)
{
    release();
    DFBRectangle r = geometry.dfbRect();
    _parentSurface = parent;

    r.x += zIndex;
    DFBResult ret = _parentSurface->GetSubSurface(_parentSurface, &r, &_dfbSurface);
    if (ret)
    {
        ILOG_ERROR( ILX_SURFACE, "Cannot get sub-surface left: %s", DirectFBErrorString(ret));
        return false;
    }
    ret = _dfbSurface->SetBlittingFlags(_dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);
    _dfbSurface->SetStereoEye(_dfbSurface, DSSE_LEFT);

    r.x -= zIndex + zIndex;
    ret = _parentSurface->GetSubSurface(_parentSurface, &r, &_rightSurface);
    if (ret)
    {
        ILOG_ERROR( ILX_SURFACE, "Cannot get sub-surface right: %s", DirectFBErrorString(ret));
        return false;
    }
    ret = _rightSurface->SetBlittingFlags(_rightSurface, DSBLIT_BLEND_ALPHACHANNEL);
    _rightSurface->SetStereoEye(_rightSurface, DSSE_RIGHT);

    return true;
}

void
Surface::setStereoGeometry(const Rectangle& geometry, int zIndex)
{
    setStereoGeometry(geometry.x(), geometry.y(), geometry.width(), geometry.height(), zIndex);
}

void
Surface::setStereoGeometry(int x, int y, int width, int height, int zIndex)
{
    DFBRectangle r = { x, y, width, height };
    if (_parentSurface)
    {
        r.x += zIndex;
        DFBResult ret = _dfbSurface->MakeSubSurface(_dfbSurface, _parentSurface, &r);
        if (ret)
            ILOG_ERROR( ILX_SURFACE, "Cannot set left geometry: %s\n", DirectFBErrorString(ret));

        r.x -= zIndex + zIndex;
        ret = _rightSurface->MakeSubSurface(_rightSurface, _parentSurface, &r);
        if (ret)
            ILOG_ERROR( ILX_SURFACE, "Cannot set right geometry: %s\n", DirectFBErrorString(ret));
    } else
        ILOG_ERROR(ILX_SURFACE, "No Parent Surface, need to create surface again!\n");
}

PaintEvent::PaintEventEye
Surface::stereoEye() const
{
    return _eye;
}

void
Surface::setStereoEye(PaintEvent::PaintEventEye eye)
{
    ILOG_DEBUG(ILX_SURFACE, "[%p] %s(%s)\n", this, __FUNCTION__, eye & PaintEvent::LeftEye ? "Left" : "Right");
    _eye = eye;
    if (_flags & RootSurface)
    {
#ifdef ILIXI_STEREO_OUTPUT
        if (_eye == PaintEvent::RightEye)
            _rightSurface->SetStereoEye(_rightSurface, DSSE_RIGHT);
        else
#endif
            _dfbSurface->SetStereoEye(_dfbSurface, DSSE_LEFT);
    }
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
    ILOG_DEBUG(ILX_SURFACE, "[%p] %s Left(%d,%d,%d,%d) Right(%d,%d,%d,%d)\n", this, __FUNCTION__, left.x(), left.y(), left.width(), left.height(), right.x(), right.y(), right.width(), right.height());
    DFBResult ret = _dfbSurface->FlipStereo(_dfbSurface, &l, &r, DSFLIP_WAITFORSYNC);
    if (ret)
        ILOG_ERROR(ILX_SURFACE, "Flip error: %s\n", DirectFBErrorString(ret));
}

#endif

Surface::SurfaceFlags
Surface::flags() const
{
    return _flags;
}

void
Surface::setSurfaceFlag(SurfaceFlags flags)
{
    _flags = (SurfaceFlags) (_flags | flags);
}

void
Surface::unsetSurfaceFlag(SurfaceFlags flags)
{
    _flags = (SurfaceFlags) (_flags & ~flags);
}

void
Surface::updateSurface(const PaintEvent& event)
{
    ILOG_TRACE_F(ILX_SURFACE);
    ILOG_DEBUG(ILX_SURFACE, " -> owner: %p\n", _owner);

    if (_flags & DoZSort)
    {
        _owner->_children.sort(compareZ);
        unsetSurfaceFlag(Surface::DoZSort);
    }

    if (_flags & Surface::ModifiedGeometry)
        _owner->sigGeometryUpdated();

    // initialisation is done once for both surfaces in stereoscopic mode.
    if (_flags & InitialiseSurface)
    {
        ILOG_DEBUG(ILX_SURFACE, " -> InitialiseSurface\n");
        bool ret = false;

        release();

        if (_flags & HasOwnSurface)
        {
            // XXX This surface will be painted once by owner; but it will be blitted by parent using z() twice.
            ILOG_DEBUG(ILX_SURFACE, " -> HasOwnSurface: 0x%03x\n", _flags);
            ret = createDFBSurface(_owner->width(), _owner->height());
        } else if (_flags & RootSurface)
        {
#ifdef ILIXI_STEREO_OUTPUT
            ILOG_DEBUG(ILX_SURFACE, " -> RootSurface: 0x%03x (RIGHT)\n", _flags);
            _rightSurface = _owner->_rootWindow->windowSurface();
            _rightSurface->AddRef(_rightSurface);
            ILOG_DEBUG(ILX_SURFACE, " -> RootSurface: 0x%03x (LEFT)\n", _flags);
            _dfbSurface = _owner->_rootWindow->windowSurface();
            _dfbSurface->AddRef(_dfbSurface);
#else
            ILOG_DEBUG(ILX_SURFACE, " -> RootSurface %p flags 0x%03x\n", this, _flags);
            _dfbSurface = _owner->_rootWindow->windowSurface();
            _dfbSurface->AddRef(_dfbSurface);
#endif
            ret = true;
        } else if (_flags & SubSurface)
        {
#ifdef ILIXI_STEREO_OUTPUT
            if (_eye == PaintEvent::RightEye)
            {
                ILOG_DEBUG(ILX_SURFACE, " -> SubSurface: 0x%03x (RIGHT)\n", _flags);
                _rightSurface = _owner->parent()->surface()->dfbSurface();
                _rightSurface->AddRef(_rightSurface);
            } else
            {
                ILOG_DEBUG(ILX_SURFACE, " -> SubSurface: 0x%03x (LEFT)\n", _flags);
                ret = createDFBSubSurface(_owner->surfaceGeometry(), _owner->_parent->surface()->dfbSurface());
            }
#else
            ILOG_DEBUG(ILX_SURFACE, " -> SubSurface: 0x%03x\n", _flags);

            Widget* parent =_owner->_parent;
            while(parent->surface()->flags() & SharedSurface)
            {
                if (parent->parent())
                    parent = parent->parent();
                else
                    break;
            }

            ret = createDFBSubSurface(_owner->frameGeometry(), parent->surface()->dfbSurface());
#endif
        } else if (_owner->_parent)
        {
#ifdef ILIXI_STEREO_OUTPUT
            ILOG_DEBUG(ILX_SURFACE, " -> SharedSurface: 0x%03x (RIGHT)\n", _flags);
            _rightSurface = _owner->parent()->surface()->_rightSurface;
            _rightSurface->AddRef(_rightSurface);
            ILOG_DEBUG(ILX_SURFACE, " -> SharedSurface: 0x%03x (LEFT)\n", _flags);
            _dfbSurface = _owner->parent()->surface()->_dfbSurface;
            _dfbSurface->AddRef(_dfbSurface);
#else
            ILOG_DEBUG(ILX_SURFACE, " -> SharedSurface: 0x%03x\n", _flags);
            _dfbSurface = _owner->parent()->surface()->dfbSurface();
            _dfbSurface->AddRef(_dfbSurface);
#endif
            ret = true;
        }

        if (ret)
            unsetSurfaceFlag(Surface::InitialiseSurface);
    }
#ifdef ILIXI_STEREO_OUTPUT
    ILOG_DEBUG(ILX_SURFACE, " -> eye: %s\n", event.eye & PaintEvent::LeftEye ? "Left" : "Right");
    _eye = event.eye;
#endif
}

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

} /* namespace ilixi */
