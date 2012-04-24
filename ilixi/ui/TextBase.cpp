/*
 Copyright 2011 Tarik Sekmen.

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

TextBase::TextBase(Widget* parent) :
    Widget(parent), _font(NULL)
{
  sigGeometryUpdated.connect(
      sigc::mem_fun(this, &TextBase::updateTextLayoutGeometry));
}

TextBase::TextBase(const std::string& text, Widget* parent) :
    Widget(parent), _font(NULL), _layout(text)
{
  sigGeometryUpdated.connect(
      sigc::mem_fun(this, &TextBase::updateTextLayoutGeometry));
}

TextBase::TextBase(const TextBase& tb) :
    Widget(tb), _font(tb._font), _layout(tb._layout)
{
  sigGeometryUpdated.connect(
      sigc::mem_fun(this, &TextBase::updateTextLayoutGeometry));
}

TextBase::~TextBase()
{
  if (_font)
    {
      if (_font->_font && !_font->_font->refs)
        delete _font;
      else
        _font->release();
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
      delete _font;
      _font = font;
      if (_font->_font)
        _font->_font->AddRef(_font->_font);
      doLayout();
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
      update();
    }
}

void
TextBase::updateTextLayoutGeometry()
{
  _layout.setBounds(0, 0, width(), height());
  _layout.doLayout(font()); // Fixme can not connect to signal as it is.
}

Font*
TextBase::defaultFont() const
{
  return stylist()->defaultFont(StyleHint::DefaultFont);
}
