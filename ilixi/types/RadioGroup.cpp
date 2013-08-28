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

#include <types/RadioGroup.h>
#include <ui/RadioButton.h>

namespace ilixi
{

RadioGroup::RadioGroup()
        : _selected(0)
{
}

RadioGroup::~RadioGroup()
{
}

RadioButton*
RadioGroup::selected() const
{
    return _selected;
}

void
RadioGroup::add(RadioButton* radio)
{
    if (!_selected)
    {
        _selected = radio;
        _selected->setChecked(true);
    }
    radio->setGroup(this);
}

void
RadioGroup::remove(RadioButton* radio)
{
    if (_selected == radio)
        _selected = NULL;
    radio->setGroup(NULL);
}

void
RadioGroup::select(RadioButton* radio)
{
    if (_selected && _selected != radio)
    {
        _selected->setChecked(false);
        _selected = radio;
    }
}

void
RadioGroup::clear()
{
    _selected = NULL;
}

} /* namespace ilixi */
