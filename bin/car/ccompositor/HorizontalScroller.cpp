#include "HorizontalScroller.h"
#include <ui/HBoxLayout.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_HSCROLLER, "ilixi/comp/HScroller", "HorizontalScroller");

HorizontalScroller::HorizontalScroller(Widget* parent)
        : ContainerBase(parent),
          _selectedWidget(NULL)
{
    setLayout(new HBoxLayout());
    _anim.setDuration(500);
    _anim.sigExec.connect(sigc::mem_fun(this, &HorizontalScroller::tweenSlot));
    _tween = new Tween(Tween::CIRCLE, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_tween);
}

HorizontalScroller::~HorizontalScroller()
{
}

void
HorizontalScroller::scrollTo(Widget* widget)
{
    if (widget && widget != _selectedWidget)
    {
        ILOG_TRACE_W(ILX_HSCROLLER);
        ILOG_DEBUG(ILX_HSCROLLER, " -> _selectedWidget: %p\n", _selectedWidget);
        if (_selectedWidget)
        {
            _anim.stop();
            _tween->setInitialValue(-_selectedWidget->x());
            _tween->setEndValue(-widget->x());
            _anim.start();
        }
        _selectedWidget = widget;
    }
}

void
HorizontalScroller::scrollToNext()
{

}

void
HorizontalScroller::scrollToPrevious()
{

}

void
HorizontalScroller::setOffset(const Point& offset)
{
    _offset = offset;
}

void
HorizontalScroller::compose(const PaintEvent& event)
{
}

void
HorizontalScroller::tweenSlot()
{
    _layout->setX(_offset.x() + _tween->value());
    update();
}

void
HorizontalScroller::updateLayoutGeometry()
{
    Size s = _layout->preferredSize();
    if (_selectedWidget)
    {
        ILOG_TRACE_W(ILX_HSCROLLER);
        _layout->setGeometry(_offset.x() - _selectedWidget->x(),
                             _offset.y() + _selectedWidget->y(), s.width(),
                             height());
    } else
        _layout->setGeometry(_offset.x(), _offset.y(), s.width(), height());
}

} /* namespace ilixi */
