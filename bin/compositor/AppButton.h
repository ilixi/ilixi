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

#ifndef ILIXI_APPBUTTON_H_
#define ILIXI_APPBUTTON_H_

#include "ui/ToolButton.h"
#include "CWindow.h"
#include "CompositorSurfaceView.h"

namespace ilixi
{

  class AppButton : public ToolButton
  {
  public:
    AppButton(const std::string& name, const char *program, const char *args,
        const char* image, Widget* parent = 0);

    virtual
    ~AppButton();

    const char*
    getArgs() const;

    void
    setArgs(const char* args);

    pid_t
    getPid() const;

    void
    setPid(pid_t pid);

    const char*
    getProgram() const;

    void
    setProgram(const char* program);

    long long
    getStartTime() const;

    void
    setStartTime(long long startTime);

    bool
    isStarted() const;

    void
    setStarted(bool started);

    void
    startApp();

    unsigned long
    getAppID() const;

    void
    setAppID(unsigned long appId);

    sigc::signal<void, const std::string&> sigStartRequest;

  protected:

    void
    compose();

  private:
    const char* _program;
    const char* _args;
    long long _startTime;
    pid_t _pid;
    unsigned long _appID;

    bool _started;
  };

} /* namespace ilixi */
#endif /* ILIXI_APPBUTTON_H_ */
