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

#include <ui/ScrollArea.h>
#include <graphics/Painter.h>
#include <lib/TweenAnimation.h>
#include <core/Logger.h>
#include <cmath>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SCROLLAREA, "ilixi/ui/ScrollArea", "ScrollArea");

ScrollArea::ScrollArea(Widget* parent)
        : Widget(parent),
          _options(HorizontalAuto | VerticalAuto),
          _content(NULL)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    setInputMethod(PointerTracking);
    setConstraints(NoConstraint, NoConstraint);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &ScrollArea::updateScollAreaGeometry));

    _ani = new TweenAnimation();
    _tween = new Tween(Tween::BACK, Tween::EASE_OUT, 1, 0);
    _ani->addTween(_tween);

    _barTween = new Tween(Tween::LINEAR, Tween::EASE_OUT, 1, 0);
    _ani->addTween(_barTween);

    _xTween = new Tween(Tween::BACK, Tween::EASE_OUT, 0, 0);
    _ani->addTween(_xTween);

    _yTween = new Tween(Tween::BACK, Tween::EASE_OUT, 0, 0);
    _ani->addTween(_yTween);

    _ani->setDuration(300);
    _ani->sigExec.connect(sigc::mem_fun(this, &ScrollArea::updateScrollArea));
}

ScrollArea::~ScrollArea()
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    delete _ani;
}

int
ScrollArea::heightForWidth(int width) const
{
    if (_content)
        return _content->heightForWidth(width);
    return -1;
}

Size
ScrollArea::preferredSize() const
{
    if (_content)
        return _content->preferredSize();
    return Size(100, 100);
}

void
ScrollArea::setContent(Widget* content)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (content)
    {
        removeChild(_content);
        _content = content;
        if (_options & SmoothScrolling)
            surface()->setSurfaceFlag(Surface::BlitDescription);
        addChild(_content);
        raiseChildToFront(_content);
        doLayout();
    }
}

void
ScrollArea::setScollerAlways(bool horizontal, bool vertical)
{
    if (horizontal)
        _options |= HorizontalAlways;
    else
        _options &= ~HorizontalAlways;

    if (vertical)
        _options |= VerticalAlways;
    else
        _options &= ~VerticalAlways;
}

void
ScrollArea::setAutoScrolling(bool autoScroll)
{
    if (autoScroll)
        _options |= (HorizontalAuto | VerticalAuto);
    else
        _options &= ~(HorizontalAuto | VerticalAuto);
}

void
ScrollArea::setSmoothScrolling(bool smoothScroll)
{
    if (smoothScroll)
    {
        _options |= SmoothScrolling;
        surface()->setSurfaceFlag(Surface::BlitDescription);
    } else
    {
        _options &= ~SmoothScrolling;
        surface()->unsetSurfaceFlag(Surface::HasOwnSurface);
        surface()->setSurfaceFlag(Surface::DefaultDescription);
    }
}

void
ScrollArea::scrollTo(int x, int y)
{
    _ani->stop();
    _xTween->setInitialValue(_xTween->value());
    _yTween->setInitialValue(_yTween->value());
    _xTween->setEndValue(x);
    _yTween->setEndValue(y);
    _options |= TargetedScroll;
    _ani->start();
}

void
ScrollArea::scrollTo(Widget* widget, bool center)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (widget == NULL)
        return;

    if (!_content->isChild(widget))
        return;

    Rectangle wRect = Rectangle(_content->x() + widget->x(), _content->y() + widget->y(), widget->width(), widget->height());
    if (!center && surfaceGeometry().contains(wRect, true))
    {
        ILOG_DEBUG(ILX_SCROLLAREA, " -> widget is fully visible.\n");
        return;
    } else
    {
        Point endP = wRect.topLeft();
        if (wRect.right() > x() + width())
            endP.setX(wRect.right() - (x() + width()));

        if (wRect.bottom() > y() + height())
            endP.setY(wRect.bottom() - (y() + height()));

        if (endP.x() < x())
            endP.setX(wRect.x() - x());

        if (endP.y() < y())
            endP.setY(wRect.y() - y());

        _ani->stop();

        if (center)
        {
            _xTween->setRange(_content->x(), _content->x() - endP.x() + (width() - widget->width()) / 2.0);
            _yTween->setRange(_content->y(), _content->y() - endP.y() + (height() - widget->height()) / 2.0);
        } else
        {
            _xTween->setRange(_content->x(), _content->x() - endP.x());
            _yTween->setRange(_content->y(), _content->y() - endP.y());
        }

        _options |= TargetedScroll;
        ILOG_DEBUG(ILX_SCROLLAREA, " -> content %d, %d\n", _content->x(), _content->y());
        ILOG_DEBUG(ILX_SCROLLAREA, " -> endP %d, %d\n", endP.x(), endP.y());
        ILOG_DEBUG(ILX_SCROLLAREA, " -> scrolling content to %f, %f\n", _xTween->endValue(), _yTween->endValue());
        _ani->start();
    }
}

void
ScrollArea::paint(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (visible())
    {
        PaintEvent evt(this, event);
        if (evt.isValid())
        {
            if (_options & SmoothScrolling)
            {

                if (_content->surface())
                    _content->surface()->clear();

                _content->paint(PaintEvent(Rectangle(0, 0, _content->width(), _content->height()), evt.eye));
                _content->surface()->flip();
//                surface()->blit(_content->surface(), Rectangle(-_cx, -_cy, width(), height()), 0, 0);
            } else
                paintChildren(evt);
            compose(evt);
        }
    }
}

void
ScrollArea::doLayout()
{
    updateScollAreaGeometry();
}

void
ScrollArea::pointerGrabEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);

    _ani->stop();
    updateScrollArea();
    _options &= ~TargetedScroll;
}

void
ScrollArea::pointerReleaseEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    _ani->stop();
    float vx = 0;
    float vy = 0;
    if (_events.size() > 1)
    {
        int deltaX = 0;
        int deltaY = 0;
        int deltaT = 0;
        float k = 1.0 / _events.size();
        float w = k;
        float wT = 0;
        PointerEvent cur;
        PointerEvent pre;
        pre = _events.front();
        _events.pop();
        while (!_events.empty())
        {
            cur = _events.front();
            deltaX = cur.x - pre.x;
            deltaY = cur.y - pre.y;
            deltaT = cur.timestamp - pre.timestamp;
            vx += deltaX == 0 ? 0 : deltaT > 0 ? w * deltaX / deltaT : 0;
            vy += deltaY == 0 ? 0 : deltaT > 0 ? w * deltaY / deltaT : 0;
            pre = cur;
            w += k;
            wT += w;
            _events.pop();
        }
        vx /= wT;
        vy /= wT;
    }
    _options &= ~TargetedScroll;
    _ani->start();
}

void
ScrollArea::pointerMotionEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (pressed())
    {
//        _sCur = Point(event.x, event.y);
//        _events.push(event);
//        if (_events.size() == queueSize)
//            _events.pop();
        updateScrollArea();
    }
}

void
ScrollArea::pointerWheelEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    ILOG_DEBUG(ILX_SCROLLAREA, " -> step: %d\n", event.wheelStep);
    _ani->stop();
    if (_options & VerticalScroll)
    {
        int y = _yTween->endValue() + height() / 10 * event.wheelStep;
        if (y < height() - _content->height())
            y = height() - _content->height();
        else if (y > 0)
            y = 0;
        _yTween->setRange(_content->y(), y);
    } else if (_options & HorizontalScroll)
    {
        int x = _xTween->endValue() + width() / 10 * event.wheelStep;
        if (x < width() - _content->width())
            x = width() - _content->width();
        else if (x > 0)
            x = 0;
        _xTween->setRange(_content->x(), x);
    }
    _options |= TargetedScroll;
    _ani->start();
}

void
ScrollArea::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    Painter p(this);
    p.begin(event);

    if (_options & DrawFrame)
        stylist()->drawScrollArea(&p, this);

    if (_barTween->value())
    {
        p.setBrush(Color(0, 0, 0, _barTween->value() * 255));
        if (_options & DrawHorizontalThumb)
        {
            int y = _thumbs.y();
            int h = 4;
            int w = _thumbs.width();
            int x = 1 + (width() - w - 12.0) * _xTween->value() / _sMax.x();
            ILOG_DEBUG(ILX_SCROLLAREA, " -> Horizontal: %d, %d, %d, %d\n", x, y, w, h);
            stylist()->drawScrollBar(&p, x, y, w, h, Horizontal);
        }

        if (_options & DrawVerticalThumb)
        {
            int x = _thumbs.x();
            int w = 4;
            int h = _thumbs.height();
            int y = 1 + (height() - h - 12.0) * _yTween->value() / _sMax.y();
            ILOG_DEBUG(ILX_SCROLLAREA, " -> Vertical: %d, %d, %d, %d\n", x, y, w, h);
            stylist()->drawScrollBar(&p, x, y, w, h, Vertical);
        }
    }
}

void
ScrollArea::updateHThumb()
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (_options & HorizontalAlways)
        _options |= (HorizontalScroll | DrawHorizontalThumb);
    else if ((_options & HorizontalAuto) && _content->width() > width())
        _options |= (HorizontalScroll | DrawHorizontalThumb);
    else
        _options &= ~(HorizontalScroll | DrawHorizontalThumb);
}

void
ScrollArea::updateVThumb()
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (_options & VerticalAlways)
        _options |= (VerticalScroll | DrawVerticalThumb);
    else if ((_options & VerticalAuto) && _content->height() > height())
        _options |= (VerticalScroll | DrawVerticalThumb);
    else
        _options &= ~(VerticalScroll | DrawVerticalThumb);
}

void
ScrollArea::updateScollAreaGeometry()
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (!_content)
        return;

    _content->moveTo(0, 0);
    Size contentSize = _content->preferredSize();
    if (contentSize.isValid())
    {
        _content->setSize(contentSize);

        _sMax = Rectangle(width() - _content->width(), height() - _content->height(), width() / 3, height() / 3);

        _thumbs = Rectangle(width() - 11, height() - 11, width() * width() / _content->width() - 4, height() * height() / _content->height() - 4);
    }

    updateHThumb();

    if (_content->width() < width() && (_content->xConstraint() & GrowPolicy))
        _content->setWidth(width());

    updateVThumb();

    if (_content->height() < height() && (_content->yConstraint() & GrowPolicy))
        _content->setHeight(height());
}

void
ScrollArea::updateScrollArea()
{
    if (pressed())
    {
//        if (_options & HorizontalScroll)
//        {
//            float deltaX = _sCur.x() - _sPre.x();
//
//            if (_cx > 0)
//                deltaX *= 1.0 - (_cx / _sMax.width());
//            else if (_cx < _sMax.x())
//                deltaX *= 1.0 - ((_sMax.x() - _cx) / _sMax.width());
//
//            _cx += deltaX;
//        }
//
//        if (_options & VerticalScroll)
//        {
//            float deltaY = _sCur.y() - _sPre.y();
//
//            if (_cy > 0)
//                deltaY *= 1.0 - (_cy / _sMax.height());
//            else if (_cy < _sMax.y())
//                deltaY *= 1.0 - ((_sMax.y() - _cy) / _sMax.height());
//
//            _cy += deltaY;
//        }
//
//        _sPre = _sCur;
    } else if (_options & TargetedScroll)
    {
        if (_options & HorizontalScroll)
            _content->setX(_xTween->value());
        if (_options & VerticalScroll)
            _content->setY(_yTween->value());
        ILOG_DEBUG(ILX_SCROLLAREA, " -> content at %f, %f\n", _xTween->value(), _yTween->value());
    } else
    {
    }

    update();
}

} /* namespace ilixi */
