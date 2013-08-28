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

#ifndef ILIXI_LINEINPUT_H_
#define ILIXI_LINEINPUT_H_

#include <ui/TextBase.h>
#include <lib/Timer.h>
#include <types/Margin.h>

namespace ilixi
{

//! Single line text input
class LineInput : public Widget, public TextBase
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
     * Clears text.
     */
    void
    clear(int from = -1, int chars = -1);

    /*!
     * Returns true if line input draws frame.
     */
    bool
    drawFrame() const;

    /*!
     * Returns margin around text.
     */
    const Margin&
    margin() const;

    // TODO Add postfix and prefix to LineInput?
//    /*!
//     * Returns text appended to the start of text.
//     */
//    const std::string&
//    postfix() const;
//
//    /*!
//     * Returns text prepended to the start of text.
//     */
//    const std::string&
//    prefix() const;

    /*!
     * Returns a rectangle at caret position.
     */
    const Rectangle&
    cursorRect() const;

    /*!
     * Returns a rectangle around selected text.
     */
    const Rectangle&
    selectionRect() const;

    /*!
     * Sets margin around text.
     */
    void
    setMargin(const Margin& margin);

    /*!
     * Sets number of maximum characters to store.
     */
    void
    setMaxLength(int maxLen);

    /*!
     * Enable/Disable drawing frame.
     */
    void
    setDrawFrame(bool drawFrame);

    void
    append(const std::string& text);

    /*!
     * This signal is emitted once cursor is moved.
     * First param is old position, second is new position.
     */
    sigc::signal<void, int, int> sigCursorMoved;

    /*!
     * This signal is emitted once selection is changed.
     */
    sigc::signal<void> sigSelectionChanged;

    /*!
     * This signal is emitted if text is edited.
     */
    sigc::signal<void> sigTextEdited;

    sigc::signal<void, const std::string&> sigTextEntered;

private:
    enum LineInputFlags
    {
        DrawFrame = 0x001, //!< This flag controls whether line input draws frame.
        ReadOnly = 0x002, //!< Text can not be edited.
        Password = 0x004, //!< Asterisks are displayed instead of characters.
        PasswordEdit = 0x008, //!< Display characters as entered, otherwise display asterisks.
        NoText = 0x010,  //!< Text is not displayed at all.
        Numeric = 0x100, //!< Only accepts numbers.
        Alphabetic = 0x200, //!< Only accepts letters.
        AlphaNumeric = 0x300 //!< Accept characters and numbers.
    };

    //! This flag controls whether line input draws frame.
    LineInputFlags _lineInputFlags;
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
    //! Margin around text.
    Margin _margin;

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
    compose(const PaintEvent& event);

    void
    clearCursorIndex(const std::string&);

    void
    updateTextBaseGeometry();

    virtual Font*
    defaultFont() const;
};

} /* namespace ilixi */
#endif /* ILIXI_LINEINPUT_H_ */
