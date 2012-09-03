/*
 * BandSlider.cpp
 *
 *  Created on: 26 Aug 2012
 *      Author: tarik
 */

#include "BandSlider.h"
#include <ui/VBoxLayout.h>
#include <core/Logger.h>
#include <sstream>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_BANDSLIDER, "ilixi/Mixer/BandSlider", "BandSlider");

BandSlider::BandSlider(const std::string& title, Widget* parent)
        : ContainerBase(parent), _title(new Label(title)), _level(new Label("0.0 dB")), _slider(new Slider())
{
    ILOG_TRACE_W(ILX_BANDSLIDER);
    VBoxLayout* box = new VBoxLayout();
    box->setHorizontalAlignment(Alignment::Center);
    setLayout(box);
    _slider->setRange(-30, 30);
    _slider->setOrientation(Vertical);
    _slider->sigValueChanged.connect(sigc::mem_fun(this, &BandSlider::setLevel));

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
