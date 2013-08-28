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

#ifndef ILIXI_COLOR_H_
#define ILIXI_COLOR_H_

extern "C" {
    #include <dfb_types.h>
}

#include <directfb.h>

namespace ilixi
{
//! 32-bit color with transparency.
/*!
 * Colors are defined by rgba components. Each component can store a value between 0 and 255.
 *
 */
class Color
{
    //! Hue-Saturation-Value representation
    struct HSV
    {
        float hue; //!< Hue in degrees [0, 360)
        float saturation; //!< Lightness [0, 1]
        float value; //!< Lightness [0, 1]

        /*!
         * Set values to 0.
         */
        HSV();

        /*!
         * Create HSV from values.
         */
        HSV(float hue, float saturation, float value);

        /*!
         * Create HSV from color.
         */
        HSV(const Color& color);
    };

    //! Hue-Saturation-Lightness representation
    struct HSL
    {
        float hue; //!< Hue in degrees [0, 360)
        float saturation; //!< Saturation [0, 1]
        float lightness; //!< Lightness [0, 1]

        /*!
         * Set values to 0.
         */
        HSL();

        /*!
         * Set HSL from values.
         */
        HSL(float hue, float saturation, float lightness);

        /*!
         * Set HSL from color.
         */
        HSL(const Color& color);
    };

public:
    /*!
     * Creates an opaque black color.
     */
    Color();

    /*!
     * Copy constructor.
     */
    Color(const Color& color);

    /*!
     * Creates a color using given values.
     * @param red [0-255]
     * @param green [0-255]
     * @param blue [0-255]
     * @param alpha [0-255] default is 255.
     */
    Color(u8 red, u8 green, u8 blue, u8 alpha = 255);

    /*!
     * Creates a color from hex value, e.g. FF0000
     *
     * @param hex [00000-FFFFF]
     * @param alpha default is 255.
     */
    Color(const char* hex, u8 alpha = 255);

    /*!
     * Destructor.
     */
    ~Color();

    /*!
     * Returns color alpha value.
     */
    u8
    alpha() const;

    /*!
     * Returns color blue value.
     */
    u8
    blue() const;

    /*!
     * Returns color green value.
     */
    u8
    green() const;

    /*!
     * Returns color red value.
     */
    u8
    red() const;

    /*!
     * Set color alpha value.
     * @param alpha [0-255]
     */
    void
    setAlpha(u8 alpha);

    /*!
     * Set color blue value.
     * @param blue [0-255]
     */
    void
    setBlue(u8 blue);

    /*!
     * Set color green value.
     * @param green [0-255]
     */
    void
    setGreen(u8 green);

    /*!
     * Set color red value.
     * @param red [0-255]
     */
    void
    setRed(u8 red);

    /*!
     * Sets brightness using given value [0-1].
     */
    void
    setBrightness(float brightness);

    /*!
     * Multiplies brightness by factor [0-1].
     */
    void
    adjustBrightness(float factor);

    /*!
     * Sets saturation using given value [0-1].
     */
    void
    setSaturation(float saturation);

    /*!
     * Multiplies saturation by factor [0-1].
     */
    void
    adjustSaturation(float factor);

    /*!
     * Sets hue using given value [0-360].
     */
    void
    setHue(u16 hue);

    /*!
     * Multiplies hue by factor [0-1].
     */
    void
    adjustHue(float factor);

    /*!
     * Set color using given values.
     */
    void
    setRGBA(u8 red, u8 green, u8 blue, u8 alpha = 255);

    /*!
     * Set RGB from html hex values, e.g. FFFFFF
     */
    void
    setFromHex(const char* hex, u8 a = 255);

    /*!
     * Set RGB from HSV.
     */
    void
    setFromHSV(const HSV& hsb);

    /*!
     * Set RGB from HSL.
     */
    void
    setFromHSL(const HSL& hsl);

    void
    interpolate(const Color& color, float factor);

    /*!
     * Assigns a copy of color to this color, and returns a reference to it.
     */
    Color&
    operator=(const Color& color);

    /*!
     * Returns true if this color has the same ARGB values as c; otherwise returns false.
     */
    bool
    operator==(const Color& c) const;

    /*!
     * Returns true if this color has a different ARGB values from c; otherwise returns false.
     */
    bool
    operator!=(const Color& c) const;

    /*!
     * Returns the DFBColor.
     */
    DFBColor
    dfbColor() const;

private:
    //! Red component
    u8 _red;
    //! Green component
    u8 _green;
    //! Blue component
    u8 _blue;
    //! Alpha component
    u8 _alpha;
};

inline u8
Color::alpha() const
{
    return _alpha;
}

inline u8
Color::blue() const
{
    return _blue;
}

inline u8
Color::green() const
{
    return _green;
}

inline u8
Color::red() const
{
    return _red;
}
}

#endif /* ILIXI_COLOR_H_ */
