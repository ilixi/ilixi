/*
 Copyright 2010-2015 Tarik Sekmen.

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
#include <string.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_TEXTLAYOUT, "ilixi/types/TextLayout", "TextLayout");

TextLayout::TextLayout()
        : _modified(true),
          _singleLine(false),
#ifdef ILIXI_USE_WSTRING
          _text(L""),
#else
          _text(""),
#endif
          _alignment(Left)
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
}

TextLayout::TextLayout(const std::string& text)
        : _modified(true),
          _singleLine(false),
#ifdef ILIXI_USE_WSTRING
          _text(L""),
#else
          _text(""),
#endif
          _alignment(Left)
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
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
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
}

TextLayout::~TextLayout()
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
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
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
    if (_singleLine)
        return font->extents(text(), -1);
    return multiExtents(font);
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
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
#ifdef ILIXI_USE_WSTRING
    char* out = (char*) calloc(_text.size() * 4 + 1, 1);
    size_t bytes = wchar_to_utf8(_text.c_str(), _text.size(), out, _text.size() * 4 + 1, UTF8_SKIP_BOM);
    std::string s = out;
    free(out);
    return s;
#else
    return _text;
#endif
}

#ifdef ILIXI_USE_WSTRING
std::wstring
TextLayout::wtext() const
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
    return _text;
}
#endif

Point
TextLayout::cursorPositon(Font* font, int index)
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
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
        } else
        {
            int lineIndex = 0;
            for (LineList::iterator it = _lines.begin(); it != _lines.end(); ++it)
            {
                if (index >= lineIndex && index <= lineIndex + it->length)
                {
                    y = it->y;
                    for (int i = lineIndex; i < index; ++i)
                        x += font->glyphAdvance(_text.at(i));
                    break;
                }
                lineIndex += it->length + 1;
            }
        }
    }
    return Point(x, y);
}

int
TextLayout::xyToIndex(Font* font, int x, int y)
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
    if (!font)
        return -1;

    if (!_text.length())
        return -1;

    doLayout(font);

    if (_singleLine)
    {
        int xPre = _bounds.x();
        if (x < xPre)
            return 0;

        int xNext = xPre;
        for (int i = 0; i < _text.length(); ++i)
        {
            xPre = xNext;
            xNext += font->glyphAdvance(_text.at(i));

            if (x > xPre && x <= xNext)
                return i + 1;
        }
    } else
    {
        int yPre = 0;
        int index = 0;
        int leading = font->leading();
        for (LineList::iterator it = _lines.begin(); it != _lines.end(); ++it)
        {
            if (y >= yPre && y < yPre + leading)
            {
                int xPre = _bounds.x();
                if (x < xPre)
                    return index;

                int xNext = xPre;
                for (int i = index; i < index + it->length; ++i)
                {
                    xPre = xNext;
                    xNext += font->glyphAdvance(_text.at(i));

                    if (x > xPre && x <= xNext)
                        return i + 1;
                }
                return index + it->length;
            }
            yPre += leading;
            index += it->length + 1;
        }
    }
    return _text.length();
}

void
#ifdef ILIXI_USE_WSTRING
TextLayout::insert(int pos, wchar_t c)
#else
TextLayout::insert(int pos, char c)
#endif
{
    _text.insert(pos, 1, c);
    _modified = true;
}

void
#ifdef ILIXI_USE_WSTRING
TextLayout::insert(int pos, const std::wstring& str)
#else
TextLayout::insert(int pos, const std::string& str)
#endif
{
    if (_text.empty())
        _text = str;
    else
        _text.insert(pos, str);
    _modified = true;
}

void
#ifdef ILIXI_USE_WSTRING
TextLayout::replace(int pos, int number, const wchar_t c)
#else
TextLayout::replace(int pos, int number, const char c)
#endif
{
    _text.replace(pos, number, 1, c);
    _modified = true;
}

void
#ifdef ILIXI_USE_WSTRING
TextLayout::replace(int pos, int number, const std::wstring& str)
#else
TextLayout::replace(int pos, int number, const std::string& str)
#endif
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
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
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
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
#ifdef ILIXI_USE_WSTRING
    wchar_t* out = (wchar_t*) calloc(text.size() + 1, sizeof(wchar_t));
    size_t bytes = utf8_to_wchar(text.c_str(), text.size(), out, text.size() + 1, UTF8_SKIP_BOM);
    _text = out;
    free(out);
#else
    _text = text;
#endif
    _modified = true;
}

#ifdef ILIXI_USE_WSTRING
void
TextLayout::setText(const std::wstring& text)
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
    _text = text;
    _modified = true;
}
#endif

void
TextLayout::setModified()
{
    _modified = true;
}

void
TextLayout::doLayout(Font* font)
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
    if (!_modified || font == NULL)
        return;

    _lines.clear();
    LayoutLine l;

    if (_singleLine)
    {
        l.offset = 0;
        l.bytes = _text.size();
        l.length = _text.length();
        l.y = _bounds.y();
        _lines.push_back(l);
    } else
    {
        l.y = _bounds.y();
        int leading = font->leading();

#ifdef ILIXI_USE_WSTRING
        char* out = (char*) calloc(_text.size() * 4 + 1, 1);
        size_t bytes = wchar_to_utf8(_text.c_str(), _text.size(), out, _text.size() * 4 + 1, UTF8_SKIP_BOM);
        const char* start = out;
#else
        const char* start = _text.c_str();
#endif

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
#ifdef ILIXI_USE_WSTRING
        free(out);
#endif
    }
    _modified = false;
}

int
TextLayout::heightForWidth(int width, Font* font) const
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
    if (font == NULL)
        return -1;

    int leading = font->leading();

    if (_singleLine)
        return leading;
    else
    {
#ifdef ILIXI_USE_WSTRING
        char* out = (char*) calloc(_text.size() * 4 + 1, 1);
        size_t bytes = wchar_to_utf8(_text.c_str(), _text.size(), out, _text.size() * 4 + 1, UTF8_SKIP_BOM);
        const char* text = out;
#else
        const char* text = _text.c_str();
#endif
        const char* next = text;
        int lw = 0;
        int len = 0;
        int h = 0;

        while (text)
        {
            font->stringBreak(text, -1, width, &lw, &len, &next);
            text = next;
            h += leading;
        }
#ifdef ILIXI_USE_WSTRING
        free(out);
#endif
        return h;
    }
}

Size
TextLayout::multiExtents(Font* font) const
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
    int w = 0;
    int h = 0;
    int lw, len;

#ifdef ILIXI_USE_WSTRING
    char* out = (char*) calloc(_text.size() * 4 + 1, 1);
    size_t bytes = wchar_to_utf8(_text.c_str(), _text.size(), out, _text.size() * 4 + 1, UTF8_SKIP_BOM);
    const char* start = out;
#else
    const char* start = _text.c_str();
#endif

    const char* text = start;
    const char* next = text;
    int leading = font->leading();

    while (text)
    {
        font->stringBreak(text, -1, INT_MAX, &lw, &len, &next);
        if (w < lw)
            w = lw + 1;
        text = next;
        h += leading;
    }
#ifdef ILIXI_USE_WSTRING
    free(out);
#endif
    return Size(w, h);
}

void
TextLayout::drawTextLayout(IDirectFBSurface* surface, int x, int y) const
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
#ifdef ILIXI_USE_WSTRING
    char* out = (char*) calloc(_text.size() * 4 + 1, 1);
    size_t bytes = wchar_to_utf8(_text.c_str(), _text.size(), out, _text.size() * 4 + 1, UTF8_SKIP_BOM);
    const char* text = out;
#else
    const char* text = _text.c_str();
#endif
    DFBRegion clip;
    surface->GetClip(surface, &clip);
    Rectangle intersect = Rectangle(clip.x1, clip.y1, clip.x2 - clip.x1 + 1, clip.y2 - clip.y1 + 1);
    Rectangle layoutRect = _bounds;
    layoutRect.translate(x, y);
    intersect = intersect.intersected(layoutRect);
    DFBRegion clipLayout = intersect.dfbRegion();
    surface->SetClip(surface, &clipLayout);

    x += _bounds.x();
    if (_alignment == Center)
        x += _bounds.width() / 2;
    else if (_alignment == Right)
        x += _bounds.width();

    for (TextLayout::LineList::const_iterator it = _lines.begin(); it != _lines.end(); ++it)
        surface->DrawString(surface, text + ((TextLayout::LayoutLine) *it).offset, ((TextLayout::LayoutLine) *it).bytes, x, y + ((TextLayout::LayoutLine) *it).y, (DFBSurfaceTextFlags) _alignment);
#ifdef ILIXI_USE_WSTRING
    free(out);
#endif
    surface->SetClip(surface, &clip);
}

#ifdef ILIXI_HAVE_CAIRO
void
TextLayout::drawTextLayout(cairo_t* context, int x, int y) const
{
    ILOG_TRACE_F(ILX_TEXTLAYOUT);
#ifdef ILIXI_USE_WSTRING
    char* out = (char*) calloc(_text.size() * 4 + 1, 1);
    size_t bytes = wchar_to_utf8(_text.c_str(), _text.size(), out, _text.size() * 4 + 1, UTF8_SKIP_BOM);
    const char* text = out;
#else
    const char* text = _text.c_str();
#endif

    cairo_save(context);
    cairo_rectangle(context, _bounds.x(), _bounds.y(), _bounds.width(), _bounds.height());
    cairo_clip(context);

    x += _bounds.x();
    if (_alignment == Center)
        x += _bounds.width() / 2;
    else if (_alignment == Right)
        x += _bounds.width();

    for (TextLayout::LineList::const_iterator it = _lines.begin(); it != _lines.end(); ++it)
    {
        char subtxt[((TextLayout::LayoutLine) *it).length + 1];
        strncpy(subtxt, text + ((TextLayout::LayoutLine) *it).offset, ((TextLayout::LayoutLine) *it).length);
        subtxt[((TextLayout::LayoutLine) *it).length] = '\0';
        cairo_move_to(context, x, y + ((TextLayout::LayoutLine) *it).y);
        cairo_show_text(context, subtxt);
    }
#ifdef ILIXI_USE_WSTRING
    free(out);
#endif
    cairo_restore(context);
}
#endif

} /* namespace ilixi */
