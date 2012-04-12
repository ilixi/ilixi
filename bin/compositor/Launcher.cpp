/*
 Copyright 2012 Tarik Sekmen.

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

#include "Launcher.h"
#include "graphics/Painter.h"
#include <sys/wait.h>

namespace ilixi
{

  Launcher::Launcher(Widget* parent) :
      Widget(parent), _currentButton(-1)
  {
    setInputMethod(PointerInput);

    _font = new Font(ILIXI_DATADIR"compositor/verabd.ttf");
    _font->setSize(12);
    _font->setAttributes(DFFA_STYLE_BOLD);

    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &Launcher::updateLauncherGeometry));
  }

  Launcher::~Launcher()
  {
    delete _font;
  }

  Size
  Launcher::preferredSize() const
  {
    return Size(_apps.size() * 96, _apps.size() * 96);
  }

  void
  Launcher::addApplication(const char *name, const char *program,
      const char *args, const char* image)
  {
    AppButton* app = new AppButton(name, program, args, image);
    app->setFont(_font);
    addChild(app);
    _apps.push_back(app);

    app->sigStartRequest.connect(
        sigc::mem_fun(this, &Launcher::startApplication));
  }

  AppButton*
  Launcher::lookUpApplication(const std::string& name, unsigned int *id)
  {
    int i = 0;
    for (AppList::iterator it = _apps.begin(); it != _apps.end(); ++it, ++i)
      {
        if (((AppButton*) (*it))->text() == name)
          {
            if (id)
              *id = i;
            return ((AppButton*) (*it));
          }
      }
    return NULL;
  }

  AppButton*
  Launcher::lookUpApplication(pid_t pid, unsigned int *id)
  {
    int i = 0;
    for (AppList::iterator it = _apps.begin(); it != _apps.end(); ++it, ++i)
      {
        if (((AppButton*) (*it))->getPid() == pid)
          {
            if (id)
              *id = i;
            return ((AppButton*) (*it));
          }
      }
    return NULL;
  }

  AppButton*
  Launcher::lookUpApplication(DFBWindowID wid, unsigned int *id)
  {
    int i = 0;
    for (AppList::iterator it = _apps.begin(); it != _apps.end(); ++it, ++i)
      {
        AppButton* app = (AppButton*) *it;
        if (app->getThumb() && app->getThumb()->windowID() == wid)
          {
            if (id)
              *id = i;
            return app;
          }

      }
    return NULL;
  }

  void
  Launcher::startApplication(const std::string& name)
  {
    pid_t pid;
    unsigned int id;
    AppButton* app = lookUpApplication(name, &id);

    if (!app)
      return;

    if (app->isStarted() && !waitpid(app->getPid(), NULL, WNOHANG))
      {
        D_DEBUG("Already running '%s' (%d)!", name.c_str(), app->getPid());
        sigViewRequest(app->getCompositedSurface());
        return;
      }

    D_INFO("Starting %s\n", name.c_str());

    app->setStarted(true);
    app->setStartTime(direct_clock_get_millis());

    setCurrent(id);

    char str[256];
    int arC = 1;
    char *p;
    int i = 0;
    pid = vfork();
    switch (pid)
      {
    case -1:
      perror("vfork");
      return;

    case 0:
      setsid();

      if (app->getArgs() != NULL)
        {
          strcpy(str, app->getArgs());
          p = strtok(str, " ");
          while (p != NULL)
            {
              arC++;
              p = strtok(NULL, " ");
            }

          const char *args[arC + 1];
          args[0] = app->getProgram();

          strcpy(str, app->getArgs());
          arC = 1;
          p = strtok(str, " ");
          while (p != NULL)
            {
              args[arC++] = p;
              p = strtok(NULL, " ");
            }
          args[arC++] = NULL;

          while (i < arC)
            D_INFO("ARG: %d - %s\n", i++, args[i]);

          execvp(app->getProgram(), (char**) args);
        }
      else
        {
          const char* args[2];
          args[0] = app->getProgram();
          args[1] = NULL;
          execvp(app->getProgram(), (char**) args);
        }

      perror("execvp");
      _exit(0);
      break;

    default:
      app->setPid(pid);
      break;
      }
  }

  void
  Launcher::compose()
  {
    Painter p(this);
    p.begin();
    p.setBrush(Color(0, 0, 0, 120));
    p.fillRectangle(0, 0, width(), height());
    p.end();
  }

  AppButton*
  Launcher::getCurrent() const
  {
    if (_currentButton >= 0)
      return _apps[_currentButton];
    return NULL;
  }

  void
  Launcher::setCurrent(unsigned int id)
  {
    _currentButton = id;
  }

  void
  Launcher::updateLauncherGeometry()
  {
    int hOffset = (height() - 260) / 2;
    int wC = width() / 130.0;
    int xOffset = (width() - (wC * 130)) / 2;
    if (wC)
      {
        int y = -1;
        for (unsigned int i = 0; i < _apps.size(); ++i)
          {
            if (i % wC == 0)
              y++;
            _apps[i]->moveTo(xOffset + i % wC * 130, hOffset + y * 130);
            _apps[i]->setSize(120, 120);
          }
      }
  }

} /* namespace ilixi */
