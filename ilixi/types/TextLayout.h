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

#ifndef TEXTLAYOUT_H_
#define TEXTLAYOUT_H_

#include <list>
#include <types/Font.h>
#include <types/Rectangle.h>

namespace ilixi
{
//! Used for rendering multiple lines of text.
/*!
 * TextLayout provides a simple way to render multiple lines of text
 * inside a bounding rectangle.
 */
class TextLayout
{
    friend class Painter; // drawLayout
    friend class Notify;

public:
    //! This enum specifies text layout alignment.
    enum Alignment
    {
        Left = DSTF_TOPLEFT,        //!< left aligned
        Center = DSTF_TOPCENTER,    //!< horizontally centered
        Right = DSTF_TOPRIGHT       //!< right aligned
    };

    //! This structure is used for caching line parameters.
    struct LayoutLine
    {
        LayoutLine()
                : offset(0),
                  bytes(0),
                  y(0),
                  length(0),
                  lineWidth(0)
        {
        }

        int offset;     //! offset from first character of line
        int bytes;      //! number of bytes to render on line
        int y;          //! top-left coordinate of line
        int length;     //! number of characters on line
        int lineWidth;  //! logical width of text on line
    };

    /*!
     * Constructor.
     */
    TextLayout();

    /*!
     * Constructor.
     */
    TextLayout(const std::string& text);

    /*!
     * Copy constructor.
     */
    TextLayout(const TextLayout& layout);

    /*!
     * Destructor.
     */
    virtual
    ~TextLayout();

    /*!
     * Returns the horizontal alignment of text inside layout.
     */
    Alignment
    alignment() const;

    /*!
     * Returns bounds of layout.
     */
    Rectangle
    bounds() const;

    /*!
     * Returns the dimension of text in pixels.
     */
    Size
    extents(Font* font) const;

    /*!
     * Returns x-coordinate of layout.
     */
    int
    x() const;

    /*!
     * Returns y-coordinate of layout.
     */
    int
    y() const;

    /*!
     * Returns width of layout.
     */
    int
    width() const;

    /*!
     * Returns height of layout.
     */
    int
    height() const;

    /*!
     * Returns true if layout does not provide multiple lines.
     */
    bool
    singleLine() const;

    /*!
     * Returns true if layout text is empty.
     */
    bool
    isEmpty() const;

    /*!
     * Returns text inside layout.
     */
    std::string
    text() const;

    /*!
     * Returns text inside layout.
     */
    std::wstring
    wtext() const;

    /*!
     * Calculates coordinates for top-left corner of cursor.
     *
     * @param index
     */
    Point
    cursorPositon(Font* font, int index);

    /*!
     * Returns the character index at given coordinates.
     *
     * @param x
     * @param y
     */
    int
    xyToIndex(Font* font, int x, int y);

    /*!
     * Insert a character at given position.
     *
     * @param pos
     * @param c
     */
    void
    insert(int pos, wchar_t c);

    /*!
     * Insert a string at given position.
     *
     * @param pos
     * @param str
     */
    void
    insert(int pos, const std::wstring& str);

    /*!
     * Replace a portion of text with a char.
     * @param pos
     * @param number
     * @param c
     */
    void
    replace(int pos, int number, wchar_t c);

    /*!
     * Replace a portion of text with another string.
     *
     * @param pos
     * @param number
     * @param str
     */
    void
    replace(int pos, int number, const std::wstring& str);

    /*!
     * Erases a part of text, shortening the length of the string.
     *
     * @param pos
     * @param amount
     */
    void
    erase(int pos, int amount);

    /*!
     * Sets the horizontal alignment of text inside layout.
     */
    void
    setAlignment(Alignment alignment);

    /*!
     * Sets x-coordinate of bounding rectangle.
     */
    void
    setX(int x);

    /*!
     * Sets y-coordinate of bounding rectangle.
     */
    void
    setY(int y);

    /*!
     * Sets width of bounding rectangle.
     */
    void
    setWidth(int width);

    /*!
     * Sets height of bounding rectangle.
     */
    void
    setHeight(int height);

    /*!
     * Sets the bounding rectangle of layout.
     */
    void
    setBounds(int x, int y, int w, int h);

    /*!
     * Sets the bounding rectangle of layout.
     */
    void
    setBounds(const Rectangle& rect);

    /*!
     * Sets whether layout is multiple or single line.
     */
    void
    setSingleLine(bool singleLine);

    /*!
     * Sets the text of layout.
     */
    void
    setText(const std::string& text);

    /*!
     * Sets the text of layout.
     */
    void
    setText(const std::wstring& text);

    /*!
     * Sets the modified flag.
     */
    void
    setModified();

    /*!
     * Lays out text inside bounding rectangle using given font.
     */
    void
    doLayout(Font* font);

    /*!
     * Returns height of layout for given width.
     */
    int
    heightForWidth(int width, Font* font) const;

private:
    //! Flag is set to true if layout is modified.
    bool _modified;
    //! Flag is set to true if layout is single line.
    bool _singleLine;
    //! Text inside layout
    std::wstring _text;
    //! Horizontal alignment of text inside layout.
    Alignment _alignment;
    //! Bounding rectangle of layout.
    Rectangle _bounds;

    typedef std::list<LayoutLine> LineList;
    //! List of lines inside layout.
    LineList _lines;

    void
    drawTextLayout(IDirectFBSurface* surface, int x = 0, int y = 0) const;
};

}

#endif /* TEXTLAYOUT_H_ */
