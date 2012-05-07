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

#include "graphics/Stylist.h"
#include "graphics/Painter.h"
#include "lib/TweenAnimation.h"
#include "ilixiGUI.h"
#include <string.h>
#include "sigc++/bind.h"

#include "core/Logger.h"

using namespace ilixi;

Image* Stylist::_noImage = NULL;

Stylist::Stylist()
{
  _borderWidth = 1;
  _noImage = new Image(ILIXI_DATADIR"images/noImage.png", 48, 48);
  setPaletteFromFile(ILIXI_DATADIR"def_palette.xml");
  setStyleFromFile(ILIXI_DATADIR"def_style.xml");
  initAnimations();
}

Stylist::~Stylist()
{
  delete _focus.in;
  delete _focus.out;
  delete _noImage;
}

bool
Stylist::setPaletteFromFile(const char* palette)
{
  return _palette.parsePalette(palette);
}

bool
Stylist::setStyleFromFile(const char* style)
{
  return _style.parseStyle(style);
}

Size
Stylist::defaultSize(StyleHint::Size size) const
{
  switch (size)
    {
  case StyleHint::Icon:
    return _style._iconSize;
  case StyleHint::PushButton:
    return _style._buttonSize;
  case StyleHint::CheckBox:
    return _style._checkboxSize;
  case StyleHint::RadioButton:
    return _style._radiobuttonSize;
  case StyleHint::ProgressBar:
    return _style._progressbarSize;
  case StyleHint::Slider:
    return _style._sliderSize;
  case StyleHint::ScrollBarButton:
    return Size(_style._scrollbarSize.height(), _style._scrollbarSize.height());
  case StyleHint::ScrollBar:
    return _style._scrollbarSize;
  case StyleHint::TabPanel:
    return _style._tabPanelSize;
  case StyleHint::TabPanelButton:
    return _style._tabPanelButtonSize;
  case StyleHint::TabPanelButtonIcon:
    return Size(
        _style._tabPanelButtonSize.height() - _style._tabPanelButtonOffset,
        _style._tabPanelButtonSize.height() - _style._tabPanelButtonOffset);

  default:
    return Size();
    }
}

int
Stylist::defaultParameter(StyleHint::Parameter parameter) const
{
  switch (parameter)
    {
  case StyleHint::BorderWidth:
    return _borderWidth;

  case StyleHint::FrameBorderRadius:
    return _style._frameRadius;

  case StyleHint::TextInputRadius:
    return _style._textInputRadius;

  case StyleHint::ButtonHeight:
    return _style._buttonSize.height();
  case StyleHint::ButtonWidth:
    return _style._buttonSize.width();
  case StyleHint::ButtonOffset:
    return _style._buttonOffset;
  case StyleHint::ButtonRadius:
    return _style._buttonRadius;
  case StyleHint::ButtonIndicator:
    return _style._buttonIndicator;

  case StyleHint::RadioHeight:
    return _style._radiobuttonSize.height();
  case StyleHint::RadioWidth:
    return _style._radiobuttonSize.width();
  case StyleHint::RadioOffset:
    return _style._radiobuttonOffset;

  case StyleHint::CheckBoxHeight:
    return _style._checkboxSize.height();
  case StyleHint::CheckBoxWidth:
    return _style._checkboxSize.width();
  case StyleHint::CheckBoxOffset:
    return _style._checkboxOffset;
  case StyleHint::CheckBoxRadius:
    return _style._checkboxRadius;

  case StyleHint::ComboBoxRadius:
    return _style._comboboxRadius;
  case StyleHint::ComboBoxButtonWidth:
    return _style._comboboxButtonWidth;

  case StyleHint::ProgressBarHeight:
    return _style._progressbarSize.height();
  case StyleHint::ProgressBarWidth:
    return _style._progressbarSize.width();
  case StyleHint::ProgressBarRadius:
    return _style._progressbarRadius;

  case StyleHint::SliderHeight:
    return _style._sliderSize.height();
  case StyleHint::SliderWidth:
    return _style._sliderSize.width();
  case StyleHint::SliderRadius:
    return _style._sliderRadius;

  case StyleHint::ScrollBarHeight:
    return _style._scrollbarSize.height();
  case StyleHint::ScrollBarWidth:
    return _style._scrollbarSize.width();
  case StyleHint::ScrollBarRadius:
    return _style._scrollbarRadius;

  case StyleHint::ScrollBarButtonHeight:
    return _style._scrollbarSize.height();
  case StyleHint::ScrollBarButtonWidth:
    return _style._scrollbarSize.height();

  case StyleHint::TabPanelHeight:
    return _style._tabPanelSize.height();
  case StyleHint::TabPanelWidth:
    return _style._tabPanelSize.width();
  case StyleHint::TabPanelButtonWidth:
    return _style._tabPanelButtonSize.width();
  case StyleHint::TabPanelButtonHeight:
    return _style._tabPanelButtonSize.height();
  case StyleHint::TabPanelOffset:
    return _style._tabPanelButtonOffset;

  case StyleHint::ToolBarHeight:
    return _style._toolbarHeight;

  default:
    return -1;
    }
}

Font*
Stylist::defaultFont(StyleHint::Font font) const
{
  switch (font)
    {
  case StyleHint::ButtonFont:
    return _style._buttonFont;
  case StyleHint::TitleFont:
    return _style._titleFont;
  case StyleHint::InputFont:
    return _style._inputFont;
  default:
    return _style._defaultFont;
    }
}

Image*
Stylist::defaultIcon(StyleHint::DefaultIcon icon) const
{
  switch (icon)
    {
  case StyleHint::Information:
    return _style._info;
  case StyleHint::Question:
    return _style._question;
  case StyleHint::Warning:
    return _style._warning;
  case StyleHint::Critical:
    return _style._critical;
    }
}

//////////////////////////////////////////////////////////////////////////
// Drawing methods
//////////////////////////////////////////////////////////////////////////

void
Stylist::drawAppFrame(Painter* painter, Application* app)
{
  if (app->background())
    painter->drawImage(app->background(), 0, 0, app->width(), app->height());
  else
    {
      painter->setBrush(_palette.bgBottom);
      painter->fillRectangle(0, 0, app->width(), app->height());
    }
}

void
Stylist::drawDialog(Painter* painter, Dialog* dialog)
{
  int titleHeight = dialog->titleSize().height();

  // draw frame
  painter->setBrush(_palette.bgBottom);
  painter->fillRectangle(0, 0, dialog->width(), dialog->height());

  painter->setPen(_palette._default.borderBottom);
  painter->drawRectangle(0, 0, dialog->width(), dialog->height());

  // draw title
  if (dialog->state() & DisabledState)
    painter->setBrush(_palette.textDisabled);
  else
    painter->setBrush(_palette.text);

  painter->setFont(*_style._titleFont);
  painter->drawText(dialog->title(), _style._frameRadius, 1);
}

void
Stylist::drawFrame(Painter* painter, BorderBase* frame)
{
  // TODO complete drawing for all border styles.
  if (frame->borderStyle() == NoBorder)
    return;
  painter->setPen(_palette.getGroup(frame->_owner->state()).borderBottom);
  painter->drawRectangle(0, 0, frame->_owner->width(), frame->_owner->height());
}

void
Stylist::drawLabel(Painter* painter, Label* label)
{
  if (label->state() & DisabledState)
    painter->setBrush(_palette.textDisabled);
  else
    painter->setBrush(_palette.text);

  painter->setFont(*label->font());
  painter->drawLayout(label->layout());
}

void
Stylist::drawIcon(Painter* painter, Icon* icon)
{
  if (icon->parent() && (icon->parent()->state() & PressedState))
    {
      painter->setBrush(Color(128, 128, 128));
      painter->drawImage(icon->image(), 0, 0,
          (DFBSurfaceBlittingFlags) (DSBLIT_COLORIZE | DSBLIT_BLEND_ALPHACHANNEL));
    }
  else
    painter->drawImage(icon->image(),
        Rectangle(0, 0, icon->width(), icon->height()));
}

void
Stylist::drawCheckBox(Painter* painter, CheckBox* checkbox)
{
  const WidgetState state = checkbox->state();

  Rectangle r(_style._checkboxOffset, 0, _style._checkboxSize.width(),
      _style._checkboxSize.height());

  // Draw frame
  if (checkbox->checkable() && (checkbox->checked() || checkbox->partial()))
    painter->setBrush(_palette._pressed.bgBottom);
  else
    painter->setBrush(_palette.getGroup(state).bgBottom);
  painter->fillRectangle(r);

  // Draw frame border
  if (state & FocusedState)
    painter->setPen(_palette.focusBottom);
  else
    painter->setPen(_palette.getGroup(state).borderBottom);
  painter->drawRectangle(r);

  // draw checkbox state
  if (checkbox->checked())
    painter->drawImage(_style._checkFull, r);
  else if (checkbox->partial())
    painter->drawImage(_style._checkPartial, r);
  else
    painter->drawImage(_style._checkEmpty, r);

  // draw text
  if (!checkbox->text().empty())
    {
      painter->setBrush(_palette.getGroup(state).text);
      painter->drawLayout(checkbox->layout());
    }
}

void
Stylist::drawPushButton(Painter* painter, PushButton* button)
{
  const WidgetState state = button->state();

  // Draw frame
  if (button->checkable() && button->checked())
    painter->setBrush(_palette._pressed.bgBottom);
  else
    painter->setBrush(_palette.getGroup(state).bgBottom);
  painter->fillRectangle(0, 0, button->width(), button->height());

  if (state & FocusedState)
    {
      Color c = _palette.focusBottom;
      painter->setPen(c);
    }
  else
    painter->setPen(_palette.getGroup(state).borderBottom);
  painter->drawRectangle(0, 0, button->width(), button->height());

  // Draw button text
  if (!button->text().empty())
    {
      painter->setBrush(_palette.getGroup(state).text);
      painter->drawLayout(button->layout());
    }
}

void
Stylist::drawRadioButton(Painter* painter, RadioButton* button)
{
  const WidgetState state = button->state();

  Rectangle r(_style._radiobuttonOffset, 0, _style._radiobuttonSize.width(),
      _style._radiobuttonSize.height());

  // frame
  if (button->checkable() && button->checked())
    painter->setBrush(_palette._pressed.bgBottom);
  else
    painter->setBrush(_palette.getGroup(state).bgBottom);
  painter->fillRectangle(r);

  // frame border
  if (state & FocusedState)
    painter->setPen(_palette.focusBottom);
  else
    painter->setPen(_palette.getGroup(state).borderBottom);
  painter->drawRectangle(r);

  // selection indicator
  painter->setPen(_palette.getGroup(state).borderBottom);
  if (button->checked())
    painter->setBrush(_palette.focusTop);
  else
    painter->setBrush(_palette.getGroup(state).borderMid);
  painter->fillRectangle(_style._radiobuttonOffset + 4, 4,
      _style._radiobuttonSize.width() - 8,
      _style._radiobuttonSize.height() - 8);

  // layout
  if (!button->text().empty())
    {
      painter->setBrush(_palette.getGroup(state).text);
      painter->drawLayout(button->layout());
    }
}

void
Stylist::drawToolButton(Painter* painter, ToolButton* button)
{
  const WidgetState state = button->state();

  // Draw frame
  painter->setBrush(_palette.getGroup(state).bgBottom);
  painter->fillRectangle(0, 0, button->width(), button->height());

  if (state & FocusedState)
    painter->setPen(_palette.focusBottom);
  else
    painter->setPen(_palette.getGroup(state).borderBottom);
  painter->drawRectangle(0, 0, button->width(), button->height());

  // Draw check indicator
  ToolButton::ToolButtonStyle buttonStyle = button->getToolButtonStyle();
  if (button->checkable())
    {
      bool horizontal = false;
      if ((buttonStyle == ToolButton::IconBelowText)
          || (buttonStyle == ToolButton::IconAboveText))
        horizontal = true;
      const WidgetState state = button->state();
      int y;
      if (horizontal)
        y = button->height() - _borderWidth - _style._buttonIndicator;
      else
        y = _borderWidth;

      if (button->checked())
        painter->setBrush(_palette.getGroup(state).fillBottom);
      else
        painter->setBrush(Color(0, 0, 0, 0.2));

      // draw indicator
      if (horizontal)
        painter->fillRectangle(2, y, button->width() - 4,
            _style._buttonIndicator);
      else
        painter->fillRectangle(2, _borderWidth, _style._buttonIndicator,
            button->height() - 2);
    }

  // Draw button text
  if (buttonStyle != ToolButton::IconOnly && !button->text().empty())
    {
      painter->setBrush(_palette.getGroup(state).text);
      painter->drawLayout(button->layout());
    }
}

void
Stylist::drawComboBox(Painter* painter, ComboBox* combo)
{
  const WidgetState state = combo->state();

  // draw frame
  painter->setBrush(_palette.getGroup(state).base);
  painter->fillRectangle(0, 0, combo->width(), combo->height());
  if (state & FocusedState)
    painter->setPen(_palette.focusBottom);
  else
    painter->setPen(_palette.getGroup(state).borderBottom);
  painter->drawRectangle(0, 0, combo->width(), combo->height());

  // draw button
  int x = combo->width() - _style._comboboxButtonWidth - 1;
  painter->setBrush(_palette.getGroup(state).bgBottom);
  painter->fillRectangle(x, 1, _style._comboboxButtonWidth,
      combo->height() - 2);
  painter->drawImage(_style._arrowDown, x, 0, _style._comboboxButtonWidth,
      combo->height());

  painter->setBrush(_palette.getGroup(state).baseText);
  painter->drawLayout(combo->layout());
}

void
Stylist::animate(StyledAnimation type, Widget* target)
{
  switch (type)
    {
  case FocusIn:
    _focus.targetIn = target;
    _focus.in->start();
    break;
  case FocusOut:
    _focus.targetOut = target;
    _focus.out->start();
    break;
  default:
    break;
    }
}

void
Stylist::initAnimations()
{
  _focus.in = new TweenAnimation();
  _focus.in->setDuration(500);
  _focus.in->addTween(Tween::SINE, Tween::EASE_OUT, 0, 1);
  _focus.in->sigExec.connect(
      sigc::bind<Stylist::StyledAnimation>(
          sigc::mem_fun(this, &Stylist::runAnimation), FocusIn));

  _focus.out = new TweenAnimation();
  _focus.out->setDuration(250);
  _focus.out->addTween(Tween::SINE, Tween::EASE_IN, 1, 0);
  _focus.out->sigExec.connect(
      sigc::bind<Stylist::StyledAnimation>(
          sigc::mem_fun(this, &Stylist::runAnimation), FocusOut));
}

void
Stylist::runAnimation(StyledAnimation type)
{
  if (type == FocusIn)
    _focus.targetIn->repaint();
  else if (type == FocusOut)
    _focus.targetOut->repaint();
}
