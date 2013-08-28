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

#ifndef ILIXI_RADIOBUTTON_H_
#define ILIXI_RADIOBUTTON_H_

#include <ui/Button.h>
#include <types/RadioGroup.h>

namespace ilixi
{

//! A radio button widget.
class RadioButton : public Button
{
public:
    RadioButton(const std::string& text = "", Widget* parent = 0);

    virtual
    ~RadioButton();

    int
    heightForWidth(int width) const;

    Size
    preferredSize() const;

    void
    setGroup(RadioGroup* group);

    virtual void
    toggleChecked();

    virtual void
    setChecked(bool checked = true);

private:
    RadioGroup* _group;

    void
    informGroup(bool checked);

    void
    compose(const PaintEvent& event);

    virtual void
    updateTextBaseGeometry();

    virtual Font*
    defaultFont() const;
};

} /* namespace ilixi */
#endif /* ILIXI_RADIOBUTTON_H_ */
