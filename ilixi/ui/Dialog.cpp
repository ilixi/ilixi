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

#include <ui/Dialog.h>
#include <ui/ToolButton.h>
#include <ui/Spacer.h>
#include <graphics/Painter.h>
#include <sigc++/bind.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_DIALOG, "ilixi/ui/Dialog", "Dialog");

Dialog::Dialog(const std::string& title, ButtonOption option, Widget* parent)
        : WindowWidget(),
          _buttonLayout(NULL),
          _result(-1)
{
    ILOG_TRACE_W(ILX_DIALOG);
    setTitle(title);
    setButtonLayoutOption(option);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &Dialog::updateButtonLayoutGeometry));
    sigAbort.connect(sigc::mem_fun(this, &Dialog::reject));
}

Dialog::~Dialog()
{
    ILOG_TRACE_W(ILX_DIALOG);
    closeWindow();
}

int
Dialog::heightForWidth(int width) const
{
    ILOG_TRACE_W(ILX_DIALOG);
    return _layout->heightForWidth(width - _margin.hSum() - stylist()->defaultParameter(StyleHint::FrameOffsetLR)) + _margin.vSum() + stylist()->defaultParameter(StyleHint::FrameOffsetTB) + _titleSize.height() + _buttonLayout->preferredSize().height();
}

Size
Dialog::preferredSize() const
{
    ILOG_TRACE_W(ILX_DIALOG);
    Size layoutSize = _layout->preferredSize();
    Size buttonLayoutSize = _buttonLayout->preferredSize();

    Size s(std::max(layoutSize.width() + _margin.hSum(), std::max(_titleSize.width(), buttonLayoutSize.width())) + stylist()->defaultParameter(StyleHint::FrameOffsetLR),
            layoutSize.height() + _margin.vSum() + _titleSize.height() + buttonLayoutSize.height() + 2 * spacing() + stylist()->defaultParameter(StyleHint::FrameOffsetTB));
    ILOG_DEBUG(ILX_DIALOG, " -> size: %dx%d\n", s.width(), s.height());
    return s;
}

void
Dialog::accept()
{
    ILOG_TRACE_W(ILX_DIALOG);
    sigAccepted();
    finish(1);
}

void
Dialog::reject()
{
    ILOG_TRACE_W(ILX_DIALOG);
    sigRejected();
    finish(0);
}

void
Dialog::finish(int result)
{
    ILOG_TRACE_W(ILX_DIALOG);
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
    ILOG_TRACE_W(ILX_DIALOG);
    setVisible(true);
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
    } else if (option == CancelButtonOption)
    {
        button = new ToolButton("Cancel");
        _buttonLayout->addWidget(button);
        button->sigClicked.connect(sigc::mem_fun(this, &Dialog::reject));
    } else if (option == OKCancelButtonOption)
    {
        button = new ToolButton("OK");
        _buttonLayout->addWidget(button);
        button->sigClicked.connect(sigc::mem_fun(this, &Dialog::accept));

        button = new ToolButton("Cancel");
        _buttonLayout->addWidget(button);
        button->sigClicked.connect(sigc::mem_fun(this, &Dialog::reject));
    } else
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
            button->sigClicked.connect(sigc::bind<int>(sigc::mem_fun(this, &Dialog::finish), -1));
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
    return _margin.top() + stylist()->defaultParameter(StyleHint::FrameOffsetTop) + _titleSize.height();
}

int
Dialog::canvasHeight() const
{
    return height() - (_margin.vSum() + stylist()->defaultParameter(StyleHint::FrameOffsetTB) + _titleSize.height() + _buttonLayout->preferredSize().height() + 2 * spacing());
}

void
Dialog::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
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

    _buttonLayout->moveTo(stylist()->defaultParameter(StyleHint::FrameOffsetLeft), height() - (buttonLayoutSize.height() + stylist()->defaultParameter(StyleHint::FrameOffsetBottom)));

    _buttonLayout->setSize(width() - stylist()->defaultParameter(StyleHint::FrameOffsetLR), buttonLayoutSize.height());
}

} /* namespace ilixi */
