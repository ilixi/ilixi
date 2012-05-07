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
    Widget(parent), TextBase(text, this), BorderBase(this), _cursorOn(false), _selecting(
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
  // FIXME should not move to 0 for left.
  int x = _layout.cursorPositon(font(), _cursorIndex).x();

  if (x < borderOffset())
    {
      int dif = x - _layout.x();
      if (dif <= borderOffset() && _layout.text().length())
        _layout.setX(dif);

      printf("1 LX: %d  CX: %d D: %d Len: %d\n", _layout.x(), x, dif,
          _layout.text().length());
    }
  else
    {
      int dif = x - _layout.bounds().right();
      if (dif > 0)
        _layout.setX(-dif);

      printf("2 LX: %d  CX: %d D: %d Len: %d\n", _layout.x(), x, dif,
          _layout.text().length());
    }

  _cursor.moveTo(_layout.cursorPositon(font(), _cursorIndex).x(),
      _layout.cursorPositon(font(), _cursorIndex).y());
}

void
LineInput::updateSelectionRect()
{
  if (_selecting)
    {
      if (_selectedIndex == _cursorIndex)
        _selection.setSize(0, 0);
      else
        {
          if (_selectedIndex > _cursorIndex)
            {
              _selection.moveTo(_cursor.topLeft());
              _selection.setRight(
                  _layout.cursorPositon(font(), _selectedIndex).x());
              _selection.setHeight(_cursor.height());
            }
          else
            {
              _selection.setX(
                  _layout.cursorPositon(font(), _selectedIndex).x());
              _selection.setRight(_cursor.x());
              _selection.setHeight(_cursor.height());
            }
          sigSelectionChanged();
        }
    }
}

void
LineInput::startSelection()
{
  _selecting = true;
  _selectedIndex = _cursorIndex;
  _selection.moveTo(_cursor.x(), _cursor.y());
  _selection.setSize(0, 0);
}

void
LineInput::stopSelection()
{
  _selecting = false;
  _selection.setSize(0, 0);
}

void
LineInput::pointerButtonDownEvent(const PointerEvent& mouseEvent)
{
  Point p = mapToSurface(Point(mouseEvent.x, mouseEvent.y));

  int index = _layout.xyToIndex(font(), p.x() - _layout.x(), p.y());

  if (index != _cursorIndex)
    {
      _cursorTimer.stop();
      sigCursorMoved(_cursorIndex, index);
      _cursorIndex = index;

      updateCursorPosition();
      startSelection();
      _cursorOn = true;
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
  if (_selecting && (mouseEvent.buttonMask & ButtonMaskLeft))
    {
      Point p = mapToSurface(Point(mouseEvent.x, mouseEvent.y));
      int index = _layout.xyToIndex(font(), p.x() - _layout.x(), p.y());

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
    if (keyEvent.modifierMask & DIMM_SHIFT)
      {
        if (!_selecting)
          startSelection();
      }
    else
      stopSelection();

    if (_cursorIndex)
      sigCursorMoved(_cursorIndex, --_cursorIndex);
    break;

  case DIKS_CURSOR_RIGHT:
  case DIKS_CURSOR_DOWN:
    if (keyEvent.modifierMask & DIMM_SHIFT)
      {
        if (!_selecting)
          startSelection();
      }
    else
      stopSelection();

    if (_cursorIndex < text().length())
      sigCursorMoved(_cursorIndex, ++_cursorIndex);
    break;

  case DIKS_PAGE_UP:
  case DIKS_HOME:
    if (keyEvent.modifierMask & DIMM_SHIFT)
      {
        if (!_selecting)
          startSelection();
      }
    else
      stopSelection();

    sigCursorMoved(_cursorIndex, 0);
    _cursorIndex = 0;
    break;

  case DIKS_PAGE_DOWN:
  case DIKS_END:
    if (keyEvent.modifierMask & DIMM_SHIFT)
      {
        if (!_selecting)
          startSelection();
      }
    else
      stopSelection();

    sigCursorMoved(_cursorIndex, text().length());
    _cursorIndex = text().length() - 1;
    break;

  case DIKS_DELETE:
    _selecting = false;
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
    _selecting = false;
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
        _selecting = false;
        if (maxLength() > 0 && text().length() == maxLength())
          return;

        if (_selection.isNull())
          {
            _layout.insert(_cursorIndex, (char) keyEvent.keySymbol);
            sigCursorMoved(_cursorIndex, ++_cursorIndex);
            printf("Append %c\n", (char) keyEvent.keySymbol);
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
  updateSelectionRect();
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

  p.setBrush(Color(51, 204, 255, 50));
  p.fillRectangle(_selection);

  p.setBrush(Color(0, 0, 0));
  p.setClip(borderOffset(), borderWidth(), width() - 2 * borderOffset(),
      height() - 2 * borderWidth());
  p.drawLayout(_layout);
  p.resetClip();

  if (_cursorOn)
    {
      p.setBrush(Color(0, 0, 255));
      p.fillRectangle(_cursor);
    }

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
