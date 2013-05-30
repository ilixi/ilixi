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

#ifndef ILIXI_BUTTONGROUP_H_
#define ILIXI_BUTTONGROUP_H_

#include <ui/DirectionalButton.h>

namespace ilixi
{

class LayoutBase;

//! A simple container for grouping directional buttons.
class ButtonGroup : public Widget
{
public:
    /*!
     * Constructor.
     *
     * @param orientation specifies orientation of button group
     * @param parent
     */
    ButtonGroup(Orientation orientation = Horizontal, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~ButtonGroup();

    virtual Size
    preferredSize() const;

    /*!
     * Adds a new directional button to this group.
     *
     * \warning ButtonGroup owns button
     * @return true if successful, false otherwise.
     */
    bool
    addButton(DirectionalButton* button);

    /*!
     * Removes a directional button from this group.
     *
     * \warning ButtonGroup will destroy button.
     * @return true if successful, false otherwise.
     */
    bool
    removeButton(DirectionalButton* button);

    /*!
     * Setups button directions and notifies parent.
     */
    virtual void
    doLayout();

protected:
    virtual void
    compose(const ilixi::PaintEvent& event);

private:
    //! This property stores orientation of button group.
    Orientation _orientation;
    typedef std::list<DirectionalButton*> ButtonList;
    //! This list stores children buttons for easy access.
    ButtonList _buttons;
    //! This property stores the internal layout for button group.
    LayoutBase* _box;

    //! Sets child button properties.
    void
    updateButtonGroupGeometry();
};

} /* namespace ilixi */
#endif /* ILIXI_BUTTONGROUP_H_ */
