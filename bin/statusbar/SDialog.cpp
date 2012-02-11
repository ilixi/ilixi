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

#include "SDialog.h"
#include "ui/VBoxLayout.h"
#include "sigc++/bind.h"
#include "ilixiConfig.h"

using namespace ilixi;

SDialog::SDialog(std::string title, Widget* parent) :
  Dialog(title, CancelButtonOption, parent)
{
  setLayout(new VBoxLayout());
  _kill = new PushButton("Kill");
  _kill->setIcon(ILIXI_DATADIR"statusbar/kill.png");
  _kill->sigClicked.connect(
      sigc::bind<int>(sigc::mem_fun(this, &SDialog::finish), 2));
  addWidget(_kill);

  _shutdown = new PushButton("Shutdown");
  _shutdown->setIcon(ILIXI_DATADIR"statusbar/shutdown.png");
  _shutdown->sigClicked.connect(
      sigc::bind<int>(sigc::mem_fun(this, &SDialog::finish), 5));
  addWidget(_shutdown);
}

SDialog::~SDialog()
{
}

void
SDialog::setAppName(std::string appName)
{
  _kill->setText("Terminate " + appName);
}

void
SDialog::setKillVisibility(bool visible)
{
  _kill->setVisible(visible);
}
