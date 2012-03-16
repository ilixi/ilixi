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

#ifndef ILIXI_LINEINPUT_H_
#define ILIXI_LINEINPUT_H_

#include "ui/TextBase.h"
#include "ui/BorderBase.h"
#include "lib/Timer.h"

namespace ilixi
{

  //! Single line text input
  class LineInput : public TextBase, public BorderBase
  {
  public:
    /*!
     * Constructor.
     *
     * @param text Default text to show.
     * @param parent Widget.
     */
    LineInput(const std::string& text, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~LineInput();

    /*!
     * Preferred size calculated using current text.
     */
    virtual Size
    preferredSize() const;

    /*!
     * Returns number of maximum characters to store.
     */
    int
    maxLength() const;

    /*!
     * Sets number of maximum characters to store.
     */
    void
    setMaxLength(int maxLen);

    /*!
     * This signal is emitted once cursor is moved.
     * First param is old position, second is new position.
     */
    sigc::signal<void, int, int> sigCursorMoved;

    /*!
     * This signal is emitted once selection is changed.
     */
    sigc::signal<void> sigSelectionChanged;

  private:
    //! Cursor display flag.
    bool _cursorOn;
    //! Selection active flag.
    bool _selecting;
    //! Number of maximum characters to store.
    int _maxLength;
    //! Current cursor index.
    int _cursorIndex;
    //! Selection start index.
    int _selectedIndex;

    //! Cursor position and size.
    Rectangle _cursor;
    //! Selection position and size.
    Rectangle _selection;

    //! Used for drawing cursor, by default interval is set to 500ms.
    Timer _cursorTimer;

    //! Update cursor rectangle using current index.
    void
    updateCursorPosition();

    //! Update selection rectangle using selection and cursor index.
    void
    updateSelectionRect();

    void
    startSelection();

    void
    stopSelection();

    virtual void
    pointerButtonDownEvent(const PointerEvent& mouseEvent);

    void
    pointerButtonUpEvent(const PointerEvent& mouseEvent);

    void
    pointerMotionEvent(const PointerEvent& mouseEvent);

    virtual void
    keyDownEvent(const KeyEvent& keyEvent);

    virtual void
    keyUpEvent(const KeyEvent& keyEvent);

    virtual void
    focusInEvent();

    virtual void
    focusOutEvent();

    virtual void
    enterEvent(const ilixi::PointerEvent& event);

    virtual void
    leaveEvent(const ilixi::PointerEvent& event);

    void
    drawCursor();

    void
    compose();

    void
    updateTextLayoutGeometry();

    virtual Font*
    defaultFont() const;
  };

} /* namespace ilixi */
#endif /* ILIXI_LINEINPUT_H_ */
