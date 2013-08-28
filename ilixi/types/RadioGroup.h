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

#ifndef ILIXI_RADIOGROUP_H_
#define ILIXI_RADIOGROUP_H_

namespace ilixi
{
class RadioButton;

//! Groups radio buttons for exclusive selection.
/*!
 * Each layout comes with a radio group. However, users can create their
 * own groups assign buttons later.
 */
class RadioGroup
{
public:
    /*!
     * Constuctor.
     */
    RadioGroup();

    /*!
     * Destructor.
     */
    virtual
    ~RadioGroup();

    /*!
     * Returns currenly selected radio button.
     */
    RadioButton*
    selected() const;

    /*!
     * Adds a radio button to this group.
     *
     * @param radio button to add.
     */
    void
    add(RadioButton* radio);

    /*!
     * Remove a radio button from this group.
     */
    void
    remove(RadioButton* radio);

    /*!
     * Unchecks selected button and selects given button.
     *
     * @param radio button to select.
     */
    void
    select(RadioButton* radio);

    /*!
     * Clears selected button.
     */
    void
    clear();

private:
    //! This property stores currently selected button.
    RadioButton* _selected;
};

} /* namespace ilixi */
#endif /* ILIXI_RADIOGROUP_H_ */
