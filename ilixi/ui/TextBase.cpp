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

#include "ui/TextBase.h"
#include "core/Logger.h"

using namespace ilixi;

TextBase::TextBase(Widget* owner) :
    _owner(owner), _font(NULL)
{
  _owner->sigGeometryUpdated.connect(
      sigc::mem_fun(this, &TextBase::updateTextLayoutGeometry));
}

TextBase::TextBase(const std::string& text, Widget* owner) :
    _owner(owner), _font(NULL), _layout(text)
{
  _owner->sigGeometryUpdated.connect(
      sigc::mem_fun(this, &TextBase::updateTextLayoutGeometry));
}

TextBase::TextBase(const TextBase& tb) :
    _owner(tb._owner), _font(tb._font), _layout(tb._layout)
{
  _owner->sigGeometryUpdated.connect(
      sigc::mem_fun(this, &TextBase::updateTextLayoutGeometry));
}

TextBase::~TextBase()
{
  if (_font)
    {
      _font->_ref--;
      if (!_font->_ref)
        delete _font;
    }
  ILOG_DEBUG(ILX_TEXTBASE, "~TextBase %p\n", this);
}

Font*
TextBase::font() const
{
  if (_font)
    return _font;
  return defaultFont();
}

TextLayout
TextBase::layout() const
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
  return font()->extents(_layout.text());
}

int
TextBase::textLayoutHeightForWidth(int width) const
{
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
  if (font)
    {
      _font->_ref--;
      if (!_font->_ref)
        delete _font;

      _font = font;
      _font->_ref++;
      _owner->doLayout();
      sigFontChanged();
    }
}

void
TextBase::setText(const std::string &text)
{
  if (_layout.text() != text)
    {
      _layout.setText(text);
      sigTextChanged(text);
      _owner->update();
    }
}

void
TextBase::updateTextLayoutGeometry()
{
  _layout.setBounds(0, 0, _owner->width(), _owner->height());
  _layout.doLayout(font()); // Fixme can not connect to signal as it is.
}

Font*
TextBase::defaultFont() const
{
  return _owner->stylist()->defaultFont(StyleHint::DefaultFont);
}
