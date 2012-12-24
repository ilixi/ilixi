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

#ifndef BANDSLIDER_H_
#define BANDSLIDER_H_

#include <ui/Slider.h>
#include <ui/Label.h>
#include <ui/ContainerBase.h>

namespace ilixi
{

class BandSlider : public ContainerBase
{
public:
    BandSlider(const std::string& title, Font* font, Widget* parent = 0);

    virtual
    ~BandSlider();

protected:
    void
    setLevel(float value);

    void
    compose(const PaintEvent& event);

private:
    Label* _title;
    Label* _level;
    Slider* _slider;
};

} /* namespace ilixi */
#endif /* BANDSLIDER_H_ */
