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

#ifndef ILIXI_SCROLLAREA_H_
#define ILIXI_SCROLLAREA_H_

#include <ui/ScrollBar.h>
#include <queue>

namespace ilixi
{
class Tween;

//! A scroll area.
/*!
 * This class provides a way to scroll over a large content using pointer.
 * By default, smooth scrolling is off.
 */
class ScrollArea : public Widget
{
public:
    /*!
     * Constructor.
     */
    ScrollArea(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~ScrollArea();

    /*!
     * Returns height for width for content.
     */
    virtual int
    heightForWidth(int width) const;

    /*!
     * Returns default dimension.
     */
    virtual Size
    preferredSize() const;

    /*!
     * Returns whether frame is drawn.
     */
    bool
    drawFrame() const;

    /*!
     * Sets the content for scroll area.
     *
     * Warning: Previous widget will be destroyed implicitly.
     */
    void
    setContent(Widget* content);

    /*!
     * Sets whether auto scrolling is on or off.
     */
    void
    setAutoScrolling(bool autoScroll);

    /*!
     * Sets whether to use thumbs or bars.
     */
    void
    setUseThumbs(bool useThumbs);

    /*!
     * Sets smooth scrolling on or off.
     */
    void
    setSmoothScrolling(bool smoothScroll);

    /*!
     * Sets whether frame is drawn.
     */
    void
    setDrawFrame(bool drawFrame);

    /*!
     * Scrolls to given local coordinates.
     */
    void
    scrollTo(int x, int y);

    /*!
     * Scrolls to widget, and centers the view.
     *
     * @param center if true, widget will be centered inside visible rectangle.
     */
    void
    scrollTo(Widget* widget, bool center = false);

    /*!
     * Paints scroll area.
     */
    virtual void
    paint(const PaintEvent& event);

    virtual void
    doLayout();

protected:
    /*!
     * Gets ready for scrolling.
     */
    virtual void
    pointerGrabEvent(const PointerEvent& event);

    /*!
     * Calculates weighted average velocity and starts scrolls.
     */
    virtual void
    pointerReleaseEvent(const PointerEvent& event);

    /*!
     * Scrolls to current pointer position.
     */
    virtual void
    pointerMotionEvent(const PointerEvent& event);

    /*!
     * Scrolls in vertical direction.
     */
    virtual void
    pointerWheelEvent(const PointerEvent& event);

    /*!
     * Draws scroll thumbs.
     */
    virtual void
    compose(const PaintEvent& event);
private:
    //! These options control the functionality of ScrollArea.
    enum ScrollAreaOptions
    {
        DrawFrame = 0x00001,                //!< Draws frame around scroll area

        SmoothScrolling = 0x00100,          //!< Content has its own surface.
        TargetedScroll = 0x00200,           //!< Alters scrolling mode, used for scrolling to desired coordinates using animation.

        HasHorizontal = 0x00002,            //!< Flag controlling drawing of horizontal thumb
        HasVertical = 0x00004,              //!< Flag controlling drawing of vertical thumb
        UseBars = 0x00008,                  //!< Flag controlling drawing of bars

        HorizontalAlways = 0x00010,         //!< Makes horizontal thumb/bar always visible.
        HorizontalAuto = 0x00020,           //!< Makes horizontal thumb/bar visible automatically.
        HorizontalScrollEnabled = 0x00040,  //!< Whether horizontal scrolling is enabled.

        VerticalAlways = 0x01000,           //!< Makes vertical thumb/bar always visible automatically.
        VerticalAuto = 0x02000,             //!< Makes vertical thumb/bar visible automatically.
        VerticalScrollEnabled = 0x04000,    //!< Whether vertical scrolling is enabled.
        ContentWasScrolled = 0x08000
    };

    //! This property stores the options for ScrollArea.
    int _options;
    //! This is the content of scroll area.
    Widget* _content;
    //! This property stores the horizontal bar.
    ScrollBar* _horizontalBar;
    //! This property stores the vertical bar.
    ScrollBar* _verticalBar;

    //! Animation for sliding in x and y directions..
    TweenAnimation* _ani;
    //! Tween for x axis.
    Tween* _xTween;
    //! Tween for y axis.
    Tween* _yTween;

    //! Content can not go beyond this point.
    Rectangle _sMax;
    //! This rectangle stores the thumbs.
    Size _thumbs;

    //! This is used to calculate weighted average velocity.
    std::queue<PointerEvent> _events;

    void
    updateHDraws(int contentWidth);

    void
    updateVDraws(int contentHeight);

    //! Sets helper variables and dimension of content.
    void
    updateScollAreaGeometry();

    //! Updates position of content.
    void
    updateScrollArea();

    void
    barScrollX(int x);

    void
    barScrollY(int y);
};

}

#endif /* ILIXI_SCROLLAREA_H_ */
