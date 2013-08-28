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

#include <ui/Icon.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_ICON, "ilixi/ui/Icon", "Icon");

Icon::Icon(Widget* parent)
        : Widget(parent),
          _colorize(false),
          _image(new Image())
{
    ILOG_TRACE_W(ILX_ICON);
    setConstraints(FixedConstraint, FixedConstraint);
}

Icon::Icon(const std::string& path, Widget* parent)
        : Widget(parent),
          _colorize(false),
          _image(new Image(path))
{
    ILOG_TRACE_W(ILX_ICON);
    ILOG_DEBUG(ILX_ICON, " -> path: %s\n", path.c_str());
    setConstraints(FixedConstraint, FixedConstraint);
}

Icon::Icon(const Image& image, Widget* parent)
        : Widget(parent),
          _colorize(false),
          _image(new Image(image))
{
    ILOG_TRACE_W(ILX_ICON);
    ILOG_DEBUG(ILX_ICON, " -> Source image: %p\n", &image);
    setConstraints(FixedConstraint, FixedConstraint);
}

Icon::Icon(StyleHint::PackedIcon packedIcon, Widget* parent)
        : Widget(parent),
          _colorize(true),
          _image(NULL)
{
    ILOG_TRACE_W(ILX_ICON);
    setConstraints(FixedConstraint, FixedConstraint);
    _image = stylist()->defaultIcon(packedIcon);
    ILOG_DEBUG(ILX_ICON, " -> packed icon: %p\n", _image);
}

Icon::~Icon()
{
    ILOG_TRACE_W(ILX_ICON);
    delete _image;
}

Size
Icon::preferredSize() const
{
    ILOG_TRACE_W(ILX_ICON);
    if (_image)
        return _image->size();
    return Size();
}

bool
Icon::colorize() const
{
    return _colorize;
}

Image* const
Icon::image()
{
    return _image;
}

void
Icon::setColorize(bool colorize)
{
    _colorize = colorize;
}

void
Icon::setImage(const std::string& path)
{
    if (_image)
        _image->setImagePath(path);
    else
        _image = new Image(path);
    doLayout();
}

void
Icon::setState(WidgetState state)
{
    _state = state;
    sigStateChanged(this, _state);
}

void
Icon::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
    stylist()->drawIcon(&painter, this);
    painter.end();
}

} /* namespace ilixi */
