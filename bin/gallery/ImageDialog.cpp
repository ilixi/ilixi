/*
 * ImageDialog.cpp
 *
 *  Created on: 10 Nov 2011
 *      Author: tarik
 */

#include "ImageDialog.h"
#include "ui/VBoxLayout.h"
#include "ui/Icon.h"
#include "ui/ScrollArea.h"

using namespace ilixi;

ImageDialog::ImageDialog(const std::string& title, Widget* parent) :
  Dialog(title, OKButtonOption, parent)
{
  setLayout(new VBoxLayout());

  ScrollArea* scroller = new ScrollArea();
  addWidget(scroller);
  scroller->setMinimumSize(400, 300);

  _icon = new Icon();
  _icon->setSize(1000, 800);
  addWidget(_icon);

  scroller->setContent(_icon);
}

ImageDialog::~ImageDialog()
{
}

void
ImageDialog::setImagePath(const std::string& path)
{
  _icon->setDefaultImage(path);
}
