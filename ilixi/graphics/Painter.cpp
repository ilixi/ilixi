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

#include <graphics/Painter.h>
#include <types/TextLayout.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_PAINTER, "ilixi/graphics/Painter", "Painter");

Painter::Painter(Widget* widget)
        : _myWidget(widget),
          dfbSurface(_myWidget->surface()->dfbSurface()),
          _brush(),
          _pen(),
          _font(),
          _state(None)
{
    _affine = NULL;
    ILOG_TRACE(ILX_PAINTER);
}

Painter::~Painter()
{
    ILOG_TRACE(ILX_PAINTER);
    end();
}

void
Painter::begin(const PaintEvent& event)
{
    ILOG_TRACE(ILX_PAINTER);
    _myWidget->surface()->lock();
#ifdef ILIXI_STEREO_OUTPUT
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
    {
        if (event.eye == PaintEvent::LeftEye)
            _myWidget->surface()->clip(event.rect);
        else
            _myWidget->surface()->clip(event.right);
    } else
    {
        if (event.eye == PaintEvent::LeftEye)
            _myWidget->surface()->clip(Rectangle(event.rect.x() - _myWidget->absX() - _myWidget->z(), event.rect.y() - _myWidget->absY(), event.rect.width(), event.rect.height()));
        else
            _myWidget->surface()->clip(Rectangle(event.right.x() - _myWidget->absX() + _myWidget->z(), event.right.y() - _myWidget->absY(), event.right.width(), event.right.height()));
    }
#else
    if (_myWidget->surface()->flags() & Surface::SharedSurface)
        _myWidget->surface()->clip(event.rect);
    else
        _myWidget->surface()->clip(Rectangle(event.rect.x() - _myWidget->absX(), event.rect.y() - _myWidget->absY(), event.rect.width(), event.rect.height()));
#endif
    _state = Active;
    dfbSurface->SetDrawingFlags(dfbSurface, DSDRAW_NOFX);
    dfbSurface->SetPorterDuff(dfbSurface, DSPD_SRC_OVER);
}

void
Painter::end()
{
    if (_state & Active)
    {
        ILOG_TRACE(ILX_PAINTER);
        if (_state & Clipped)
            _myWidget->surface()->resetClip();
        if (_state & Transformed)
        {
            int32_t* tmp = _affine->invert().m();
            dfbSurface->SetMatrix(dfbSurface, tmp);
            dfbSurface->SetRenderOptions(dfbSurface, DSRO_NONE);
            delete tmp;
            delete _affine;
        }
        _state = None;
        dfbSurface->ReleaseSource(dfbSurface);
        _myWidget->surface()->unlock();
    }
}

const Brush&
Painter::brush() const
{
    return _brush;
}

const Font&
Painter::font() const
{
    return _font;
}

const Pen&
Painter::pen()
{
    return _pen;
}

void
Painter::drawLine(double x1, double y1, double x2, double y2, const DFBSurfaceDrawingFlags& flags)
{
    if (_state & Active)
    {
        applyPen();
        dfbSurface->SetDrawingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
#ifdef ILIXI_STEREO_OUTPUT
        {
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                dfbSurface->DrawLine(dfbSurface, _myWidget->absX() + x1 + _myWidget->z(), _myWidget->absY() + y1, _myWidget->absX() + x2 - _myWidget->z(), _myWidget->absY() + y2);
            else
                dfbSurface->DrawLine(dfbSurface, _myWidget->absX() + x1 - _myWidget->z(), _myWidget->absY() + y1, _myWidget->absX() + x2 + _myWidget->z(), _myWidget->absY() + y2);
        }
#else
            dfbSurface->DrawLine(dfbSurface, _myWidget->absX() + x1, _myWidget->absY() + y1, _myWidget->absX() + x2, _myWidget->absY() + y2);
#endif
        else
            dfbSurface->DrawLine(dfbSurface, x1, y1, x2, y2);
    }
}

void
Painter::drawLine(const Rectangle& rect, const DFBSurfaceDrawingFlags& flags)
{
    drawLine(rect.x(), rect.y(), rect.right(), rect.bottom(), flags);
}

void
Painter::drawLine(const Point& p1, const Point& p2, const DFBSurfaceDrawingFlags& flags)
{
    drawLine(p1.x(), p1.y(), p2.x(), p2.y(), flags);
}

void
Painter::drawRectangle(double x, double y, double width, double height, const DFBSurfaceDrawingFlags& flags)
{
    if (_state & Active)
    {
        applyPen();
        dfbSurface->SetDrawingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
#ifdef ILIXI_STEREO_OUTPUT
        {
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                dfbSurface->DrawRectangle(dfbSurface, _myWidget->absX() + x + _myWidget->z(), _myWidget->absY() + y, width, height);
            else
                dfbSurface->DrawRectangle(dfbSurface, _myWidget->absX() + x - _myWidget->z(), _myWidget->absY() + y, width, height);
        }
#else
            dfbSurface->DrawRectangle(dfbSurface, _myWidget->absX() + x, _myWidget->absY() + y, width, height);
#endif
        else
            dfbSurface->DrawRectangle(dfbSurface, x, y, width, height);
    }
}

void
Painter::drawRectangle(const Rectangle& rect, const DFBSurfaceDrawingFlags& flags)
{
    drawRectangle(rect.x(), rect.y(), rect.width(), rect.height(), flags);
}

void
Painter::fillRectangle(double x, double y, double width, double height, const DFBSurfaceDrawingFlags& flags)
{
    ILOG_TRACE(ILX_PAINTER);
    if (_state & Active)
    {
        applyBrush();
        dfbSurface->SetDrawingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
#ifdef ILIXI_STEREO_OUTPUT
        {
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                dfbSurface->FillRectangle(dfbSurface, _myWidget->absX() + x + _myWidget->z(), _myWidget->absY() + y, width, height);
            else
                dfbSurface->FillRectangle(dfbSurface, _myWidget->absX() + x - _myWidget->z(), _myWidget->absY() + y, width, height);
        }
#else
            dfbSurface->FillRectangle(dfbSurface, _myWidget->absX() + x, _myWidget->absY() + y, width, height);
#endif
        else
            dfbSurface->FillRectangle(dfbSurface, x, y, width, height);
    }
}

void
Painter::fillRectangle(const Rectangle& rect, const DFBSurfaceDrawingFlags& flags)
{
    fillRectangle(rect.x(), rect.y(), rect.width(), rect.height());
}

void
Painter::drawText(const std::string& text, int x, int y, const DFBSurfaceDrawingFlags& flags)
{
    if (text.empty())
        return;

    if (_state & Active)
    {
        applyBrush();
        applyFont();
        dfbSurface->SetDrawingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
#ifdef ILIXI_STEREO_OUTPUT
        {
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                dfbSurface->DrawString(dfbSurface, text.c_str(), -1, _myWidget->absX() + x + _myWidget->z(), _myWidget->absY() + y, DSTF_TOPLEFT);
            else
                dfbSurface->DrawString(dfbSurface, text.c_str(), -1, _myWidget->absX() + x - _myWidget->z(), _myWidget->absY() + y, DSTF_TOPLEFT);
        }
#else
            dfbSurface->DrawString(dfbSurface, text.c_str(), -1, _myWidget->absX() + x, _myWidget->absY() + y, DSTF_TOPLEFT);
#endif
        else
            dfbSurface->DrawString(dfbSurface, text.c_str(), -1, x, y, DSTF_TOPLEFT);
    }
}

void
Painter::drawLayout(const TextLayout& layout, const DFBSurfaceDrawingFlags& flags)
{
    ILOG_TRACE(ILX_PAINTER);
    if (layout.text().empty())
        return;

    if (_state & Active)
    {
        applyBrush();
        applyFont();
        dfbSurface->SetDrawingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
#ifdef ILIXI_STEREO_OUTPUT
        {
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                layout.drawTextLayout(dfbSurface, _myWidget->absX() + _myWidget->z(), _myWidget->absY());
            else
                layout.drawTextLayout(dfbSurface, _myWidget->absX() - _myWidget->z(), _myWidget->absY());
        }
#else
            layout.drawTextLayout(dfbSurface, _myWidget->absX(), _myWidget->absY());
#endif
        else
            layout.drawTextLayout(dfbSurface);
    }
}

void
Painter::drawLayout(const TextLayout& layout, int x, int y, const DFBSurfaceDrawingFlags& flags)
{
    if (layout.text().empty())
        return;

    if (_state & Active)
    {
        applyBrush();
        applyFont();
        dfbSurface->SetDrawingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
#ifdef ILIXI_STEREO_OUTPUT
        {
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                layout.drawTextLayout(dfbSurface, _myWidget->absX() + x + _myWidget->z(), _myWidget->absY() + y);
            else
                layout.drawTextLayout(dfbSurface, _myWidget->absX() + x - _myWidget->z(), _myWidget->absY() + y);
        }
#else
            layout.drawTextLayout(dfbSurface, _myWidget->absX() + x, _myWidget->absY() + y);
#endif
        else
            layout.drawTextLayout(dfbSurface, x, y);
    }
}

void
Painter::stretchImage(Image* image, int x, int y, int w, int h, const DFBSurfaceBlittingFlags& flags)
{
    stretchImage(image, Rectangle(x, y, w, h), flags);
}

void
Painter::stretchImage(Image* image, const Rectangle& destRect, const DFBSurfaceBlittingFlags& flags)
{
    if ((_state & Active) && image)
    {
        applyBrush();
        DFBRectangle dest = destRect.dfbRect();
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
        {
#ifdef ILIXI_STEREO_OUTPUT
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                dest.x += _myWidget->absX() + _myWidget->z();
            else
                dest.x += _myWidget->absX() - _myWidget->z();
#else
            dest.x += _myWidget->absX();
#endif
            dest.y += _myWidget->absY();
        }
        if (!(image->getCaps() & DICAPS_ALPHACHANNEL))
            dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (flags & ~DSBLIT_BLEND_ALPHACHANNEL));
        else
            dfbSurface->SetBlittingFlags(dfbSurface, flags);
        dfbSurface->StretchBlit(dfbSurface, image->getDFBSurface(), NULL, &dest);
    }
}

void
Painter::stretchImage(Image* image, const Rectangle& destRect, const Rectangle& sourceRect, const DFBSurfaceBlittingFlags& flags)
{
    if ((_state & Active) && image)
    {
        applyBrush();
        DFBRectangle source = sourceRect.dfbRect();
        DFBRectangle dest = destRect.dfbRect();
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
        {
#ifdef ILIXI_STEREO_OUTPUT
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                dest.x += _myWidget->absX() + _myWidget->z();
            else
                dest.x += _myWidget->absX() - _myWidget->z();
#else
            dest.x += _myWidget->absX();
#endif
            dest.y += _myWidget->absY();
        }
        if (!(image->getCaps() & DICAPS_ALPHACHANNEL))
            dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (flags & ~DSBLIT_BLEND_ALPHACHANNEL));
        else
            dfbSurface->SetBlittingFlags(dfbSurface, flags);
        dfbSurface->StretchBlit(dfbSurface, image->getDFBSurface(), &source, &dest);
    }
}

void
Painter::drawImage(Image* image, int x, int y, const DFBSurfaceBlittingFlags& flags)
{
    if ((_state & Active) && image)
    {
        applyBrush();
        if (!(image->getCaps() & DICAPS_ALPHACHANNEL))
            dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (flags & ~DSBLIT_BLEND_ALPHACHANNEL));
        else
            dfbSurface->SetBlittingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
#ifdef ILIXI_STEREO_OUTPUT
        {
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                dfbSurface->Blit(dfbSurface, image->getDFBSurface(), NULL, _myWidget->absX() + x + _myWidget->z(), _myWidget->absY() + y);
            else
                dfbSurface->Blit(dfbSurface, image->getDFBSurface(), NULL, _myWidget->absX() + x - _myWidget->z(), _myWidget->absY() + y);
        }
#else
            dfbSurface->Blit(dfbSurface, image->getDFBSurface(), NULL, _myWidget->absX() + x, _myWidget->absY() + y);
#endif
        else
            dfbSurface->Blit(dfbSurface, image->getDFBSurface(), NULL, x, y);
    }
}

void
Painter::drawImage(Image* image, const Point& point, const DFBSurfaceBlittingFlags& flags)
{
    drawImage(image, point.x(), point.y(), flags);
}

void
Painter::tileImage(Image* image, int x, int y, const DFBSurfaceBlittingFlags& flags)
{
    if ((_state & Active) && image)
    {
        applyBrush();
        if (!(image->getCaps() & DICAPS_ALPHACHANNEL))
            dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (flags & ~DSBLIT_BLEND_ALPHACHANNEL));
        else
            dfbSurface->SetBlittingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
#ifdef ILIXI_STEREO_OUTPUT
        {
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                dfbSurface->TileBlit(dfbSurface, image->getDFBSurface(), NULL, _myWidget->absX() + x + _myWidget->z(), _myWidget->absY() + y);
            else
                dfbSurface->TileBlit(dfbSurface, image->getDFBSurface(), NULL, _myWidget->absX() + x - _myWidget->z(), _myWidget->absY() + y);
        }
#else
            dfbSurface->TileBlit(dfbSurface, image->getDFBSurface(), NULL, _myWidget->absX() + x, _myWidget->absY() + y);
#endif
        else
            dfbSurface->TileBlit(dfbSurface, image->getDFBSurface(), NULL, x, y);
    }
}

void
Painter::tileImage(Image* image, int x, int y, const Rectangle& source, const DFBSurfaceBlittingFlags& flags)
{
    if ((_state & Active) && image)
    {
        applyBrush();
        if (!(image->getCaps() & DICAPS_ALPHACHANNEL))
            dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (flags & ~DSBLIT_BLEND_ALPHACHANNEL));
        else
            dfbSurface->SetBlittingFlags(dfbSurface, flags);
        DFBRectangle r = source.dfbRect();
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
#ifdef ILIXI_STEREO_OUTPUT
        {
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                dfbSurface->TileBlit(dfbSurface, image->getDFBSurface(), &r, _myWidget->absX() + x + _myWidget->z(), _myWidget->absY() + y);
            else
                dfbSurface->TileBlit(dfbSurface, image->getDFBSurface(), &r, _myWidget->absX() + x - _myWidget->z(), _myWidget->absY() + y);
        }
#else
            dfbSurface->TileBlit(dfbSurface, image->getDFBSurface(), &r, _myWidget->absX() + x, _myWidget->absY() + y);
#endif
        else
            dfbSurface->TileBlit(dfbSurface, image->getDFBSurface(), &r, x, y);
    }
}

void
Painter::blitImage(Image* image, const Rectangle& source, int x, int y, const DFBSurfaceBlittingFlags& flags)
{
    ILOG_TRACE(ILX_PAINTER);
    if ((_state & Active) && image)
    {
        applyBrush();
        if (!(image->getCaps() & DICAPS_ALPHACHANNEL))
            dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (flags & ~DSBLIT_BLEND_ALPHACHANNEL));
        else
            dfbSurface->SetBlittingFlags(dfbSurface, flags);
        DFBRectangle r = source.dfbRect();
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
#ifdef ILIXI_STEREO_OUTPUT
        {
            if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                dfbSurface->Blit(dfbSurface, image->getDFBSurface(), &r, _myWidget->absX() + x + _myWidget->z(), _myWidget->absY() + y);
            else
                dfbSurface->Blit(dfbSurface, image->getDFBSurface(), &r, _myWidget->absX() + x - _myWidget->z(), _myWidget->absY() + y);
        }
#else
            dfbSurface->Blit(dfbSurface, image->getDFBSurface(), &r, _myWidget->absX() + x, _myWidget->absY() + y);
#endif
        else
            dfbSurface->Blit(dfbSurface, image->getDFBSurface(), &r, x, y);
    }
}

void
Painter::batchBlitImage(Image* image, const DFBRectangle* sourceRects, const DFBPoint* points, int num, const DFBSurfaceBlittingFlags& flags)
{
    if ((_state & Active) && image)
    {
        applyBrush();
        if (!(image->getCaps() & DICAPS_ALPHACHANNEL))
            dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (flags & ~DSBLIT_BLEND_ALPHACHANNEL));
        else
            dfbSurface->SetBlittingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
        {
            DFBPoint dfbP[num];
            for (int i = 0; i < num; ++i)
            {
#ifdef ILIXI_STEREO_OUTPUT
                if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                    dfbP[i].x = points[i].x + _myWidget->absX() + _myWidget->z();
                else
                    dfbP[i].x = points[i].x + _myWidget->absX() - _myWidget->z();
#else
                dfbP[i].x = points[i].x + _myWidget->absX();
#endif
                dfbP[i].y = points[i].y + _myWidget->absY();
            }

            dfbSurface->BatchBlit(dfbSurface, image->getDFBSurface(), sourceRects, dfbP, num);
        } else
            dfbSurface->BatchBlit(dfbSurface, image->getDFBSurface(), sourceRects, points, num);
    }
}

void
Painter::batchBlitImage(Image* image, const Rectangle* sourceRects, const Point* points, int num, const DFBSurfaceBlittingFlags& flags)
{
    if ((_state & Active) && image)
    {
        applyBrush();
        if (!(image->getCaps() & DICAPS_ALPHACHANNEL))
            dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (flags & ~DSBLIT_BLEND_ALPHACHANNEL));
        else
            dfbSurface->SetBlittingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
        {
            DFBRectangle dfbR[num];
            DFBPoint dfbP[num];
            for (int i = 0; i < num; ++i)
            {
                dfbR[i] = sourceRects[i].dfbRect();
#ifdef ILIXI_STEREO_OUTPUT
                if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                    dfbP[i].x = points[i].x() + _myWidget->absX() + _myWidget->z();
                else
                    dfbP[i].x = points[i].x() + _myWidget->absX() - _myWidget->z();
#else
                dfbP[i].x = points[i].x() + _myWidget->absX();
#endif
                dfbP[i].y = points[i].y() + _myWidget->absY();
            }

            dfbSurface->BatchBlit(dfbSurface, image->getDFBSurface(), dfbR, dfbP, num);
        } else
        {
            DFBRectangle dfbR[num];
            DFBPoint dfbP[num];
            for (int i = 0; i < num; ++i)
            {
                dfbR[i] = sourceRects[i].dfbRect();
                dfbP[i].x = points[i].x();
                dfbP[i].y = points[i].y();
            }

            dfbSurface->BatchBlit(dfbSurface, image->getDFBSurface(), dfbR, dfbP, num);
        }
    }
}

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)

void Painter::batchStretchBlitImage(Image* image, const DFBRectangle* sourceRects, const DFBRectangle* destRects, int num, const DFBSurfaceBlittingFlags& flags)
{
    if ((_state & Active) && image)
    {
        applyBrush();
        if (!(image->getCaps() & DICAPS_ALPHACHANNEL))
        dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (flags & ~DSBLIT_BLEND_ALPHACHANNEL));
        else
        dfbSurface->SetBlittingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
        {
            DFBRectangle dfbR[num];
            for (int i = 0; i < num; ++i)
            {
#ifdef ILIXI_STEREO_OUTPUT
                if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                    dfbR[i].x = destRects[i].x + _myWidget->absX() + _myWidget->z();
                else
                    dfbR[i].x = destRects[i].x + _myWidget->absX() - _myWidget->z();
#else
                dfbR[i].x = destRects[i].x + _myWidget->absX();
#endif
                dfbR[i].y = destRects[i].y + _myWidget->absY();
                dfbR[i].w = destRects[i].w;
                dfbR[i].h = destRects[i].h;
            }

            dfbSurface->BatchStretchBlit(dfbSurface, image->getDFBSurface(), sourceRects, dfbR, num);
        } else
        dfbSurface->BatchStretchBlit(dfbSurface, image->getDFBSurface(), sourceRects, destRects, num);
    }
}

void
Painter::batchStretchBlitImage(Image* image, const Rectangle* sourceRects, const Rectangle* destRects, int num, const DFBSurfaceBlittingFlags& flags)
{
    if ((_state & Active) && image)
    {
        applyBrush();
        if (!(image->getCaps() & DICAPS_ALPHACHANNEL))
            dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (flags & ~DSBLIT_BLEND_ALPHACHANNEL));
        else
            dfbSurface->SetBlittingFlags(dfbSurface, flags);
        if (_myWidget->surface()->flags() & Surface::SharedSurface)
        {
            DFBRectangle dfbS[num];
            DFBRectangle dfbD[num];
            for (int i = 0; i < num; ++i)
            {
                dfbS[i] = sourceRects[i].dfbRect();
#ifdef ILIXI_STEREO_OUTPUT
                if (_myWidget->surface()->stereoEye() == PaintEvent::LeftEye)
                    dfbD[i].x = destRects[i].x() + _myWidget->absX() + _myWidget->z();
                else
                    dfbD[i].x = destRects[i].x() + _myWidget->absX() - _myWidget->z();
#else
                dfbD[i].x = destRects[i].x() + _myWidget->absX();
#endif
                dfbD[i].x = destRects[i].x() + _myWidget->absX();
                dfbD[i].y = destRects[i].y() + _myWidget->absY();
                dfbD[i].w = destRects[i].width();
                dfbD[i].h = destRects[i].height();
            }

            dfbSurface->BatchStretchBlit(dfbSurface, image->getDFBSurface(), dfbS, dfbD, num);
        } else
        {
            DFBRectangle dfbS[num];
            DFBRectangle dfbD[num];
            for (int i = 0; i < num; ++i)
            {
                dfbS[i] = sourceRects[i].dfbRect();
                dfbD[i] = destRects[i].dfbRect();
            }

            dfbSurface->BatchStretchBlit(dfbSurface, image->getDFBSurface(), dfbS, dfbD, num);
        }
    }
}
#endif

void
Painter::setClip(int x, int y, int w, int h)
{
    setClip(Rectangle(x, y, w, h));
}

void
Painter::setClip(const Rectangle& rect)
{
    if (_state & Active)
    {
        _myWidget->surface()->clip(rect);
        _state = (PainterFlags) (_state | Clipped);
    }
}

void
Painter::resetClip()
{
    if (_state & Clipped)
    {
        _myWidget->surface()->resetClip();
        _state = (PainterFlags) (_state & ~Clipped);
    }
}

void
Painter::setBrush(const Brush& brush)
{
    _brush = brush;
    _brush.applyBrush(dfbSurface);
    ILOG_DEBUG(ILX_PAINTER, "setBrush() %p\n", this);
}

void
Painter::setBrush(const Color& color)
{
    _brush.setColor(color);
    _brush.applyBrush(dfbSurface);
    ILOG_DEBUG(ILX_PAINTER, "setBrush() %p\n", this);
}

void
Painter::setFont(const Font& font)
{
    _font = font;
    _state = (PainterFlags) (_state | FontModified);
    ILOG_DEBUG(ILX_PAINTER, "setFont() %p\n", this);
}

void
Painter::setPen(const Pen& pen)
{
    _pen = pen;
    ILOG_DEBUG(ILX_PAINTER, "setPen() %p\n", this);
}

void
Painter::setPen(const Color& color)
{
    _pen.setColor(color);
    ILOG_DEBUG(ILX_PAINTER, "setPen() %p\n", this);
}

void
Painter::setAffine2D(const Affine2D& affine2D)
{
    if (_state & Active)
    {
        if (!_affine)
            _affine = new Affine2D(affine2D);
        else
            *_affine *= affine2D;

        dfbSurface->SetRenderOptions(dfbSurface, DSRO_MATRIX);
        int32_t* tmp = affine2D.m();
        dfbSurface->SetMatrix(dfbSurface, tmp);
        delete tmp;
        _state = (PainterFlags) (_state | Transformed);
        ILOG_DEBUG(ILX_PAINTER, "setAffine2D() %p\n", this);
    }
}

Size
Painter::textExtents(const std::string& text, int bytes)
{
    return _font.extents(text, bytes);
}

void
Painter::applyBrush()
{
    if (!(_state & BrushActive))
    {
        _brush.applyBrush(dfbSurface);
        _state = (PainterFlags) (_state | BrushActive);
    }
}

void
Painter::applyFont()
{
    if (_state & FontModified)
    {
        if (!_font.applyFont(dfbSurface))
            _myWidget->stylist()->defaultFont()->applyFont(dfbSurface);
    } else
        _myWidget->stylist()->defaultFont()->applyFont(dfbSurface);
}

void
Painter::applyPen()
{
    if (_state & BrushActive)
    {
        _pen.applyPen(dfbSurface);
        _state = (PainterFlags) (_state & ~BrushActive);
    }
}

} /* namespace ilixi */
