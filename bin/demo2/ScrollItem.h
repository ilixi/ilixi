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

#ifndef ILIXI_SCROLLITEM_H_
#define ILIXI_SCROLLITEM_H_

#include "ui/TextBase.h"

class ScrollItem : public ilixi::Widget, public ilixi::TextBase
{
public:
  ScrollItem(const std::string& text, ilixi::Widget* parent = 0);

  virtual
  ~ScrollItem();

  virtual ilixi::Size
  preferredSize() const;

  void
  setImage(ilixi::Image* image);

  sigc::signal<void, std::string> sigFocused;

protected:
  void
  tweenSlot();

  virtual void
  focusInEvent();

  virtual void
  focusOutEvent();

  virtual void
  enterEvent(const ilixi::PointerEvent& event);

  virtual void
  leaveEvent(const ilixi::PointerEvent& event);

  virtual void
  compose(const ilixi::PaintEvent& event);

  void
  updateTextBaseGeometry();

private:
  float _val1;
  ilixi::Image* _image;
};

#endif /* ILIXI_SCROLLITEM_H_ */
