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

#include <types/Font.h>
#include <core/AppBase.h>
#include <core/Logger.h>
#include <types/FontCache.h>
#include <sstream>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_FONT, "ilixi/types/Font", "Font");

Font::Font()
        : _modified(true),
          _font(NULL),
          _size(12),
          _attr(DFFA_NONE),
          _name("decker"),
          _ref(1)
{
    ILOG_TRACE(ILX_FONT);
}

Font::Font(const std::string& name, int size)
        : _modified(true),
          _font(NULL),
          _size(size),
          _attr(DFFA_NONE),
          _name(name),
          _ref(1)
{
    ILOG_TRACE(ILX_FONT);
    ILOG_DEBUG(ILX_FONT, " -> name: %s, size: %d\n", _name.c_str(), _size);
}

Font::Font(const Font& font)
        : _modified(true),
          _font(font._font),
          _size(font._size),
          _attr(font._attr),
          _name(font._name),
          _ref(1)
{
    if (_font)
        _font->AddRef(_font);
    ILOG_TRACE(ILX_FONT);
    ILOG_DEBUG(ILX_FONT,
               " -> copied name: %s, size: %d\n", _name.c_str(), _size);
}

Font::~Font()
{
    release();
    ILOG_TRACE(ILX_FONT);
}

IDirectFBFont*
Font::dfbFont()
{
    if (loadFont())
        return _font;
    return 0;
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
Font::extents(const std::string& text, int bytes)
{
    if (!loadFont())
        return Size();
    ILOG_TRACE(ILX_FONT);
    DFBRectangle rect;
    _font->GetStringExtents(_font, text.c_str(), bytes, &rect, NULL);
    ILOG_DEBUG(
            ILX_FONT,
            " -> \"%s\" (%d, %d, %d, %d)\n", text.c_str(), rect.x, rect.y, rect.w, rect.h);
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
    return _size;
}

Font::Style
Font::style() const
{
    return (Style) _attr;
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
    _size = size;
    _modified = true;
}

void
Font::setStyle(Style style)
{
    _attr = (DFBFontAttributes) (_attr | style);
    _modified = true;
}

Font&
Font::operator=(const Font& font)
{
    if (this != &font)
    {
        _name = font._name;
        _size = font._size;
        _attr = font._attr;
        if (font._font)
        {
            _font = font._font;
            _font->AddRef(_font);
            _modified = false;
        } else
            _modified = true;
    }
    return *this;
}

bool
Font::operator==(const Font &font)
{
    return ((_name == font._name) && (_size == font._size)
            && (_attr == font._attr) && (_font == font._font));
}

bool
Font::operator!=(const Font &font)
{
    return !(*this == font);
}

std::string
Font::toString() const
{
    std::stringstream ss;
    ss << "Name: " << _name << " Size: " << _size << " Attr: " << _attr;
    return ss.str();
}

bool
Font::applyFont(IDirectFBSurface* surface)
{
    if (!loadFont())
        return false;

    ILOG_TRACE(ILX_FONT);
    ILOG_DEBUG(ILX_FONT, " -> Font: %p\n", _font);
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
        ILOG_TRACE(ILX_FONT);
        _font = FontCache::Instance()->getEntry(_name.c_str(), _size, _attr);
        ILOG_DEBUG(ILX_FONT, " -> Font: %p\n", _font);
        _modified = false;
    }

    return true;
}

void
Font::release()
{
    if (_font)
    {
        ILOG_TRACE(ILX_FONT);
        FontCache::Instance()->releaseEntry(_name.c_str(), _size, _attr);
        _font = NULL;
    }
}

void
Font::addRef()
{
    _ref++;
}

void
Font::relRef()
{
    if (!--_ref)
        delete this;
}

} /* namespace ilixi */
