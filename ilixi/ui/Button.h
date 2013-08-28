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

#ifndef ILIXI_BUTTON_H_
#define ILIXI_BUTTON_H_

#include <ui/TextBase.h>
#include <ui/Icon.h>

namespace ilixi
{

//! Base class for button type widgets.
class Button : public Widget, public TextBase
{
public:
    enum ButtonFlags
    {
        None = 0x000, //!< Default button state
        Checkable = 0x002, //!< Button is checkable
        Checked = 0x004, //!< A checkable button is checked
        Partial = 0x008, //!< Partially checked.
        Tristate = 0x010, //! Whether widget has three states, e.g. checkbox.
        PressedDown = 0x020
    //!< Pointer button is down on button
    };

    /*!
     * Constructor.
     */
    Button(const std::string& text, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~Button();

    /*!
     * Returns true if button is checkable.
     */
    bool
    checkable() const;

    /*!
     * Returns true if button is checkable and checked, false otherwise.
     */
    bool
    checked() const;

    /*!
     * Simulates a pointer click over button.  Signals (pressed and clicked) are emitted.
     *
     * @param ms If this value is positive click action is animated for given milliseconds.
     */
    void
    click(unsigned int ms = 0);

    /*!
     * Changes button's checked state.
     */
    virtual void
    toggleChecked();

    /*!
     * Sets button as checkable.
     */
    void
    setCheckable(bool checkable);

    /*!
     * Sets button as checked state only if button is checkable.
     */
    virtual void
    setChecked(bool checked = true);

    /*!
     * This signal is emitted when button is pressed and then released.
     */
    sigc::signal<void> sigClicked;

    /*!
     * This signal is emitted when button is pressed down.
     *
     * \sa sigClicked
     */
    sigc::signal<void> sigPressed;

    /*!
     * This signal is emitted when button is released.
     *
     * \sa sigClicked
     */
    sigc::signal<void> sigReleased;

    /*!
     * This signal is emitted if button is checkable and its state is toggled.
     */
    sigc::signal<void, bool> sigCheckChanged;

protected:
    //! This property holds button's flags.
    ButtonFlags _buttonFlag;

    /*!
     * If key is SPACE then click.
     */
    virtual void
    keyDownEvent(const KeyEvent& event);

    /*!
     * Emits sigPressed.
     */
    virtual void
    pointerButtonDownEvent(const PointerEvent& event);

    /*!
     * Emits sigReleased and if pointer was down emits sigClicked and toggles state.
     */
    virtual void
    pointerButtonUpEvent(const PointerEvent& event);

    /*!
     * Just updates button.
     */
    virtual void
    enterEvent(const PointerEvent& event);

    /*!
     * Just updates button.
     */
    virtual void
    leaveEvent(const PointerEvent& event);

    /*!
     * Just updates button.
     */
    virtual void
    focusInEvent();

    /*!
     * Just updates button.
     */
    virtual void
    focusOutEvent();

private:

    //! Returns button font from stylist.
    virtual Font*
    defaultFont() const;
};

}

#endif /* ILIXI_BUTTON_H_ */
