/*
 Copyright 2011 Tarik Sekmen.

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

using namespace ilixi;

Gallery::Gallery(int argc, char* argv[]) :
  Application(argc, argv)
{
  setTitle("Gallery");
  setBackgroundFilled(true);
  setBackgroundImage(ILIXI_DATADIR"images/ilixi_bg.jpg");

  char file[128];
  int i = 0;
  ImageWidget* widget;
  for (int x = 0; x < 4; ++x)
    {
      for (int y = 0; y < 3; ++y)
        {
          sprintf(file, "%sgallery/%d.jpg\0", ILIXI_DATADIR, i % 5);
          widget = new ImageWidget(file);
          widget->setGeometry(x * 198, y * 160, 197, 159);
          widget->setImage(new Image(file, 400, 275));
          widget->sigPressed.connect(sigc::mem_fun(this, &Gallery::showImage));
          addWidget(widget);
          ++i;
        }
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
