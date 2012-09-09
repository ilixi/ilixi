/*
 * SpinBox.h
 *
 *  Created on: Sep 9, 2012
 *      Author: tarik
 */

#ifndef ILIXI_SPINBOX_H_
#define ILIXI_SPINBOX_H_

#include <ui/LineInput.h>
#include <ui/ToolButton.h>

namespace ilixi
{

class SpinBox : public Widget
{
public:
    SpinBox(int value, Widget* parent = 0);

    virtual
    ~SpinBox();

    virtual Size
    preferredSize() const;

    int
    max() const;

    int
    min() const;

    int
    step() const;

    int
    value() const;

    bool
    wrapping() const;

    void
    setMax(int max);

    void
    setMin(int min);

    void
    setRange(int min, int max);

    void
    setStep(int step);

    void
    setValue(int value);

    void
    setWrapping(bool wrapping);

    sigc::signal<void, int> sigValueChanged;

protected:
    void
    compose(const PaintEvent& event);

private:
    //! line input is used for input.
    LineInput* _lineInput;
    //! Button for increments.
    ToolButton* _plus;
    //! Button for decrements.
    ToolButton* _minus;

    //! This property stores the minimum value.
    int _min;
    //! This property stores the maximum value.
    int _max;
    //! This property stores the current value.
    int _value;
    //! This property stores the increment/decrement amount.
    int _step;
    //! This property stores whether spinbox wraps at ends.
    bool _wrapping;

    void
    decrement();

    void
    increment();

    void
    updateSpinBoxGeometry();
};

} /* namespace ilixi */
#endif /* ILIXI_SPINBOX_H_ */
