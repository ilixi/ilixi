/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#ifndef ILIXI_STYLE_H_
#define ILIXI_STYLE_H_

#include "types/Image.h"
#include "types/Font.h"

namespace ilixi
{
  struct Style
  {
    /*!
     * Initialise to default.
     */
    Style();

    /*!
     * Destructor.
     */
    ~Style();

    void
    release();

    /*!
     * Initialise style from an XML file.
     *
     * Returns false on error.
     */
    bool
    parseStyle(const char* filename);

    //--------------------------------------------------------------
    // Fonts
    //--------------------------------------------------------------
    //! Font for rendering button text.
    Font* _buttonFont;
    //! Default font to render text.
    Font* _defaultFont;
    //! Font for rendering input text.
    Font* _inputFont;
    //! Font for rendering title text.
    Font* _titleFont;
    //--------------------------------------------------------------
    // Images
    //--------------------------------------------------------------
    //! Plus sign image.
    Image* _plusSign;
    //! Minus sign image.
    Image* _minusSign;
    //! ScrollBar grid image.
    Image* _grid;

    //! Arrow image used in buttons for up.
    Image* _arrowUp;
    //! Arrow image used in buttons for down.
    Image* _arrowDown;
    //! Arrow image used in buttons for left.
    Image* _arrowLeft;
    //! Arrow image used in buttons for right.
    Image* _arrowRight;

    //! CheckBox not selected image.
    Image* _checkEmpty;
    //! CheckBox partially selected image.
    Image* _checkPartial;
    //! CheckBox selected state image.
    Image* _checkFull;

    //! Critical icon.
    Image* _critical;
    //! Information icon.
    Image* _info;
    //! Question icon.
    Image* _question;
    //! Warning icon.
    Image* _warning;
    //--------------------------------------------------------------
    // Hints
    //--------------------------------------------------------------
    //! This property specifies the button radius.
    int _buttonRadius;
    //! This property specifies the space between icon and text.
    int _buttonOffset;
    //! This property specifies the width of tool button indicators.
    int _buttonIndicator;
    //! This property specifies  the default size for buttons.
    Size _buttonSize;

    //! This property specifies the check box radius.
    int _checkboxRadius;
    //! This property specifies the space between icon and text.
    int _checkboxOffset;
    //! This property specifies the default size for check box.
    Size _checkboxSize;

    //! This property specifies the combo box frame radius.
    int _comboboxRadius;
    //! This property specifies the combo box button width.
    int _comboboxButtonWidth;

    //! This property specifies the default frame border radius.
    int _frameRadius;

    //! This property specidies the default icon size.
    Size _iconSize;

    //! This property specifies progress bar frame radius.
    int _progressbarRadius;
    //! This property specifies the default size for progress bar.
    Size _progressbarSize;

    //! This property specifies the space between icon and text.
    int _radiobuttonOffset;
    //! This property specifies the default size for radio button.
    Size _radiobuttonSize;

    //! This property specifies the slider frame radius.
    int _sliderRadius;
    //! This property specifies the default size for slider.
    Size _sliderSize;

    //! This property specifies the scroll bar frame radius.
    int _scrollbarRadius;
    //! This property specifies the default size for scroll bar.
    Size _scrollbarSize;

    //! This property specifies the offset for buttons.
    int _tabPanelButtonOffset;
    //! This property specifies the default size for panel.
    Size _tabPanelSize;
    //! This property specifies the default size for tab panel buttons.
    Size _tabPanelButtonSize;

    //! This property specifies the frame text input frame radius.
    int _textInputRadius;

    //! This property specifies the default height for a toolbar.
    int _toolbarHeight;

  };

}

#endif /* ILIXI_STYLE_H_ */
