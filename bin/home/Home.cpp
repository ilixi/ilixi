/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#include "Home.h"
#include "ui/HBoxLayout.h"
#include "ui/PushButton.h"
#include <sigc++/bind.h>
#include <spawn.h>
#include <sys/wait.h>

using namespace ilixi;

Home::Home(int argc, char* argv[]) :
  Application(argc, argv)
{
  setTitle("Home");
  setBackgroundFilled(true);
  HBoxLayout* box = new HBoxLayout();
  setLayout(box);

  PushButton* demo1 = new PushButton("Picture gallery");
  addWidget(demo1);
  demo1->sigClicked.connect(sigc::bind(sigc::mem_fun(this, &Home::runApp), "ilixi_demo1"));

  PushButton* demo2 = new PushButton("Demo2");
  addWidget(demo2);
  demo2->sigClicked.connect(sigc::bind(sigc::mem_fun(this, &Home::runApp), "ilixi_demo2"));
}

Home::~Home()
{
}

void
Home::runApp(const char* path)
{
  pid_t id;
  int err;
  char* sArg[2];
  sArg[0]= const_cast<char*>(path);
  sArg[1] = NULL;
  if ((err = posix_spawnp(&id, sArg[0], NULL, NULL, sArg, environ)) != 0)
    {
      D_INFO("posix_spawn() error: %d\n", err);
      exit(EXIT_FAILURE);
    }
  else
    D_INFO("Spawned %s\n", sArg[0]);
  int childExitStatus;
  waitpid(id, &childExitStatus, WCONTINUED);
}

int
main(int argc, char* argv[])
{
  Home app(argc, argv);
  app.exec();
  return 0;
}
