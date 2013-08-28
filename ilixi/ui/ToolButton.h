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

#ifndef ILIXI_TOOLBUTTON_H_
#define ILIXI_TOOLBUTTON_H_

#include <ui/Button.h>
#include <lib/Thread.h>

namespace ilixi
{

class ToolButtonThread;

//! A tool button with text label and icon.
class ToolButton : public Button
{
public:
    /*!
     * This enum helps control various options of ToolButton.
     */
    enum ToolButtonOptions
    {
        DrawFrame = 0x001, //!< This flag specifies whether button frame is drawn.
        Repeatable = 0x002 //!< This flag specifies whether button repeats press.
    };

    /*!
     * This enum controls toolbutton's drawing style.
     */
    enum ToolButtonStyle
    {
        IconOnly, //!< Only icon is drawn.
        TextOnly, //!< Only text is drawn.
        IconBeforeText, //!< Icon is placed before text.
        IconBelowText, //!< Icon is placed below text and centered.
        IconAboveText  //!< Icon is placed above text and centered.
    };

    /*!
     * Constructor
     * @param text
     * @param parent
     */
    ToolButton(const std::string& text = "", Widget* parent = 0);

    /*!
     * Destructor
     */
    virtual
    ~ToolButton();

    Size
    preferredSize() const;

    /*!
     * Returns button icon, if any.
     */
    Icon*
    icon() const;

    /*!
     * Returns true if drawing frame, false otherwise.
     */
    bool
    drawFrame() const;

    /*!
     * Returns tool button's style.
     */
    ToolButtonStyle
    toolButtonStyle() const;

    /*!
     * Set tool button's style.
     * @param style
     */
    void
    setToolButtonStyle(ToolButtonStyle style);

    /*!
     * Sets button icon.
     */
    void
    setIcon(Icon* icon);

    /*!
     * Sets button icon by automatically creating an icon.
     *
     * @param path to an image file.
     * @param size in pixels.
     */
    void
    setIcon(const std::string& path, const Size& size = Size(48, 48));

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

    /*!
     * Sets whether frame is drawn or not.
     */
    void
    setDrawFrame(bool drawFrame);

    /*!
     * Sets whether button is repeatable.
     */
    void
    setRepeatable(bool repeatable);

protected:
    //! This property holds button's icon.
    Icon* _icon;

    /*!
     * Paints ToolButton.
     */
    void
    compose(const PaintEvent& event);

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
    //! This property holds tool button's style.
    ToolButtonStyle _toolButtonStyle;
    //! This flag specifies options for tool button.
    ToolButtonOptions _tbOptions;
    //! This thread helps create repeated clicks.
    static ToolButtonThread* __tbThread;

    virtual void
    updateTextBaseGeometry();
};

//! This thread helps create repeated button clicks.
class ToolButtonThread : public Thread
{
    friend class ToolButton;
public:
    ToolButtonThread();

    ~ToolButtonThread();

    void
    setTarget(ToolButton* target);

    virtual int
    run();

private:
    ToolButton* _target;
};

}

#endif /* ILIXI_TOOLBUTTON_H_ */
