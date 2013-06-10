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

#include "Gallery.h"
#include "ImageWidget.h"
#include <ui/GridLayout.h>
#include <core/Logger.h>
#include <sigc++/bind.h>

using namespace ilixi;

Gallery::Gallery(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setBackgroundImage(ILIXI_DATADIR"images/ilixi_bg.jpg");
    setLayout(new GridLayout(4, 4));
    appWindow()->setSpacing(1);

    char file[128];
    ImageWidget* widget;
    for (int i = 0; i < 16; ++i)
    {
        sprintf(file, "%sgallery/%d.jpg\0", ILIXI_DATADIR, i % 5);
        widget = new ImageWidget(file);
        widget->setImage(new Image(file, 300, 300));
        widget->sigPressed.connect(
                sigc::bind<std::string>(
                        sigc::mem_fun(this, &Gallery::showImage), file));
        addWidget(widget);
    }

    _dialog = new ImageDialog("Image Dialog");
}

Gallery::~Gallery()
{
    delete _dialog;
}

void
Gallery::showImage(const std::string& file)
{
    _dialog->setImagePath(file);
    _dialog->execute();
}

int
main(int argc, char* argv[])
{
    Gallery app(argc, argv);
    app.exec();
    return 0;
}
