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

#include "VKMenu.h"
#include "ui/HBoxLayout.h"
#include "ui/SurfaceView.h"
#include "graphics/Painter.h"
#include <sigc++/bind.h>
#include "core/Logger.h"

namespace ilixi
{

  VKMenu::VKMenu(Widget* parent) :
      Widget(parent), _val1(0)
  {
    setInputMethod(PointerInput);
    setConstraints(ExpandingConstraint, FixedConstraint);
    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &VKMenu::updateVKMenuGeometry));

    _level1 = new ScrollArea();
    _level1->setSurfaceFlags(BlitDescription);
//    _level1->setAutoScrolling(false);
    addChild(_level1);

    HBoxLayout* box1 = new HBoxLayout();
    box1->setSpacing(0);

    MenuItem* item;
    char title[128];

    SurfaceView* view = new SurfaceView(4);
    view->setMaximumSize(196,196);
    box1->addWidget(view);

    for (int i = 0; i < 20; ++i)
      {
        sprintf(title, "Menu 1 Item %d", i);
        item = new MenuItem(title);

        sprintf(title, "%sgallery/%d.jpg", ILIXI_DATADIR, i % 5);
        item->setImage(new Image(title, 196, 196));

        item->sigClicked.connect(
            sigc::bind<MenuItem*>(sigc::mem_fun(this, &VKMenu::showScroller),
                item));
        item->sigFocused.connect(sigc::mem_fun(this, &VKMenu::scrollTo));

        box1->addWidget(item);
      }
    _level1->setContent(box1);

// ----------------------------------------------------

    _inAni = new TweenAnimation();
    _inAni->addTween(Tween::BOUNCE, Tween::EASE_OUT, _val1, 1, 0);
    _inAni->addTween(Tween::CIRCLE, Tween::EASE_OUT, _val2, 0, 1);
    _inAni->setDuration(1000);
    _inAni->sigExec.connect(sigc::mem_fun(this, &VKMenu::tweenSlot));
    _inAni->sigFinished.connect(
        sigc::mem_fun(this, &VKMenu::clearScrollerAnimDelay));

    _inAni->setDelay(1000);
    _inAni->start();

    _dialog = new ImageDialog("Image Dialog");
  }

  VKMenu::~VKMenu()
  {
    delete _dialog;
  }

  Size
  VKMenu::preferredSize() const
  {
    return _level1->preferredSize();
  }

  void
  VKMenu::showScroller(MenuItem* widget)
  {
    _dialog->setImagePath(widget->image()->getImagePath());
    _dialog->execute();
  }

  void
  VKMenu::clearScrollerAnimDelay()
  {
    _inAni->setDelay(0);
  }

  void
  VKMenu::scrollTo(Widget* widget)
  {
    _level1->scrollTo(widget);
  }

  void
  VKMenu::tweenSlot()
  {
    _level1->moveTo(0, _val1 * height());
    repaint();
  }

  void
  VKMenu::paint(const Rectangle& rect)
  {
    if (visible())
      {
        updateSurface();
        Rectangle intersect = _frameGeometry.intersected(rect);
        if (intersect.isValid())
          {
            IDirectFBSurface* dfbSurface = surface()->DFBSurface();

            if (_level1->visible())
              {
                if (_level1->surface())
                  _level1->surface()->clear();

                _level1->paint(intersect);
                _level1->surface()->flip();

                dfbSurface->SetBlittingFlags(dfbSurface,
                    (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL
                        | DSBLIT_BLEND_COLORALPHA));
                dfbSurface->SetColor(dfbSurface, 0, 0, 0, _val2 * 255);
                surface()->blit(_level1->surface(),
                    Rectangle(0, 0, width(), height()), _level1->x(),
                    _level1->y());

                /*
                 DFBRectangle sRect =
                 Rectangle(0, 0, width(), height()).dfbRect();
                 DFBRectangle dRect = Rectangle(_level1->x() + _val1 * _level1->width() / 2, _level1->y(),
                 width() * _val2, height() * _val2).dfbRect();

                 dfbSurface->StretchBlit(dfbSurface,
                 _level1->surface()->DFBSurface(), &sRect, &dRect);
                 */
              }

          }
      }
  }

  void
  VKMenu::compose()
  {
  }

  void
  VKMenu::updateVKMenuGeometry()
  {
    _level1->setGeometry(0, height(), width(), height());
  }

} /* namespace ilixi */
