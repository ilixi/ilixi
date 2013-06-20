/*
 Copyright 2010-2012 Tarik Sekmen.

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
#include "graphics/Painter.h"
#include <sigc++/bind.h>
#include "core/Logger.h"

namespace ilixi
{

  VKMenu::VKMenu(Widget* parent) :
      Widget(parent)
  {
    setInputMethod(PointerInput);
    setConstraints(ExpandingConstraint, FixedConstraint);
    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &VKMenu::updateVKMenuGeometry));

    _level1 = new ScrollArea();
    _level1->setUseThumbs(true);
//    _level1->surface()->setSurfaceFlag(Surface::BlitDescription);
//    _level1->setAutoScrolling(false);
    addChild(_level1);

    HBoxLayout* box1 = new HBoxLayout();
    box1->setSpacing(0);

    MenuItem* item;
    char title[128];

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

    _bounce = new Tween(Tween::BOUNCE, Tween::EASE_OUT, 1, 0);
    _inAni.addTween(_bounce);

    _circle = new Tween(Tween::CIRCLE, Tween::EASE_OUT, 0, 1);
    _inAni.addTween(_circle);

    _inAni.setDuration(1000);
    _inAni.sigExec.connect(sigc::mem_fun(this, &VKMenu::tweenSlot));
    _inAni.sigFinished.connect(
        sigc::mem_fun(this, &VKMenu::clearScrollerAnimDelay));

    _inAni.setDelay(1000);
    _inAni.start();

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
    _inAni.setDelay(0);
  }

  void
  VKMenu::scrollTo(Widget* widget)
  {
    _level1->scrollTo(widget);
  }

  void
  VKMenu::tweenSlot()
  {
    _level1->moveTo(0, _bounce->value() * height());
    repaint();
  }

//  void
//  VKMenu::paint(const PaintEvent& event)
//  {
//    if (visible())
//      {
//        PaintEvent evt(this, event);
//        if (evt.isValid())
//          {
//            IDirectFBSurface* dfbSurface = surface()->dfbSurface();
//
//            if (_level1->visible())
//              {
//                if (_level1->surface())
//                  _level1->surface()->clear();
//
//                _level1->paint(evt);
//                _level1->surface()->flip();
//
//                dfbSurface->SetBlittingFlags(dfbSurface,
//                    (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL
//                        | DSBLIT_BLEND_COLORALPHA));
//                dfbSurface->SetColor(dfbSurface, 0, 0, 0,
//                    _circle->value() * 255);
//                surface()->blit(_level1->surface(),
//                    Rectangle(0, 0, width(), height()), _level1->x(),
//                    _level1->y());
//
//                /*
//                 DFBRectangle sRect =
//                 Rectangle(0, 0, width(), height()).dfbRect();
//                 DFBRectangle dRect = Rectangle(_level1->x() + _val1 * _level1->width() / 2, _level1->y(),
//                 width() * _val2, height() * _val2).dfbRect();
//
//                 dfbSurface->StretchBlit(dfbSurface,
//                 _level1->surface()->DFBSurface(), &sRect, &dRect);
//                 */
//              }
//
//          }
//      }
//  }

  void
  VKMenu::compose(const PaintEvent& event)
  {
  }

  void
  VKMenu::updateVKMenuGeometry()
  {
    _level1->setGeometry(0, height(), width(), height());
  }

} /* namespace ilixi */
