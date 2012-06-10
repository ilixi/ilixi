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

#ifndef ILIXI_FPSCALC_H_
#define ILIXI_FPSCALC_H_

#include "core/Callback.h"
#include <sigc++/signal.h>
#include <string>

namespace ilixi
{

  /*!
   * Calculates FPS using given interval.
   */
  class FPSCalculator : public sigc::trackable, public Functionoid
  {
  public:
    /*!
     * Constructor.
     */
    FPSCalculator();

    /*!
     * Destructor.
     */
    virtual
    ~FPSCalculator();

    /*!
     * Returns FPS.
     */
    float
    fps() const;

    /*!
     * Returns formatted FPS text.
     */
    const char*
    fpsText();

    /*!
     * Start calculation.
     */
    void
    start();

    /*!
     * Stop calculation.
     */
    void
    stop();

    /*!
     * This signal is emitted when FPS value is updated.
     */
    sigc::signal<void, float> sigUpdated;

  protected:
    /*!
     * Callback method which actually calculated FPS.
     */
    bool
    funck();

  private:
    //! Callback for FPS calculation.
    Callback _cb;
    //! Number of frames since last calculation.
    int _frames;
    //! Current FPS.
    float _fps;
    //! Time at FPS calculation.
    long long _fpsTime;
    //! This field stores formatted FPS text.
    char _fpsText[20];
  };

} /* namespace ilixi */
#endif /* ILIXI_FPSCALC_H_ */
