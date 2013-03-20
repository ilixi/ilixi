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

#ifndef ILIXI_BARCHART_H_
#define ILIXI_BARCHART_H_

#include <ui/Widget.h>
#include <list>
#include <vector>

namespace ilixi
{

class BarChart : public Widget
{
public:
    BarChart(unsigned int size = 10, Widget* parent = 0);

    virtual
    ~BarChart();

    void
    addBar(const std::string& name, const Color& color);

    void
    addValue(unsigned int bar, float value);

    void
    setDrawBG(bool drawBackground);

    void
    setMajorTicks(unsigned int major);

protected:
    void
    compose(const PaintEvent& event);

private:
    bool _drawBackground;
    unsigned int _size;
    unsigned int _major;
    typedef std::list<float> BarList;

    struct BarData
    {
        BarList values;
        std::string name;
        Color color;
    };

    typedef std::vector<BarData> BarVector;
    BarVector _data;
};

} /* namespace ilixi */
#endif /* ILIXI_BARCHART_H_ */
