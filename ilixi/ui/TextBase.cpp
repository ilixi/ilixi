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

#include <ui/TextBase.h>
#ifdef ILIXI_HAVE_NLS
#include <libintl.h>
#include <core/PlatformManager.h>
#endif
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_TEXTBASE, "ilixi/ui/TextBase", "TextBase");

TextBase::TextBase(Widget* owner)
#ifdef ILIXI_HAVE_NLS
        : I18NBase(),
          _owner(owner),
#else
        : _owner(owner),
#endif
          _font(NULL),
          _extents(-1, -1)
{
    ILOG_TRACE(ILX_TEXTBASE);
    _extents = _layout.extents(font());
    _owner->sigGeometryUpdated.connect(sigc::mem_fun(this, &TextBase::updateTextBaseGeometry));
}

TextBase::TextBase(const std::string& text, Widget* owner)
#ifdef ILIXI_HAVE_NLS
        : I18NBase(),
          _owner(owner),
#else
        : _owner(owner),
#endif
          _font(NULL),
          _layout(),
          _extents(-1, -1)
{
    ILOG_TRACE(ILX_TEXTBASE);
    setText(text);
    _extents = _layout.extents(font());
    _owner->sigGeometryUpdated.connect(sigc::mem_fun(this, &TextBase::updateTextBaseGeometry));
}

TextBase::TextBase(const TextBase& tb)
#ifdef ILIXI_HAVE_NLS
        : I18NBase(),
          _owner(tb._owner),
#else
        : _owner(tb._owner),
#endif
          _font(tb._font),
          _layout(tb._layout),
          _extents(tb._extents)
{
    ILOG_TRACE(ILX_TEXTBASE);
    _owner->sigGeometryUpdated.connect(sigc::mem_fun(this, &TextBase::updateTextBaseGeometry));
}

TextBase::~TextBase()
{
    ILOG_TRACE(ILX_TEXTBASE);
    if (_font)
        _font->relRef();
}

Font*
TextBase::font() const
{
    if (_font)
        return _font;
    return defaultFont();
}

TextLayout&
TextBase::layout()
{
    return _layout;
}

Font::Style
TextBase::style() const
{
    return font()->style();
}

std::string
TextBase::text() const
{
    return _layout.text();
}

Size
TextBase::textExtents() const
{
    ILOG_TRACE(ILX_TEXTBASE);
    return _extents;
}

int
TextBase::textLayoutHeightForWidth(int width) const
{
    ILOG_TRACE(ILX_TEXTBASE);
    return _layout.heightForWidth(width, font());
}

void
TextBase::setLayoutAlignment(TextLayout::Alignment alignment)
{
    _layout.setAlignment(alignment);
}

void
TextBase::setFont(Font* font)
{
    ILOG_TRACE(ILX_TEXTBASE);
    if (font)
    {
        if (_font)
            _font->relRef();
        ILOG_DEBUG(ILX_TEXTBASE, " -> Font changed to %p from %p\n", font, _font);
        _font = font;
        _font->addRef();
        _layout.setModified();
        _extents = _layout.extents(_font);
        _owner->doLayout();
        sigFontChanged();
    }
}

void
TextBase::setText(const std::string &text)
{
    ILOG_TRACE(ILX_TEXTBASE);
    if (_layout.text() != text)
    {
        _layout.setText(text);
        _layout.doLayout(font());
        _extents = _layout.extents(font());
        _owner->doLayout();
        sigTextChanged(text);
    }
}

#ifdef ILIXI_HAVE_NLS
void
TextBase::setText(const std::wstring &text)
{
    if (_layout.wtext() != text)
    {
        ILOG_TRACE(ILX_TEXTBASE);
        _layout.setText(text);
        _layout.doLayout(font());
        _extents = _layout.extents(font());
        _owner->doLayout();
        sigTextChanged(_layout.text());
    }
}

void
TextBase::setI18nText(const std::string& text)
{
    _i18nID = text;
    setText(gettext(_i18nID.c_str()));
    PlatformManager::instance().addI18N(this);
}
#endif

void
TextBase::setSingleLine(bool single)
{
    _layout.setSingleLine(single);
}

void
TextBase::updateTextBaseGeometry()
{
    _layout.setBounds(0, 0, _owner->width(), _owner->height());
    _layout.doLayout(font()); // Fixme can not connect to signal as it is.
}

#ifdef ILIXI_HAVE_NLS
void
TextBase::updateI18nText()
{
    setText(gettext(_i18nID.c_str()));
}
#endif

Font*
TextBase::defaultFont() const
{
    return _owner->stylist()->defaultFont(StyleHint::DefaultFont);
}

} /* namespace ilixi */
