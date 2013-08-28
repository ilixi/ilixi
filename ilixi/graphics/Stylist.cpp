/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include <graphics/Stylist.h>
#include <graphics/Painter.h>
#include <ilixiGUI.h>
#include <string.h>
#include "core/Logger.h"

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_STYLIST, "ilixi/graphics/Stylist", "Stylist");

Stylist::Stylist()
        : StylistBase()
{
    ILOG_TRACE(ILX_STYLIST);
}

Stylist::~Stylist()
{
    ILOG_TRACE(ILX_STYLIST);
}

//////////////////////////////////////////////////////////////////////////
// Drawing methods
//////////////////////////////////////////////////////////////////////////

void
Stylist::drawAppFrame(Painter* p, AppWindow* app)
{
    if (app->background())
    {
        if (app->backgroundTiled())
            p->tileImage(app->background(), 0, 0);
        else
            p->stretchImage(app->background(), 0, 0, app->width(), app->height());
    } else
    {
        p->setBrush(_palette->bg);
        p->fillRectangle(0, 0, app->width(), app->height());
    }
}

void
Stylist::drawDialog(Painter* p, Dialog* dialog)
{
//    int titleHeight = dialog->titleSize().height();

    draw9Frame(p, 0, 0, dialog->width(), dialog->height(), _style->dialog);

    // draw title
    if (!dialog->enabled())
        p->setBrush(_palette->textDisabled);
    else
        p->setBrush(_palette->text);

    p->setFont(*_fonts->getFont(StyleHint::TitleFont));
    p->drawText(dialog->title(), defaultParameter(StyleHint::FrameOffsetLeft), defaultParameter(StyleHint::FrameOffsetTop));
}

void
Stylist::drawDirectionalButton(Painter* p, DirectionalButton* button)
{
    const WidgetState state = button->state();

    if (!button->enabled())
        draw9CFrame(p, 0, 0, button->width(), button->height(), _style->db2.dis, _style->db1.dis, button->corners());
    else if (state & PressedState)
        draw9CFrame(p, 0, 0, button->width(), button->height(), _style->db2.pre, _style->db1.pre, button->corners());
    else if (state & ExposedState)
        draw9CFrame(p, 0, 0, button->width(), button->height(), _style->db2.exp, _style->db1.exp, button->corners());
    else
        draw9CFrame(p, 0, 0, button->width(), button->height(), _style->db2.def, _style->db1.def, button->corners());

    if (state & FocusedState)
        draw9CFrame(p, 0, 0, button->width(), button->height(), _style->db2.foc, _style->db1.foc, button->corners());

    // Draw button text
    if (!button->text().empty())
    {
        p->setFont(*button->font());
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(button->layout(), 0, state & PressedState ? 1 : 0);
    }
}

void
Stylist::drawFrame(Painter* p, Frame* frame)
{
    const WidgetState state = frame->state();

    // Frame
    if (!frame->enabled())
        draw9Frame(p, 0, 0, frame->width(), frame->height(), _style->fr.dis);
    else
        draw9Frame(p, 0, 0, frame->width(), frame->height(), _style->fr.def);
}

void
Stylist::drawLabel(Painter* p, Label* label)
{
    if (!label->enabled())
        p->setBrush(_palette->textDisabled);
    else
        p->setBrush(_palette->text);

    p->setFont(*label->font());
    p->drawLayout(label->layout());
}

void
Stylist::drawIcon(Painter* p, Icon* icon)
{
    if (!icon->enabled())
    {
        p->setBrush(_palette->_disabled.text);
        p->stretchImage(icon->image(), Rectangle(0, 0, icon->width(), icon->height()), (DFBSurfaceBlittingFlags) (DSBLIT_COLORIZE | DSBLIT_BLEND_ALPHACHANNEL));
    } else if (icon->colorize())
    {
        p->setBrush(_palette->_default.text);
        p->stretchImage(icon->image(), Rectangle(0, 0, icon->width(), icon->height()), (DFBSurfaceBlittingFlags) (DSBLIT_COLORIZE | DSBLIT_BLEND_ALPHACHANNEL));
    } else
        p->stretchImage(icon->image(), Rectangle(0, 0, icon->width(), icon->height()));
}

void
Stylist::drawCheckBox(Painter* p, CheckBox* checkbox)
{
    const WidgetState state = checkbox->state();

    // Frame

    // Indicator
    if (checkbox->checked())
    {
        if (!checkbox->enabled())
            p->blitImage(_style->_pack, _style->cbC.dis, 0, 0);
        else if (state & PressedState)
            p->blitImage(_style->_pack, _style->cbC.pre, 0, 0);
        else if (state & ExposedState)
            p->blitImage(_style->_pack, _style->cbC.exp, 0, 0);
        else
            p->blitImage(_style->_pack, _style->cbC.def, 0, 0);

        if (state & FocusedState)
            p->blitImage(_style->_pack, _style->cbC.foc, 0, 0);
    } else if (checkbox->partial())
    {
        if (!checkbox->enabled())
            p->blitImage(_style->_pack, _style->cbT.dis, 0, 0);
        else if (state & PressedState)
            p->blitImage(_style->_pack, _style->cbT.pre, 0, 0);
        else if (state & ExposedState)
            p->blitImage(_style->_pack, _style->cbT.exp, 0, 0);
        else
            p->blitImage(_style->_pack, _style->cbT.def, 0, 0);

        if (state & FocusedState)
            p->blitImage(_style->_pack, _style->cbT.foc, 0, 0);
    } else
    {
        if (!checkbox->enabled())
            p->blitImage(_style->_pack, _style->cb.dis, 0, 0);
        else if ((state & PressedState) || (checkbox->checkable() && checkbox->checked()))
            p->blitImage(_style->_pack, _style->cb.pre, 0, 0);
        else if (state & ExposedState)
            p->blitImage(_style->_pack, _style->cb.exp, 0, 0);
        else
            p->blitImage(_style->_pack, _style->cb.def, 0, 0);

        if (state & FocusedState)
            p->blitImage(_style->_pack, _style->cb.foc, 0, 0);
    }

    // Text
    if (!checkbox->text().empty())
    {
        p->setFont(*checkbox->font());
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(checkbox->layout());
    }
}

void
Stylist::drawComboBox(Painter* p, ComboBox* combo)
{
    const WidgetState state = combo->state();

    int wB = combo->height();
    int wBG = combo->width() - wB;

    if (!combo->enabled())
        draw9CFrame(p, 0, 0, wBG, wB, _style->li.dis, _style->li2.dis, LeftCorners);
    else
        draw9CFrame(p, 0, 0, wBG, wB, _style->li.def, _style->li2.def, LeftCorners);

    if (state & FocusedState)
        draw9CFrame(p, 0, 0, wBG, wB, _style->li.foc, _style->li2.foc, LeftCorners);

    // draw button
    if (!combo->enabled())
        draw9CFrame(p, wBG, 0, wB, wB, _style->db2.dis, _style->db1.dis, RightCorners);
    else if (state & PressedState)
        draw9CFrame(p, wBG, 0, wB, wB, _style->db2.pre, _style->db1.pre, RightCorners);
    else if (state & ExposedState)
        draw9CFrame(p, wBG, 0, wB, wB, _style->db2.exp, _style->db1.exp, RightCorners);
    else
        draw9CFrame(p, wBG, 0, wB, wB, _style->db2.def, _style->db1.def, RightCorners);

    if (state & FocusedState)
        draw9CFrame(p, wBG, 0, wB, wB, _style->db2.foc, _style->db1.foc, RightCorners);

    p->setBrush(_palette->getGroup(state).baseText);
    p->drawLayout(combo->layout());
}

void
Stylist::drawLineInput(Painter* p, LineInput* input, bool cursor)
{
    const WidgetState state = input->state();

    // Frame
    if (input->drawFrame())
    {
        if (!input->enabled())
            draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.dis);
        else
            draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.def);

        if (state & FocusedState)
            draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.foc);
    }

    Rectangle r(defaultParameter(StyleHint::LineInputLeft), defaultParameter(StyleHint::LineInputTop), input->width() - defaultParameter(StyleHint::LineInputLR), input->height() - defaultParameter(StyleHint::LineInputTB));
    p->setClip(input->mapFromSurface(r));

    // selection
    if (!input->selectionRect().isNull())
    {
        p->setBrush(_palette->getGroup(state).fill);
        p->fillRectangle(input->selectionRect());
    }

    // layout
    if (!input->layout().isEmpty())
    {
        p->setFont(*input->font());
        p->setBrush(_palette->getGroup(state).baseText);
        p->drawLayout(input->layout());
    }

    if (cursor)
    {
        p->setBrush(_palette->focus);
        p->fillRectangle(input->cursorRect());
    }

    p->resetClip();
}

void
Stylist::drawLineSeperator(Painter* p, LineSeperator* sep)
{
    p->stretchImage(_style->_pack, Rectangle(0, 0, sep->width(), sep->height()), sep->orientation() == Horizontal ? _style->hLine : _style->vLine);
}

void
Stylist::drawGroupBox(Painter* p, GroupBox* box)
{
    const WidgetState state = box->state();

    int tabHeight = box->titleSize().height();

    // Frame
    draw9Frame(p, 0, tabHeight, box->width(), box->height() - tabHeight, _style->panel.def);
    drawTabFrame(p, defaultParameter(StyleHint::PanelLeft), 0, box->titleSize().width() + defaultParameter(StyleHint::PanelInvLeft), tabHeight + defaultParameter(StyleHint::PanelInvOverlap), _style->panel.def, _style->panelInv);
}

void
Stylist::drawPushButton(Painter* p, PushButton* button)
{
    const WidgetState state = button->state();

    // Frame
    if (button->getPushButtonStyle() == Default)
    {
        if (!button->enabled())
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pb.dis);
        else if ((state & PressedState) || (button->checkable() && button->checked()))
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pb.pre);
        else if (state & ExposedState)
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pb.exp);
        else
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pb.def);

        if (state & FocusedState)
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pb.foc);
    } else if (button->getPushButtonStyle() == OK)
    {
        if (!button->enabled())
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pbOK.dis);
        else if ((state & PressedState) || (button->checkable() && button->checked()))
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pbOK.pre);
        else if (state & ExposedState)
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pbOK.exp);
        else
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pbOK.def);

        if (state & FocusedState)
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pbOK.foc);
    } else if (button->getPushButtonStyle() == CANCEL)
    {
        if (!button->enabled())
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pbCAN.dis);
        else if ((state & PressedState) || (button->checkable() && button->checked()))
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pbCAN.pre);
        else if (state & ExposedState)
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pbCAN.exp);
        else
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pbCAN.def);

        if (state & FocusedState)
            draw3Frame(p, 0, 0, button->width(), button->height(), _style->pbCAN.foc);
    }

    // Text
    if (!button->text().empty())
    {
        p->setFont(*button->font());
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(button->layout(), 0, state & PressedState ? 1 : 0);
    }
}

void
Stylist::drawRadioButton(Painter* p, RadioButton* button)
{
    const WidgetState state = button->state();

    if (button->checked())
    {
        if (!button->enabled())
            p->blitImage(_style->_pack, _style->rbOn.dis, 0, 0);
        else if (state & PressedState)
            p->blitImage(_style->_pack, _style->rbOn.pre, 0, 0);
        else if (state & ExposedState)
            p->blitImage(_style->_pack, _style->rbOn.exp, 0, 0);
        else
            p->blitImage(_style->_pack, _style->rbOn.def, 0, 0);

        if (state & FocusedState)
            p->blitImage(_style->_pack, _style->rbOn.foc, 0, 0);
    } else
    {
        if (!button->enabled())
            p->blitImage(_style->_pack, _style->rbOff.dis, 0, 0);
        else if (state & PressedState)
            p->blitImage(_style->_pack, _style->rbOff.pre, 0, 0);
        else if (state & ExposedState)
            p->blitImage(_style->_pack, _style->rbOff.exp, 0, 0);
        else
            p->blitImage(_style->_pack, _style->rbOff.def, 0, 0);

        if (state & FocusedState)
            p->blitImage(_style->_pack, _style->rbOff.foc, 0, 0);
    }

    // Text
    if (!button->text().empty())
    {
        p->setFont(*button->font());
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(button->layout());
    }
}

void
Stylist::drawProgressBar(Painter* p, ProgressBar* bar)
{
    const WidgetState state = bar->state();

    if (bar->orientation() == Horizontal)
    {
        // Frame
        if (!bar->enabled())
            draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->prH.dis);
        else
            draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->prH.def);

        // Indicator
        if (bar->value())
        {
            int fillWidth = bar->width() * bar->value() / bar->range();
            if (!bar->enabled())
                draw3Frame(p, 0, 0, fillWidth, bar->height(), _style->prHI.dis);
            else
                draw3Frame(p, 0, 0, fillWidth, bar->height(), _style->prHI.def);
        }
    } else
    {
        // Frame
        if (!bar->enabled())
            draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->prV.dis, true);
        else
            draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->prV.def, true);

        // Indicator
        if (bar->value())
        {
            int fillHeight = bar->height() * bar->value() / bar->range();
            if (!bar->enabled())
                draw3Frame(p, 0, bar->height() - fillHeight, bar->width(), fillHeight, _style->prVI.dis, true);
            else
                draw3Frame(p, 0, bar->height() - fillHeight, bar->width(), fillHeight, _style->prVI.def, true);
        }
    }
}

void
Stylist::drawScrollArea(Painter* p, ScrollArea* area, const Rectangle& content)
{
    if (!area->enabled())
        draw9Frame(p, 0, 0, content.width(), content.height(), _style->li2.dis);
    else
        draw9Frame(p, 0, 0, content.width(), content.height(), _style->li2.def);
}

void
Stylist::drawScrollBar(Painter* p, int x, int y, int w, int h, Orientation orientation)
{
    if (orientation == Horizontal)
        draw3Frame(p, x, y, w, h, _style->hScr, false, (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA));
    else
        draw3Frame(p, x, y, w, h, _style->vScr, true, (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA));
}

void
Stylist::drawScrollBar(Painter* p, ScrollBar* bar)
{
    const WidgetState state = bar->state();

    // background
    if (!bar->enabled())
        draw9Frame(p, bar->indicatorRegion(), _style->sb.dis);
    else
        draw9Frame(p, bar->indicatorRegion(), _style->sb.def);

    // indicator
    if (!bar->enabled())
        draw9Frame(p, bar->indicator(), _style->db1.dis);
    else if (state & PressedState)
        draw9Frame(p, bar->indicator(), _style->db1.pre);
    else if (state & ExposedState)
        draw9Frame(p, bar->indicator(), _style->db1.exp);
    else
        draw9Frame(p, bar->indicator(), _style->db1.def);

    int wDif = bar->indicator().width() - _style->sbRH.width();
    int hDif = bar->indicator().height() - _style->sbRV.height();

    if (wDif > 5 && hDif > 5)
    {
        if (bar->orientation() == Horizontal)
            p->blitImage(_style->_pack, _style->sbRH, bar->indicator().x() + wDif / 2, hDif / 2);
        else
            p->blitImage(_style->_pack, _style->sbRV, wDif / 2, bar->indicator().y() + hDif / 2);
    }
}

void
Stylist::drawSlider(Painter* p, Slider* bar)
{
    const WidgetState state = bar->state();

    if (bar->orientation() == Horizontal)
    {
        if (!bar->enabled())
            draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->hSl.dis);
        else
            draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->hSl.def);

        // fill
        if (bar->value() > bar->minimum())
        {
            int offset = bar->_indicator.x() + defaultParameter(StyleHint::SliderIndicatorWidth) / 2;
            if (bar->inverted())
                draw3Frame(p, offset, 0, bar->width() - offset, bar->height(), bar->enabled() ? _style->hSl.fill : _style->hSl.fill_dis);
            else
                draw3Frame(p, 0, 0, offset, bar->height(), bar->enabled() ? _style->hSl.fill : _style->hSl.fill_dis);
        }
    } else // Vertical
    {
        if (!bar->enabled())
            draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->vSl.dis);
        else
            draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->vSl.def, true);

        // fill
        if (bar->value() > bar->minimum())
        {
            int offset = bar->_indicator.y() + defaultParameter(StyleHint::SliderIndicatorHeight) / 2;
            if (bar->inverted())
                draw3Frame(p, 0, 0, bar->width(), offset, bar->enabled() ? _style->vSl.fill : _style->vSl.fill_dis, true);
            else
                draw3Frame(p, 0, offset, bar->width(), bar->height() - offset, bar->enabled() ? _style->vSl.fill : _style->vSl.fill_dis, true);
        }
    }

// control
    if (!bar->enabled())
        p->blitImage(_style->_pack, _style->slI.dis, bar->_indicator.x(), bar->_indicator.y());
    else if (state & PressedState)
        p->blitImage(_style->_pack, _style->slI.pre, bar->_indicator.x(), bar->_indicator.y());
    else if (state & ExposedState)
        p->blitImage(_style->_pack, _style->slI.exp, bar->_indicator.x(), bar->_indicator.y());
    else
        p->blitImage(_style->_pack, _style->slI.def, bar->_indicator.x(), bar->_indicator.y());
    if (state & FocusedState)
        p->blitImage(_style->_pack, _style->slI.foc, bar->_indicator.x(), bar->_indicator.y());
}

void
Stylist::drawSpinBox(Painter* p, SpinBox* box, const Size& size)
{
    const WidgetState state = box->state();

    //Frame
    if (!box->enabled())
        draw9FrameMid(p, size.width(), 0, box->width() - 2 * size.width(), box->height(), _style->li.dis);
    else
        draw9FrameMid(p, size.width(), 0, box->width() - 2 * size.width(), box->height(), _style->li.def);

    if (state & FocusedState)
        draw9FrameMid(p, size.width(), 0, box->width() - 2 * size.width(), box->height(), _style->li.foc);

    if (box->layout().text().size())
    {
        p->setFont(*defaultFont(StyleHint::InputFont));
        p->setBrush(_palette->getGroup(state).baseText);
        p->drawLayout(box->layout());
        p->resetClip();
    }
}

void
Stylist::drawTabPanelButton(Painter* p, TabPanelButton* button, bool first)
{
    const WidgetState state = button->state();

    if (first)
    {
        if (button->checked())
            drawTabFrame(p, 0, 0, button->width(), button->height(), _style->panel.def, _style->panelInv);
        else
            drawTabFramePassive(p, 0, 0, button->width(), button->height(), _style->panel.dis);
    } else
    {
        if (button->checked())
            drawTabFrame(p, 0, 0, button->width(), button->height(), _style->panel.def, _style->panelInv);
        else
            drawTabFramePassive(p, 0, 0, button->width(), button->height(), _style->panel.dis);
    }

    // Text
    if (!button->text().empty())
    {
        p->setFont(*button->font());
        if (state & FocusedState)
            p->setBrush(_palette->focus);
        else
            p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(button->layout());
    }
}

void
Stylist::drawTabPanel(Painter* p, TabPanel* panel, int y, int index)
{
    // TODO use index to draw AllExceptTopLeft
    draw9Frame(p, 0, y, panel->width(), panel->height() - y, _style->panel.def);
}

void
Stylist::drawToolBar(Painter* p, ToolBar* bar)
{
    p->stretchImage(_style->_pack, Rectangle(0, 0, bar->width(), bar->height()), _style->tbar);
}

void
Stylist::drawToolBarButton(Painter* p, ToolBarButton* button)
{
    const WidgetState state = button->state();

    if (!button->enabled())
        draw3Frame(p, 0, 0, button->width(), button->height(), _style->tbarb.dis);
    else if (state & PressedState)
        draw3Frame(p, 0, 0, button->width(), button->height(), _style->tbarb.pre);
    else if (state & ExposedState)
        draw3Frame(p, 0, 0, button->width(), button->height(), _style->tbarb.exp);
    else
        draw3Frame(p, 0, 0, button->width(), button->height(), _style->tbarb.def);

    if (state & FocusedState)
        draw3Frame(p, 0, 0, button->width(), button->height(), _style->tbarb.foc);

    // Draw button text
    if (!button->text().empty())
    {
        p->setFont(*button->font());
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(button->layout(), 0, state & PressedState ? 1 : 0);
    }
}

void
Stylist::drawToolButton(Painter* p, ToolButton* button)
{
    const WidgetState state = button->state();
    int y = 0;
    if (button->drawFrame())
    {
        // Frame
        if (!button->enabled())
            draw9Frame(p, 0, 0, button->width(), button->height(), _style->tb.dis);
        else if (state & PressedState)
        {
            draw9Frame(p, 0, 0, button->width(), button->height(), _style->tb.pre);
            y = 1;
        } else if (state & ExposedState)
            draw9Frame(p, 0, 0, button->width(), button->height(), _style->tb.exp);
        else
            draw9Frame(p, 0, 0, button->width(), button->height(), _style->tb.def);

        if (state & FocusedState)
            draw9Frame(p, 0, 0, button->width(), button->height(), _style->tb.foc);
    }

    // Draw check indicator
    ToolButton::ToolButtonStyle buttonStyle = button->toolButtonStyle();
    if (button->checkable())
    {
        int wIndicator = defaultParameter(StyleHint::ToolButtonIndicator);

        bool horizontal = false;
        if ((buttonStyle == ToolButton::IconBelowText) || (buttonStyle == ToolButton::IconAboveText))
            horizontal = true;

        if (horizontal)
            draw3Frame(p, defaultParameter(StyleHint::ToolButtonLeft), button->height() - defaultParameter(StyleHint::ToolButtonBottom) - wIndicator + y, button->width() - defaultParameter(StyleHint::ToolButtonLR), wIndicator, button->checked() ? _style->tbIndH.def : _style->tbIndH.dis);
        else
            draw3Frame(p, defaultParameter(StyleHint::ToolButtonLeft), defaultParameter(StyleHint::ToolButtonTop) + y, wIndicator, button->height() - defaultParameter(StyleHint::ToolButtonTB), button->checked() ? _style->tbIndV.def : _style->tbIndV.dis, true);

    }

    // Draw button text
    if (buttonStyle != ToolButton::IconOnly && !button->text().empty())
    {
        p->setFont(*button->font());
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(button->layout(), 0, y);
    }
}

void
Stylist::drawFrame(Painter* painter, int x, int y, int w, int h)
{
    draw9Frame(painter, x, y, w, h, _style->fr.def);
}

void
Stylist::drawToolButtonFrame(Painter* p, int x, int y, int w, int h, WidgetState state)
{
    if (state & DisabledState)
        draw9Frame(p, 0, 0, w, h, _style->tb.dis);
    else if (state & PressedState)
    {
        draw9Frame(p, 0, 0, w, h, _style->tb.pre);
        y = 1;
    } else if (state & ExposedState)
        draw9Frame(p, 0, 0, w, h, _style->tb.exp);
    else
        draw9Frame(p, 0, 0, w, h, _style->tb.def);

    if (state & FocusedState)
        draw9Frame(p, 0, 0, w, h, _style->tb.foc);
}

void
Stylist::drawHeader(Painter* p, int x, int y, int w, int h)
{
    draw9CFrame(p, x, y, w, h, _style->db2.pre, _style->db1.pre, NoCorners);
}

void
Stylist::draw3Frame(Painter* p, int x, int y, int w, int h, const Style::r3& rect, bool vertical, const DFBSurfaceBlittingFlags& flags)
{
    if (vertical)
    {
        if (h < rect.r.dfbRect().h)
            return;
        else if (h < rect.l.dfbRect().h + rect.r.dfbRect().h)
            p->blitImage(_style->_pack, rect.r, x, y + h - rect.r.height());
        else
        {
            DFBRectangle blitRects[2];

            blitRects[0] = rect.l.dfbRect();
            blitRects[1] = rect.r.dfbRect();

            DFBPoint blitPoints[2];

            blitPoints[0].x = x;
            blitPoints[0].y = y;

            blitPoints[1].x = x;
            blitPoints[1].y = y + h - rect.r.height();

            p->batchBlitImage(_style->_pack, blitRects, blitPoints, 2, flags);
            p->stretchImage(_style->_pack, Rectangle(x, y + rect.l.height(), w, h - rect.l.height() - rect.r.height()), rect.m, flags);
        }
    } else
    {
        if (w < rect.l.dfbRect().w)
            return;
        if (w <= rect.l.dfbRect().w + rect.r.dfbRect().w)
            p->blitImage(_style->_pack, rect.l, x, y);
        else
        {
            DFBRectangle blitRects[2];

            blitRects[0] = rect.l.dfbRect();
            blitRects[1] = rect.r.dfbRect();

            DFBPoint blitPoints[2];

            blitPoints[0].x = x;
            blitPoints[0].y = y;

            blitPoints[1].x = x + w - rect.r.width();
            blitPoints[1].y = y;

            p->batchBlitImage(_style->_pack, blitRects, blitPoints, 2, flags);
            p->stretchImage(_style->_pack, Rectangle(x + rect.l.width(), y, w - rect.l.width() - rect.r.width(), h), rect.m, flags);
        }
    }
}

void
Stylist::draw9Frame(Painter* p, const Rectangle& region, const Style::r9& rect)
{
    draw9Frame(p, region.x(), region.y(), region.width(), region.height(), rect);
}

void
Stylist::draw9Frame(Painter* p, int x, int y, int w, int h, const Style::r9& rect)
{
    int midWidth = w - rect.tl.width() - rect.tr.width();
    int midHeight = h - rect.bl.height() - rect.tl.height();
    int by = y + h - rect.bl.height();

    DFBRectangle blitRects[4];

    blitRects[0] = rect.tl.dfbRect();
    blitRects[1] = rect.tr.dfbRect();
    blitRects[2] = rect.bl.dfbRect();
    blitRects[3] = rect.br.dfbRect();

    DFBPoint blitPoints[4];

    blitPoints[0].x = x;
    blitPoints[0].y = y;

    blitPoints[1].x = x + w - rect.tr.width();
    blitPoints[1].y = y;

    blitPoints[2].x = x;
    blitPoints[2].y = by;

    blitPoints[3].x = x + w - rect.br.width();
    blitPoints[3].y = by;

    p->batchBlitImage(_style->_pack, blitRects, blitPoints, 4);

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    DFBRectangle stretchDestRects[5];

    stretchDestRects[0].x = x + rect.tl.width();
    stretchDestRects[0].y = y;
    stretchDestRects[0].w = midWidth;
    stretchDestRects[0].h = rect.tm.height();

    stretchDestRects[1].x = x;
    stretchDestRects[1].y = y + rect.tl.height();
    stretchDestRects[1].w = rect.tm.height();
    stretchDestRects[1].h = midHeight;

    stretchDestRects[2].x = x + w - rect.r.width();
    stretchDestRects[2].y = y + rect.tl.height();
    stretchDestRects[2].w = rect.r.width();
    stretchDestRects[2].h = midHeight;

    stretchDestRects[3].x = x + rect.l.width();
    stretchDestRects[3].y = y + rect.tl.height();
    stretchDestRects[3].w = midWidth;
    stretchDestRects[3].h = midHeight;

    stretchDestRects[4].x = x + rect.bl.width();
    stretchDestRects[4].y = by;
    stretchDestRects[4].w = midWidth;
    stretchDestRects[4].h = rect.bm.height();

    DFBRectangle stretchSourceRects[5];
    stretchSourceRects[0] = rect.tm.dfbRect();
    stretchSourceRects[1] = rect.l.dfbRect();
    stretchSourceRects[2] = rect.r.dfbRect();
    stretchSourceRects[3] = rect.m.dfbRect();
    stretchSourceRects[4] = rect.bm.dfbRect();

    p->batchStretchBlitImage(_style->_pack, stretchSourceRects, stretchDestRects, 5);
#else
    p->stretchImage(_style->_pack, Rectangle(x + rect.tl.width(), y, midWidth, rect.tm.height()), rect.tm);
    p->stretchImage(_style->_pack, Rectangle(x, y + rect.tl.height(), rect.tm.height(), midHeight), rect.l);
    p->stretchImage(_style->_pack, Rectangle(x + w - rect.r.width(), y + rect.tl.height(), rect.r.width(), midHeight), rect.r);
    p->stretchImage(_style->_pack, Rectangle(x + rect.l.width(), y + rect.tl.height(), midWidth, midHeight), rect.m);
    p->stretchImage(_style->_pack, Rectangle(x + rect.bl.width(), by, midWidth, rect.bm.height()), rect.bm);
#endif
}

void
Stylist::draw9FrameMid(Painter* p, int x, int y, int w, int h, const Style::r9& rect)
{
    int midHeight = h - rect.bm.height() - rect.tm.height();

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    DFBRectangle stretchDestRects[3];

    stretchDestRects[0].x = x;
    stretchDestRects[0].y = y;
    stretchDestRects[0].w = w;
    stretchDestRects[0].h = rect.tm.height();

    stretchDestRects[1].x = x;
    stretchDestRects[1].y = y + rect.tm.height();
    stretchDestRects[1].w = w;
    stretchDestRects[1].h = midHeight;

    stretchDestRects[2].x = x;
    stretchDestRects[2].y = h - rect.bm.height();
    stretchDestRects[2].w = w;
    stretchDestRects[2].h = rect.bm.height();

    DFBRectangle stretchSourceRects[3];
    stretchSourceRects[0] = rect.tm.dfbRect();
    stretchSourceRects[1] = rect.m.dfbRect();
    stretchSourceRects[2] = rect.bm.dfbRect();

    p->batchStretchBlitImage(_style->_pack, stretchSourceRects, stretchDestRects, 3);
#else
    p->stretchImage(_style->_pack, Rectangle(x, y, w, rect.tm.height()), rect.tm);
    p->stretchImage(_style->_pack, Rectangle(x, y + rect.tm.height(), w, midHeight), rect.m);
    p->stretchImage(_style->_pack, Rectangle(x, h - rect.bm.height(), w, rect.bm.height()), rect.bm);
#endif
}

void
Stylist::draw9CFrame(Painter* p, int x, int y, int w, int h, const Style::r9& rect1, const Style::r9& rect2, Corners corners)
{
    int midWidth = w - rect1.tl.width() - rect1.tr.width();
    int midHeight = h - rect1.bl.height() - rect1.tl.height();
    int by = y + h - rect1.bl.height();

    DFBRectangle blitRects[4];

    if (corners & TopLeft)
        blitRects[0] = rect1.tl.dfbRect();
    else
        blitRects[0] = rect2.tl.dfbRect();

    if (corners & TopRight)
        blitRects[1] = rect1.tr.dfbRect();
    else
        blitRects[1] = rect2.tr.dfbRect();

    if (corners & BottomLeft)
        blitRects[2] = rect1.bl.dfbRect();
    else
        blitRects[2] = rect2.bl.dfbRect();

    if (corners & BottomRight)
        blitRects[3] = rect1.br.dfbRect();
    else
        blitRects[3] = rect2.br.dfbRect();

    DFBPoint blitPoints[4];

    blitPoints[0].x = x;
    blitPoints[0].y = y;

    blitPoints[1].x = x + w - rect1.tr.width();
    blitPoints[1].y = y;

    blitPoints[2].x = x;
    blitPoints[2].y = by;

    blitPoints[3].x = x + w - rect1.br.width();
    blitPoints[3].y = by;

    p->batchBlitImage(_style->_pack, blitRects, blitPoints, 4);

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    DFBRectangle stretchDestRects[5];

    stretchDestRects[0].x = x + rect1.tl.width();
    stretchDestRects[0].y = y;
    stretchDestRects[0].w = midWidth;
    stretchDestRects[0].h = rect1.tm.height();

    stretchDestRects[1].x = x;
    stretchDestRects[1].y = y + rect1.tl.height();
    stretchDestRects[1].w = rect1.tm.height();
    stretchDestRects[1].h = midHeight;

    stretchDestRects[2].x = x + w - rect1.r.width();
    stretchDestRects[2].y = y + rect1.tl.height();
    stretchDestRects[2].w = rect1.r.width();
    stretchDestRects[2].h = midHeight;

    stretchDestRects[3].x = x + rect1.l.width();
    stretchDestRects[3].y = y + rect1.tl.height();
    stretchDestRects[3].w = midWidth;
    stretchDestRects[3].h = midHeight;

    stretchDestRects[4].x = x + rect1.bl.width();
    stretchDestRects[4].y = by;
    stretchDestRects[4].w = midWidth;
    stretchDestRects[4].h = rect1.bm.height();

    DFBRectangle stretchSourceRects[5];
    stretchSourceRects[0] = rect1.tm.dfbRect();
    stretchSourceRects[1] = rect1.l.dfbRect();
    stretchSourceRects[2] = rect1.r.dfbRect();
    stretchSourceRects[3] = rect1.m.dfbRect();
    stretchSourceRects[4] = rect1.bm.dfbRect();

    p->batchStretchBlitImage(_style->_pack, stretchSourceRects, stretchDestRects, 5);
#else
    p->stretchImage(_style->_pack, Rectangle(x + rect1.tl.width(), y, midWidth, rect1.tm.height()), rect1.tm);
    p->stretchImage(_style->_pack, Rectangle(x, y + rect1.tl.height(), rect1.tm.height(), midHeight), rect1.l);
    p->stretchImage(_style->_pack, Rectangle(x + w - rect1.r.width(), y + rect1.tl.height(), rect1.r.width(), midHeight), rect1.r);
    p->stretchImage(_style->_pack, Rectangle(x + rect1.l.width(), y + rect1.tl.height(), midWidth, midHeight), rect1.m);
    p->stretchImage(_style->_pack, Rectangle(x + rect1.bl.width(), by, midWidth, rect1.bm.height()), rect1.bm);
#endif
}

void
Stylist::drawTabFrame(Painter* p, int x, int y, int w, int h, const Style::r9& rect, const Style::r1_View_Panel& rect2)
{
    int midWidth = w - rect.tl.width() - rect.tr.width();
    int midHeight = h - rect.tl.height();
    int by = y + h - rect.bl.height();

    DFBRectangle blitRects[4];

    blitRects[0] = rect.tl.dfbRect();
    blitRects[1] = rect.tr.dfbRect();
    blitRects[2] = rect2.br.dfbRect();
    blitRects[3] = rect2.bl.dfbRect();

    DFBPoint blitPoints[4];

    blitPoints[0].x = x + rect2.br.width() - defaultParameter(StyleHint::PanelInvOverlap);
    blitPoints[0].y = y;

    blitPoints[1].x = x + w - rect.tr.width() - rect2.br.width() + defaultParameter(StyleHint::PanelInvOverlap);
    blitPoints[1].y = y;

    blitPoints[2].x = x;
    blitPoints[2].y = h - rect2.br.height();

    blitPoints[3].x = x + w - rect2.bl.width();
    blitPoints[3].y = h - rect2.bl.height();

    p->batchBlitImage(_style->_pack, blitRects, blitPoints, 4);

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    DFBRectangle stretchDestRects[4];

    stretchDestRects[0].x = x + rect.tl.width() + rect2.br.width() - defaultParameter(StyleHint::PanelInvOverlap);
    stretchDestRects[0].y = y;
    stretchDestRects[0].w = midWidth - rect2.br.width();
    stretchDestRects[0].h = rect.tm.height();

    stretchDestRects[1].x = x + rect2.br.width() - defaultParameter(StyleHint::PanelInvOverlap);
    stretchDestRects[1].y = y + rect.tl.height();
    stretchDestRects[1].w = rect.tm.height();
    stretchDestRects[1].h = midHeight - rect2.br.height();

    stretchDestRects[2].x = x + w - rect.tr.width() - rect2.br.width() + defaultParameter(StyleHint::PanelInvOverlap);
    stretchDestRects[2].y = y + rect.tl.height();
    stretchDestRects[2].w = rect.tm.height();
    stretchDestRects[2].h = midHeight - rect2.br.height();

    stretchDestRects[3].x = x + rect.l.width();
    stretchDestRects[3].y = y + rect.tl.height();
    stretchDestRects[3].w = midWidth;
    stretchDestRects[3].h = midHeight;

    DFBRectangle stretchSourceRects[4];

    stretchSourceRects[0] = rect.tm.dfbRect();
    stretchSourceRects[1] = rect.l.dfbRect();
    stretchSourceRects[2] = rect.r.dfbRect();
    stretchSourceRects[3] = rect.m.dfbRect();

    p->batchStretchBlitImage(_style->_pack, stretchSourceRects, stretchDestRects, 4);
#else
    p->stretchImage(_style->_pack, Rectangle(x + rect.tl.width(), y, midWidth, rect.tm.height()), rect.tm);
    p->stretchImage(_style->_pack, Rectangle(x, y + rect.tl.height(), rect.tm.height(), midHeight), rect.l);
    p->stretchImage(_style->_pack, Rectangle(x + w - rect.tr.width(), y + rect.tl.height(), rect.tm.height(), midHeight), rect.r);
    p->stretchImage(_style->_pack, Rectangle(x + rect.l.width(), y + rect.tl.height(), midWidth, midHeight), rect.m);
#endif
}

void
Stylist::drawTabFramePassive(Painter* p, int x, int y, int w, int h, const Style::r9& rect)
{
    int midWidth = w - rect.tl.width() - rect.tr.width();
    int midHeight = h - rect.tl.height() - defaultParameter(StyleHint::PanelInvOverlap);
    int by = y + h - rect.bl.height();

    DFBRectangle blitRects[2];

    blitRects[0] = rect.tl.dfbRect();
    blitRects[1] = rect.tr.dfbRect();

    DFBPoint blitPoints[2];

    blitPoints[0].x = x;
    blitPoints[0].y = y;

    blitPoints[1].x = x + w - rect.tr.width();
    blitPoints[1].y = y;

    p->batchBlitImage(_style->_pack, blitRects, blitPoints, 2);

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    DFBRectangle stretchDestRects[4];

    stretchDestRects[0].x = x + rect.tl.width();
    stretchDestRects[0].y = y;
    stretchDestRects[0].w = midWidth;
    stretchDestRects[0].h = rect.tm.height();

    stretchDestRects[1].x = x;
    stretchDestRects[1].y = y + rect.tl.height();
    stretchDestRects[1].w = rect.tm.height();
    stretchDestRects[1].h = midHeight;

    stretchDestRects[2].x = x + w - rect.tr.width();
    stretchDestRects[2].y = y + rect.tl.height();
    stretchDestRects[2].w = rect.tm.height();
    stretchDestRects[2].h = midHeight;

    stretchDestRects[3].x = x + rect.l.width();
    stretchDestRects[3].y = y + rect.tl.height();
    stretchDestRects[3].w = midWidth;
    stretchDestRects[3].h = midHeight;

    DFBRectangle stretchSourceRects[4];

    stretchSourceRects[0] = rect.tm.dfbRect();
    stretchSourceRects[1] = rect.l.dfbRect();
    stretchSourceRects[2] = rect.r.dfbRect();
    stretchSourceRects[3] = rect.m.dfbRect();

    p->batchStretchBlitImage(_style->_pack, stretchSourceRects, stretchDestRects, 4);
#else
    p->stretchImage(_style->_pack, Rectangle(x + rect.tl.width(), y, midWidth, rect.tm.height()), rect.tm);
    p->stretchImage(_style->_pack, Rectangle(x, y + rect.tl.height(), rect.tm.height(), midHeight), rect.l);
    p->stretchImage(_style->_pack, Rectangle(x + w - rect.tr.width(), y + rect.tl.height(), rect.tm.height(), midHeight), rect.r);
    p->stretchImage(_style->_pack, Rectangle(x + rect.l.width(), y + rect.tl.height(), midWidth, midHeight), rect.m);
#endif
}

} /* namespace ilixi */
