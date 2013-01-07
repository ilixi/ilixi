/*
 Copyright 2010-2012 Tarik Sekmen.

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

#include "Helper.h"
#include <ui/HBoxLayout.h>
#include <ui/Label.h>
#include <lib/InputHelperJP.h>
#include "utf8-decoder.h"

namespace ilixi
{

OSKHelper::OSKHelper(Widget* parent)
        : ContainerBase(parent)
{
    setVisible(false);
    setLayout(new HBoxLayout());

    _font = new Font("Sazanami Gothic", 14);

    _text = new Label(">");
    _text->setFont(_font);
    _text->setXConstraint(ExpandingConstraint);
    addWidget(_text);

    _candidate = new Label("candidates");
    _candidate->setFont(_font);
    addWidget(_candidate);

    _helper = new InputHelperJP();
    _helper->sigConverted.connect(sigc::mem_fun(this, &OSKHelper::updateText));
}

OSKHelper::~OSKHelper()
{
    delete _font;
    delete _helper;
}

void
OSKHelper::convert(uint32_t symbol)
{
    _helper->append(symbol);
}

void
OSKHelper::compose(const PaintEvent& event)
{
}

void
OSKHelper::updateText()
{
    _text->setText(std::wstring(L"> ").append(_helper->conversion()));
}

} /* namespace ilixi */
