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

#include "ui/Dialog.h"
#include "ui/ToolButton.h"
#include "ui/Spacer.h"
#include "graphics/Painter.h"
#include <sigc++/bind.h>
#include "core/Logger.h"

using namespace ilixi;

Dialog::Dialog(const std::string& title, ButtonOption option, Widget* parent) :
    WindowWidget(), _buttonLayout(NULL), _result(-1)
{
  setTitle(title);
  setButtonLayoutOption(option);
  sigGeometryUpdated.connect(
      sigc::mem_fun(this, &Dialog::updateButtonLayoutGeometry));
  sigAbort.connect(sigc::mem_fun(this, &Dialog::reject));
}

Dialog::~Dialog()
{
  closeWindow();
}

int
Dialog::heightForWidth(int width) const
{
  int usedHorizontalSpace = (_canvasTopLeft.x() + _margin.hSum())
      - 2 * borderOffset();
  return _layout->heightForWidth(width - usedHorizontalSpace)
      + _canvasTopLeft.y() + _margin.vSum() + 2 * borderWidth()
      + _titleSize.height() + _buttonLayout->preferredSize().height();
}

Size
Dialog::preferredSize() const
{
  Size layoutSize = _layout->preferredSize();
  Size buttonLayoutSize = _buttonLayout->preferredSize();

  int layoutWidth = layoutSize.width() + _canvasTopLeft.x() + _margin.hSum();
  int w = std::max(layoutWidth, _titleSize.width());
  w = std::max(w, buttonLayoutSize.width());

  int hButtonLayout = buttonLayoutSize.height() + 2 * spacing();

  return Size(
      w + 2 * borderOffset(),
      layoutSize.height() + _canvasTopLeft.y() + _margin.vSum()
          + _titleSize.height() + hButtonLayout + 2 * borderWidth());
}

void
Dialog::accept()
{
  sigAccepted();
  finish(1);
}

void
Dialog::reject()
{
  sigRejected();
  finish(0);
}

void
Dialog::finish(int result)
{
  _result = result;
  sigFinished(_result);
  closeWindow();
}

int
Dialog::result() const
{
  return _result;
}

void
Dialog::execute()
{
  setVisible(true);
  initWindow();
  showWindow();
}

std::string
Dialog::title() const
{
  return _title;
}

Size
Dialog::titleSize() const
{
  return _titleSize;
}

void
Dialog::setButtonLayoutOption(ButtonOption option)
{
  if (_buttonLayout)
    removeChild(_buttonLayout);

  if (option == NoButtonOption)
    return;

  _buttonLayout = new HBoxLayout();
  _buttonLayout->addWidget(new Spacer());

  ToolButton* button;
  if (option == OKButtonOption)
    {
      button = new ToolButton("OK");
      _buttonLayout->addWidget(button);
      button->sigClicked.connect(sigc::mem_fun(this, &Dialog::accept));
    }
  else if (option == CancelButtonOption)
    {
      button = new ToolButton("Cancel");
      _buttonLayout->addWidget(button);
      button->sigClicked.connect(sigc::mem_fun(this, &Dialog::reject));
    }
  else if (option == OKCancelButtonOption)
    {
      button = new ToolButton("OK");
      _buttonLayout->addWidget(button);
      button->sigClicked.connect(sigc::mem_fun(this, &Dialog::accept));

      button = new ToolButton("Cancel");
      _buttonLayout->addWidget(button);
      button->sigClicked.connect(sigc::mem_fun(this, &Dialog::reject));
    }
  else
    {
      button = new ToolButton("Yes");
      _buttonLayout->addWidget(button);
      button->sigClicked.connect(sigc::mem_fun(this, &Dialog::accept));

      button = new ToolButton("No");
      _buttonLayout->addWidget(button);
      button->sigClicked.connect(sigc::mem_fun(this, &Dialog::reject));

      if (option == YesNoCancelButtonOption)
        {
          button = new ToolButton("Cancel");
          _buttonLayout->addWidget(button);
          button->sigClicked.connect(
              sigc::bind<int>(sigc::mem_fun(this, &Dialog::finish), -1));
        }
    }

  addChild(_buttonLayout);
  _buttonLayout->setNeighbour(Up, _layout);
  _layout->setNeighbour(Down, _buttonLayout);
}

void
Dialog::setButtonLayout(LayoutBase* buttonLayout)
{
  if (!buttonLayout)
    return;

  if (_buttonLayout)
    removeChild(_buttonLayout);

  _buttonLayout = buttonLayout;
  addChild(_buttonLayout);
}

void
Dialog::setTitle(const std::string& title)
{
  _title = title;
  _titleSize = stylist()->defaultFont(StyleHint::TitleFont)->extents(_title);
}

int
Dialog::canvasY() const
{
  return _canvasTopLeft.y() + _margin.top() + borderWidth()
      + _titleSize.height();
}

int
Dialog::canvasHeight() const
{
  return height()
      - (_canvasTopLeft.y() + _margin.vSum() + 2 * borderWidth()
          + _titleSize.height() + _buttonLayout->preferredSize().height()
          + 2 * spacing());
}

void
Dialog::compose()
{
  Painter painter(this);
  painter.begin();
  stylist()->drawDialog(&painter, this);
  painter.end();
}

void
Dialog::updateButtonLayoutGeometry()
{
  if (_buttonLayout->count() == 0)
    return;

  Size buttonLayoutSize = _buttonLayout->preferredSize();

  _layout->setNeighbour(Down, _buttonLayout);
  _buttonLayout->setNeighbour(Up, _layout);

  _buttonLayout->moveTo(borderOffset(),
      height() - (buttonLayoutSize.height() + spacing() + borderWidth()));

  _buttonLayout->setSize(width() - 2 * borderOffset(),
      buttonLayoutSize.height());
}
