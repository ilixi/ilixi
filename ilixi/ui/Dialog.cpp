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

#include <ui/Dialog.h>
#include <ui/PushButton.h>
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
    if (_buttonLayout)
        return _layout->heightForWidth(width - _margin.hSum() - stylist()->defaultParameter(StyleHint::DialogLR)) + _margin.vSum() + stylist()->defaultParameter(StyleHint::DialogTB) + _titleSize.height() + _buttonLayout->preferredSize().height();
    return _layout->heightForWidth(width - _margin.hSum() - stylist()->defaultParameter(StyleHint::DialogLR)) + _margin.vSum() + stylist()->defaultParameter(StyleHint::DialogTB) + _titleSize.height();
}

Size
Dialog::preferredSize() const
{
    ILOG_TRACE_W(ILX_DIALOG);
    Size layoutSize = _layout->preferredSize();
    ILOG_DEBUG(ILX_DIALOG, " -> _layout size: %dx%d\n", layoutSize.width(), layoutSize.height());

    Size buttonLayoutSize = _buttonLayout ? _buttonLayout->preferredSize() : Size();
    ILOG_DEBUG(ILX_DIALOG, " -> _buttonLayout size: %dx%d\n", buttonLayoutSize.width(), buttonLayoutSize.height());

    Size s(std::max(layoutSize.width() + _margin.hSum(), std::max(_titleSize.width(), buttonLayoutSize.width())) + stylist()->defaultParameter(StyleHint::DialogLR), layoutSize.height() + _margin.vSum() + _titleSize.height() + buttonLayoutSize.height() + 2 * spacing() + stylist()->defaultParameter(StyleHint::DialogTB));
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
    ILOG_TRACE_W(ILX_DIALOG);
    if (_buttonLayout)
        removeChild(_buttonLayout);

    if (option == NoButtonOption)
    {
        _buttonLayout = NULL;
        return;
    }

    _buttonLayout = new HBoxLayout();
    _buttonLayout->addWidget(new Spacer());

    PushButton* button;
    if (option == OKButtonOption)
    {
        button = new PushButton("OK");
        button->setPushButtonStyle(OK);
        _buttonLayout->addWidget(button);
        button->sigClicked.connect(sigc::mem_fun(this, &Dialog::accept));
    } else if (option == CancelButtonOption)
    {
        button = new PushButton("Cancel");
        button->setPushButtonStyle(CANCEL);
        _buttonLayout->addWidget(button);
        button->sigClicked.connect(sigc::mem_fun(this, &Dialog::reject));
    } else if (option == OKCancelButtonOption)
    {
        button = new PushButton("OK");
        button->setPushButtonStyle(OK);
        _buttonLayout->addWidget(button);
        button->sigClicked.connect(sigc::mem_fun(this, &Dialog::accept));

        button = new PushButton("Cancel");
        button->setPushButtonStyle(CANCEL);
        _buttonLayout->addWidget(button);
        button->sigClicked.connect(sigc::mem_fun(this, &Dialog::reject));
    } else
    {
        button = new PushButton("Yes");
        button->setPushButtonStyle(OK);
        _buttonLayout->addWidget(button);
        button->sigClicked.connect(sigc::mem_fun(this, &Dialog::accept));

        button = new PushButton("No");
        button->setPushButtonStyle(CANCEL);
        _buttonLayout->addWidget(button);
        button->sigClicked.connect(sigc::mem_fun(this, &Dialog::reject));

        if (option == YesNoCancelButtonOption)
        {
            button = new PushButton("Cancel");
            _buttonLayout->addWidget(button);
            button->sigClicked.connect(sigc::bind<int>(sigc::mem_fun(this, &Dialog::finish), -1));
        }
    }
    Size buttonLayoutSize = _buttonLayout ? _buttonLayout->preferredSize() : Size();
    ILOG_DEBUG(ILX_DIALOG, " -> _buttonLayout size: %dx%d\n", buttonLayoutSize.width(), buttonLayoutSize.height());
    addChild(_buttonLayout);
    _buttonLayout->setNeighbour(Up, _layout);
    _layout->setNeighbour(Down, _buttonLayout);
}

void
Dialog::setButtonLayout(LayoutBase* buttonLayout)
{
    ILOG_TRACE_W(ILX_DIALOG);
    removeChild(_buttonLayout);
    _buttonLayout = buttonLayout;
    addChild(_buttonLayout);
}

void
Dialog::setTitle(const std::string& title)
{
    ILOG_TRACE_W(ILX_DIALOG);
    if (_title != title)
    {
        _title = title;
        _titleSize = stylist()->defaultFont(StyleHint::TitleFont)->extents(_title);
    }
}

int
Dialog::canvasY() const
{
    ILOG_TRACE_W(ILX_DIALOG);
    return _margin.top() + stylist()->defaultParameter(StyleHint::DialogTop) + _titleSize.height();
}

int
Dialog::canvasHeight() const
{
    ILOG_TRACE_W(ILX_DIALOG);
    if (_buttonLayout)
        return height() - (_margin.vSum() + stylist()->defaultParameter(StyleHint::DialogTB) + _titleSize.height() + _buttonLayout->preferredSize().height() + 2 * spacing());
    return height() - (_margin.vSum() + stylist()->defaultParameter(StyleHint::DialogTB) + _titleSize.height() + spacing());
}

void
Dialog::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_DIALOG);
    Painter painter(this);
    painter.begin(event);
    stylist()->drawDialog(&painter, this);
    painter.end();
}

void
Dialog::updateButtonLayoutGeometry()
{
    ILOG_TRACE_W(ILX_DIALOG);
    if (!_buttonLayout)
        return;

    if (_buttonLayout->count() == 0)
        return;

    Size buttonLayoutSize = _buttonLayout->preferredSize();

    _layout->setNeighbour(Down, _buttonLayout);
    _buttonLayout->setNeighbour(Up, _layout);

    _buttonLayout->moveTo(stylist()->defaultParameter(StyleHint::DialogLeft), height() - (buttonLayoutSize.height() + stylist()->defaultParameter(StyleHint::DialogBottom)));

    _buttonLayout->setSize(width() - stylist()->defaultParameter(StyleHint::DialogLR), buttonLayoutSize.height());
}

} /* namespace ilixi */
