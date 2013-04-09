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

#include <ui/GroupBox.h>
#include <graphics/Painter.h>
#include <types/Font.h>
#include <core/Logger.h>
#ifdef ILIXI_HAVE_NLS
#include <libintl.h>
#include <core/PlatformManager.h>
#endif
namespace ilixi
{

D_DEBUG_DOMAIN( ILX_GROUPBOX, "ilixi/ui/GroupBox", "GroupBox");

GroupBox::GroupBox(const std::string& title, Widget* parent)
        : Frame(parent),
          _titleIcon(NULL),
          _title(new Label(""))
{
    ILOG_TRACE_W(ILX_GROUPBOX);
    setConstraints(MinimumConstraint, MinimumConstraint);
    _title->setFont(stylist()->defaultFont(StyleHint::TitleFont));
    _title->setSingleLine(true);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &GroupBox::setTitleGeometry));
    setTitle(title);
    setMargin(5);
    addChild(_title);
}

GroupBox::~GroupBox()
{
    ILOG_TRACE_W(ILX_GROUPBOX);
}

int
GroupBox::heightForWidth(int width) const
{
    ILOG_TRACE_W(ILX_GROUPBOX);
    return _layout->heightForWidth(width - _margin.hSum() - stylist()->defaultParameter(StyleHint::FrameOffsetLR)) + stylist()->defaultParameter(StyleHint::FrameOffsetTB) + _titleSize.height();
}

Size
GroupBox::preferredSize() const
{
    ILOG_TRACE_W(ILX_GROUPBOX);
    Size s = _layout->preferredSize();
    return Size(std::max(s.width(), _titleSize.width()) + _margin.hSum() + stylist()->defaultParameter(StyleHint::FrameOffsetLR), s.height() + _margin.vSum() + stylist()->defaultParameter(StyleHint::FrameOffsetTB) + _titleSize.height());
}

std::string
GroupBox::title() const
{
    return _title->text();
}

Size
GroupBox::titleSize() const
{
    return _titleSize;
}

void
GroupBox::setTitle(const std::string& title)
{
    _title->setText(title);
}

#ifdef ILIXI_HAVE_NLS
void
GroupBox::setI18nText(const std::string& text)
{
    ILOG_TRACE_W(ILX_GROUPBOX);
    _i18nID = text;
    setTitle(gettext(_i18nID.c_str()));
    ILOG_DEBUG(ILX_GROUPBOX, " -> title: %s\n", _title->text().c_str());
    PlatformManager::instance().addI18N(this);
}
#endif

void
GroupBox::setTitleIcon(StyleHint::PackedIcon icon)
{
    removeChild(_titleIcon);
    _titleIcon = new Icon(icon);
    doLayout();
    addChild(_titleIcon);
}

void
GroupBox::doLayout()
{
    ILOG_TRACE_W(ILX_GROUPBOX);
    Size textSize = _title->preferredSize();
    if (_titleIcon)
        _titleSize.setWidth(textSize.width() + textSize.height() + 5 + stylist()->defaultParameter(StyleHint::TabOffsetLR));
    else
        _titleSize.setWidth(textSize.width() + stylist()->defaultParameter(StyleHint::TabOffsetLR));
    _titleSize.setHeight(textSize.height() + stylist()->defaultParameter(StyleHint::TabOffsetTop));
    _title->setSize(textSize);

    if (parent())
        parent()->doLayout();
}

void
GroupBox::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
    stylist()->drawGroupBox(&painter, this);
    painter.end();
}

int
GroupBox::canvasY() const
{
    return _margin.top() + stylist()->defaultParameter(StyleHint::FrameOffsetTop) + _titleSize.height();
}

int
GroupBox::canvasHeight() const
{
    return height() - _margin.vSum() - _titleSize.height() - stylist()->defaultParameter(StyleHint::FrameOffsetTB);
}

void
GroupBox::setTitleGeometry()
{
    Size textSize = _title->preferredSize();
    int x = 2 * stylist()->defaultParameter(StyleHint::TabOffsetLeft);
    int y = stylist()->defaultParameter(StyleHint::TabOffsetTop);
    if (_titleIcon)
    {
        _titleIcon->setGeometry(x, y, textSize.height(), textSize.height());
        x += _titleIcon->width() + 5;
    }

    _title->setGeometry(x, y, textSize.width(), textSize.height());
}

#ifdef ILIXI_HAVE_NLS
void
GroupBox::updateI18nText()
{
    ILOG_TRACE_W(ILX_GROUPBOX);
    setTitle(gettext(_i18nID.c_str()));
}
#endif

} /* namespace ilixi */
