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

#ifndef CAROUSELSWITCHER_H_
#define CAROUSELSWITCHER_H_

#include <compositor/Switcher.h>
#include <ui/Carousel.h>

namespace ilixi
{

class CarouselSwitcher : public Switcher
{
public:
    CarouselSwitcher(ILXCompositor* compositor, Widget* parent = 0);

    virtual
    ~CarouselSwitcher();

    virtual Size
    preferredSize() const;

    virtual void
    addThumb(AppThumbnail* thumb);

    virtual void
    removeThumb(AppThumbnail* thumb);

    virtual void
    scrollTo(AppThumbnail* thumb);

    virtual void
    scrollToNext();

    virtual void
    scrollToPrevious();

    virtual AppThumbnail*
    currentThumb();

    virtual void
    killCurrentApp();

protected:
    Carousel* _carousel;

    virtual void
    compose(const PaintEvent& event);

    void
    updateSwitcherGeometry();

private:
    void
    carouselItemSelected(CarouselItem* item);
};

} /* namespace ilixi */
#endif /* CAROUSELSWITCHER_H_ */
