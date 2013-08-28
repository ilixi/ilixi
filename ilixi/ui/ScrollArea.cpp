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
          _options(HorizontalAuto | VerticalAuto | UseBars),
          _content(NULL),
          _horizontalBar(NULL),
          _verticalBar(NULL)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    setInputMethod((WidgetInputMethod) (PointerInput | PointerTracking | PointerGrabbing));
    setConstraints(NoConstraint, NoConstraint);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &ScrollArea::updateScollAreaGeometry));

    _horizontalBar = new ScrollBar();
    _horizontalBar->sigValueChanged.connect(sigc::mem_fun(this, &ScrollArea::barScrollX));
    addChild(_horizontalBar);

    _verticalBar = new ScrollBar(Vertical);
    _verticalBar->sigValueChanged.connect(sigc::mem_fun(this, &ScrollArea::barScrollY));
    addChild(_verticalBar);

    _ani = new TweenAnimation();

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
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (_content)
        return _content->heightForWidth(width);
    return -1;
}

Size
ScrollArea::preferredSize() const
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (_content)
        return _content->preferredSize();
    return Size(100, 100);
}

bool
ScrollArea::drawFrame() const
{
    return _options & DrawFrame;
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
ScrollArea::setUseThumbs(bool useThumbs)
{
    if (useThumbs)
        _options &= ~UseBars;
    else
        _options |= UseBars;
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
ScrollArea::setDrawFrame(bool drawFrame)
{
    if (drawFrame)
        _options |= DrawFrame;
    else
        _options &= ~DrawFrame;
}

void
ScrollArea::scrollTo(int x, int y)
{
    _ani->stop();
    _xTween->setInitialValue(_xTween->value());
    _yTween->setInitialValue(_yTween->value());
    _xTween->setEndValue(x);
    _yTween->setEndValue(y);
    _options |= TargetedScroll | ContentWasScrolled;
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

    Rectangle wRect = Rectangle(widget->x() + _content->x(), widget->y() + _content->y(), widget->width(), widget->height());
    ILOG_DEBUG(ILX_SCROLLAREA, " -> wRect %d %d %d %d\n", wRect.x(), wRect.y(), wRect.width(), wRect.height());
    Rectangle sRect = Rectangle(0, 0, width(), height());
    if (_options & DrawFrame)
        sRect.setRectangle(stylist()->defaultParameter(StyleHint::LineInputLeft), stylist()->defaultParameter(StyleHint::LineInputTop), width() - stylist()->defaultParameter(StyleHint::LineInputLR), height() - stylist()->defaultParameter(StyleHint::LineInputTB));
    ILOG_DEBUG(ILX_SCROLLAREA, " -> sRect %d %d %d %d\n", sRect.x(), sRect.y(), sRect.width(), sRect.height());

    if (!center && sRect.contains(wRect, true))
    {
        ILOG_DEBUG(ILX_SCROLLAREA, " -> widget is fully visible.\n");
        return;
    } else
    {
        wRect = mapToSurface(widget->frameGeometry());
        Point endP = wRect.topLeft();

        if (wRect.right() > x() + width())
            if (_options & DrawFrame)
                endP.setX(wRect.right() - width() + stylist()->defaultParameter(StyleHint::LineInputLR));
            else
                endP.setX(wRect.right() - width());

        if (wRect.bottom() > y() + height())
            if (_options & DrawFrame)
                endP.setY(wRect.bottom() - height() + stylist()->defaultParameter(StyleHint::LineInputTB));
            else
                endP.setY(wRect.bottom() - height());

        if (center)
        {
            _xTween->setRange(_content->x(), _content->x() - endP.x() + (width() - widget->width()) / 2.0);
            _yTween->setRange(_content->y(), _content->y() - endP.y() + (height() - widget->height()) / 2.0);
        } else
        {
            _xTween->setRange(_content->x(), _content->x() - endP.x());
            _yTween->setRange(_content->y(), _content->y() - endP.y());
        }

        if (endP.x() != 0 || endP.y() != 0)
        {
            _ani->stop();
            _options |= TargetedScroll | ContentWasScrolled;
            ILOG_DEBUG(ILX_SCROLLAREA, " -> content %d, %d\n", _content->absX(), _content->absY());
            ILOG_DEBUG(ILX_SCROLLAREA, " -> endP %d, %d\n", endP.x(), endP.y());
            ILOG_DEBUG(ILX_SCROLLAREA, " -> scrolling content to %f, %f\n", _xTween->endValue(), _yTween->endValue());
            _ani->start();
        }
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
            compose(evt);

            if (_options & SmoothScrolling)
            {

                if (_content->surface())
                    _content->surface()->clear();

                _content->paint(PaintEvent(Rectangle(0, 0, _content->width(), _content->height()), evt.eye));
                _content->surface()->flip();
//                surface()->blit(_content->surface(), Rectangle(-_cx, -_cy, width(), height()), 0, 0);
            } else
            {
                _horizontalBar->paint(evt);
                _verticalBar->paint(evt);
                if (_content)
                {
                    if (_options & UseBars)
                    {
                        int w = width();
                        int h = height();
                        if (_options & HasVertical)
                            w = width() - _verticalBar->width();
                        if (_options & HasHorizontal)
                            h = height() - _horizontalBar->height();
                        Rectangle r(absX(), absY(), w, h);
                        if (_options & DrawFrame)
                            r.setRectangle(absX() + stylist()->defaultParameter(StyleHint::LineInputLeft), absY() + stylist()->defaultParameter(StyleHint::LineInputTop), w - stylist()->defaultParameter(StyleHint::LineInputLR), h - stylist()->defaultParameter(StyleHint::LineInputTB));
                        evt.rect = evt.rect.intersected(r);
                        _content->paint(evt);
                    } else
                    {
                        if (_options & DrawFrame)
                        {
                            Rectangle r(absX(), absY(), width(), height());
                            r.setRectangle(absX() + stylist()->defaultParameter(StyleHint::LineInputLeft), absY() + stylist()->defaultParameter(StyleHint::LineInputTop), width() - stylist()->defaultParameter(StyleHint::LineInputLR), height() - stylist()->defaultParameter(StyleHint::LineInputTB));
                            evt.rect = evt.rect.intersected(r);
                        }
                        _content->paint(evt);
                    }
                }
            }
            if (!(_options & UseBars) && (_ani->state() == Animation::Running))
            {
                Painter p(this);
                p.begin(event);
                if (_options & HasHorizontal)
                {
                    int x = 1 + (width() - _thumbs.width() - 12.0) * _xTween->value() / _sMax.x();
                    stylist()->drawScrollBar(&p, x, height() - stylist()->defaultParameter(StyleHint::ScrollBarHeight), _thumbs.width(), stylist()->defaultParameter(StyleHint::ScrollBarHeight), Horizontal);
                }

                if (_options & HasVertical)
                {
                    int y = 1 + (height() - _thumbs.height() - 12.0) * _yTween->value() / _sMax.y();
                    stylist()->drawScrollBar(&p, width() - stylist()->defaultParameter(StyleHint::ScrollBarWidth), y, stylist()->defaultParameter(StyleHint::ScrollBarWidth), _thumbs.height(), Vertical);
                }
            }
        }
    }
}

void
ScrollArea::doLayout()
{
    updateScollAreaGeometry();
    if (parent())
        parent()->doLayout();
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
    if (_options & UseBars)
    {
        if (_options & VerticalScrollEnabled)
            _verticalBar->setValue(_verticalBar->value() - event.wheelStep * _verticalBar->step());
        else if (_options & HorizontalScrollEnabled)
            _horizontalBar->setValue(_horizontalBar->value() - event.wheelStep * _horizontalBar->step());
    } else
    {
        _ani->stop();
        if (_options & VerticalScrollEnabled)
        {
            int y = _yTween->endValue() + height() / 10 * event.wheelStep;
            if (y < height() - _content->height())
                y = height() - _content->height();
            else if (y > 0)
                y = 0;
            _yTween->setRange(_content->y(), y);
        } else if (_options & HorizontalScrollEnabled)
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
}

void
ScrollArea::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (_options & DrawFrame)
    {
        Painter p(this);
        p.begin(event);
        stylist()->drawScrollArea(&p, this, _sMax);
        p.end();
    }
}

void
ScrollArea::updateHDraws(int contentWidth)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (_options & HorizontalAlways)
        _options |= (HorizontalScrollEnabled | HasHorizontal);
    else if ((_options & HorizontalAuto) && contentWidth > width())
        _options |= (HorizontalScrollEnabled | HasHorizontal);
    else
        _options &= ~(HorizontalScrollEnabled | HasHorizontal);

    if ((_options & UseBars) && (_options & HasHorizontal))
    {
        int val = contentWidth - width();
        if (_options & DrawFrame)
            val += stylist()->defaultParameter(StyleHint::LineInputLR);
        if (val < 0)
            _horizontalBar->setDisabled();
        else
            _horizontalBar->setEnabled();
        _horizontalBar->setVisible(true);
        _horizontalBar->setRange(0, val);
        _horizontalBar->setStep(val / 10 + val % 10);

    } else
        _horizontalBar->setVisible(false);
}

void
ScrollArea::updateVDraws(int contentHeight)
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (_options & VerticalAlways)
        _options |= (VerticalScrollEnabled | HasVertical);
    else if ((_options & VerticalAuto) && contentHeight > height())
        _options |= (VerticalScrollEnabled | HasVertical);
    else
        _options &= ~(VerticalScrollEnabled | HasVertical);

    if ((_options & UseBars) && (_options & HasVertical))
    {
        int val = contentHeight - height();
        if (_options & DrawFrame)
            val += stylist()->defaultParameter(StyleHint::LineInputTB);
        if (val < 0)
            _verticalBar->setDisabled();
        else
            _verticalBar->setEnabled();
        _verticalBar->setVisible(true);
        _verticalBar->setRange(0, val);
        _verticalBar->setStep(val / 10 + val % 10);
    } else
        _verticalBar->setVisible(false);
}

void
ScrollArea::updateScollAreaGeometry()
{
    ILOG_TRACE_W(ILX_SCROLLAREA);
    if (!_content)
        return;

    int w = width();
    int h = height();

    if (!(_options & ContentWasScrolled))
    {
        if (_options & DrawFrame)
        {
            _content->moveTo(stylist()->defaultParameter(StyleHint::LineInputLeft), stylist()->defaultParameter(StyleHint::LineInputTop));
            w -= stylist()->defaultParameter(StyleHint::LineInputLR);
            h -= stylist()->defaultParameter(StyleHint::LineInputTB);
        } else
            _content->moveTo(0, 0);
    }
    _content->sendToBack();
    _content->setNeighbours(getNeighbour(Up), getNeighbour(Down), getNeighbour(Left), getNeighbour(Right));

    Size contentSize = _content->preferredSize();

    if (contentSize.isValid())
    {
        _content->setSize(contentSize);
        _sMax = Rectangle(w - _content->width(), h - _content->height(), w / 3, h / 3);
        _thumbs = Size(w * w / _content->width() - 4, h * h / _content->height() - 4);
        updateHDraws(_content->width());
        updateVDraws(_content->height());

        if (_options & UseBars)
        {
            Size vS = Size(0, 0);
            if (_options & HasVertical)
            {
                vS = _verticalBar->preferredSize();
                _verticalBar->setGeometry(width() - vS.width(), 0, vS.width(), height());
                _verticalBar->setNeighbours(getNeighbour(Up), getNeighbour(Down), _content, getNeighbour(Right));
                _content->setNeighbour(Right, _verticalBar);
                _sMax.setWidth(width() - _verticalBar->width());
            } else
                _sMax.setWidth(width());

            Size hS = Size(0, 0);
            if (_options & HasHorizontal)
            {
                hS = _horizontalBar->preferredSize();
                _horizontalBar->setGeometry(0, h - hS.height(), w - vS.width(), hS.height());
                _horizontalBar->setNeighbours(_content, getNeighbour(Down), getNeighbour(Left), getNeighbour(Right));
                _content->setNeighbour(Down, _horizontalBar);
                _sMax.setHeight(height() - _horizontalBar->height());
            } else
                _sMax.setHeight(height());

            w -= vS.width();
            h -= hS.height();
        }
    }

    if (_content->width() < w && (_content->xConstraint() & GrowPolicy))
        _content->setWidth(w);

    if (_content->height() < h && (_content->yConstraint() & GrowPolicy))
        _content->setHeight(h);
}

void
ScrollArea::updateScrollArea()
{
    if (pressed())
    {
    } else if (_options & TargetedScroll)
    {
        if (_options & HorizontalScrollEnabled)
        {
            if (_options & UseBars)
                _horizontalBar->setValue(-_xTween->value());
            else
                _content->setX(_xTween->value());
        }
        if (_options & VerticalScrollEnabled)
        {
            if (_options & UseBars)
                _verticalBar->setValue(-_yTween->value());
            else
                _content->setY(_yTween->value());

        }
        ILOG_DEBUG(ILX_SCROLLAREA, " -> content at %f, %f\n", _xTween->value(), _yTween->value());
    } else
    {
    }

    update();
}

void
ScrollArea::barScrollX(int x)
{
    if (_options & DrawFrame)
        _content->setX(-x + stylist()->defaultParameter(StyleHint::LineInputLeft));
    else
        _content->setX(-x);
    update();
}

void
ScrollArea::barScrollY(int y)
{
    if (_options & DrawFrame)
        _content->setY(-y + stylist()->defaultParameter(StyleHint::LineInputTop));
    else
        _content->setY(-y);
    update();
}

} /* namespace ilixi */
