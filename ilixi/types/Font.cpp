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

#include <types/Font.h>
#include <core/Logger.h>
#include <types/FontCache.h>
#include <sstream>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_FONT, "ilixi/types/Font", "Font");

Font::Font()
        : _modified(true),
          _font(NULL),
          _size(12),
          _attr(DFFA_NONE),
          _name("sans"),
          _ref(1),
          _key(0)
{
    ILOG_TRACE(ILX_FONT);
    ILOG_DEBUG(ILX_FONT, " -> name: %s, size: %d (default)\n", _name.c_str(), _size);
}

Font::Font(const std::string& name, int size)
        : _modified(true),
          _font(NULL),
          _size(size),
          _attr(DFFA_NONE),
          _name(name),
          _ref(1),
          _key(0)
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
          _ref(1),
          _key(font._key)
{
    if (_font)
        _font->AddRef(_font);
    ILOG_TRACE(ILX_FONT);
    ILOG_DEBUG(ILX_FONT, " -> copied, name: %s, size: %d\n", _name.c_str(), _size);
}

Font::~Font()
{
    ILOG_TRACE(ILX_FONT);
    release();
}

IDirectFBFont*
Font::dfbFont()
{
    if (loadFont())
        return _font;
    return 0;
}

const std::string&
Font::name() const
{
    return _name;
}

int
Font::ascender()
{
    ILOG_TRACE(ILX_FONT);
    if (!loadFont())
        return 0;

    int ascender;
    _font->GetAscender(_font, &ascender);
    return ascender;
}

int
Font::descender()
{
    ILOG_TRACE(ILX_FONT);
    if (!loadFont())
        return 0;

    int descender;
    _font->GetDescender(_font, &descender);
    return descender;
}

int
Font::leading()
{
    ILOG_TRACE(ILX_FONT);
    if (!loadFont())
        return 0;

    int height;
    _font->GetHeight(_font, &height);
    return height;
}

Size
Font::extents(const std::string& text, int bytes)
{
    ILOG_TRACE(ILX_FONT);
    if (!loadFont())
        return Size();
    DFBRectangle rect;
    _font->GetStringExtents(_font, text.c_str(), bytes, &rect, NULL);
    ILOG_DEBUG(ILX_FONT, " -> \"%s\" (%d, %d, %d, %d)\n", text.c_str(), rect.x, rect.y, rect.w, rect.h);
    return Size(rect.w + 1, rect.h);
}

Size
Font::glyphExtents(unsigned int c)
{
    ILOG_TRACE(ILX_FONT);
    if (!loadFont())
        return Size();

    DFBRectangle r;
    _font->GetGlyphExtents(_font, c, &r, NULL);
    return Size(r.w, r.h);
}

int
Font::glyphAdvance(unsigned int c)
{
    ILOG_TRACE(ILX_FONT);
    if (!loadFont())
        return 0;

    int r;
    _font->GetGlyphExtents(_font, c, NULL, &r);
    return r;
}

void
Font::stringBreak(const char* text, int offset, int maxWidth, int* lineWidth, int* length, const char** nextLine)
{
    ILOG_TRACE(ILX_FONT);
    if (!loadFont())
        return;

    DFBResult ret = _font->GetStringBreak(_font, text, offset, maxWidth, lineWidth, length, nextLine);
    ILOG_DEBUG(ILX_FONT, " -> text: %s - offset: %d - maxWidth: %d - lineWidth: %d - length: %d - nextLine: %s\n", text, offset, maxWidth, *lineWidth, *length, *nextLine);
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
    ILOG_TRACE(ILX_FONT);
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
    ILOG_TRACE(ILX_FONT);
    if (this != &font)
    {
        _name = font._name;
        _size = font._size;
        _attr = font._attr;
        if (font._font)
        {
            ILOG_DEBUG(ILX_FONT, " -> %s\n", toString().c_str());
            release();
            _key = FontCache::Instance()->getEntry(_name, _size, _attr, &_font);
            _modified = false;
        } else
            _modified = true;
    }
    return *this;
}

bool
Font::operator==(const Font &font)
{
    return ((_name == font._name) && (_size == font._size) && (_attr == font._attr) && (_font == font._font));
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
    ILOG_TRACE(ILX_FONT);
    if (!loadFont())
        return false;

    ILOG_DEBUG(ILX_FONT, " -> Font: %p key: %u\n", _font, _key);
    DFBResult ret = surface->SetFont(surface, _font);
    if (ret)
    {
        ILOG_ERROR(ILX_FONT, "Error while setting font!\n");
        return false;
    }

    return true;
}

#ifdef ILIXI_HAVE_CAIRO
bool
Font::applyFont(cairo_t* context)
{
    cairo_select_font_face(context, _name.c_str(), (_attr & DFFA_STYLE_ITALIC) ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL, (_attr & DFFA_STYLE_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(context, _size);
    return true;
}
#endif

bool
Font::loadFont()
{
    ILOG_TRACE(ILX_FONT);
    if (_modified)
    {
        release();
        _key = FontCache::Instance()->getEntry(_name, _size, _attr, &_font);
        ILOG_DEBUG(ILX_FONT, " -> Font: %p key: %u\n", _font, _key);
        _modified = false;
        if (!_font)
            return false;
    }
    return true;
}

void
Font::release()
{
    if (_font && _key)
    {
        ILOG_TRACE(ILX_FONT);
        ILOG_DEBUG(ILX_FONT, " -> name: %s size: %d - key: %u\n", _name.c_str(), _size, _key);
        FontCache::Instance()->releaseEntry(_key);
        _font = NULL;
        _key = 0;
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
    ILOG_TRACE(ILX_FONT);
    if (!--_ref)
        delete this;
}

std::istream&
operator>>(std::istream& is, Font& obj)
{
    std::string name;
    int size;
    int style;
    std::getline(is, name, ',');
    is >> size;
    is.ignore(2);
    is >> style;
    obj._name = name;
    obj._size = size;
    obj._attr = (DFBFontAttributes) style;
    obj._modified = true;
    return is;
}

std::ostream&
operator<<(std::ostream& os, const Font& obj)
{
    return os << obj._name << ", " << obj._size << ", " << obj.style();
}

} /* namespace ilixi */
