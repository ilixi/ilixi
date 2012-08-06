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
    _borderWidth = 1;
    _palette = new Palette();
    _style = new Style();
    setPaletteFromFile(ILIXI_DATADIR"def_palette.xml");
    setStyleFromFile(ILIXI_DATADIR"def_style.xml");
}

Stylist::~Stylist()
{
    delete _palette;
    delete _style;
}

//////////////////////////////////////////////////////////////////////////
// Drawing methods
//////////////////////////////////////////////////////////////////////////

void
Stylist::drawAppFrame(Painter* p, Application* app)
{
    if (app->background())
        p->drawImage(app->background(), 0, 0, app->width(), app->height());
    else
    {
        p->setBrush(_palette->bgBottom);
        p->fillRectangle(0, 0, app->width(), app->height());
    }
}

void
Stylist::drawDialog(Painter* p, Dialog* dialog)
{
//    int titleHeight = dialog->titleSize().height();
//
    // draw frame
    drawFrame(p, dialog);

    // draw title
    if (dialog->state() & DisabledState)
        p->setBrush(_palette->textDisabled);
    else
        p->setBrush(_palette->text);

    p->setFont(*_style->_titleFont);
    p->drawText(dialog->title(), defaultParameter(StyleHint::FrameOffsetLeft),
                defaultParameter(StyleHint::FrameOffsetTop));
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
Stylist::drawIcon(Painter* p, Icon* icon)
{
    if (icon->parent() && (icon->parent()->state() & PressedState))
    {
        p->setBrush(Color(128, 128, 128));
        p->drawImage(
                icon->image(),
                0,
                0,
                (DFBSurfaceBlittingFlags) (DSBLIT_COLORIZE
                        | DSBLIT_BLEND_ALPHACHANNEL));
    } else
        p->drawImage(icon->image(),
                     Rectangle(0, 0, icon->width(), icon->height()));
}

void
Stylist::drawCheckBox(Painter* p, CheckBox* checkbox)
{
    const WidgetState state = checkbox->state();

    // Frame
    if (state & DisabledState)
        p->blitImage(_style->_pack, _style->cb.dis, 0, 0);
    else if ((state & PressedState)
            || (checkbox->checkable() && checkbox->checked()))
        p->blitImage(_style->_pack, _style->cb.pre, 0, 0);
    else if (state & ExposedState)
        p->blitImage(_style->_pack, _style->cb.exp, 0, 0);
    else
        p->blitImage(_style->_pack, _style->cb.def, 0, 0);

    if (state & FocusedState)
        p->blitImage(_style->_pack, _style->cb.foc, 0, 0);

    // Indicator
    if (checkbox->checked())
        p->blitImage(_style->_pack, _style->check, 2, -2);
    else if (checkbox->partial())
        p->blitImage(_style->_pack, _style->tri_check, 0, 0);

    // Text
    if (!checkbox->text().empty())
    {
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(checkbox->layout());
    }
}

void
Stylist::drawLineInput(Painter* p, LineInput* input)
{
    const WidgetState state = input->state();

    // Frame
    if (state & DisabledState)
    {
        p->blitImage(_style->_pack, _style->li.dis.l, 0, 0);
        p->setClip(
                _style->li.dis.l.width(),
                0,
                input->width() - _style->li.dis.l.width()
                        - _style->li.dis.r.width(),
                input->height());
        p->tileImage(_style->_pack, 0, 0, _style->li.dis.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->li.dis.r,
                     input->width() - _style->li.dis.r.width(), 0);
    } else if (state & PressedState)
    {
        p->blitImage(_style->_pack, _style->li.pre.l, 0, 0);
        p->setClip(
                _style->li.pre.l.width(),
                0,
                input->width() - _style->li.pre.l.width()
                        - _style->li.pre.r.width(),
                input->height());
        p->tileImage(_style->_pack, 0, 0, _style->li.pre.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->li.pre.r,
                     input->width() - _style->li.pre.r.width(), 0);
    } else if (state & ExposedState)
    {
        p->blitImage(_style->_pack, _style->li.exp.l, 0, 0);
        p->setClip(
                _style->li.exp.l.width(),
                0,
                input->width() - _style->li.exp.l.width()
                        - _style->li.exp.r.width(),
                input->height());
        p->tileImage(_style->_pack, 0, 0, _style->li.exp.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->li.exp.r,
                     input->width() - _style->li.exp.r.width(), 0);
    } else
    {
        p->blitImage(_style->_pack, _style->li.def.l, 0, 0);
        p->setClip(
                _style->li.def.l.width(),
                0,
                input->width() - _style->li.def.l.width()
                        - _style->li.def.r.width(),
                input->height());
        p->tileImage(_style->_pack, 0, 0, _style->li.def.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->li.def.r,
                     input->width() - _style->li.def.r.width(), 0);
    }

    if (state & FocusedState)
    {
        p->blitImage(_style->_pack, _style->li.foc.l, 0, 0);
        p->setClip(
                _style->li.foc.l.width(),
                0,
                input->width() - _style->li.foc.l.width()
                        - _style->li.foc.r.width(),
                input->height());
        p->tileImage(_style->_pack, 0, 0, _style->li.foc.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->li.foc.r,
                     input->width() - _style->li.foc.r.width(), 0);
    }

    p->setBrush(Color(0, 0, 0));
    p->setClip(
            _style->li.def.l.width(),
            3,
            input->width() - _style->li.def.l.width()
                    - _style->li.def.r.width(),
            input->height() - 6);
    p->drawLayout(input->layout());
    p->resetClip();

    if (input->_cursorOn)
    {
        p->setBrush(Color(0, 0, 255));
        p->fillRectangle(input->_cursor);
    }
}

void
Stylist::drawGroupBox(Painter* p, GroupBox* box)
{
    const WidgetState state = box->state();

    int tabHeight = box->titleSize().height();

    // Frame
    drawTabFrame(p, 30, 0, box->titleSize().width(), tabHeight,
                 _style->tab.def);
    draw9Frame(p, 0, tabHeight, box->width(), box->height() - tabHeight,
               _style->fr.def);
}

void
Stylist::drawPushButton(Painter* p, PushButton* button)
{
    const WidgetState state = button->state();

    // Frame
    if (state & DisabledState)
    {
        p->blitImage(_style->_pack, _style->pb.dis.l, 0, 0);
        p->setClip(
                _style->pb.dis.l.width(),
                0,
                button->width() - _style->pb.dis.l.width()
                        - _style->pb.dis.r.width(),
                button->height());
        p->tileImage(_style->_pack, 0, 0, _style->pb.dis.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->pb.dis.r,
                     button->width() - _style->pb.dis.r.width(), 0);
    } else if ((state & PressedState)
            || (button->checkable() && button->checked()))
    {
        p->blitImage(_style->_pack, _style->pb.pre.l, 0, 0);
        p->setClip(
                _style->pb.pre.l.width(),
                0,
                button->width() - _style->pb.pre.l.width()
                        - _style->pb.pre.r.width(),
                button->height());
        p->tileImage(_style->_pack, 0, 0, _style->pb.pre.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->pb.pre.r,
                     button->width() - _style->pb.pre.r.width(), 0);
    } else if (state & ExposedState)
    {
        p->blitImage(_style->_pack, _style->pb.exp.l, 0, 0);
        p->setClip(
                _style->pb.exp.l.width(),
                0,
                button->width() - _style->pb.exp.l.width()
                        - _style->pb.exp.r.width(),
                button->height());
        p->tileImage(_style->_pack, 0, 0, _style->pb.exp.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->pb.exp.r,
                     button->width() - _style->pb.exp.r.width(), 0);
    } else
    {
        p->blitImage(_style->_pack, _style->pb.def.l, 0, 0);
        p->setClip(
                _style->pb.def.l.width(),
                0,
                button->width() - _style->pb.def.l.width()
                        - _style->pb.def.r.width(),
                button->height());
        p->tileImage(_style->_pack, 0, 0, _style->pb.def.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->pb.def.r,
                     button->width() - _style->pb.def.r.width(), 0);
    }

    if (state & FocusedState)
    {
        p->blitImage(_style->_pack, _style->pb.foc.l, 0, 0);
        p->setClip(
                _style->pb.foc.l.width(),
                0,
                button->width() - _style->pb.foc.l.width()
                        - _style->pb.foc.r.width(),
                button->height());
        p->tileImage(_style->_pack, 0, 0, _style->pb.foc.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->pb.foc.r,
                     button->width() - _style->pb.foc.r.width(), 0);
    }

    // Text
    if (!button->text().empty())
    {
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(button->layout());
    }
}

void
Stylist::drawRadioButton(Painter* p, RadioButton* button)
{
    const WidgetState state = button->state();

    // Frame
    if (state & DisabledState)
        p->blitImage(_style->_pack, _style->rb.dis, 0, 0);
    else if (state & PressedState)
        p->blitImage(_style->_pack, _style->rb.pre, 0, 0);
    else if (state & ExposedState)
        p->blitImage(_style->_pack, _style->rb.exp, 0, 0);
    else
        p->blitImage(_style->_pack, _style->rb.def, 0, 0);

    if (state & FocusedState)
        p->blitImage(_style->_pack, _style->rb.foc, 0, 0);

    // Indicator
    if (button->checked())
        p->blitImage(_style->_pack, _style->radioOn, 3, 3);
    else
        p->blitImage(_style->_pack, _style->radioOff, 3, 3);

    // Text
    if (!button->text().empty())
    {
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
    {
        p->blitImage(_style->_pack, _style->pr.dis.l, 0, 0);
        p->setClip(
                _style->pr.dis.l.width(),
                0,
                bar->width() - _style->pr.dis.l.width()
                        - _style->pr.dis.r.width(),
                bar->height());
        p->tileImage(_style->_pack, 0, 0, _style->pr.dis.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->pr.dis.r,
                     bar->width() - _style->pr.dis.r.width(), 0);
    } else
    {
        p->blitImage(_style->_pack, _style->pr.def.l, 0, 0);
        p->setClip(
                _style->pr.def.l.width(),
                0,
                bar->width() - _style->pr.def.l.width()
                        - _style->pr.def.r.width(),
                bar->height());
        p->tileImage(_style->_pack, 0, 0, _style->pr.def.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->pr.def.r,
                     bar->width() - _style->pr.def.r.width(), 0);
    }

    // Indicator
    if (bar->value())
    {
        int fillWidth = bar->width() * bar->value() / bar->range();
        if (state & DisabledState)
        {
            p->blitImage(_style->_pack, _style->prI.dis.l, 0, 0);
            p->setClip(
                    _style->prI.dis.l.width(),
                    0,
                    fillWidth - _style->prI.dis.l.width()
                            - _style->prI.dis.r.width(),
                    bar->height());
            p->tileImage(_style->_pack, 0, 0, _style->prI.dis.m);
            p->resetClip();
            p->blitImage(_style->_pack, _style->prI.dis.r,
                         fillWidth - _style->prI.dis.r.width(), 0);
        } else
        {
            p->blitImage(_style->_pack, _style->prI.def.l, 0, 0);
            p->setClip(
                    _style->prI.def.l.width(),
                    0,
                    fillWidth - _style->pr.def.l.width()
                            - _style->prI.def.r.width(),
                    bar->height());
            p->tileImage(_style->_pack, 0, 0, _style->prI.def.m);
            p->resetClip();
            p->blitImage(_style->_pack, _style->prI.def.r,
                         fillWidth - _style->prI.def.r.width(), 0);
        }
    }
}

void
Stylist::drawSlider(Painter* p, Slider* bar)
{
    const WidgetState state = bar->state();
    {
        p->blitImage(_style->_pack, _style->sl.def.l, 0, 0);
        p->setClip(
                _style->sl.def.l.width(),
                0,
                bar->width() - _style->sl.def.l.width()
                        - _style->sl.def.r.width(),
                bar->height());
        p->tileImage(_style->_pack, 0, 0, _style->sl.def.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->sl.def.r,
                     bar->width() - _style->sl.def.r.width(), 0);
    }

    // Frame
    if (bar->value())
    {
        int fillWidth = bar->value() * bar->width() / bar->range();
        p->blitImage(_style->_pack, _style->sl.dis.l, 0, 0);
        p->setClip(
                _style->sl.dis.l.width(), 0,
                fillWidth - _style->sl.dis.l.width() - _style->sl.dis.r.width(),
                bar->height());
        p->tileImage(_style->_pack, 0, 0, _style->sl.dis.m);
        p->resetClip();
        p->blitImage(_style->_pack, _style->sl.dis.r,
                     fillWidth - _style->sl.dis.r.width(), 0);
    }

    // Indicator
    if (state & DisabledState)
        p->blitImage(_style->_pack, _style->slI.dis, bar->_indicator.x(), 0);
    else if (state & PressedState)
        p->blitImage(_style->_pack, _style->slI.pre, bar->_indicator.x(), 0);
    else if (state & ExposedState)
        p->blitImage(_style->_pack, _style->slI.exp, bar->_indicator.x(), 0);
    else
        p->blitImage(_style->_pack, _style->slI.def, bar->_indicator.x(), 0);
    if (state & FocusedState)
        p->blitImage(_style->_pack, _style->slI.foc, bar->_indicator.x(), 0);

}

void
Stylist::drawTabPanelButton(Painter* painter, TabPanelButton* button)
{

}

void
Stylist::drawTabPanel(Painter* painter, TabPanel* panel)
{

}

void
Stylist::drawToolButton(Painter* p, ToolButton* button)
{
    const WidgetState state = button->state();

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

    // Draw check indicator
    ToolButton::ToolButtonStyle buttonStyle = button->toolButtonStyle();
    if (button->checkable())
    {
        int wIndicator = defaultParameter(StyleHint::ToolButtonIndicator);
        bool horizontal = false;
        if ((buttonStyle == ToolButton::IconBelowText)
                || (buttonStyle == ToolButton::IconAboveText))
            horizontal = true;
        const WidgetState state = button->state();
        int y;
        if (horizontal)
            y = button->height() - _borderWidth - wIndicator;
        else
            y = _borderWidth;

        if (button->checked())
            p->setBrush(_palette->getGroup(state).fillBottom);
        else
            p->setBrush(_palette->getGroup(state).bgBottom);

        // draw indicator
        if (horizontal)
            p->fillRectangle(2, y, button->width() - 4, wIndicator);
        else
            p->fillRectangle(2, _borderWidth, wIndicator, button->height() - 2);
    }

    // Draw button text
    if (buttonStyle != ToolButton::IconOnly && !button->text().empty())
    {
        ILOG_DEBUG(ILX_STYLIST, " -> text: %s\n", button->text().c_str());
        p->setFont(*button->font());
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(button->layout());
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
Stylist::drawFrame(Painter* painter, int x, int y, int w, int h,
                   Corners corners)
{
    draw9Frame(painter, x, y, w, h, _style->fr.def, corners);
}

void
Stylist::draw9Frame(Painter* p, int x, int y, int w, int h,
                    const Style::r9& rect, Corners corners)
{
    int midWidth = w - rect.tl.width() - rect.tr.width();
    int midHeight = h - rect.bl.height() - rect.tl.height();
    int by = y + h - rect.bl.height();

//    if (corners & TopLeft)
//    {
//        // top
//        p->blitImage(_style->_pack, rect.tl, x, y);
//        p->setClip(x + rect.tl.width(), y, midWidth, rect.tm.height());
//        p->tileImage(_style->_pack, x + rect.tl.width(), y, rect.tm);
//        p->resetClip();
//        p->blitImage(_style->_pack, rect.tr, x + w - rect.tr.width(), y);
//    }

    // top
    p->blitImage(_style->_pack, rect.tl, x, y);
    p->setClip(x + rect.tl.width(), y, midWidth, rect.tm.height());
    p->tileImage(_style->_pack, x + rect.tl.width(), y, rect.tm);
    p->resetClip();
    p->blitImage(_style->_pack, rect.tr, x + w - rect.tr.width(), y);

    // left
    p->setClip(x, y + rect.tl.height(), rect.tm.height(), midHeight);
    p->tileImage(_style->_pack, x, y + rect.tl.height(), rect.l);
    p->resetClip();

    // right
    p->setClip(x + w - rect.tr.width(), y + rect.tl.height(), rect.tm.height(),
               midHeight);
    p->tileImage(_style->_pack, x + w - rect.tr.width(), y + rect.tl.height(),
                 rect.r);
    p->resetClip();

    // mid
    p->setClip(x + rect.l.width(), y + rect.tl.height(), midWidth, midHeight);
    p->tileImage(_style->_pack, x + rect.l.width(), y + rect.tl.height(),
                 rect.m);
    p->resetClip();

    // bottom
    p->blitImage(_style->_pack, rect.bl, x, by);
    p->setClip(x + rect.bl.width(), by, midWidth, rect.bm.height());
    p->tileImage(_style->_pack, x + rect.bl.width(), by, rect.bm);
    p->resetClip();
    p->blitImage(_style->_pack, rect.br, x + w - rect.br.width(), by);
}

void
Stylist::drawTabFrame(Painter* p, int x, int y, int w, int h,
                      const Style::r9& rect)
{
    int midWidth = w - rect.tl.width() - rect.tr.width();
    int midHeight = h - rect.tl.height();
    int by = y + h - rect.bl.height();

    // top
    p->blitImage(_style->_pack, rect.tl, x, y);
    p->setClip(x + rect.tl.width(), y, midWidth, rect.tm.height());
    p->tileImage(_style->_pack, x + rect.tl.width(), y, rect.tm);
    p->resetClip();
    p->blitImage(_style->_pack, rect.tr, x + w - rect.tr.width(), y);

    // left
    p->setClip(x, y + rect.tl.height(), rect.tm.height(), midHeight);
    p->tileImage(_style->_pack, x, y + rect.tl.height(), rect.l);
    p->resetClip();

    // right
    p->setClip(x + w - rect.tr.width(), y + rect.tl.height(), rect.tm.height(),
               midHeight);
    p->tileImage(_style->_pack, x + w - rect.tr.width(), y + rect.tl.height(),
                 rect.r);
    p->resetClip();

    // mid
    p->setClip(x + rect.l.width(), y + rect.tl.height(), midWidth, midHeight);
    p->tileImage(_style->_pack, x + rect.l.width(), y + rect.tl.height(),
                 rect.m);
    p->resetClip();
}

} /* namespace ilixi */
