/*
 Copyright 2010-2015 Tarik Sekmen.

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

#ifndef ILIXI_FONT_H_
#define ILIXI_FONT_H_

#include <ilixiConfig.h>
#include <string>
#include <directfb.h>
#include <types/Size.h>
#include <lib/Util.h>   // for ILIXI_DEPRECATED

#ifdef ILIXI_HAVE_CAIRO
#include <cairo-directfb.h>
#endif

namespace ilixi
{
//! Specifies a font for drawing text.
/*!
 * This class enables to set and query attributes of a font. It also
 * provides measurement information for laying out text.
 */
class Font
{
    friend class CairoPainter; // applyFont
    friend class Painter; // applyFont
    friend class TextBase;

public:
    //! Specifies style for font.
    enum Style
    {
        Plain = 0x00000000, //!< Normal style
        Bold = 0x00000800,  //!< Bold style
        Italic = 0x00000200 //!< Italic style
    };

    /*!
     * Creates a font with a default description.
     */
    Font();

    /*!
     * Creates a new font.
     */
    Font(const std::string& name, int size = 12);

    /*!
     * Copy constructor.
     */
    Font(const Font& font);

    /*!
     * Destructor.
     */
    ~Font();

    /*!
     * Returns IDirectFBFont interface for font.
     */
    IDirectFBFont*
    dfbFont();

    /*!
     * Returns name of font.
     */
    const std::string&
    name() const;

    /*!
     * Returns distance from the baseline to the ascender line.
     *
     * Ascender usually specifies the height of capital letters.
     *
     * 0 is returned if font can not be loaded.
     */
    int
    ascender();

    /*!
     * Returns distance from the baseline to the descender line.
     *
     * Descender usually specifies the lowest point for some characters.
     *
     * 0 is returned if font can not be loaded.
     */
    int
    descender();

    /*!
     * Returns the distance from one baseline to the next.
     *
     * This value is useful for rendering multiple lines of text.
     *
     * 0 is returned if font can not be loaded.
     */
    int
    leading();

    /*!
     * Returns dimension of text in pixels.
     *
     * An invalid size is returned if font can not be loaded.
     *
     * @param bytes number of bytes to use, -1 for whole string.
     */
    Size
    extents(const std::string& text, int bytes = -1);

    /*!
     * Returns the dimension of a glyph specified by its character code.
     *
     * @param c
     */
    Size
    glyphExtents(unsigned int c);

    /*!
     * Returns the width of a glyph.
     *
     * @param c
     */
    int
    glyphAdvance(unsigned int c);

    /*!
     * Returns next explicit or automatic break within a string along with
     * the logical width of the text, the string length, and a pointer to
     * the next text line.
     *
     * @param offset number of bytes to use, -1 for whole string.
     */
    void
    stringBreak(const char* text, int offset, int maxWidth, int* lineWidth, int* length, const char** nextLine);

    /*!
     * Returns the logical width of text in pixels.
     *
     * Negative width indicates right-to-left rendering.
     *
     * 0 is returned if font can not be loaded.
     *
     * @param offset number of bytes to use, -1 for whole string.
     */
    int
    textWidth(const std::string& text, int offset = -1);

    /*!
     * Returns font size.
     */
    int
    size() const;

    /*!
     * Returns font style.
     */
    Style
    style() const;

    /*!
     * Sets the default encoding for font.
     */
    void
    setEncoding(DFBTextEncodingID encoding);

    /*!
     * Set font size.
     */
    void
    setSize(int size);

    /*!
     * Sets style for font, e.g. italic, bold.
     */
    void
    setStyle(Style style);

    /*!
     * Assigns f to this font and returns a reference to it.
     */
    Font&
    operator=(const Font& f);

    /*!
     * Returns true if this font is equal to f; otherwise returns false.
     */
    bool
    operator==(const Font &f);

    /*!
     * Returns true if this font is different from f; otherwise returns false.
     */
    bool
    operator!=(const Font &f);

    /*!
     * Returns a string for this font.
     */
    std::string
    toString() const;

private:
    //! Flag is set to true if font is modified.
    bool _modified;
    //! DirectFB font interface.
    IDirectFBFont* _font;
    //! Font size.
    int _size;
    //! Font attributes.
    DFBFontAttributes _attr;
    //! Font name.
    std::string _name;
    //! Key returned from FontCache.
    unsigned int _key;

    //! Applies font to surface.
    bool
    applyFont(IDirectFBSurface* surface);

#ifdef ILIXI_HAVE_CAIRO
    bool
    applyFont(cairo_t* context);
#endif

    //! Load IDirectFB font.
    bool
    loadFont();

    //! Release DirectFB font interface.
    void
    release();

    ILIXI_DEPRECATED(void addRef(), "References are now automatic");

    ILIXI_DEPRECATED(void relRef(), "References are now automatic");

    friend std::istream&
    operator>>(std::istream& is, Font& obj);

    friend std::ostream&
    operator<<(std::ostream& os, const Font& obj);
};
}

#endif /* ILIXI_FONT_H_ */
