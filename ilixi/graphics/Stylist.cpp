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
                (DFBSurfaceBlittingFlags) (DSBLIT_COLORIZE | DSBLIT_BLEND_ALPHACHANNEL));
    } else
        p->stretchImage(icon->image(),
                        Rectangle(0, 0, icon->width(), icon->height()));
}

void
Stylist::drawCheckBox(Painter* p, CheckBox* checkbox)
{
    const WidgetState state = checkbox->state();

    // Frame
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

    // Indicator
    if (checkbox->checked())
        p->blitImage(_style->_pack, _style->check, 2, -2);
    else if (checkbox->partial())
        p->blitImage(_style->_pack, _style->tri_check, 0, 0);

    // Text
    if (!checkbox->text().empty())
    {
        p->setFont(*checkbox->font());
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
        draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.dis);
    else if (state & PressedState)
        draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.pre);
    else if (state & ExposedState)
        draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.exp);
    else
        draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.def);

    if (state & FocusedState)
        draw9Frame(p, 0, 0, input->width(), input->height(), _style->li.foc);

    p->setBrush(Color(0, 0, 0));
    p->setClip(
            _style->li.def.l.width(),
            3,
            input->width() - _style->li.def.l.width() - _style->li.def.r.width(),
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
    drawTabFrame(p, defaultParameter(StyleHint::TabOffsetLeft), 0,
                 box->titleSize().width(), tabHeight, _style->tab.def);
    draw9Frame(p, 0, tabHeight, box->width(), box->height() - tabHeight,
               _style->tab.def);
}

void
Stylist::drawPushButton(Painter* p, PushButton* button)
{
    const WidgetState state = button->state();

    // Frame
    if (state & DisabledState)
    {
        p->blitImage(_style->_pack, _style->pb.dis.l, 0, 0);
        p->stretchImage(
                _style->_pack,
                Rectangle(
                        _style->pb.dis.l.width(),
                        0,
                        button->width() - _style->pb.dis.l.width() - _style->pb.dis.r.width(),
                        button->height()),
                _style->pb.dis.m);
        p->blitImage(_style->_pack, _style->pb.dis.r,
                     button->width() - _style->pb.dis.r.width(), 0);
    } else if ((state & PressedState) || (button->checkable() && button->checked()))
    {
        p->blitImage(_style->_pack, _style->pb.pre.l, 0, 0);
        p->stretchImage(
                _style->_pack,
                Rectangle(
                        _style->pb.pre.l.width(),
                        0,
                        button->width() - _style->pb.pre.l.width() - _style->pb.pre.r.width(),
                        button->height()),
                _style->pb.pre.m);
        p->blitImage(_style->_pack, _style->pb.pre.r,
                     button->width() - _style->pb.pre.r.width(), 0);
    } else if (state & ExposedState)
    {
        p->blitImage(_style->_pack, _style->pb.exp.l, 0, 0);
        p->stretchImage(
                _style->_pack,
                Rectangle(
                        _style->pb.exp.l.width(),
                        0,
                        button->width() - _style->pb.exp.l.width() - _style->pb.exp.r.width(),
                        button->height()),
                _style->pb.exp.m);
        p->blitImage(_style->_pack, _style->pb.exp.r,
                     button->width() - _style->pb.exp.r.width(), 0);
    } else
    {
        p->blitImage(_style->_pack, _style->pb.def.l, 0, 0);
        p->stretchImage(
                _style->_pack,
                Rectangle(
                        _style->pb.def.l.width(),
                        0,
                        button->width() - _style->pb.def.l.width() - _style->pb.def.r.width(),
                        button->height()),
                _style->pb.def.m);
        p->blitImage(_style->_pack, _style->pb.def.r,
                     button->width() - _style->pb.def.r.width(), 0);
    }

    if (state & FocusedState)
    {
        p->blitImage(_style->_pack, _style->pb.foc.l, 0, 0);
        p->stretchImage(
                _style->_pack,
                Rectangle(
                        _style->pb.foc.l.width(),
                        0,
                        button->width() - _style->pb.foc.l.width() - _style->pb.foc.r.width(),
                        button->height()),
                _style->pb.foc.m);
        p->blitImage(_style->_pack, _style->pb.foc.r,
                     button->width() - _style->pb.foc.r.width(), 0);
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
        p->blitImage(_style->_pack, _style->radioOn, 0, 0);
    else
        p->blitImage(_style->_pack, _style->radioOff, 0, 0);

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
    {
        p->blitImage(_style->_pack, _style->pr.dis.l, 0, 0);
        p->stretchImage(
                _style->_pack,
                Rectangle(
                        _style->pr.dis.l.width(),
                        0,
                        bar->width() - _style->pr.dis.l.width() - _style->pr.dis.r.width(),
                        bar->height()),
                _style->pr.dis.m);
        p->blitImage(_style->_pack, _style->pr.dis.r,
                     bar->width() - _style->pr.dis.r.width(), 0);
    } else
    {
        p->blitImage(_style->_pack, _style->pr.def.l, 0, 0);
        p->stretchImage(
                _style->_pack,
                Rectangle(
                        _style->pr.def.l.width(),
                        0,
                        bar->width() - _style->pr.def.l.width() - _style->pr.def.r.width(),
                        bar->height()),
                _style->pr.def.m);
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
            p->stretchImage(
                    _style->_pack,
                    Rectangle(
                            _style->prI.dis.l.width(),
                            0,
                            fillWidth - _style->prI.dis.l.width() - _style->prI.dis.r.width(),
                            bar->height()),
                    _style->prI.dis.m);
            p->blitImage(_style->_pack, _style->prI.dis.r,
                         fillWidth - _style->prI.dis.r.width(), 0);
        } else
        {
            p->blitImage(_style->_pack, _style->prI.def.l, 0, 0);
            p->stretchImage(
                    _style->_pack,
                    Rectangle(
                            _style->prI.def.l.width(),
                            0,
                            fillWidth - _style->pr.def.l.width() - _style->prI.def.r.width(),
                            bar->height()),
                    _style->prI.def.m);
            p->blitImage(_style->_pack, _style->prI.def.r,
                         fillWidth - _style->prI.def.r.width(), 0);
        }
    }
}

void
Stylist::drawSlider(Painter* p, Slider* bar)
{
    const WidgetState state = bar->state();

    if (bar->orientation() == Horizontal)
    {
        p->blitImage(_style->_pack, _style->sl.def.l, 0, 0);
        p->stretchImage(
                _style->_pack,
                Rectangle(
                        _style->sl.def.l.width(),
                        0,
                        bar->width() - _style->sl.def.l.width() - _style->sl.def.r.width(),
                        bar->height()),
                _style->sl.def.m);
        p->blitImage(_style->_pack, _style->sl.def.r,
                     bar->width() - _style->sl.def.r.width(), 0);
        // fill
        if (bar->value() > bar->minimum())
        {
            if (bar->inverted())
            {
                // FIXME not implemented!
            } else
            {
                int fillWidth = bar->_indicator.x() + defaultParameter(
                        StyleHint::SliderIndicatorWidth);
                p->blitImage(_style->_pack, _style->sl.dis.l, 0, 0);
                p->stretchImage(
                        _style->_pack,
                        Rectangle(
                                _style->sl.dis.l.width(),
                                0,
                                fillWidth - _style->sl.dis.l.width() - _style->sl.dis.r.width(),
                                bar->height()),
                        _style->sl.dis.m);
                p->blitImage(_style->_pack, _style->sl.dis.r,
                             fillWidth - _style->sl.dis.r.width(), 0);
            }
        }
    } else // Vertical
    {
        p->blitImage(_style->_pack, _style->sl.defV.l, 0, 0);
        p->stretchImage(
                _style->_pack,
                Rectangle(
                        0,
                        _style->sl.defV.l.height(),
                        bar->width(),
                        bar->height() - _style->sl.defV.l.height() - _style->sl.defV.r.height()),
                _style->sl.defV.m);
        p->blitImage(_style->_pack, _style->sl.defV.r, 0,
                     bar->height() - _style->sl.defV.r.height());

        // fill
        if (bar->value() > bar->minimum())
        {
            if (bar->inverted())
            {
                // FIXME not implemented!
            } else
            {
                int fillHeight = bar->_indicator.y();
                p->blitImage(_style->_pack, _style->sl.disV.l, 0, fillHeight);
                p->stretchImage(
                        _style->_pack,
                        Rectangle(
                                0,
                                fillHeight + defaultParameter(
                                        StyleHint::SliderIndicatorHeight),
                                bar->width(),
                                bar->height() - fillHeight - _style->sl.dis.l.height()),
                        _style->sl.disV.m);
                p->blitImage(_style->_pack, _style->sl.disV.r, 0,
                             bar->height() - _style->sl.disV.r.height());
            }
        }
    }

// control
    if (state & DisabledState)
        p->blitImage(_style->_pack, _style->slI.dis, bar->_indicator.x(),
                     bar->_indicator.y());
    else if (state & PressedState)
        p->blitImage(_style->_pack, _style->slI.pre, bar->_indicator.x(),
                     bar->_indicator.y());
    else if (state & ExposedState)
        p->blitImage(_style->_pack, _style->slI.exp, bar->_indicator.x(),
                     bar->_indicator.y());
    else
        p->blitImage(_style->_pack, _style->slI.def, bar->_indicator.x(),
                     bar->_indicator.y());
    if (state & FocusedState)
        p->blitImage(_style->_pack, _style->slI.foc, bar->_indicator.x(),
                     bar->_indicator.y());

}

void
Stylist::drawTabPanelButton(Painter* p, TabPanelButton* button)
{
    const WidgetState state = button->state();

    if (button->checked())
        drawTabFrame(p, 0, 0, button->width(), button->height(),
                     _style->tab.def);
    else
        drawTabFrame(p, 0, 0, button->width(), button->height(),
                     _style->tab.dis);

    // Text
    if (!button->text().empty())
    {
        p->setFont(*button->font());
        p->setBrush(_palette->getGroup(state).text);
        p->drawLayout(button->layout());
    }
}

void
Stylist::drawTabPanel(Painter* p, TabPanel* panel, int y)
{
    drawFrame(p, 0, y, panel->width(), panel->height() - y);
}

void
Stylist::drawToolButton(Painter* p, ToolButton* button)
{
    const WidgetState state = button->state();

    if (button->drawFrame())
    {
        // Frame
        if (state & DisabledState)
            draw9Frame(p, 0, 0, button->width(), button->height(),
                       _style->tb.dis);
        else if (state & PressedState)
            draw9Frame(p, 0, 0, button->width(), button->height(),
                       _style->tb.pre);
        else if (state & ExposedState)
            draw9Frame(p, 0, 0, button->width(), button->height(),
                       _style->tb.exp);
        else
            draw9Frame(p, 0, 0, button->width(), button->height(),
                       _style->tb.def);

        if (state & FocusedState)
            draw9Frame(p, 0, 0, button->width(), button->height(),
                       _style->tb.foc);
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
            y = button->height() - _borderWidth - wIndicator;
        else
            y = _borderWidth;

        if (button->checked())
            p->setBrush(_palette->getGroup(state).fill);
        else
            p->setBrush(_palette->getGroup(state).bg);

        // draw indicator
        if (horizontal)
            p->fillRectangle(2, y, button->width() - 4, wIndicator);
        else
            p->fillRectangle(
                    defaultParameter(StyleHint::ToolButtonLeft),
                    defaultParameter(StyleHint::ToolButtonTop),
                    wIndicator,
                    button->height() - defaultParameter(
                            StyleHint::ToolButtonTB));
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
Stylist::draw9Frame(Painter* p, int x, int y, int w, int h, const Style::r9& rect, Corners corners)
{
    int midWidth = w - rect.tl.width() - rect.tr.width();
    int midHeight = h - rect.bl.height() - rect.tl.height();
    int by = y + h - rect.bl.height();

    if (corners & TopLeft)
        p->blitImage(_style->_pack, rect.tl, x, y);

    p->blitImage(_style->_pack, rect.tr, x + w - rect.tr.width(), y);
    p->blitImage(_style->_pack, rect.bl, x, by);
    p->blitImage(_style->_pack, rect.br, x + w - rect.br.width(), by);

    p->stretchImage(
            _style->_pack,
            Rectangle(x + rect.tl.width(), y, midWidth, rect.tm.height()),
            rect.tm);
    p->stretchImage(
            _style->_pack,
            Rectangle(x, y + rect.tl.height(), rect.tm.height(), midHeight),
            rect.l);
    p->stretchImage(
            _style->_pack,
            Rectangle(x + w - rect.r.width(), y + rect.tl.height(),
                      rect.r.width(), midHeight),
            rect.r);
    p->stretchImage(
            _style->_pack,
            Rectangle(x + rect.l.width(), y + rect.tl.height(), midWidth,
                      midHeight),
            rect.m);
    p->stretchImage(
            _style->_pack,
            Rectangle(x + rect.bl.width(), by, midWidth, rect.bm.height()),
            rect.bm);
}

void
Stylist::drawTabFrame(Painter* p, int x, int y, int w, int h, const Style::r9& rect)
{
    int midWidth = w - rect.tl.width() - rect.tr.width();
    int midHeight = h - rect.tl.height();
    int by = y + h - rect.bl.height();

    p->blitImage(_style->_pack, rect.tl, x, y);
    p->blitImage(_style->_pack, rect.tr, x + w - rect.tr.width(), y);

    p->stretchImage(
            _style->_pack,
            Rectangle(x + rect.tl.width(), y, midWidth, rect.tm.height()),
            rect.tm);

    p->stretchImage(
            _style->_pack,
            Rectangle(x, y + rect.tl.height(), rect.tm.height(), midHeight),
            rect.l);
    p->stretchImage(
            _style->_pack,
            Rectangle(x + w - rect.tr.width(), y + rect.tl.height(),
                      rect.tm.height(), midHeight),
            rect.r);
    p->stretchImage(
            _style->_pack,
            Rectangle(x + rect.l.width(), y + rect.tl.height(), midWidth,
                      midHeight),
            rect.m);
}

} /* namespace ilixi */
