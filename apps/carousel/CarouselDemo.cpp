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

#include "CarouselDemo.h"
#include "ui/VBoxLayout.h"
#include "ui/Carousel.h"
#include "graphics/Painter.h"
#include "../gallery/ImageWidget.h"

namespace ilixi
{

CarouselDemo::CarouselDemo(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setBackgroundImage(ILIXI_DATADIR"images/ilixi_bg.jpg");

    setLayout(new VBoxLayout());

    Carousel* carousel = new Carousel();
    addWidget(carousel);

    char file[128];
    ImageWidget* widget;
    for (int i = 0; i < 8; ++i)
    {
        sprintf(file, "%sgallery/%d.jpg\0", ILIXI_DATADIR, i % 5);
        widget = new ImageWidget(file);
        widget->setImage(new Image(file, 196, 196));
        carousel->addWidget(widget);
    }
}

CarouselDemo::~CarouselDemo()
{
}

void
CarouselDemo::compose(const PaintEvent& event)
{
    Painter painter(appWindow());
    painter.begin(event);
#ifdef ILIXI_STEREO_OUTPUT
    if (event.eye == PaintEvent::LeftEye)
    painter.stretchImage(appWindow()->background(), -30, 0, width(), height());
    else
    painter.stretchImage(appWindow()->background(), 30, 0, width(), height());
#else
    painter.stretchImage(appWindow()->background(), 0, 0, width(), height());
#endif
    painter.end();
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::CarouselDemo app(argc, argv);
    app.exec();
    return 0;
}
