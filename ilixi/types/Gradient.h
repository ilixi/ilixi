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

#ifndef ILIXI_GRADIENT_H_
#define ILIXI_GRADIENT_H_

#include <types/Color.h>
#include <cairo.h>

namespace ilixi
{
  //! A gradient is used to interpolate between colors.
  /*!
   * Gradients are used by a Brush or Pen to interpolate colors during drawing of primitive shapes.
   * There are two types of gradients, LinearGradient and RadialGradient.
   *
   * Note that at least one color stop should be defined using addStop().
   */
  class Gradient
  {
    friend class SimpleDesigner;
    friend class Brush;
    friend class Pen;
  public:
    /*!
     * This enum is used to define the behaviour of gradient for areas outside the gradient's area.
     */
    enum GradientExtendMethod
    {
      ExtendNone,       //!< Pixels outside of the gradient are fully transparent.
      ExtendRepeat,     //!< Gradient is tiled by repeating.
      ExtendReflect,    //!< Gradient is tiled by reflecting at the edges.
      ExtendPad         //!< Pixels outside of the gradient copy the closest pixel from the source (Default).
    };

    /*!
     * This enum is to define the type of the gradient
     */
    enum GradientType
    {
      None,     //!< Empty gradient
      Linear,   //!< A linear gradient.
      Radial    //!< A radial gradient.
    };

    /*!
     * Destructor.
     */
    virtual
    ~Gradient();

    /*!
     * Returns the number of stops for gradient.
     */
    int
    stops();

    /*!
     * Returns the cairo pattern of this gradient.
     */
    cairo_pattern_t*
    cairoGradient() const;

    /*!
     * Returns the type of the gradient.
     */
    GradientType
    type() const;

    /*!
     * Add a new color stop to gradient at given offset.
     *
     * The following code creates a radial gradient and adds two stops.
     * \code
     * RadialGradient g(50, 50, 10, 50, 50, 100);
     * g.addStop(Color(0, 0, 0), 0.0);
     * g.addStop(Color(0, 255, 0), 1);
     * \endcode
     *
     * @param color Color to use at the stop.
     * @param offset Position of stop on gradient [0-1].
     */
    void
    addStop(const Color& color, double offset);

    /*!
     * Add a new color stop to gradient at given offset.
     * @param red value [0-255].
     * @param green value [0-255].
     * @param blue value [0-255].
     * @param alpha value [0-255].
     * @param offset Position of stop on gradient [0-1].
     */
    void
    addStop(double red, double green, double blue, double alpha, double offset);

    /*!
     * Sets the extend method of this gradient.
     * @param extendType Default type is Pad.
     */
    void
    setExtendMethod(GradientExtendMethod extendType);

    /*!
     * Overloaded assignment operator.
     * Calls destroy on self cairo_pattern and increases the reference count of
     * assigned cairo_pattern by 1.
     */
    Gradient&
    operator=(const Gradient &gradient);

  protected:
    /*!
     * Constructor.
     */
    Gradient();

    /*!
     * Private constructor.
     * Use either LinearGradient() or RadialGradient().
     */
    Gradient(GradientType type);

    /*!
     * Copy constructor.
     */
    Gradient(const Gradient& gradient);

    //! This property holds the type of the gradient.
    GradientType _type;

    //! Pointer to cairo pattern instance.
    cairo_pattern_t* _pattern;
  };
}
#endif /* ILIXI_GRADIENT_H_ */
