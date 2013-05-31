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

#ifndef ILIXI_DIRECTIONALBUTTON_H_
#define ILIXI_DIRECTIONALBUTTON_H_

#include <ui/Button.h>
#include <lib/Thread.h>

namespace ilixi
{

class DirectionalButtonThread;

//! A button with corners to indicate direction.
class DirectionalButton : public Button
{
public:
    enum DirectionalButtonType
    {
        Arrow,      //!< Directional arrows, e.g. scroll bar buttons.
        PlusMinus, //!< Directional plus and minus buttons for up, left and down, right respectively, e.g. spin box buttons.
        Custom      //!< A custom directional button, e.g. button group buttons.
    };

    /*!
     * Constructor.
     *
     * @param text
     * @param parent
     */
    DirectionalButton(const std::string& text = "", Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~DirectionalButton();

    virtual Size
    preferredSize() const;

    /*!
     * Returns button direction.
     */
    Direction
    buttonDirection() const;

    /*!
     * Returns button type.
     */
    DirectionalButtonType
    buttonType() const;

    /*!
     * Returns rounded corners of directional button.
     */
    Corners
    corners() const;

    /*!
     * Sets button direction.
     *
     * If button type is not custom, corners will be set automatically.
     *
     * @param buttonDirection
     */
    void
    setButtonDirection(Direction buttonDirection);

    /*!
     * Sets button type.
     *
     * @param buttonType
     */
    void
    setButtonType(DirectionalButtonType buttonType);

    /*!
     * Sets button corners.
     *
     * @param corners
     */
    void
    setCorners(Corners corners);

    /*!
     * Sets button icon by automatically creating an icon.
     *
     * @param path to an image file.
     * @param size in pixels.
     */
    void
    setIcon(const std::string& path, const Size& size = Size(16, 16));

    /*!
     * Sets icon using stock icons.
     */
    void
    setIcon(StyleHint::PackedIcon icon, const Size& size = Size(-1, -1));

    /*!
     * Sets the size of button icon.
     */
    void
    setIconSize(const Size& size);

protected:
    virtual void
    compose(const ilixi::PaintEvent& event);

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

    virtual void
    leaveEvent(const PointerEvent& event);

private:
    //! This property stores which corners are drawn.
    Corners _corners;
    //! This property stores type of button.
    DirectionalButtonType _buttonType;
    //! This property stores direction of button.
    Direction _buttonDirection;
    //! This property holds button's icon.
    Icon* _icon;
    //! This thread helps create repeated clicks.
    static DirectionalButtonThread* __dbThread;

    virtual void
    updateTextBaseGeometry();
};

//! This thread helps create repeated button clicks.
class DirectionalButtonThread : public Thread
{
    friend class DirectionalButton;
public:
    DirectionalButtonThread();

    ~DirectionalButtonThread();

    void
    setTarget(DirectionalButton* target);

    virtual int
    run();

private:
    DirectionalButton* _target;
};

} /* namespace ilixi */
#endif /* ILIXI_DIRECTIONALBUTTON_H_ */
