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

#include "CarouselSwitcher.h"
#include <compositor/Compositor.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

CarouselSwitcher::CarouselSwitcher(ILXCompositor* compositor, Widget* parent)
        : Switcher(compositor, parent)
{
    _carousel = new Carousel();
    addChild(_carousel);

    _carousel->sigItemSelected.connect(sigc::mem_fun(this, &CarouselSwitcher::carouselItemSelected));

    sigGeometryUpdated.connect(sigc::mem_fun(this, &CarouselSwitcher::updateSwitcherGeometry));
}

CarouselSwitcher::~CarouselSwitcher()
{
}

Size
CarouselSwitcher::preferredSize() const
{
    return _carousel->preferredSize();
}

void
CarouselSwitcher::addThumb(AppThumbnail* thumb)
{
    thumb->setVisible(true);
    thumb->setCloseVisible(false);
    CarouselItem* item = new CarouselItem(_carousel);
    item->setSource(thumb);
    _carousel->addItem(item);
    _thumbs.push_back(thumb);
    update();
}

void
CarouselSwitcher::removeThumb(AppThumbnail* thumb)
{
    _carousel->removeWidget(thumb);
    update();
}

void
CarouselSwitcher::scrollTo(AppThumbnail* thumb)
{
    _carousel->showWidget(thumb);
}

void
CarouselSwitcher::scrollToNext()
{
    if (_thumbs.size())
        _carousel->showItem((CarouselItem*) _carousel->itemAtFront()->getNeighbour(Left));
}

void
CarouselSwitcher::scrollToPrevious()
{
    if (_thumbs.size())
        _carousel->showItem((CarouselItem*) _carousel->itemAtFront()->getNeighbour(Right));
}

AppThumbnail*
CarouselSwitcher::currentThumb()
{
    return ((AppThumbnail*) _carousel->itemAtFront()->source());
}

void
CarouselSwitcher::killCurrentApp()
{
    _compositor->killApp(((AppThumbnail*) _carousel->itemAtFront()->source())->instance());
}

void
CarouselSwitcher::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    p.setBrush(Color(0, 0, 0, 80));
    p.fillRectangle(0, 0, width(), height(), (DFBSurfaceDrawingFlags) (DSDRAW_SRC_PREMULTIPLY | DSDRAW_BLEND));
    p.end();
}

void
CarouselSwitcher::updateSwitcherGeometry()
{
    _carousel->setGeometry(0, 0, width(), height());
}

void
CarouselSwitcher::carouselItemSelected(CarouselItem* item)
{
    AppThumbnail* current = dynamic_cast<AppThumbnail*>(item->source());
    if (current)
        sigSwitchRequest(current->instance());
}

} /* namespace ilixi */
