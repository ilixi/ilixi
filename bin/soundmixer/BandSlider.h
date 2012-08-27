/*
 * BandSlider.h
 *
 *  Created on: 26 Aug 2012
 *      Author: tarik
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
    BandSlider(const std::string& title, Widget* parent = 0);

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
