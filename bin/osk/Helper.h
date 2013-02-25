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

#ifndef ILIXI_OSKHELPER_H_
#define ILIXI_OSKHELPER_H_

#include <ui/ContainerBase.h>
#include <lib/InputHelper.h>

namespace ilixi
{

class HBoxLayout;
class Label;
class ToolButton;

class OSKSegmentRenderer : public Widget
{
public:
    OSKSegmentRenderer(Font* font, InputHelper* helper, Widget* parent = 0);

    virtual
    ~OSKSegmentRenderer();

    Size
    preferredSize() const;

protected:
    void
    compose(const PaintEvent& event);

private:
    Font* _font;
    InputHelper* _helper;
};

/*!
 *
 */
class OSKHelper : public ContainerBase
{
public:
    OSKHelper(Widget* parent = 0);

    virtual
    ~OSKHelper();

    void
    setResizeMode(bool resizeMode);

    void
    handleInput(uint32_t symbol);

protected:
    void
    compose(const PaintEvent& event);

private:
    bool _candidateMode;bool _resizeMode;
    Font* _font;
    InputHelper* _helper;

    OSKSegmentRenderer* _segmentRenderer;

    Label* _text;
    Label* _candidate;
    ToolButton* _accept;

    void
    updateText();

    void
    acceptInput();
};

} /* namespace ilixi */
#endif /* ILIXI_OSKHELPER_H_ */
