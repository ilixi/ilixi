#ifndef ILIXI_LIST_H_
#define ILIXI_LIST_H_

#include <ui/ContainerBase.h>
#include <lib/TweenAnimation.h>

namespace ilixi
{

class HorizontalScroller : public ContainerBase
{
    friend class HorizontalSwitcher;
public:
    HorizontalScroller(Widget* parent = 0);

    virtual
    ~HorizontalScroller();

    /*!
     * Scrolls to widget, positions it to top left.
     */
    void
    scrollTo(Widget* widget);

    void
    scrollToNext();

    void
    scrollToPrevious();

    void
    setOffset(const Point& offset);

protected:
    virtual void
    compose(const PaintEvent& event);

private:
    TweenAnimation _anim;
    Tween* _tween;
    Widget* _selectedWidget;
    Point _offset;

    void
    tweenSlot();

    //! Updates position of content.
    void
    updateLayoutGeometry();
};

} /* namespace ilixi */
#endif /* LIST_H_ */
