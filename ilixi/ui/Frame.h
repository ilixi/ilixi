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

#ifndef ILIXI_FRAME_H_
#define ILIXI_FRAME_H_

#include "ui/ContainerBase.h"
#include "ui/BorderBase.h"
#include "types/Margin.h"

namespace ilixi
{
  //! Base class of widgets with frames and possible children.
  class Frame : public ContainerBase, public BorderBase
  {
  public:

    /*!
     * Constructor.
     *
     * @param parent
     */
    Frame(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~Frame();

    /*!
     * Returns a height for frame if its contents are fit inside width.
     *
     * @param width Desired width of frame.
     */
    virtual int
    heightForWidth(int width) const;

    /*!
     * Calculates the preferred size of contents and returns an updated size using canvas, margins and borders.
     */
    virtual Size
    preferredSize() const;

    /*!
     * Returns true if frame paints its background.
     */
    bool
    backgroundFilled() const;

    /*!
     * Returns container's margin.
     */
    Margin
    margin() const;

    /*!
     * Sets whether frame paints its background.
     *
     * @param fill
     */
    void
    setBackgroundFilled(bool fill);

    /*!
     * Sets the canvas position of this container. Layout is positioned at this point.
     *
     * @param topLeft
     */
    void
    setCanvasPosition(const Point& topLeft);

    /*!
     * Set layout's margins.
     *
     * @param top Margin top.
     * @param bottom Margin bottom.
     * @param left Margin left.
     * @param right Margin right.
     */
    void
    setMargins(int top, int bottom, int left, int right);

    /*!
     * Set container's margin.
     *
     * @param margin Margins.
     */
    void
    setMargin(const Margin& margin);

  protected:
    enum BackgroundFlags
    {
      BGFNone = 0x000, BGFClear = 0x001, BGFFill = 0x002, BGFAll = 0x003
    };
    //! This flag specifies whether frame fills its background.
    char _backgroundFlags;
    //! This property holds the margin of container.
    Margin _margin;
    //! This property holds the top-left point of canvas.
    Point _canvasTopLeft;

    /*!
     * Reimplemented from ContainerBase.
     *
     * This method sets layout's geometry. Layout is positioned at canvasX(), canvasY() and its width and height are
     * set to canvasWidth() and canvasHeight() of frame respectively.
     */
    virtual void
    updateContainerGeometry();

    /*!
     * Draw frame borders.
     *
     * @param rect
     */
    virtual void
    compose(const PaintEvent& event);

    /*!
     * Returns frame's canvas x-coordinate including the left margin.
     */
    virtual int
    canvasX() const;

    /*!
     * Returns frame's canvas y-coordinate including the top margin.
     */
    virtual int
    canvasY() const;

    /*!
     * Returns frame's canvas height excluding margins.
     */
    virtual int
    canvasHeight() const;

    /*!
     * Returns frame's canvas width excluding margins.
     */
    virtual int
    canvasWidth() const;
  };
}

#endif /* ILIXI_FRAME_H_ */
