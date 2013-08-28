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

#ifndef ILIXI_CHECKBOX_H_
#define ILIXI_CHECKBOX_H_

#include <ui/Button.h>

namespace ilixi
{

//! Provides a check box.
class CheckBox : public Button
{
public:

    //! This enum specifies CheckBox states.
    enum CheckState
    {
        Checked,    //!< Checked
        Partial,    //!< Partial, available for tristate.
        Unchecked   //!< Unchecked
    };

    /*!
     * Constructor.
     *
     * @param text
     * @param parent widget.
     */
    CheckBox(const std::string& text = "", Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~CheckBox();

    int
    heightForWidth(int width) const;

    Size
    preferredSize() const;

    /*!
     * Returns true if checkbox is partially checked, false otherwise.
     */
    bool
    partial() const;

    /*!
     * Changes checkbox's CheckState. CheckStateChanged signal is emitted.
     */
    virtual void
    toggleChecked();

    /*!
     * Enables or disables tristate mode.
     */
    void
    setTriState(bool triState);

    /*!
     * This signal is emitted when button is released.
     *
     * \sa sigClicked
     */
    sigc::signal<void, CheckState> sigCheckStateChanged;

private:
    /*!
     * Paints Checkbox on its surface using current designer.
     * @param rect Clipped rectangle to paint.
     */
    void
    compose(const PaintEvent& event);

    virtual void
    updateTextBaseGeometry();

    virtual Font*
    defaultFont() const;
};

} /* namespace ilixi */
#endif /* ILIXI_CHECKBOX_H_ */
