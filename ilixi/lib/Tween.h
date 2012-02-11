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

#ifndef ILIXI_TWEEN_H_
#define ILIXI_TWEEN_H_

namespace ilixi
{

  class Tween
  {
    friend class TweenAnimation;

  public:

    enum Transition
    {
      LINEAR,
      SINE,
      QUINT,
      QUART,
      QUAD,
      EXPO,
      ELASTIC,
      CUBIC,
      CIRCLE,
      BOUNCE,
      BACK
    };

    enum Equation
    {
      EASE_IN, EASE_OUT, EASE_IN_OUT
    };

    /*!
     * Creates a new tween.
     *
     * @param transition
     * @param equation
     * @param variable
     * @param initialValue
     * @param endValue
     */
    Tween(Transition transition, Equation equation, float& variable,
        float initialValue = 0, float endValue = 1);

    /*!
     * Copy constructor.
     */
    Tween(const Tween& tween);

    /*!
     * Destructor.
     */
    virtual
    ~Tween();

    /*!
     * Returns end value.
     */
    float
    getEndValue() const;

    /*!
     * Returns the type of equation.
     */
    Equation
    getEquation() const;

    /*!
     * Returns initial value.
     */
    float
    getInitialValue() const;

    /*!
     * Returns the type of transition.
     */
    Transition
    getTransition() const;

    /*!
     * Sets end value of tweened variable.
     */
    void
    setEndValue(float endValue);

    /*!
     * Sets the type of equation.
     */
    void
    setEquation(Equation equation);

    /*!
     * Sets the initial value of tweened variable.
     */
    void
    setInitialValue(float initialValue);

    /*!
     * Sets the type of transition.
     */
    void
    setTransition(Transition transition);

  private:
    //! This property stores transition.
    Transition _transition;
    //! This property stores equation.
    Equation _equation;
    //! This property stores tweened variable.
    float* _variable;
    //! This is the initial values of tweened variable.
    float _initialValue;
    //! This is the required change in value.
    float _change;

    //! Updates using transition and equation.
    void
    runEase(float time, float duration);
  };

}
#endif /* ILIXI_TWEEN_H_ */
