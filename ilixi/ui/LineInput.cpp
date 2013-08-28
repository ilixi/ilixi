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

#include <ui/LineInput.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <lib/utf8.h>
#include <sstream>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_LINEINPUT, "ilixi/ui/LineInput", "LineInput");

LineInput::LineInput(const std::string& text, Widget* parent)
        : Widget(parent),
          TextBase("", this),
          _lineInputFlags(DrawFrame),
          _cursorOn(false),
          _selecting(false),
          _maxLength(-1),
          _cursorIndex(0),
          _selectedIndex(0),
          _margin(0)
{
    ILOG_TRACE_W(ILX_LINEINPUT);
    setText(text);
    setConstraints(ExpandingConstraint, FixedConstraint);
    setInputMethod(KeyPointerTrackingOSK);
    _layout.setSingleLine(true);
    _cursorTimer.sigExec.connect(sigc::mem_fun(this, &LineInput::drawCursor));
    sigTextChanged.connect(sigc::mem_fun(this, &LineInput::clearCursorIndex));
}

LineInput::~LineInput()
{
    ILOG_TRACE_W(ILX_LINEINPUT);
}

Size
LineInput::preferredSize() const
{
    ILOG_TRACE_W(ILX_LINEINPUT);
    Size s = font()->extents(text());
    return Size(s.width() + stylist()->defaultParameter(StyleHint::LineInputLR) + _margin.hSum(), s.height() + stylist()->defaultParameter(StyleHint::LineInputTB) + _margin.vSum());
}

int
LineInput::maxLength() const
{
    return (_maxLength);
}

void
LineInput::clear(int from, int chars)
{
    ILOG_TRACE_W(ILX_LINEINPUT);
    if (from == -1 && chars == -1)
    {
        setText("");
        _cursorIndex = 0;
        updateCursorPosition();
        updateSelectionRect();
        update();
    } else if (from == -1 && chars > 0)
    {
        if (chars > _cursorIndex)
            chars = _cursorIndex;

        _selecting = false;
        if (_selection.isNull())
        {
            if (_cursorIndex)
            {
                int oldIndex = _cursorIndex;
                _cursorIndex -= chars;
                sigCursorMoved(oldIndex, _cursorIndex);
                _layout.erase(_cursorIndex, chars);
                sigTextEdited();
            }
        } else
        {
            int pos1 = std::min(_selectedIndex, _cursorIndex);
            int n1 = abs(_selectedIndex - _cursorIndex);
            _layout.erase(pos1, n1);
            _selection.setSize(0, 0);
            sigTextEdited();
            sigSelectionChanged();
            sigCursorMoved(_cursorIndex, pos1);
            _cursorIndex = pos1;
        }
    }     // TODO implement from > 0
    updateCursorPosition();
    updateSelectionRect();
    update();

}

bool
LineInput::drawFrame() const
{
    return (_lineInputFlags & DrawFrame);
}

const Margin&
LineInput::margin() const
{
    return _margin;
}

const Rectangle&
LineInput::cursorRect() const
{
    return _cursor;
}

const Rectangle&
LineInput::selectionRect() const
{
    return _selection;
}

void
LineInput::setMargin(const Margin& margin)
{
    _margin = margin;
}

void
LineInput::setMaxLength(int maxLen)
{
    _maxLength = maxLen;
}

void
LineInput::setDrawFrame(bool drawFrame)
{
    if (drawFrame)
        _lineInputFlags = (LineInputFlags) (_lineInputFlags | DrawFrame);
    else
        _lineInputFlags = (LineInputFlags) (_lineInputFlags & ~DrawFrame);
}

void
LineInput::append(const std::string& text)
{
    if (_selection.isNull())
    {
        _layout.insert(_cursorIndex, std::wstring(text.begin(), text.end()));
        int oldIndex = _cursorIndex;
        _cursorIndex += text.length();
        sigCursorMoved(oldIndex, _cursorIndex);
        sigTextEdited();
    } else
    {
        int pos1 = std::min(_selectedIndex, _cursorIndex);
        int n1 = abs(_selectedIndex - _cursorIndex);
        _layout.replace(pos1, n1, std::wstring(text.begin(), text.end()));
        _selection.setSize(0, 0);
        sigCursorMoved(_cursorIndex, pos1 + 1);
        sigTextEdited();
        _cursorIndex = pos1 + 1;
    }
    updateCursorPosition();
    updateSelectionRect();
    update();
}

void
LineInput::updateCursorPosition()
{
    ILOG_TRACE_W(ILX_LINEINPUT);

    if (_cursorIndex == 0)
    {
        _layout.setX(stylist()->defaultParameter(StyleHint::LineInputLeft) + _margin.left());
        _cursor.moveTo(_layout.cursorPositon(font(), _cursorIndex));
        return;
    } else if (_layout.extents(font()).width() < width() - stylist()->defaultParameter(StyleHint::LineInputLR) - _margin.hSum())
    {
        _layout.setX(stylist()->defaultParameter(StyleHint::LineInputLeft) + _margin.left());
        _cursor.moveTo(_layout.cursorPositon(font(), _cursorIndex));
        return;
    }

    int x = _layout.cursorPositon(font(), _cursorIndex).x();
    ILOG_DEBUG(ILX_LINEINPUT, " -> x: %d\n", x);

    if (x < stylist()->defaultParameter(StyleHint::LineInputLeft) + _margin.left())
    {
        int dif = (stylist()->defaultParameter(StyleHint::LineInputLeft) + _margin.left()) - x;
        _layout.setX(dif);

        ILOG_DEBUG( ILX_LINEINPUT, " -> 1 Layout x: %d  CursorX: %d Dif: %d Len: %lu\n", _layout.x(), x, dif, _layout.text().length());
    } else if (x > _layout.bounds().right())
    {
        int dif = _layout.bounds().right() - x;
        _layout.setX(dif);

        ILOG_DEBUG( ILX_LINEINPUT, " -> 2 Layout x: %d  CursorX: %d Dif: %d Len: %lu\n", _layout.x(), x, dif, _layout.text().length());
    }

    _cursor.moveTo(_layout.cursorPositon(font(), _cursorIndex));
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
                _selection.setRight(_layout.cursorPositon(font(), _selectedIndex).x());
                _selection.setHeight(_cursor.height());
            } else
            {
                _selection.setX(_layout.cursorPositon(font(), _selectedIndex).x());
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
    if (_layout.isEmpty())
        return;

    Point p = mapToSurface(Point(mouseEvent.x, mouseEvent.y));
    int index = _layout.xyToIndex(font(), p.x() - _layout.x(), p.y());

    ILOG_TRACE_W(ILX_LINEINPUT);
    ILOG_DEBUG(ILX_LINEINPUT, " -> index: %d\n", index);

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
        if (_layout.isEmpty())
            return;

        Point p = mapToSurface(Point(mouseEvent.x, mouseEvent.y));
        int index = _layout.xyToIndex(font(), p.x() - _layout.x(), p.y());

        ILOG_TRACE_W(ILX_LINEINPUT);
        ILOG_DEBUG(ILX_LINEINPUT, " -> index: %d\n", index);

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
    switch (keyEvent.keySymbol)
    {
    case DIKS_CURSOR_LEFT:
    case DIKS_CURSOR_UP:
        if (_cursorIndex)
            sigCursorMoved(_cursorIndex, --_cursorIndex);
        else
            return;
        break;

    case DIKS_CURSOR_RIGHT:
    case DIKS_CURSOR_DOWN:
        if (_cursorIndex < text().length())
            sigCursorMoved(_cursorIndex, ++_cursorIndex);
        else
            return;
        break;

    case DIKS_PAGE_UP:
    case DIKS_HOME:
        sigCursorMoved(_cursorIndex, 0);
        _cursorIndex = 0;
        break;

    case DIKS_PAGE_DOWN:
    case DIKS_END:
        sigCursorMoved(_cursorIndex, text().length());
        _cursorIndex = text().length();
        break;

    case DIKS_DELETE:
        _selecting = false;
        if (_selection.isNull())
        {
            if (_cursorIndex > -1 && _cursorIndex < text().length())
                _layout.erase(_cursorIndex, 1);
        } else
        {
            int pos1 = std::min(_selectedIndex, _cursorIndex);
            int n1 = abs(_selectedIndex - _cursorIndex);
            _layout.erase(pos1, n1);
            _selection.setSize(0, 0);
            sigTextEdited();
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
                sigTextEdited();
            }
        } else
        {
            int pos1 = std::min(_selectedIndex, _cursorIndex);
            int n1 = abs(_selectedIndex - _cursorIndex);
            _layout.erase(pos1, n1);
            _selection.setSize(0, 0);
            sigTextEdited();
            sigSelectionChanged();
            sigCursorMoved(_cursorIndex, pos1);
            _cursorIndex = pos1;
        }
        break;

    case DIKS_ENTER:
        sigTextEntered(text());
        break;

    case DIKS_SHIFT:
        if (!_selecting)
            startSelection();
        break;

    default:
        _selecting = false;
        if (maxLength() > 0 && text().length() == maxLength())
            return;

        if (_selection.isNull())
        {
            _layout.insert(_cursorIndex, (wchar_t) keyEvent.keySymbol);
            sigCursorMoved(_cursorIndex, ++_cursorIndex);
            sigTextEdited();
            ILOG_DEBUG( ILX_LINEINPUT, "Append U+%04X at %d\n", keyEvent.keySymbol, _cursorIndex);
        } else
        {
            int pos1 = std::min(_selectedIndex, _cursorIndex);
            int n1 = abs(_selectedIndex - _cursorIndex);
            _layout.replace(pos1, n1, (wchar_t) keyEvent.keySymbol);
            _selection.setSize(0, 0);
            sigCursorMoved(_cursorIndex, pos1 + 1);
            sigTextEdited();
            _cursorIndex = pos1 + 1;
            ILOG_DEBUG( ILX_LINEINPUT, "Replaced U+%04X at %d\n", keyEvent.keySymbol, pos1);
        }
        break;
    }
    updateCursorPosition();
    updateSelectionRect();
    update();
}

void
LineInput::keyUpEvent(const KeyEvent& keyEvent)
{
    _cursorOn = true;
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
    _selection.setSize(0, 0);
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
    update(PaintEvent(mapFromSurface(_cursor), z()));
}

void
LineInput::clearCursorIndex(const std::string& text)
{
    _cursorIndex = text.length();
    _selectedIndex = 0;
    updateCursorPosition();
    updateSelectionRect();
    update();
}

void
LineInput::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    stylist()->drawLineInput(&p, this, _cursorOn);
    p.end();
}

void
LineInput::updateTextBaseGeometry()
{
    _layout.setBounds(stylist()->defaultParameter(StyleHint::LineInputLeft) + _margin.left(), stylist()->defaultParameter(StyleHint::LineInputTop) + _margin.top(), width() - stylist()->defaultParameter(StyleHint::LineInputLR) - _margin.hSum(), height() - stylist()->defaultParameter(StyleHint::LineInputTB) - _margin.vSum());
    _layout.doLayout(font());
    _cursor.setSize(1, height() - stylist()->defaultParameter(StyleHint::LineInputTB));
    updateCursorPosition();
}

Font*
LineInput::defaultFont() const
{
    return stylist()->defaultFont(StyleHint::InputFont);
}

}
