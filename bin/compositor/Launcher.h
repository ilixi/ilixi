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

#ifndef ILIXI_LAUNCHER_H_
#define ILIXI_LAUNCHER_H_

#include "LauncherButton.h"
#include "ApplicationManager.h"
#include <vector>

namespace ilixi
{

  class Compositor;

  class Launcher : public Widget
  {
  public:
    Launcher(Compositor* parent);

    virtual
    ~Launcher();

    virtual Size
    preferredSize() const;

  protected:
    void
    initButtons();

    void
    addButton(const std::string& name, const std::string& icon);

    void
    compose(const PaintEvent& event);

  private:
    Compositor* _compositor;
    Font* _font;

    void
    updateLauncherGeometry();
  };

} /* namespace ilixi */
#endif /* ILIXI_LAUNCHER_H_ */
