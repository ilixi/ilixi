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

#ifndef ILIXI_FONTS_H_
#define ILIXI_FONTS_H_

#include <types/Font.h>
#include <types/Enums.h>
#include <iostream>

namespace ilixi
{
//! Provides a font pack.
class FontPack
{
public:
    /*!
     * Constructor.
     */
    FontPack();

    /*!
     * Destructor.
     */
    virtual
    ~FontPack();

    /*!
     * Returns the font for given type.
     */
    Font*
    getFont(StyleHint::FontHint font = StyleHint::DefaultFont) const;

    /*!
     * Initialise fonts from an XML file.
     *
     * Returns false on error.
     */
    bool
    parseFonts(const char* fontsFile);

private:
    //! Font for rendering button text.
    Font* _buttonFont;
    //! Default font to render text.
    Font* _defaultFont;
    //! Font for rendering input text.
    Font* _inputFont;
    //! Font for rendering title text.
    Font* _titleFont;
    //! Font for rendering info text.
    Font* _infoFont;

    //! Release fonts.
    void
    release();

    friend std::istream&
    operator>>(std::istream& is, FontPack& obj);

    friend std::ostream&
    operator<<(std::ostream& os, const FontPack& obj);
};

} /* namespace ilixi */
#endif /* ILIXI_FONTS_H_ */
