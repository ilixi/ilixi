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

#include <TextLayout.h>
#include <core/Logger.h>
#include <lib/utf8.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_TEXTLAYOUT, "ilixi/types/TextLayout", "TextLayout");

TextLayout::TextLayout()
        : _modified(true),
          _singleLine(false),
          _text(L""),
          _alignment(Left)
{
}

TextLayout::TextLayout(const std::string& text)
        : _modified(true),
          _singleLine(false),
          _text(L""),
          _alignment(Left)
{
    setText(text);
}

TextLayout::TextLayout(const TextLayout& layout)
        : _modified(true),
          _singleLine(layout._singleLine),
          _text(layout._text),
          _alignment(layout._alignment),
          _bounds(layout._bounds),
          _lines(layout._lines)
{
}

TextLayout::~TextLayout()
{
}

TextLayout::Alignment
TextLayout::alignment() const
{
    return _alignment;
}

Rectangle
TextLayout::bounds() const
{
    return _bounds;
}

Size
TextLayout::extents(Font* font) const
{
    return font->extents(text(), _text.length());
}

int
TextLayout::x() const
{
    return _bounds.x();
}

int
TextLayout::y() const
{
    return _bounds.y();
}

int
TextLayout::width() const
{
    return _bounds.width();
}

int
TextLayout::height() const
{
    return _bounds.height();
}

bool
TextLayout::singleLine() const
{
    return _singleLine;
}

bool
TextLayout::isEmpty() const
{
    return _text.empty();
}

std::string
TextLayout::text() const
{
    char* out = (char*) calloc(_text.size() * 4 + 1, 1);
    size_t bytes = wchar_to_utf8(_text.c_str(), _text.size(), out, _text.size() * 4 + 1, UTF8_SKIP_BOM);
    std::string s = out;
    free(out);
    return s;
}

std::wstring
TextLayout::wtext() const
{
    return _text;
}

Point
TextLayout::cursorPositon(Font* font, int index)
{
    // TODO Implement multi line layout version
    int x = _bounds.x();
    int y = _bounds.y();

    if (index == 0 || !font || !_text.length())
        return Point(x, y);

    doLayout(font);

    if (index <= _text.length())
    {
        if (_singleLine)
        {
            for (int i = 0; i < index; ++i)
                x += font->glyphAdvance(_text.at(i));

            return Point(x, y);
        }
    } else
        return Point(x, y);
}

int
TextLayout::xyToIndex(Font* font, int x, int y)
{
    if (!font)
        return -1;

    if (!_text.length())
        return -1;

    doLayout(font);

    if (_singleLine)
    {
        int xPre = _bounds.x();
        int xNext = font->glyphAdvance(_text.at(0));
        if (x > xPre && x <= (xPre + xNext))
            return 0;

        for (int i = 1; i < _text.length(); ++i)
        {
            xPre = xNext;
            xNext += font->glyphAdvance(_text.at(i));

            if (x > xPre && x <= xNext)
                return i;
        }

        return _text.length();
    } else
    {
        // TODO Implement multi line layout version
        int pre = 0;
        int index;
        for (LineList::iterator it = _lines.begin(); it != _lines.end(); ++it)
        {
            if (y >= pre && y < it->y)
            {

            }
            pre = it->y;
        }
        return -1;
    }
}

void
TextLayout::insert(int pos, wchar_t c)
{
    _text.insert(pos, 1, c);
    _modified = true;
}

void
TextLayout::insert(int pos, const std::wstring& str)
{
    if (_text.empty())
        _text = str;
    else
        _text.insert(pos, str);
    _modified = true;
}

void
TextLayout::replace(int pos, int number, const wchar_t c)
{
    _text.replace(pos, number, 1, c);
    _modified = true;
}

void
TextLayout::replace(int pos, int number, const std::wstring& str)
{
    _text.replace(pos, number, str);
    _modified = true;
}

void
TextLayout::erase(int pos, int amount)
{
    _text.erase(pos, amount);
    _modified = true;
}

void
TextLayout::setAlignment(Alignment alignment)
{
    _alignment = alignment;
}

void
TextLayout::setX(int x)
{
    _bounds.setX(x);
    _modified = true;
}

void
TextLayout::setY(int y)
{
    _bounds.setY(y);
    _modified = true;
}

void
TextLayout::setWidth(int width)
{
    _bounds.setWidth(width);
    _modified = true;
}

void
TextLayout::setHeight(int height)
{
    _bounds.setHeight(height);
    _modified = true;
}

void
TextLayout::setBounds(int x, int y, int w, int h)
{
    ILOG_TRACE(ILX_TEXTLAYOUT);
    ILOG_DEBUG(ILX_TEXTLAYOUT, " -> (%d, %d, %d, %d)\n", x, y, w, h);
    _bounds.setRectangle(x, y, w, h);
    _modified = true;
}

void
TextLayout::setBounds(const Rectangle& rect)
{
    _bounds = rect;
    _modified = true;
}

void
TextLayout::setSingleLine(bool singleLine)
{
    _singleLine = singleLine;
}

void
TextLayout::setText(const std::string& text)
{
    wchar_t* out = (wchar_t*) calloc(text.size() + 1, sizeof(wchar_t));
    size_t bytes = utf8_to_wchar(text.c_str(), text.size(), out, text.size() + 1, UTF8_SKIP_BOM);
    _text = out;
    free(out);
    _modified = true;
}

void
TextLayout::setText(const std::wstring& text)
{
    _text = text;
    _modified = true;
}

void
TextLayout::setModified()
{
    _modified = true;
}

void
TextLayout::doLayout(Font* font)
{
    if (!_modified || font == NULL)
        return;

    _lines.clear();
    LayoutLine l;

    if (_singleLine)
    {
        l.bytes = -1;
        l.y = _bounds.y();
        _lines.push_back(l);
    } else
    {
        l.y = _bounds.y();
        int leading = font->leading();

        char* out = (char*) calloc(_text.size() * 4 + 1, 1);
        size_t bytes = wchar_to_utf8(_text.c_str(), _text.size(), out, _text.size() * 4 + 1, UTF8_SKIP_BOM);

        const char* start = out;
        const char* text = start;
        const char* next = text;

        while (text && l.y <= _bounds.bottom())
        {
            l.offset = text - start;

            font->stringBreak(text, -1, _bounds.width(), &l.lineWidth, &l.length, &next);
            l.bytes = next - text;
            _lines.push_back(l);

            text = next;
            l.y += leading;
        }
        free(out);
    }
    _modified = false;
}

int
TextLayout::heightForWidth(int width, Font* font) const
{
    if (font == NULL)
        return -1;

    int leading = font->leading();

    if (_singleLine)
        return leading;
    else
    {
        char* out = (char*) calloc(_text.size() * 4 + 1, 1);
        size_t bytes = wchar_to_utf8(_text.c_str(), _text.size(), out, _text.size() * 4 + 1, UTF8_SKIP_BOM);
        const char* text = out;
        const char* next = text;
        int lw = 0;
        int len = 0;
        int h = 0;
        int offset = -1;

        while (text)
        {
            font->stringBreak(text, offset, width, &lw, &len, &next);
            offset = next - text;
            text = next;
            h += leading;
        }
        free(out);
        return h;
    }
}

void
TextLayout::drawTextLayout(IDirectFBSurface* surface, int x, int y) const
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
    char* out = (char*) calloc(_text.size() * 4 + 1, 1);
    size_t bytes = wchar_to_utf8(_text.c_str(), _text.size(), out, _text.size() * 4 + 1, UTF8_SKIP_BOM);
    const char* text = out;

    x += _bounds.x();
    if (_alignment == Center)
        x += _bounds.width() / 2;

    for (TextLayout::LineList::const_iterator it = _lines.begin(); it != _lines.end(); ++it)
        surface->DrawString(surface, text + ((TextLayout::LayoutLine) *it).offset, ((TextLayout::LayoutLine) *it).bytes, x, y + ((TextLayout::LayoutLine) *it).y, (DFBSurfaceTextFlags) _alignment);
    free(out);
}

} /* namespace ilixi */
