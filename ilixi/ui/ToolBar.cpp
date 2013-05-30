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

#include <ui/ToolBar.h>
#include <ui/HBoxLayout.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_TOOLBAR, "ilixi/ui/ToolBar", "ToolBar");
D_DEBUG_DOMAIN(ILX_TOOLBARB, "ilixi/ui/ToolBarButton", "ToolBarButton");

//*********************************************************************
// ToolBarButton
//*********************************************************************
ToolBarButton::ToolBarButton(const std::string& text, Widget* parent)
        : Button(text, parent),
          _icon(NULL)
{
    ILOG_TRACE_W(ILX_TOOLBARB);
    setConstraints(FixedConstraint, FixedConstraint);
    _layout.setSingleLine(true);
}

ToolBarButton::~ToolBarButton()
{
    ILOG_TRACE_W(ILX_TOOLBARB);
}

Size
ToolBarButton::preferredSize() const
{
    ILOG_TRACE_W(ILX_TOOLBARB);
    int imgW = 0;
    if (_icon)
        imgW = stylist()->defaultParameter(StyleHint::ToolBarButtonHeight) / 2;

    Size s = textExtents();
    int w = 0;
    if (!text().empty())
    {
        if (_icon)
            w = stylist()->defaultParameter(StyleHint::ButtonOffset);
        w += s.width();
    }

    return Size(w + imgW + stylist()->defaultParameter(StyleHint::ToolBarButtonLR), stylist()->defaultParameter(StyleHint::ToolBarButtonHeight));
}

void
ToolBarButton::setIcon(const std::string& iconPath)
{
    ILOG_TRACE_W(ILX_TOOLBARB);
    removeChild(_icon);
    _icon = new Icon(iconPath, this);
    addChild(_icon);
    doLayout();
}

void
ToolBarButton::setIcon(StyleHint::PackedIcon icon)
{
    ILOG_TRACE_W(ILX_TOOLBARB);
    removeChild(_icon);
    _icon = new Icon(icon, this);
    addChild(_icon);
    doLayout();
}

void
ToolBarButton::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_TOOLBARB);
    Painter p(this);
    p.begin(event);
    stylist()->drawToolBarButton(&p, this);
    p.end();
}

void
ToolBarButton::updateTextBaseGeometry()
{
    ILOG_TRACE_W(ILX_TOOLBARB);
    int textHeight = textExtents().height();
    int wUsed = stylist()->defaultParameter(StyleHint::ToolBarButtonLR);
    int x = stylist()->defaultParameter(StyleHint::ToolBarButtonLeft);

    if (_icon)
    {
        int s = height() / 2;
        _icon->setSize(s, s);

        _icon->moveTo(x, (height() - s) / 2);
        x += s + stylist()->defaultParameter(StyleHint::ButtonOffset);
        wUsed += s + stylist()->defaultParameter(StyleHint::ButtonOffset);
    }

    _layout.setBounds(x, (height() - textHeight) / 2, width() - wUsed, textHeight);
    _layout.doLayout(font());
}

//*********************************************************************
// ToolBar
//*********************************************************************
ToolBar::ToolBar(Widget* parent)
        : Frame(parent)
{
    ILOG_TRACE_W(ILX_TOOLBAR);
    setConstraints(ExpandingConstraint, FixedConstraint);
    setInputMethod(PointerPassthrough);
    setMargin(Margin(0, 0, 10, 10));
    HBoxLayout* layout = new HBoxLayout();
    layout->setVerticalAlignment(Alignment::Middle);
    setLayout(layout);
    setDrawFrame(false);
}

ToolBar::~ToolBar()
{
    ILOG_TRACE_W(ILX_TOOLBAR);
}

Size
ToolBar::preferredSize() const
{
    ILOG_TRACE_W(ILX_TOOLBAR);
    Size s = _layout->preferredSize();
    return Size(s.width() + _margin.hSum(), stylist()->defaultParameter(StyleHint::ToolBarHeight));
}

void
ToolBar::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    stylist()->drawToolBar(&p, this);
}

} /* namespace ilixi */
