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

#include "AppButton.h"
#include "Launcher.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

namespace ilixi
{

  AppButton::AppButton(const std::string& name, const char *p, const char *a,
      const char* i, Widget* parent) :
      ToolButton(name, parent), _program(p), _args(a), _pid(0), _appID(0), _started(
          false)
  {
    setInputMethod(PointerInput);
    setIcon(i, Size(96, 96));
    sigClicked.connect(sigc::mem_fun(this, &AppButton::startApp));
    setToolButtonStyle(ToolButton::IconAboveText);
  }

  AppButton::~AppButton()
  {
    ILOG_DEBUG(ILX, "~AppButton %p\n", this);
  }

  const char*
  AppButton::getArgs() const
  {
    return _args;
  }

  void
  AppButton::setArgs(const char* args)
  {
    _args = args;
  }

  pid_t
  AppButton::getPid() const
  {
    return _pid;
  }

  void
  AppButton::setPid(pid_t pid)
  {
    _pid = pid;
  }

  const char*
  AppButton::getProgram() const
  {
    return _program;
  }

  void
  AppButton::setProgram(const char* program)
  {
    _program = program;
  }

  long long
  AppButton::getStartTime() const
  {
    return _startTime;
  }

  void
  AppButton::setStartTime(long long startTime)
  {
    _startTime = startTime;
  }

  bool
  AppButton::isStarted() const
  {
    return _started;
  }

  void
  AppButton::setStarted(bool started)
  {
    _started = started;
  }

  void
  AppButton::startApp()
  {
    sigStartRequest(text());
  }

  unsigned long
  AppButton::getAppID() const
  {
    return _appID;
  }

  void
  AppButton::setAppID(unsigned long appId)
  {
    _appID = appId;
  }

  void
  AppButton::compose()
  {
    Painter p(this);
    p.begin();
    if (state() & PressedState)
      p.setBrush(Color(128, 128, 128));
    else
      p.setBrush(Color(255, 255, 255));
    p.setFont(*font());
    p.drawLayout(_layout);
    p.end();
  }

} /* namespace ilixi */
