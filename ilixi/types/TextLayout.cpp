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

#include "TextLayout.h"
#include "core/Logger.h"

using namespace ilixi;

TextLayout::TextLayout() :
    _modified(true), _singleLine(false), _text(""), _alignment(Left)
{
}

TextLayout::TextLayout(const std::string& text) :
    _modified(true), _singleLine(false), _text(text), _alignment(Left)
{
}

TextLayout::TextLayout(const TextLayout& layout) :
    _modified(true), _singleLine(layout._singleLine), _text(layout._text), _alignment(
        layout._alignment), _bounds(layout._bounds), _lines(layout._lines)
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

std::string
TextLayout::text() const
{
  return _text;
}

Point
TextLayout::cursorPositon(Font* font, int index)
{
  // TODO Implement multi line layout version
  int x = _bounds.x();
  int y = _bounds.y();

  if (!font || !_text.length())
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
    }
  else
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
    }
  else
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
TextLayout::insert(int pos, char c)
{
  _text.insert(pos, 1, c);
  _modified = true;
}

void
TextLayout::insert(int pos, const std::string& str)
{
  _text.insert(pos, str);
  _modified = true;
}

void
TextLayout::replace(int pos, int number, const char c)
{
  _text.replace(pos, number, 1, c);
  _modified = true;
}

void
TextLayout::replace(int pos, int number, const std::string& str)
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
  _text = text;
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
    }
  else
    {
      l.y = _bounds.y();
      int leading = font->leading();
      const char* start = _text.c_str();
      const char* text = start;
      const char* next = text;

      while (text && l.y <= _bounds.bottom())
        {
          l.offset = text - start;

          font->stringBreak(text, -1, _bounds.width(), &l.lineWidth, &l.length,
              &next);
          l.bytes = next - text;
          _lines.push_back(l);

          text = next;
          l.y += leading;
        }
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
      const char* text = _text.c_str();
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

      return h;
    }
}
