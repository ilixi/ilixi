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

#include <ui/TabPanel.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <sigc++/sigc++.h>

namespace ilixi
{

TabPanelButton::TabPanelButton(std::string label, Widget* parent)
        : Button(label, parent),
          _first(false)
{
    setInputMethod(KeyPointer);
    setCheckable(true);
    _layout.setSingleLine(true);
}

TabPanelButton::~TabPanelButton()
{
}

Size
TabPanelButton::preferredSize() const
{
    if (text().empty())
        return stylist()->defaultSize(StyleHint::PushButton);

    Size t = textExtents();
    return Size(stylist()->defaultParameter(StyleHint::PanelLR) + stylist()->defaultParameter(StyleHint::PanelInvWidth) + t.width(), stylist()->defaultParameter(StyleHint::PanelTB) + t.height());
}

void
TabPanelButton::toggleChecked()
{
}

void
TabPanelButton::setFirst(bool first)
{
    _first = first;
}

void
TabPanelButton::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
    stylist()->drawTabPanelButton(&painter, this, _first);
    painter.end();
}

void
TabPanelButton::updateTextBaseGeometry()
{
    _layout.setBounds(stylist()->defaultParameter(StyleHint::PanelInvLeft) + stylist()->defaultParameter(StyleHint::PanelLeft), stylist()->defaultParameter(StyleHint::PanelTop), width() - stylist()->defaultParameter(StyleHint::PanelLR) - stylist()->defaultParameter(StyleHint::PanelInvLeft), height() - stylist()->defaultParameter(StyleHint::PanelTop));
    _layout.doLayout(font());
}

Font*
TabPanelButton::defaultFont() const
{
    return stylist()->defaultFont(StyleHint::TitleFont);
}

//********************************************************************************

TabPanel::TabPanel(Widget* parent)
        : Frame(parent),
          _currentIndex(-1),
          _canvasOffsetY(0)
{
    setConstraints(NoConstraint, MinimumConstraint);
    setInputMethod(PointerPassthrough);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &TabPanel::updateChildrenFrameGeometry));
}

TabPanel::~TabPanel()
{
}

int
TabPanel::heightForWidth(int width) const
{
    int used = stylist()->defaultParameter(StyleHint::FrameOffsetLR) + _margin.hSum();
    int h4w = 0;
    for (unsigned int i = 0; i < _tabs.size(); i++)
        h4w = std::max(h4w, _tabs.at(i).widget->heightForWidth(width - used));

    if (h4w > 0)
        return h4w + _canvasOffsetY + _margin.vSum();
    return -1;
}

Size
TabPanel::preferredSize() const
{
    int w = 0, h = 0;
    // find max. size
    for (unsigned int i = 0; i < _tabs.size(); i++)
    {
        Size wS = _tabs[i].widget->preferredSize();
        if (wS.width() > w)
            w = wS.width();
        if (wS.height() > h)
            h = wS.height();
    }

    return Size(stylist()->defaultParameter(StyleHint::FrameOffsetLR) + _margin.hSum() + w, stylist()->defaultParameter(StyleHint::FrameOffsetTB) + _margin.vSum() + h + _canvasOffsetY);
}

int
TabPanel::canvasY() const
{
    return _margin.top() + stylist()->defaultParameter(StyleHint::FrameOffsetTop) + _canvasOffsetY;
}

int
TabPanel::canvasHeight() const
{
    return height() - _margin.vSum() - stylist()->defaultParameter(StyleHint::FrameOffsetTB) - _canvasOffsetY;
}

int
TabPanel::count() const
{
    return _tabs.size();
}

void
TabPanel::clear()
{
    _tabs.clear();
}

int
TabPanel::currentIndex() const
{
    return _currentIndex;
}

Widget*
TabPanel::currentWidget() const
{
    return _tabs.at(_currentIndex).widget;
}

int
TabPanel::indexOf(Widget* widget) const
{
    for (unsigned int i = 0; i < _tabs.size(); i++)
    {
        if (_tabs.at(i).widget == widget)
            return i;
    }
    return -1;
}

bool
TabPanel::tabEnabled(int index) const
{
    return _tabs.at(index).widget->enabled();
}

std::string
TabPanel::tabLabel(int index) const
{
    return _tabs.at(index).button->text();
}

void
TabPanel::addTab(Widget* widget, std::string label)
{
    if (!widget)
        return;

    if (_currentIndex != -1)
    {
        _tabs.at(_currentIndex).widget->setVisible(false);
        _tabs.at(_currentIndex).button->setChecked(false);
    }

    _currentIndex++;
    TabData page;
    page.widget = widget;
    page.button = new TabPanelButton(label, this);
    if (_currentIndex == 0)
        page.button->setFirst(true);
    page.button->setChecked(true);
    page.button->sigClicked.connect(sigc::bind<int>(sigc::mem_fun(this, &TabPanel::setCurrentTab), _currentIndex));

    _tabs.push_back(page);

    addChild(page.button);
    addChild(widget);

    doLayout();
}

void
TabPanel::insertTab(int index, Widget* widget, std::string label)
{
    // TODO NOT IMPLEMENTED
}

void
TabPanel::removeTab(int index)
{
    if (index < _tabs.size())
    {
        TabList::iterator it = _tabs.begin() + index;
        removeChild(it->button);
        _tabs.erase(it);
        doLayout();
        update();
    }
}

void
TabPanel::setCurrentTab(int index)
{
    if (index == _currentIndex)
        return;

    if (index >= 0 && index < _tabs.size())
    {
        // hide previous page.
        if (_currentIndex != -1)
        {
            _tabs.at(_currentIndex).widget->setVisible(false);
            _tabs.at(_currentIndex).button->setChecked(false);
        }

        // show current page.
        _currentIndex = index;
        _tabs.at(_currentIndex).widget->setVisible(true);
        _tabs.at(_currentIndex).button->setChecked(true);
        update();
    }
}

void
TabPanel::setTabLabel(int index, std::string label)
{
    _tabs.at(index).button->setText(label);
}

void
TabPanel::setTabEnabled(int index, bool enabled)
{
    if (enabled)
        _tabs.at(index).widget->setEnabled();
    else
        _tabs.at(index).widget->setDisabled();
}

void
TabPanel::doLayout()
{
}

void
TabPanel::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
    stylist()->drawTabPanel(&painter, this, _canvasOffsetY, _currentIndex);
    painter.end();
}

void
TabPanel::updateChildrenFrameGeometry()
{
    _canvasOffsetY = 0;
    for (unsigned int i = 0; i < _tabs.size(); i++)
        _canvasOffsetY = std::max(_canvasOffsetY, _tabs[i].button->preferredSize().height());
    int buttonX = stylist()->defaultParameter(StyleHint::PanelInvWidth);
    int buttonW = 0;
    int buttonH = _canvasOffsetY;
    _canvasOffsetY -= stylist()->defaultParameter(StyleHint::PanelInvOverlap);

    int pageWidth = canvasWidth();
    int pageHeight = canvasHeight();

    Widget* left = NULL;
    Widget* right = NULL;

    for (unsigned int i = 0; i < _tabs.size(); i++)
    {
        // set button
        buttonW = _tabs[i].button->preferredSize().width();
        _tabs[i].button->setGeometry(buttonX, 0, buttonW, buttonH); //- stylist()->defaultParameter(StyleHint::PanelInvWidth)
        buttonX += buttonW;
        buttonX -= stylist()->defaultParameter(StyleHint::PanelInvWidth);

        // set page
        _tabs[i].widget->moveTo(canvasX(), canvasY());

        _tabs[i].widgetSize = _tabs[i].widget->preferredSize();

        if (_tabs[i].widgetSize.width() < pageWidth && ((_tabs[i].widget->xConstraint() & GrowPolicy) || (_tabs[i].widget->xConstraint() & ExpandPolicy)))
            _tabs[i].widget->setWidth(pageWidth);
        else if (_tabs[i].widgetSize.width() > pageWidth && (_tabs[i].widget->xConstraint() & ShrinkPolicy))
            _tabs[i].widget->setWidth(pageWidth);
        else
            _tabs[i].widget->setWidth(_tabs[i].widgetSize.width());

        if (_tabs[i].widgetSize.height() < pageHeight && ((_tabs[i].widget->yConstraint() & GrowPolicy) || (_tabs[i].widget->yConstraint() & ExpandPolicy)))
            _tabs[i].widget->setHeight(pageHeight);
        else if (_tabs[i].widgetSize.height() > pageHeight && (_tabs[i].widget->yConstraint() & ShrinkPolicy))
            _tabs[i].widget->setHeight(pageHeight);
        else
            _tabs[i].widget->setHeight(_tabs[i].widgetSize.height());

        if (i == 0)
            left = _tabs[_tabs.size() - 1].button;
        else
            left = _tabs[i - 1].button;

        if (i == _tabs.size() - 1)
            right = _tabs[0].button;
        else
            right = _tabs[i + 1].button;

        _tabs[i].button->setNeighbours(getNeighbour(Up), _tabs[i].widget, left, right);
        _tabs[i].widget->setNeighbours(_tabs[i].button, getNeighbour(Down), getNeighbour(Left), getNeighbour(Right));
    }
}

void
TabPanel::paintChildren(const PaintEvent& event)
{
    for (unsigned int i = 0; i < _tabs.size(); i++)
    {
        if (i == _currentIndex)
            _tabs[i].widget->paint(event);
        else
            _tabs[i].button->paint(event);
    }
    _tabs[_currentIndex].button->paint(event);
}

} /* namespace ilixi */
