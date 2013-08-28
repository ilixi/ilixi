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

#ifndef ILIXI_AFFINE2D_H_
#define ILIXI_AFFINE2D_H_

#include <stdint.h>

namespace ilixi
{
//! Helps with 2D transforms.
class Affine2D
{
public:
    /*!
     * Constructor, creates identity matrix.
     *
     * 1 0 0
     * 0 1 0
     * 0 0 1
     */
    Affine2D();

    /*!
     * Constructor, creates matrix using given parameters.
     *
     * m11 m12 m13
     * m21 m22 m23
     *  0   0   1
     */
    Affine2D(double m11, double m12, double m13, double m21, double m22,
             double m23);

    /*!
     * Copy constructor.
     */
    Affine2D(const Affine2D& other);

    /*!
     * Destructor.
     */
    virtual
    ~Affine2D();

    /*!
     * Returns a new array.
     *
     * \warning You are responsible for deletion.
     */
    int32_t*
    m() const;

    /*!
     * Sets to identity matrix.
     */
    Affine2D&
    clear();

    /*!
     * Returns the determinant of matrix.
     */
    double
    determinant() const;

    /*!
     * Inverts matrix.
     */
    Affine2D&
    invert();

    /*!
     * Rotates in degrees.
     *
     * @param degrees not radians.
     */
    Affine2D&
    rotate(double degrees);

    /*!
     * Scales x and y coordinates.
     *
     * @param sx factor.
     * @param sy factor.
     */
    Affine2D&
    scale(float sx, float sy);

    Affine2D&
    shearX(float x);

    Affine2D&
    shearY(float y);

    Affine2D&
    skew(double sx, double sy);

    Affine2D&
    translate(double dx, double dy);

    Affine2D&
    operator =(const Affine2D& other);

    Affine2D&
    operator *=(const Affine2D& other);

    Affine2D
    operator ~() const;

private:
    double _m[6];

    //! Matrix multiplication.
    Affine2D&
    multiply(const Affine2D& other);
};

} /* namespace ilixi */
#endif /* ILIXI_AFFINE2D_H_ */
