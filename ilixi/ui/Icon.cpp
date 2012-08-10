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

#include <ui/Icon.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_ICON, "ilixi/ui/Icon", "Icon");

Icon::Icon(Widget* parent)
        : Widget(parent),
          _default(new Image()),
          _disabled(NULL),
          _exposed(NULL),
          _focused(NULL)
{
    ILOG_TRACE_W(ILX_ICON);
    setConstraints(FixedConstraint, FixedConstraint);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &Icon::updateImageSize));
}

Icon::Icon(const std::string& path, Widget* parent)
        : Widget(parent),
          _default(new Image(path)),
          _disabled(NULL),
          _exposed(NULL),
          _focused(NULL)
{
    ILOG_TRACE_W(ILX_ICON);
    setConstraints(FixedConstraint, FixedConstraint);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &Icon::updateImageSize));
}

Icon::Icon(const Image& image, Widget* parent)
        : Widget(parent),
          _default(new Image(image)),
          _disabled(NULL),
          _exposed(NULL),
          _focused(NULL)
{
    ILOG_TRACE_W(ILX_ICON);
    setConstraints(FixedConstraint, FixedConstraint);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &Icon::updateImageSize));
}

Icon::~Icon()
{
    delete _default;
    delete _disabled;
    delete _exposed;
    delete _focused;
    ILOG_TRACE_W(ILX_ICON);
}

Size
Icon::preferredSize() const
{
    if (size().isValid())
        return size();
    return _default->size();
//    return stylist()->defaultSize(StyleHint::Icon);
}

Image*
Icon::image()
{
    // XXX use parent state if a certain flag is set?
    //  if (!enabled())
    //    return image(DisabledState);
    //  else if (parent())
    //    {
    //      if (parent()->hasFocus())
    //        return image(FocusedState);
    //      else if (parent()->exposed())
    //        return image(ExposedState);
    //    }
    return _default;
}

Image*
Icon::image(WidgetState state)
{
    if (state & DisabledState && _disabled)
        return _disabled;
    else if (state & FocusedState && _focused)
        return _focused;
    else if (state & ExposedState && _exposed)
        return _exposed;
    return _default;
}

void
Icon::setDefaultImage(const std::string& path)
{
    if (_default)
        _default->setImagePath(path);
    else
        _default = new Image(path);
}

void
Icon::setDisabledImage(const std::string& path)
{
    delete _disabled;
    _disabled = new Image(path);
}

void
Icon::setExposedImage(const std::string& path)
{
    delete _exposed;
    _exposed = new Image(path);
}

void
Icon::setFocusedImage(const std::string& path)
{
    delete _focused;
    _focused = new Image(path);
}

void
Icon::updateImageSize()
{
    _default->setSize(size());
    if (_disabled)
        _disabled->setSize(size());
    if (_focused)
        _focused->setSize(size());
    if (_exposed)
        _exposed->setSize(size());
}

void
Icon::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_ICON);
    Painter painter(this);
    painter.begin(event);
    stylist()->drawIcon(&painter, this);
    painter.end();
}

} /* namespace ilixi */
