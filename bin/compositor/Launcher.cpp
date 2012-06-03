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
#include "Compositor.h"
#include "graphics/Painter.h"

#include <sigc++/bind.h>
#include "core/Logger.h"

namespace ilixi
{

  Launcher::Launcher(Compositor* parent) :
      Widget(parent), _compositor(parent)
  {
    setInputMethod(PointerInput);

    _font = new Font(ILIXI_DATADIR"compositor/verabd.ttf");
    _font->setSize(12);
    _font->setAttributes(DFFA_STYLE_BOLD);

    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &Launcher::updateLauncherGeometry));

    initButtons();
  }

  Launcher::~Launcher()
  {
    ILOG_DEBUG(ILX, "~Launcher %p\n", this);
  }

  Size
  Launcher::preferredSize() const
  {
    return Size(_children.size() * 96, _children.size() * 96);
  }

  void
  Launcher::addButton(const std::string& name, const std::string& icon)
  {
    LauncherButton* button = new LauncherButton(name, this);
    button->setFont(_font);
    button->setIcon(icon, Size(96, 96));
    button->setZ(3);
    addChild(button);

    button->sigClicked.connect(
        sigc::bind<std::string>(
            sigc::mem_fun(_compositor->appMan(), &ApplicationManager::startApp),
            name));
  }

  void
  Launcher::initButtons()
  {
    AppInfoList list = _compositor->appMan()->applicationList();

    for (AppInfoList::iterator it = list.begin(); it != list.end(); ++it)
      addButton(((AppInfo*) *it)->name(), ((AppInfo*) *it)->icon());
  }

  void
  Launcher::compose(const PaintEvent& event)
  {
    Painter p(this);
    p.begin(event.rect);
    p.setBrush(Color(0, 0, 0, 120));
    p.fillRectangle(0, 0, width(), height());
    p.end();
  }

  void
  Launcher::updateLauncherGeometry()
  {
    int hOffset = (height() - 390) / 2;
    int wC = width() / 130.0;
    int xOffset = (width() - (wC * 130)) / 2;
    if (wC)
      {
        int y = -1;
        int i = 0;
        for (WidgetList::iterator it = _children.begin(); it != _children.end();
            ++it, ++i)
          {
            if (i % wC == 0)
              y++;
            LauncherButton* button = dynamic_cast<LauncherButton*>(*it);
            if (button)
              {

                button->moveTo(xOffset + i % wC * 130, hOffset + y * 130);
                button->setSize(120, 120);
              }
          }
      }
  }

} /* namespace ilixi */
