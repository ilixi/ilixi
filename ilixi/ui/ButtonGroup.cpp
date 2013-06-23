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

#include <ui/ButtonGroup.h>
#include <ui/HBoxLayout.h>
#include <ui/VBoxLayout.h>
#include <core/Logger.h>
#include <algorithm>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_BUTTONGROUP, "ilixi/ui/ButtonGroup", "ButtonGroup");

ButtonGroup::ButtonGroup(Orientation orientation, Widget* parent)
        : Widget(parent),
          _orientation(orientation)
{
    ILOG_TRACE_W(ILX_BUTTONGROUP);
    setInputMethod(PointerPassthrough);
    setConstraints(FixedConstraint, FixedConstraint);
    if (_orientation == Horizontal)
        _box = new HBoxLayout();
    else
        _box = new VBoxLayout();
    _box->setSpacing(0);
    addChild(_box);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &ButtonGroup::updateButtonGroupGeometry));
}

ButtonGroup::~ButtonGroup()
{
    ILOG_TRACE_W(ILX_BUTTONGROUP);
}

Size
ButtonGroup::preferredSize() const
{
    return _box->preferredSize();
}

bool
ButtonGroup::addButton(DirectionalButton* button)
{
    ILOG_TRACE_W(ILX_BUTTONGROUP);
    if (!_box->isChild(button))
        _buttons.push_back(button);
    if (_box->addWidget(button))
    {
        button->setButtonType(DirectionalButton::Custom);
        return true;
    }
    return false;
}

bool
ButtonGroup::removeButton(DirectionalButton* button)
{
    ILOG_TRACE_W(ILX_BUTTONGROUP);
    if (!_box->isChild(button))
    {
        ButtonList::iterator it = std::find(_buttons.begin(), _buttons.end(), button);
        if (button == *it)
            _buttons.erase(it);
    }

    if (_box->removeWidget(button))
        return true;
    return false;
}

void
ButtonGroup::doLayout()
{
    for (ButtonList::const_iterator it = _buttons.begin(); it != _buttons.end(); ++it)
    {
        ((DirectionalButton*) *it)->setCorners(NoCorners);
        if (_orientation == Horizontal)
            ((DirectionalButton*) *it)->setConstraints(FixedConstraint, FixedConstraint);
        else
            ((DirectionalButton*) *it)->setConstraints(MinimumConstraint, FixedConstraint);
    }

    if (_buttons.size() > 1)
    {
        if (_orientation == Horizontal)
        {
            _buttons.front()->setCorners(LeftCorners);
            _buttons.back()->setCorners(RightCorners);
        } else
        {
            _buttons.front()->setCorners(TopCorners);
            _buttons.back()->setCorners(BottomCorners);
        }
    }

    if (parent())
        parent()->doLayout();
}

void
ButtonGroup::compose(const ilixi::PaintEvent& event)
{
}

void
ButtonGroup::updateButtonGroupGeometry()
{
    _box->setGeometry(0, 0, width(), height());
    _box->setNeighbours(getNeighbour(Up), getNeighbour(Down), getNeighbour(Left), getNeighbour(Right));
}

} /* namespace ilixi */
