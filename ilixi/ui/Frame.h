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

#ifndef ILIXI_FRAME_H_
#define ILIXI_FRAME_H_

#include <ui/ContainerBase.h>
#include <types/Margin.h>

namespace ilixi
{
//! Base class of container widgets with frames.
class Frame : public ContainerBase
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

    /*!
     * Returns container's margin.
     */
    Margin
    margin() const;

    /*!
     * Set whether frame is drawn or not.
     */
    void
    setDrawFrame(bool drawFrame);

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
    //! This property holds the margin of container.
    Margin _margin;
    //! This property specifies whether frame is drawn or not.
    bool _drawFrame;

    /*!
     * Reimplemented from ContainerBase.
     *
     * This method sets layout's geometry. Layout is positioned at canvasX(), canvasY() and its width and height are
     * set to canvasWidth() and canvasHeight() of frame respectively.
     */
    virtual void
    updateLayoutGeometry();

    /*!
     * Draw frame borders.
     *
     * @param rect
     */
    virtual void
    compose(const PaintEvent& event);

};
}

#endif /* ILIXI_FRAME_H_ */
