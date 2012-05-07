/*
 Copyright 2012 Tarik Sekmen.

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

#ifndef ILIXI_TEXTBASE_H_
#define ILIXI_TEXTBASE_H_

#include "ui/Widget.h"
#include "types/Font.h"
#include "types/TextLayout.h"

namespace ilixi
{
  //! Base class for widgets with text display.
  class TextBase
  {
  public:
    /*!
     * Constructor.
     */
    TextBase(Widget* owner);

    /*!
     * Constructor.
     */
    TextBase(const std::string& text, Widget* owner);

    /*!
     * Copy constructor.
     */
    TextBase(const TextBase& tb);

    /*!
     * Destructor.
     */
    virtual
    ~TextBase();

    /*!
     * Returns the font used by this widget.
     */
    Font*
    font() const;

    /*!
     * Returns text layout.
     */
    TextLayout
    layout() const;

    /*!
     * Returns font style.
     */
    Font::Style
    style() const;

    /*!
     * Returns the text to be displayed.
     */
    std::string
    text() const;

    /*!
     * Returns the dimension of text in pixels.
     */
    Size
    textExtents() const;

    /*!
     * Returns height of text for given text.
     */
    int
    textLayoutHeightForWidth(int width) const;

    /*!
     * Sets the alignment of text inside layout.
     */
    void
    setLayoutAlignment(TextLayout::Alignment alignment);

    /*!
     * Sets the font.
     *
     * Note any previous font set using this method will be destroyed.
     */
    void
    setFont(Font* font);

    /*!
     * Sets the text.
     */
    virtual void
    setText(const std::string &text);

    /*!
     * This signal is emitted if the text is changed.
     */
    sigc::signal<void, std::string> sigTextChanged;

    /*!
     * This signal is emitted once the font is changed.
     */
    sigc::signal<void> sigFontChanged;

  protected:
    //! TextLayout for this widget.
    TextLayout _layout;

    /*!
     * This method is called if widget's geometry is modified.
     *
     * You could reimplement this method if your widget needs a specific
     * bounding rectangle for text layout. You must execute layout's doLayout
     * method once you set the geometry.
     */
    virtual void
    updateTextLayoutGeometry();

  private:
    //! Font for this widget.
    Font* _font;
    //! Owner of this text layout.
    Widget* _owner;

    //! Returns default font for this widget.
    virtual Font*
    defaultFont() const;
  };

}

#endif /* TEXTBASE_H_ */
