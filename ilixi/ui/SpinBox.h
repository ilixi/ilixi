/*
 * SpinBox.h
 *
 *  Created on: Sep 9, 2012
 *      Author: tarik
 */

#ifndef ILIXI_SPINBOX_H_
#define ILIXI_SPINBOX_H_

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

    /*!
     * Returns text appended to the start of text.
     */
    const std::string&
    postfix() const;

    /*!
     * Returns text prepended to the start of text.
     */
    const std::string&
    prefix() const;

    bool
    wrapping() const;

    /*!
     * Returns text layout.
     */
    TextLayout
    layout() const;

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

    /*!
     * Sets text appended at the end.
     */
    void
    setPostfix(const std::string& postfix);

    /*!
     * Sets text prepended at the start.
     */
    void
    setPrefix(const std::string& prefix);

    sigc::signal<void, int> sigValueChanged;

protected:
    void
    compose(const PaintEvent& event);

private:
    //! Button for increments.
    ToolButton* _plus;
    //! Button for decrements.
    ToolButton* _minus;

    //! Prefix for input.
    std::string _prefix;
    //! Postfix for input.
    std::string _postfix;
    //! TextLayout for this widget.
    TextLayout _layout;

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
