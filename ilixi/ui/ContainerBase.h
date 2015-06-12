/*
 Copyright 2010-2015 Tarik Sekmen.

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

#ifndef ILIXI_CONTAINERBASE_H_
#define ILIXI_CONTAINERBASE_H_

#include <ui/LayoutBase.h>

namespace ilixi
{
//! An abstract base class for widgets which store and maintain various child widgets.
/*!
 * Containers are used to store widgets and align them using a Layout.
 */
class ContainerBase : public Widget
{
    friend class Application;
    friend class Dialog;
public:
    /*!
     * Constructor.
     *
     * @param parent Pointer to parent widget.
     */
    ContainerBase(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~ContainerBase();

    /*!
     * Returns a height for container if its contents are fit inside width.
     *
     * @param width Desired width of container.
     */
    virtual int
    heightForWidth(int width) const;

    /*!
     * Calculates the preferred size of contents and returns an updated size using canvas and margins.
     */
    virtual Size
    preferredSize() const;

    /*!
     * Returns the bounding rectangle around visible children.
     */
    Rectangle
    childrenRect() const;

    /*!
     * Returns spacing between widgets used by layout.
     */
    unsigned int
    spacing() const;

    /*!
     * Adds a new child widget to container. This method will automatically
     * add the widget to container's underlying layout. Setting a new layout for this container
     * will destroy widget's inside its old layout.
     *
     * @param widget Pointer to child widget.
     *
     * \sa Layout::addWidget()
     */
    bool
    addWidget(Widget* widget);

    /*!
     * Removes a child widget from container.
     *
     * \sa addWidget()
     */
    bool
    removeWidget(Widget* widget);

    /*!
     * Returns container's layout.
     */
    LayoutBase*
    layout();

    /*!
     * Sets a new layout and destroys old one.
     *
     * Warning: Widgets added to previous layout will be destroyed implicitly.
     *
     * @param layout
     */
    virtual void
    setLayout(LayoutBase* layout);

    /*!
     * Sets spacing used by layout.
     * This property has no effect over default layout.
     *
     * @param spacing
     */
    void
    setSpacing(unsigned int spacing);

    virtual void
    doLayout();

protected:
    //! This is the container's current layout.
    LayoutBase* _layout;

    /*!
     * This method sets container's layout geometry. Layout is positioned at 0,0 and its width and height are
     * set to width and height of container respectively.
     *
     * If your implementation needs specific position or dimension for layout, you should reimplement this method.
     */
    virtual void
    updateLayoutGeometry();

    /*!
     * Redirects the incoming pointer events to layout.
     */
    virtual bool
    consumePointerEvent(const PointerEvent& pointerEvent);
};
}

#endif /* ILIXI_CONTAINERBASE_H_ */
