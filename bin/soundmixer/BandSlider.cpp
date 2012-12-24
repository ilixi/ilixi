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

#include "BandSlider.h"
#include <ui/VBoxLayout.h>
#include <core/Logger.h>
#include <sstream>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_BANDSLIDER, "ilixi/Mixer/BandSlider", "BandSlider");

BandSlider::BandSlider(const std::string& title, Font* font, Widget* parent)
        : ContainerBase(parent),
          _title(new Label(title)),
          _level(new Label("0.0 dB")),
          _slider(new Slider())
{
    ILOG_TRACE_W(ILX_BANDSLIDER);
    VBoxLayout* box = new VBoxLayout();
    box->setHorizontalAlignment(Alignment::Center);
    setLayout(box);
    _slider->setRange(-30, 30);
    _slider->setOrientation(Vertical);
    _slider->sigValueChanged.connect(sigc::mem_fun(this, &BandSlider::setLevel));

    _title->setFont(font);
    _level->setFont(font);
    _title->setLayoutAlignment(TextLayout::Center);
    _level->setLayoutAlignment(TextLayout::Center);

    addWidget(_title);
    addWidget(_slider);
    addWidget(_level);
}

BandSlider::~BandSlider()
{
}

void
BandSlider::setLevel(float value)
{
    std::stringstream ss;
    ss << int(value) << " dB";
    _level->setText(ss.str());
}

void
BandSlider::compose(const PaintEvent& event)
{
}

} /* namespace ilixi */
