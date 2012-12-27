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

#include <graphics/Stylist.h>
#include <graphics/Palette.h>
#include <graphics/Style.h>
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
    _palette = new Palette();
    _style = new Style();
}

Stylist::~Stylist()
{
    ILOG_TRACE(ILX_STYLIST);
}

//////////////////////////////////////////////////////////////////////////
// Drawing methods
//////////////////////////////////////////////////////////////////////////

void
Stylist::drawAppFrame(Painter* p, Application* app)
{
    if (app->background())
        p->stretchImage(app->background(), 0, 0, app->width(), app->height());
    else
    {
        p->setBrush(_palette->bg);
        p->fillRectangle(0, 0, app->width(), app->height());
    }
}

void
Stylist::drawDialog(Painter* p, Dialog* dialog)
{
//    int titleHeight = dialog->titleSize().height();

    drawFrame(p, dialog);

    // draw title
    if (dialog->state() & DisabledState)
        p->setBrush(_palette->textDisabled);
    else
        p->setBrush(_palette->text);

    p->setFont(*_style->_titleFont);
    p->drawText(dialog->title(), defaultParameter(StyleHint::FrameOffsetLeft), defaultParameter(StyleHint::FrameOffsetTop));
}

void
Stylist::drawFrame(Painter* p, Frame* frame)
{
    const WidgetState state = frame->state();

    // Frame
    if (state & DisabledState)
        draw9Frame(p, 0, 0, frame->width(), frame->height(), _style->fr.dis);
    else
        draw9Frame(p, 0, 0, frame->width(), frame->height(), _style->fr.def);
}

void
Stylist::drawLabel(Painter* p, Label* label)
{
    if (label->state() & DisabledState)
        p->setBrush(_palette->textDisabled);
    else
        p->setBrush(_palette->text);

    p->setFont(*label->font());
    p->drawLayout(label->layout());
}

void
Stylist::drawListBox(Painter* p, ListBox* box)
{
    // TODO: draw ListBox frame
    p->drawRectangle(0, 0, box->width(), box->height());
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
        if (state & DisabledState)
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
        if (state & DisabledState)
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
        if (state & DisabledState)
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
Stylist::drawLineInput(Painter* p, LineInput* input, bool cursor)
{
    const WidgetState state = input->state();

    // Frame
    if (input->drawFrame())
    {
        if (state & DisabledState)
            draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.dis);
        else if (state & PressedState)
            draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.pre);
        else if (state & ExposedState)
            draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.exp);
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
Stylist::drawGridView(Painter* p, GridView* grid)
{
    // TODO: draw GridView frame
    p->drawRectangle(0, 0, grid->width(), grid->height());
}

void
Stylist::drawGroupBox(Painter* p, GroupBox* box)
{
    const WidgetState state = box->state();

    int tabHeight = box->titleSize().height();

    // Frame
    drawTabFrame(p, defaultParameter(StyleHint::TabOffsetLeft), 0, box->titleSize().width(), tabHeight, _style->box.def);
    draw9Frame(p, 0, tabHeight, box->width(), box->height() - tabHeight, _style->box.def);
}

void
Stylist::drawPushButton(Painter* p, PushButton* button)
{
    const WidgetState state = button->state();

    // Frame
    if (button->getPushButtonStyle() == Default)
    {
        if (state & DisabledState)
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
        if (state & DisabledState)
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
        if (state & DisabledState)
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
        if (state & DisabledState)
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
        if (state & DisabledState)
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

    // Frame
    if (state & DisabledState)
        draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->pr.dis);
    else
        draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->pr.def);

    // Indicator
    if (bar->value())
    {
        int fillWidth = bar->width() * bar->value() / bar->range();
        if (state & DisabledState)
            draw3Frame(p, 0, 0, fillWidth, bar->height(), _style->prI.dis);
        else
            draw3Frame(p, 0, 0, fillWidth, bar->height(), _style->prI.def);
    }
}

void
Stylist::drawScrollArea(Painter* p, ScrollArea* area)
{
    // TODO draw ScrollArea frame.
    p->drawRectangle(0, 0, area->width(), area->height());
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
Stylist::drawSlider(Painter* p, Slider* bar)
{
    const WidgetState state = bar->state();

    if (bar->orientation() == Horizontal)
    {
        draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->hSl.def);

        // fill
        if (bar->value() > bar->minimum())
        {
            if (bar->inverted())
                draw3Frame(p, bar->_indicator.x(), 0, bar->width() - bar->_indicator.x(), bar->height(), _style->hSl.dis);
            else
                draw3Frame(p, 0, 0, bar->_indicator.x() + defaultParameter(StyleHint::SliderIndicatorWidth), bar->height(), _style->hSl.dis);
        }
    } else // Vertical
    {
        draw3Frame(p, 0, 0, bar->width(), bar->height(), _style->vSl.def, true);

        // fill
        if (bar->value() > bar->minimum())
        {
            if (bar->inverted())
                draw3Frame(p, 0, 0, bar->width(), bar->_indicator.y() + defaultParameter(StyleHint::SliderIndicatorHeight), _style->vSl.dis, true);
            else
                draw3Frame(p, 0, bar->_indicator.y(), bar->width(), bar->height() - bar->_indicator.y(), _style->vSl.dis, true);
        }
    }

    // control
    if (state & DisabledState)
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
Stylist::drawSpinBox(Painter* p, SpinBox* box)
{
    const WidgetState state = box->state();

    //Frame
    if (state & DisabledState)
        draw9Frame(p, 0, 0, box->width(), box->height(), _style->li.dis);
    else if (state & PressedState)
        draw9Frame(p, 0, 0, box->width(), box->height(), _style->li.pre);
//    else if (state & ExposedState)
//        draw9Frame(p, 0, 0, box->width(), box->height(), _style->li.exp);
    else
        draw9Frame(p, 0, 0, box->width(), box->height(), _style->li.def);

    if (state & FocusedState)
        draw9Frame(p, 0, 0, box->width(), box->height(), _style->li.foc);

    if (box->layout().text().size())
    {
        p->setFont(*defaultFont(StyleHint::InputFont));
        p->setBrush(_palette->getGroup(state).baseText);
        p->drawLayout(box->layout());
        p->resetClip();
    }
}

void
Stylist::drawTabPanelButton(Painter* p, TabPanelButton* button)
{
    const WidgetState state = button->state();

    if (button->checked())
        drawTabFrame(p, 0, 0, button->width(), button->height(), _style->box.def);
    else
        drawTabFrame(p, 0, 0, button->width(), button->height(), _style->box.dis);

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
Stylist::drawTabPanel(Painter* p, TabPanel* panel, int y)
{
    draw9Frame(p, 0, y, panel->width(), panel->height() - y, _style->box.def);
}

void
Stylist::drawToolButton(Painter* p, ToolButton* button)
{
    const WidgetState state = button->state();

    if (button->drawFrame())
    {
        // Frame
        if (state & DisabledState)
            draw9Frame(p, 0, 0, button->width(), button->height(), _style->tb.dis);
        else if (state & PressedState)
            draw9Frame(p, 0, 0, button->width(), button->height(), _style->tb.pre);
        else if (state & ExposedState)
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
        const WidgetState state = button->state();
        int y;
        if (horizontal)
            y = button->height() - defaultParameter(StyleHint::ToolButtonTop) - wIndicator;
        else
            y = defaultParameter(StyleHint::ToolButtonTop);

        if (button->checked())
            p->setBrush(_palette->getGroup(state).fill);
        else
            p->setBrush(_palette->getGroup(state).bg);

        // draw indicator
        if (horizontal)
            p->fillRectangle(2, y, button->width() - 4, wIndicator);
        else
            p->fillRectangle(defaultParameter(StyleHint::ToolButtonLeft), defaultParameter(StyleHint::ToolButtonTop), wIndicator, button->height() - defaultParameter(StyleHint::ToolButtonTB));
    }

    // Draw button text
    if (buttonStyle != ToolButton::IconOnly && !button->text().empty())
    {
        p->setFont(*button->font());
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(button->layout(), 0, state & PressedState ? 1 : 0);
    }
}

void
Stylist::drawComboBox(Painter* painter, ComboBox* combo)
{
//    const WidgetState state = combo->state();
//
//    // draw frame
//    painter->setBrush(_palette->getGroup(state).base);
//    painter->fillRectangle(0, 0, combo->width(), combo->height());
//    if (state & FocusedState)
//        painter->setPen(_palette->focusBottom);
//    else
//        painter->setPen(_palette->getGroup(state).borderBottom);
//    painter->drawRectangle(0, 0, combo->width(), combo->height());
//
//    // draw button
//    int x = combo->width() - _style->_comboboxButtonWidth - 1;
//    painter->setBrush(_palette->getGroup(state).bgBottom);
//    painter->fillRectangle(x, 1, _style->_comboboxButtonWidth,
//            combo->height() - 2);
//    painter->drawImage(_style->_arrowDown, x, 0, _style->_comboboxButtonWidth,
//            combo->height());
//
//    painter->setBrush(_palette->getGroup(state).baseText);
//    painter->drawLayout(combo->layout());
}

void
Stylist::drawFrame(Painter* painter, int x, int y, int w, int h, Corners corners)
{
    draw9Frame(painter, x, y, w, h, _style->fr.def, corners);
}

void
Stylist::draw3Frame(Painter* p, int x, int y, int w, int h, const Style::r3& rect, bool vertical, const DFBSurfaceBlittingFlags& flags)
{
    if (vertical)
    {
        DFBRectangle blitRects[2];
        blitRects[0] = rect.l.dfbRect();
        blitRects[1] = rect.r.dfbRect();

        DFBPoint blitPoints[2];
        blitPoints[0] =
        {   x,y};
        blitPoints[1] =
        {   x, y + h - rect.r.height()};

        p->batchBlitImage(_style->_pack, blitRects, blitPoints, 2, flags);
        p->stretchImage(_style->_pack, Rectangle(x, y + rect.l.height(), w, h - rect.l.height() - rect.r.height()), rect.m, flags);
    } else
    {
        DFBRectangle blitRects[2];
        blitRects[0] = rect.l.dfbRect();
        blitRects[1] = rect.r.dfbRect();

        DFBPoint blitPoints[2];
        blitPoints[0] =
        {   x,y};
        blitPoints[1] =
        {   x + w - rect.r.width(), y};

        p->batchBlitImage(_style->_pack, blitRects, blitPoints, 2, flags);
        p->stretchImage(_style->_pack, Rectangle(x + rect.l.width(), y, w - rect.l.width() - rect.r.width(), h), rect.m, flags);
    }
}

void
Stylist::draw9Frame(Painter* p, int x, int y, int w, int h, const Style::r9& rect, Corners corners)
{
    int midWidth = w - rect.tl.width() - rect.tr.width();
    int midHeight = h - rect.bl.height() - rect.tl.height();
    int by = y + h - rect.bl.height();

    // TODO implement corners

    DFBRectangle blitRects[4];
    blitRects[0] = rect.tl.dfbRect();
    blitRects[1] = rect.tr.dfbRect();
    blitRects[2] = rect.bl.dfbRect();
    blitRects[3] = rect.br.dfbRect();

    DFBPoint blitPoints[4];
    blitPoints[0] =
    {   x,y};
    blitPoints[1] =
    {   x + w - rect.tr.width(), y};
    blitPoints[2] =
    {   x, by};
    blitPoints[3] =
    {   x + w - rect.br.width(), by};

    p->batchBlitImage(_style->_pack, blitRects, blitPoints, 4);

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    DFBRectangle stretchDestRects[5];
    stretchDestRects[0] =
    {   x + rect.tl.width(), y, midWidth, rect.tm.height()};
    stretchDestRects[1] =
    {   x, y + rect.tl.height(), rect.tm.height(), midHeight};
    stretchDestRects[2] =
    {   x + w - rect.r.width(), y + rect.tl.height(), rect.r.width(), midHeight};
    stretchDestRects[3] =
    {   x + rect.l.width(), y + rect.tl.height(), midWidth, midHeight};
    stretchDestRects[4] =
    {   x + rect.bl.width(), by, midWidth, rect.bm.height()};

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
Stylist::drawTabFrame(Painter* p, int x, int y, int w, int h, const Style::r9& rect)
{
    int midWidth = w - rect.tl.width() - rect.tr.width();
    int midHeight = h - rect.tl.height();
    int by = y + h - rect.bl.height();

    DFBRectangle blitRects[2];
    blitRects[0] = rect.tl.dfbRect();
    blitRects[1] = rect.tr.dfbRect();

    DFBPoint blitPoints[2];
    blitPoints[0] =
    {   x, y};
    blitPoints[1] =
    {   x + w - rect.tr.width(), y};

    p->batchBlitImage(_style->_pack, blitRects, blitPoints, 2);

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    DFBRectangle stretchDestRects[4];
    stretchDestRects[0] =
    {   x + rect.tl.width(), y, midWidth, rect.tm.height()};
    stretchDestRects[1] =
    {   x, y + rect.tl.height(), rect.tm.height(), midHeight};
    stretchDestRects[2] =
    {   x + w - rect.tr.width(), y + rect.tl.height(), rect.tm.height(), midHeight};
    stretchDestRects[3] =
    {   x + rect.l.width(), y + rect.tl.height(), midWidth, midHeight};

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
