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
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_RADIOGROUP, "ilixi/types/RadioGroup", "RadioGroup");

RadioGroup::RadioGroup()
        : _selected(NULL)
{
    ILOG_TRACE(ILX_RADIOGROUP);
}

RadioGroup::~RadioGroup()
{
    ILOG_TRACE(ILX_RADIOGROUP);
}

RadioButton*
RadioGroup::selected() const
{
    return _selected;
}

void
RadioGroup::add(RadioButton* radio)
{
    ILOG_TRACE(ILX_RADIOGROUP);
    if (!radio)
        return;
    radio->setGroup(this);
    if (!_selected)
    {
        _selected = radio;
        _selected->setChecked(true);
    }
}

void
RadioGroup::remove(RadioButton* radio)
{
    ILOG_TRACE(ILX_RADIOGROUP);
    if (_selected == radio)
        _selected = NULL;
    radio->setGroup(NULL);
}

void
RadioGroup::select(RadioButton* radio)
{
    ILOG_TRACE(ILX_RADIOGROUP);
    if (_selected && radio && _selected != radio)
    {
        _selected->setChecked(false);
        _selected = radio;
    }
}

void
RadioGroup::clear()
{
    ILOG_TRACE(ILX_RADIOGROUP);
    _selected = NULL;
}

} /* namespace ilixi */
