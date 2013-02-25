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

#include "Helper.h"
#include <ui/VBoxLayout.h>
#include <ui/Label.h>
#include <ui/Spacer.h>
#include <ui/ToolButton.h>
#include <lib/InputHelperJP.h>
#include <core/Logger.h>
#include <graphics/Painter.h>
#include "utf8-decoder.h"

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_OSKHelper, "ilixi/osk/Helper", "Helper");
D_DEBUG_DOMAIN( ILX_OSKSegment, "ilixi/osk/SegmentRenderer", "SegmentRenderer");

OSKSegmentRenderer::OSKSegmentRenderer(Font* font, InputHelper* helper, Widget* parent)
        : Widget(parent),
          _font(font),
          _helper(helper)
{
    ILOG_TRACE_W(ILX_OSKSegment);
    setConstraints(MinimumConstraint, FixedConstraint);
}

OSKSegmentRenderer::~OSKSegmentRenderer()
{
    ILOG_TRACE_W(ILX_OSKSegment);
}

Size
OSKSegmentRenderer::preferredSize() const
{
    ILOG_TRACE_W(ILX_OSKSegment);
    if (_helper->segments())
    {
        Size s = _font->extents(_helper->concatedSegments());
        return Size(s.width(), s.height() + 3);
    }
    return Size(100, _font->extents("ん").height() + 3);
}

void
OSKSegmentRenderer::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_OSKSegment);
    int x = 0;
    Painter p(this);
    p.begin(event);
    p.setFont(*_font);
    for (unsigned int i = 0; i < _helper->segments(); i++)
    {
        Size s = p.textExtents(_helper->getSegment(i));
        if (i == _helper->getCurrentSegment())
        {
            p.setBrush(Color(255, 20, 50));
            p.fillRectangle(x, 0, s.width(), s.height());
        }
        p.setBrush(Color(255, 255, 255));
        p.drawText(_helper->getSegment(i), x, 0);
        p.fillRectangle(x + 1, s.height() + 1, s.width() - 2, 2);
        x += s.width();
    }
    p.end();
}

//********************************************************************************************************

OSKHelper::OSKHelper(Widget* parent)
        : ContainerBase(parent),
          _candidateMode(false),
          _resizeMode(false)
{
    ILOG_TRACE_W(ILX_OSKHelper);
    setVisible(false);
    setConstraints(ExpandingConstraint, FixedConstraint);
    setLayout(new VBoxLayout());

    _font = new Font("Sazanami Gothic", 14);

    _helper = new InputHelperJP();
    _helper->sigUpdateUI.connect(sigc::mem_fun(this, &OSKHelper::updateText));

    _text = new Label(">");
    _text->setFont(_font);
    _text->setXConstraint(ExpandingConstraint);
    addWidget(_text);
//
//    _candidate = new Label("");
//    _candidate->setFont(_font);
//    addWidget(_candidate);
//
//    _accept = new ToolButton("");
//    _accept->setIcon(StyleHint::Tick, Size(32, 32));
//    addWidget(_accept);
//    _accept->sigClicked.connect(sigc::mem_fun(this, &OSKHelper::acceptInput));

    _segmentRenderer = new OSKSegmentRenderer(_font, _helper, this);
    addWidget(_segmentRenderer);
}

OSKHelper::~OSKHelper()
{
    ILOG_TRACE_F(ILX_OSKHelper);
    delete _font;
    delete _helper;
}

void
OSKHelper::setResizeMode(bool resizeMode)
{
    _resizeMode = resizeMode;
}

void
OSKHelper::handleInput(uint32_t symbol)
{
    ILOG_TRACE_W(ILX_OSKHelper);
    switch (symbol)
    {
    case DIKS_SPACE:
        ILOG_DEBUG(ILX_OSKHelper, " -> DIKS_SPACE\n");
        if (_candidateMode)
            _helper->getNextCandidate();
        else
        {
            // andi:
            // there should be one more mode.
            // when pressing space the first time only process() should be called.
            // when pressed on some segment afterwards generateCandidates() should be called.
            // we could make _candidateMode and enum and rename it to allow 3 states
            _helper->process();
            _helper->generateCandidates();
            _candidateMode = true;
        }
        _helper->updateCurrentSegment();
        update();
        break;

    case DIKS_CURSOR_UP:
        if (_candidateMode)
        {
            _helper->getPreviousCandidate();
            _helper->updateCurrentSegment();
            update();
        }
        break;

    case DIKS_CURSOR_DOWN:
        if (_candidateMode)
        {
            _helper->getNextCandidate();
            _helper->updateCurrentSegment();
            update();
        }
        break;

    case DIKS_CURSOR_LEFT:
        ILOG_DEBUG(ILX_OSKHelper, " -> DIKS_CURSOR_LEFT\n");
        if (_resizeMode)
        {
            _helper->resizeSegment(-1);
            update();
        } else
        {
            _candidateMode = false;
            _helper->getPreviousSegment();
        }
        break;

    case DIKS_CURSOR_RIGHT:
        ILOG_DEBUG(ILX_OSKHelper, " -> DIKS_CURSOR_RIGHT\n");
        if (_resizeMode)
        {
            _helper->resizeSegment(1);
            update();
        } else
        {
            _candidateMode = false;
            _helper->getNextSegment();
        }
        break;

//    case DIKS_BACKSPACE:
//        ILOG_DEBUG(ILX_OSKHelper, " -> DIKS_BACKSPACE\n");
//        break;

    case DIKS_ENTER:
        ILOG_DEBUG(ILX_OSKHelper, " -> DIKS_ENTER\n");
//        _helper->process();
        break;

    default:
        _helper->append(symbol);
        _text->setText("> " + _helper->getData());
        update();
        break;
    }
}

void
OSKHelper::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_OSKHelper);
}

void
OSKHelper::updateText()
{
    ILOG_TRACE_W(ILX_OSKHelper);
    update();
}

void
OSKHelper::acceptInput()
{
}

} /* namespace ilixi */
