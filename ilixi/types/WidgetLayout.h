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

#ifndef ILIXI_WIDGETLAYOUT_H_
#define ILIXI_WIDGETLAYOUT_H_

#include <ui/Widget.h>

namespace ilixi
{

class WidgetLayout
{
public:
    WidgetLayout(Widget* parent = NULL);

    WidgetLayout(const WidgetLayout& layout);

    virtual
    ~WidgetLayout();

    virtual int
    heightForWidth(int width);

    virtual Size
    preferredSize();

    Rectangle
    bounds() const;

    /*!
     * Returns space between widgets.
     */
    unsigned int
    spacing() const;

    void
    setBounds(const Rectangle& rect);

    void
    setBounds(int x, int y, int w, int h);

    /*!
     * Sets space between elements.
     */
    void
    setSpacing(unsigned int spacing);

    void
    setParent(Widget* parent);

    bool
    addWidget(Widget* widget);

    bool
    removeWidget(Widget* widget);

    void
    invalidate();

    virtual void
    tile();

protected:
    //! Parent widget.
    Widget* _parent;
    //! Flag is set to true if modified.
    bool _modified;
    //! Space between elements in pixels.
    unsigned int _spacing;
    //! Bounding rectangle.
    Rectangle _bounds;

    struct CacheElement
    {
        Widget* widget;
        Size size;
    };

    typedef std::list<CacheElement> ElementList;

    Widget::WidgetList _widgets;

    void
    createCache(ElementList* cache);
};

}

#endif /* ILIXI_WIDGETLAYOUT_H_ */
