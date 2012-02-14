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

#include "LineInput.h"
#include "graphics/Painter.h"

using namespace ilixi;

LineInput::LineInput(const std::string& text, Widget* parent) :
    TextBase(text, parent), BorderBase(parent), _cursorOn(false), _selecting(
        false), _maxLength(-1), _cursorIndex(text.length()), _selectedIndex(0)
{
  setConstraints(ExpandingConstraint, FixedConstraint);
  setInputMethod(KeyAndPointerInputTracking);
  setBorderWidth(1);
  setBorderStyle(StyledBorder);
  _layout.setSingleLine(true);
  _cursorTimer.sigExec.connect(sigc::mem_fun(this, &LineInput::drawCursor));
}

LineInput::~LineInput()
{
}

Size
LineInput::preferredSize() const
{
  Size s = font()->extents(text());
  return Size(s.width() + 2 * borderOffset(), s.height() + 2 * borderWidth());
}

int
LineInput::maxLength() const
{
  return (_maxLength);
}

void
LineInput::setMaxLength(int maxLen)
{
  _maxLength = maxLen;
}

void
LineInput::updateCursorPosition()
{
  _cursor.moveTo(
      _layout.cursorPositon(font(), _cursorIndex).x() + borderOffset(),
      _layout.cursorPositon(font(), _cursorIndex).y() + borderWidth());
}

void
LineInput::updateSelectionRect()
{
  if (_selectedIndex == _cursorIndex)
    _selection.setSize(0, 0);
  else
    {
      if (_selectedIndex > _cursorIndex)
        {
          _selection.moveTo(_cursor.topLeft());
          _selection.setRight(
              _layout.cursorPositon(font(), _selectedIndex).x()
                  + borderOffset());
          _selection.setHeight(_cursor.height());
        }
      else
        {
          _selection.setX(
              _layout.cursorPositon(font(), _selectedIndex).x()
                  + borderOffset());
          _selection.setRight(_cursor.x());
          _selection.setBottom(_cursor.bottom());
        }
      sigSelectionChanged();
    }
}

void
LineInput::pointerButtonDownEvent(const PointerEvent& mouseEvent)
{
  Point p = mapToSurface(Point(mouseEvent.x, mouseEvent.y));

  int index = _layout.xyToIndex(font(), p.x() - borderOffset(),
      p.y() - borderWidth());

  if (index != _cursorIndex)
    {
      _cursorTimer.stop();
      sigCursorMoved(_cursorIndex, index);
      _cursorIndex = index;

      updateCursorPosition();
      _selecting = true;
      _selectedIndex = _cursorIndex;
      _selection.moveTo(_cursor.x(), _cursor.y());
      _selection.setSize(0, 0);
      _cursorOn = false;
      _cursorTimer.start(500);
      update();
    }
}

void
LineInput::pointerButtonUpEvent(const PointerEvent& mouseEvent)
{
  _selecting = false;
}

void
LineInput::pointerMotionEvent(const PointerEvent& mouseEvent)
{
  if (_selecting)
    {
      Point p = mapToSurface(Point(mouseEvent.x, mouseEvent.y));
      int index = _layout.xyToIndex(font(), p.x() - borderOffset(),
          p.y() - borderWidth());

      if (index != _cursorIndex)
        {
          sigCursorMoved(_cursorIndex, index);
          _cursorIndex = index;
          updateCursorPosition();
          updateSelectionRect();
          update();
        }
    }
}

void
LineInput::keyDownEvent(const KeyEvent& keyEvent)
{
  _cursorTimer.stop();
  switch (keyEvent.keySymbol)
    {
  case DIKS_CURSOR_LEFT:
  case DIKS_CURSOR_UP:
    if (_cursorIndex)
      sigCursorMoved(_cursorIndex, --_cursorIndex);
    break;

  case DIKS_CURSOR_RIGHT:
  case DIKS_CURSOR_DOWN:
    if (_cursorIndex < text().length())
      sigCursorMoved(_cursorIndex, ++_cursorIndex);
    break;

  case DIKS_PAGE_UP:
  case DIKS_HOME:
    sigCursorMoved(_cursorIndex, 0);
    _cursorIndex = 0;
    break;

  case DIKS_PAGE_DOWN:
  case DIKS_END:
    sigCursorMoved(_cursorIndex, text().length());
    _cursorIndex = text().length() - 1;
    break;

  case DIKS_DELETE:
    if (_selection.isNull())
      {
        if (_cursorIndex > -1 && _cursorIndex < text().length())
          _layout.erase(_cursorIndex, 1);
      }
    else
      {
        int pos1 = std::min(_selectedIndex, _cursorIndex);
        int n1 = abs(_selectedIndex - _cursorIndex);
        _layout.erase(pos1, n1);
        _selection.setSize(0, 0);
        sigSelectionChanged();
        sigCursorMoved(_cursorIndex, pos1);
        _cursorIndex = pos1;
      }
    break;

  case DIKS_BACKSPACE:
    if (_selection.isNull())
      {
        if (_cursorIndex)
          {
            sigCursorMoved(_cursorIndex, --_cursorIndex);
            _layout.erase(_cursorIndex, 1);
          }
      }
    else
      {
        int pos1 = std::min(_selectedIndex, _cursorIndex);
        int n1 = abs(_selectedIndex - _cursorIndex);
        _layout.erase(pos1, n1);
        _selection.setSize(0, 0);
        sigSelectionChanged();
        sigCursorMoved(_cursorIndex, pos1);
        _cursorIndex = pos1;
      }
    break;

  default:
    if (keyEvent.keySymbol >= DIKS_SPACE && keyEvent.keySymbol <= DIKS_TILDE)
      {
        if (maxLength() > 0 && text().length() == maxLength())
          return;

        if (_selection.isNull())
          {
            _layout.insert(_cursorIndex, (char) keyEvent.keySymbol);
            sigCursorMoved(_cursorIndex, ++_cursorIndex);
          }
        else
          {
            int pos1 = std::min(_selectedIndex, _cursorIndex);
            int n1 = abs(_selectedIndex - _cursorIndex);
            _layout.replace(pos1, n1, (char) keyEvent.keySymbol);
            _selection.setSize(0, 0);
            sigCursorMoved(_cursorIndex, pos1 + 1);
            _cursorIndex = pos1 + 1;
          }
      }
    break;
    }
  updateCursorPosition();
}

void
LineInput::keyUpEvent(const KeyEvent& keyEvent)
{
  _cursorOn = true;
  _cursorTimer.start(500);
  update();
}

void
LineInput::focusInEvent()
{
  _cursorTimer.start(500);
  update();
}

void
LineInput::focusOutEvent()
{
  _cursorTimer.stop();
  _cursorOn = false;
  update();
}

void
LineInput::enterEvent(const PointerEvent& event)
{
  update();
}

void
LineInput::leaveEvent(const PointerEvent& event)
{
  update();
}

void
LineInput::drawCursor()
{
  _cursorOn = !_cursorOn;
  update(mapFromSurface(_cursor));
}

void
LineInput::compose()
{
  Painter p(this);
  p.begin();

  if (state() & FocusedState)
    p.setPen(Color(51, 128, 204));
  else if (state() & ExposedState)
    p.setPen(Color(127, 127, 127));
  else
    p.setPen(Color(0, 0, 0));

  p.setBrush(Color(255, 255, 255));

  p.fillRectangle(0, 0, width(), height());
  p.drawRectangle(0, 0, width(), height());

  p.setBrush(Color(0, 0, 0));
  p.drawLayout(_layout, borderOffset(), borderWidth());

  if (_cursorOn)
    {
      p.setBrush(Color(0, 0, 255));
      p.fillRectangle(_cursor);
    }

  p.setBrush(Color(0, 0, 200, 100));
  p.fillRectangle(_selection);

  p.end();
}

void
LineInput::updateTextLayoutGeometry()
{
  _layout.setBounds(borderOffset(), borderWidth(), width() - 2 * borderOffset(),
      height() - 2 * borderWidth());
  _layout.doLayout(font());
  _cursor.setSize(2, height() - 2 * borderWidth());
}

Font*
LineInput::defaultFont() const
{
  return stylist()->defaultFont(StyleHint::InputFont);
}
