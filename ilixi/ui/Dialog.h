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

#ifndef ILIXI_DIALOG_H_
#define ILIXI_DIALOG_H_

#include <core/Window.h>
#include <ui/Frame.h>
#include <ui/HBoxLayout.h>
#include <ui/WindowWidget.h>

namespace ilixi
{

//! A dialog with its own window.
class Dialog : public WindowWidget
{
public:
    //! This enum designates the buttons shown on the dialog.
    enum ButtonOption
    {
        NoButtonOption,         //!< No buttons.
        OKButtonOption,         //!< OK button for accept().
        CancelButtonOption,     //!< Cancel button for reject().
        OKCancelButtonOption,   //! OK and Cancel buttons for accept() and reject() respectively.
        YesNoButtonOption,      //!< Yes and No buttons for accept() and reject() respectively.
        YesNoCancelButtonOption //!< Yes (for accept), No (for reject) and Cancel (for finish(-1)) buttons.
    };

    /*!
     * Constructor.
     *
     * @param parent
     */
    Dialog(const std::string& title, ButtonOption option = OKButtonOption, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~Dialog();

    /*!
     * Returns a height for frame if its contents are fit inside width.
     *
     * @param width Desired width of frame.
     */
    virtual int
    heightForWidth(int width) const;

    /*!
     * Calculates the preferred size of contents and returns an updated size using canvas, margins and borders.
     */
    virtual Size
    preferredSize() const;

    /*!
     * Closes the dialog and sets the result to accepted (1).
     */
    void
    accept();

    /*!
     * Closes the dialog and sets the result to rejected (0).
     */
    void
    reject();

    /*!
     * Closes the dialog and sets the result to res.
     */
    void
    finish(int res);

    /*!
     * Returns result of this dialog.
     */
    int
    result() const;

    /*!
     * Shows the dialog and blocks until it is accepted (1), rejected (0) or invalid (-1).
     */
    void
    execute();

    /*!
     * Returns the dialog's title.
     */
    std::string
    title() const;

    /*!
     * Returns height of the dialog's title bar.
     */
    Size
    titleSize() const;

    /*!
     * Sets a button layout for dialog.
     */
    void
    setButtonLayoutOption(ButtonOption option);

    /*!
     * Changes default button layout with given layout.
     */
    void
    setButtonLayout(LayoutBase* buttonLayout);

    /*!
     * Sets dialog's title.
     *
     * @param title
     */
    void
    setTitle(const std::string& title);

    /*!
     * This signal is emitted when the dialog is accepted.
     */
    sigc::signal<void> sigAccepted;

    /*!
     * This signal is emitted when the dialog is rejected.
     */
    sigc::signal<void> sigRejected;

    /*!
     * This signal is emitted when the dialog's result is set.
     */
    sigc::signal<void, int> sigFinished;

protected:
    //! This layout is placed under the dialog and usually contains buttons.
    LayoutBase* _buttonLayout;
    //! This property stores the dialog's result.
    int _result;
    //! This property stores the dialogs's title text.
    std::string _title;
    //! This property stores the dimensions of the title bar text.
    Size _titleSize;

private:
    /*!
     * Returns Dialog's canvas y-coordinate including the top margin.
     */
    virtual int
    canvasY() const;

    /*!
     * Returns frame's canvas height excluding margins.
     */
    virtual int
    canvasHeight() const;

    //! Draws dialog frame using designer.
    virtual void
    compose(const PaintEvent& event);

    //! Positions button layout if any.
    void
    updateButtonLayoutGeometry();
};
}

#endif /* ILIXI_DIALOG_H_ */
