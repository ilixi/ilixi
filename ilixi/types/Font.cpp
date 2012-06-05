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

#include "types/Font.h"
#include "core/AppBase.h"
#include "core/Logger.h"

using namespace ilixi;

Font::Font() :
    _modified(true), _ref(0), _fileName(""), _font(NULL)
{
  ILOG_TRACE(ILX_FONT);
}

Font::Font(const std::string& file, int size) :
    _modified(true), _ref(0), _fileName(file), _font(NULL)
{
  // Fixme is height absolutely necessary here?
  _desc.flags = DFDESC_HEIGHT;
  _desc.height = size;
  ILOG_TRACE(ILX_FONT);
}

Font::Font(const Font& font) :
    _modified(true), _ref(0), _fileName(font._fileName), _font(font._font), _desc(
        font._desc)
{
  if (_font)
    _font->AddRef(_font);
  ILOG_TRACE(ILX_FONT);
}

Font::~Font()
{
  release();
  ILOG_TRACE(ILX_FONT);
}

int
Font::ascender()
{
  if (!loadFont())
    return 0;

  int ascender;
  _font->GetAscender(_font, &ascender);
  return ascender;
}

int
Font::descender()
{
  if (!loadFont())
    return 0;

  int descender;
  _font->GetDescender(_font, &descender);
  return descender;
}

int
Font::leading()
{
  if (!loadFont())
    return 0;

  int height;
  _font->GetHeight(_font, &height);
  return height;
}

Size
Font::extents(const std::string& text, int offset)
{
  if (!loadFont())
    return Size();

  DFBRectangle rect;
  _font->GetStringExtents(_font, text.c_str(), offset, &rect, NULL);
  return Size(rect.w, rect.h);
}

Size
Font::glyphExtents(unsigned int c)
{
  if (!loadFont())
    return Size();

  DFBRectangle r;
  _font->GetGlyphExtents(_font, c, &r, NULL);
  return Size(r.w, r.h);
}

int
Font::glyphAdvance(unsigned int c)
{
  if (!loadFont())
    return 0;

  int r;
  _font->GetGlyphExtents(_font, c, NULL, &r);
  return r;
}

void
Font::stringBreak(const char* text, int offset, int maxWidth, int* lineWidth,
    int* length, const char** nextLine)
{
  if (!loadFont())
    return;

  DFBResult ret = _font->GetStringBreak(_font, text, offset, maxWidth,
      lineWidth, length, nextLine);
  if (ret)
    {
      ILOG_ERROR(ILX_FONT, "Error while getting string breaks!\n");
      *lineWidth = 0;
      *length = 0;
      *nextLine = NULL;
    }
}

int
Font::textWidth(const std::string& text, int offset)
{
  if (!loadFont())
    return 0;

  int width;
  _font->GetStringWidth(_font, text.c_str(), offset, &width);
  return width;
}

int
Font::size() const
{
  return _desc.height;
}

Font::Style
Font::style() const
{
  return (Style) _desc.attributes;
}

void
Font::setEncoding(DFBTextEncodingID encoding)
{
  if (!loadFont())
    return;

  _font->SetEncoding(_font, encoding);
}

void
Font::setSize(int size)
{
  _desc.flags = (DFBFontDescriptionFlags) (_desc.flags | DFDESC_HEIGHT);
  _desc.height = size;
  _modified = true;
}

void
Font::setStyle(Style style)
{
  _desc.flags = (DFBFontDescriptionFlags) (_desc.flags | DFDESC_ATTRIBUTES);
  _desc.attributes = (DFBFontAttributes) style;
  _modified = true;
}

void
Font::setAttributes(DFBFontAttributes attr)
{
  _desc.flags = (DFBFontDescriptionFlags) (_desc.flags | DFDESC_ATTRIBUTES);
  _desc.attributes = (DFBFontAttributes) attr;
  _modified = true;
}

Font&
Font::operator=(const Font& font)
{
  if (this != &font)
    {
      _fileName = font._fileName;
      _desc = font._desc;
      if (font._font)
        {
          _font = font._font;
          _font->AddRef(_font);
          _modified = false;
        }
      else
        _modified = true;
    }
  return *this;
}

bool
Font::operator==(const Font &font)
{
  return ((_fileName == font._fileName) && (_desc.flags == font._desc.flags)
      && (_desc.height == font._desc.height)
      && (_desc.attributes == font._desc.attributes) && (_font == font._font));
}

bool
Font::operator!=(const Font &font)
{
  return !(*this == font);
}

bool
Font::applyFont(IDirectFBSurface* surface)
{
  if (!loadFont())
    return false;

  DFBResult ret = surface->SetFont(surface, _font);
  if (ret)
    {
      ILOG_ERROR(ILX_FONT, "Error while setting font!\n");
      return false;
    }

  return true;
}

bool
Font::loadFont()
{
  if (_modified)
    {
      release();

      if (_fileName == "")
        {
          ILOG_ERROR(ILX_FONT, "Font filename is invalid.\n");
          return false;
        }

      DFBResult ret = AppBase::getDFB()->CreateFont(AppBase::getDFB(),
          _fileName.c_str(), &_desc, &_font);
      if (ret)
        {
          ILOG_ERROR( ILX_FONT,
              "Error while creating font %s!\n", _fileName.c_str());
          return false;
        }

      ILOG_DEBUG( ILX_FONT,
          "Font [%s:%p] is created.\n", _fileName.c_str(), _font);
      _modified = false;
    }

  return true;
}

void
Font::release()
{
  if (_font)
    {
      if (_font->Release(_font) != DR_OK)
        ILOG_ERROR(ILX_FONT, "Error while releasing font!\n");
      _font = NULL;
    }
}
