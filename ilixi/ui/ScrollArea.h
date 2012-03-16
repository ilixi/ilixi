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

#ifndef ILIXI_SCROLLAREA_H_
#define ILIXI_SCROLLAREA_H_

#include "ui/Widget.h"
#include <queue>

namespace ilixi
{
  //! A scroll area.
  /*!
   * This class provides a way to scroll over a large content using pointer.
   * By default, smooth scrolling is off.
   */
  class ScrollArea : public Widget
  {
  public:
    //! These options control the functionality of ScrollArea.
    enum ScrollAreaOptions
    {
      HorizontalAuto = 0x001, //!< Makes horizontal thumb visible automatically.
      HorizontalAlways = 0x002, //!< Makes horizontal thumb always visible.
      HorizontalScroll = 0x004, //!< Whether horizontal thumb is visible or not.
      VerticalAuto = 0x008, //!< Makes vertical thumb visible automatically.
      VerticalAlways = 0x010, //!< Makes vertical thumb always visible automatically.
      VerticalScroll = 0x020, //!< Whether vertical thumb is visible or not.
      SmoothScrolling = 0x040, //!< Content has its own surface.
      TargetedScroll = 0x080,   //!< Alters scrolling mode, used for scrolling to desired coordinates.
      DrawFrame = 0x100
    };

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
     * Sets whether horizontal and vertical thumbs are always visible.
     */
    void
    setScollerAlways(bool horizontal, bool vertical);

    /*!
     * Sets smooth scrolling on or off.
     */
    void
    setSmoothScrolling(bool smoothScroll);

    /*!
     * Scrolls to given local coordinates.
     */
    void
    scrollTo(int x, int y);

    /*!
     * Scrolls to widget, and centers the view.
     */
    void
    scrollTo(Widget* widget);

    /*!
     * Paints scroll area.
     */
    virtual void
    paint(const Rectangle& rect);

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
    compose();
  private:
    //! This property stores the options for ScrollArea.
    u8 _options;
    //! This is the content of scroll area.
    Widget* _content;

    //! This value is used for tweening.
    float _aniVal;
    //! This property stores the X coordinate of content.
    float _cx;
    //! This property stores the Y coordinate of content.
    float _cy;
    //! This property stores velocity in horizontal direction.
    double _vx;
    //! This property stores velocity in vertical direction.
    double _vy;

    //! This tween animation is essentially used for fading thumbs.
    TweenAnimation* _ani;

    //! Current point.
    Point _sCur;
    //! Previous point.
    Point _sPre;
    //! Content can not go beyond this point.
    Rectangle _sMax;
    //! This rectangle stores the thumbs.
    Rectangle _thumbs;

    //! This is used to calculate weighted average velocity.
    std::queue<PointerEvent> _events;

    //! Sets helper variables and dimension of content.
    void
    updateScollAreaGeometry();

    //! Updates position of content.
    void
    updateScrollArea(int step);
  };

}

#endif /* ILIXI_SCROLLAREA_H_ */
