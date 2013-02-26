/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>, Andreas Shimokawa <andi@directfb.org>.

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

OSKSegmentRenderer::OSKSegmentRenderer(Font* font, InputHelper* helper, OSKHelper* parent)
        : Widget(parent),
          _owner(parent),
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
    Painter p(this);
    p.begin(event);
    p.setFont(*_font);
    if (_owner->_mode == OSKHelper::Process)
    {
        p.setBrush(Color(255, 255, 255));
        p.drawText(_helper->getPdata(), 0, 0);
    } else
    {
        int x = 0;
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
    }
    p.end();
}

//********************************************************************************************************

OSKHelper::OSKHelper(Widget* parent)
        : ContainerBase(parent),
          _mode(Process),
          _resizeMode(false)
{
    ILOG_TRACE_W(ILX_OSKHelper);
    setVisible(false);
    setConstraints(ExpandingConstraint, FixedConstraint);
    setLayout(new VBoxLayout());

    _font = new Font("Sazanami Gothic", 14);

    _helper = new InputHelperJP();
    _helper->sigUpdateUI.connect(sigc::mem_fun(this, &OSKHelper::updateUI));

    _text = new Label(">");
    _text->setFont(_font);
    _text->setXConstraint(ExpandingConstraint);
    addWidget(_text);

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
        if (_mode == Process)
        {
            _helper->process();
            if (_helper->segments())
                _mode = Segment;
        } else if (_mode == Segment)
        {
            _helper->generateCandidates();
            if (_helper->canditates())
            {
                _helper->updateCurrentSegment();
                _mode = Candidate;
            }
        } else
        {
            _helper->getNextCandidate();
            _helper->updateCurrentSegment();
        }
        break;

    case DIKS_CURSOR_UP:
        if (_mode == Candidate)
        {
            _helper->getPreviousCandidate();
            _helper->updateCurrentSegment();
        }
        break;

    case DIKS_CURSOR_DOWN:
        if (_mode == Candidate)
        {
            _helper->getNextCandidate();
            _helper->updateCurrentSegment();
        }
        break;

    case DIKS_CURSOR_LEFT:
        ILOG_DEBUG(ILX_OSKHelper, " -> DIKS_CURSOR_LEFT\n");
        if (_resizeMode)
        {
            _helper->resizeSegment(-1);
        } else
        {
            _mode = Segment;
            _helper->getPreviousSegment();
        }
        break;

    case DIKS_CURSOR_RIGHT:
        ILOG_DEBUG(ILX_OSKHelper, " -> DIKS_CURSOR_RIGHT\n");
        if (_resizeMode)
        {
            _helper->resizeSegment(1);
        } else
        {
            _mode = Segment;
            _helper->getNextSegment();
        }
        break;

    case DIKS_ENTER:
        ILOG_DEBUG(ILX_OSKHelper, " -> DIKS_ENTER\n");
        if (_mode == Process)
        {
            sigSubmit(_helper->getPdata());
        } else
        {
            ILOG_DEBUG(ILX_OSKHelper, " -> submit: %s\n", _helper->concatedSegments().c_str());
            sigSubmit(_helper->concatedSegments());
            _text->setText("> ");
            _helper->reset();
            _mode = Process;
        }
        break;

    default:
        _mode = Process;
        _helper->append(symbol);
        _text->setText("> " + _helper->getData());
        break;
    }
    update();
}

void
OSKHelper::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_OSKHelper);
}

void
OSKHelper::updateUI()
{
    ILOG_TRACE_W(ILX_OSKHelper);
    update();
}

void
OSKHelper::acceptInput()
{
}

} /* namespace ilixi */
