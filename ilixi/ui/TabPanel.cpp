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

#include <ui/TabPanel.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <sigc++/sigc++.h>

namespace ilixi
{

TabPanelButton::TabPanelButton(std::string label, Widget* parent)
        : Button(label, parent)
{
    setInputMethod(PointerInput);
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
    int w = stylist()->defaultParameter(StyleHint::TabOffsetLR) + t.width();
    int h = stylist()->defaultParameter(StyleHint::TabOffsetTop) + t.height();

    return Size(w, h);
}

void
TabPanelButton::toggleChecked()
{
}

void
TabPanelButton::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
    stylist()->drawTabPanelButton(&painter, this);
    painter.end();
}

void
TabPanelButton::updateTextBaseGeometry()
{
    _layout.setBounds(
            stylist()->defaultParameter(StyleHint::TabOffsetLeft),
            stylist()->defaultParameter(StyleHint::TabOffsetTop),
            width() - stylist()->defaultParameter(StyleHint::TabOffsetLR),
            height() - stylist()->defaultParameter(StyleHint::TabOffsetTop));
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
    setInputMethod(PointerInput);
    sigGeometryUpdated.connect(
            sigc::mem_fun(this, &TabPanel::updateChildrenFrameGeometry));
}

TabPanel::~TabPanel()
{
}

int
TabPanel::heightForWidth(int width) const
{
    int used = stylist()->defaultParameter(StyleHint::FrameOffsetLR)
            + _margin.hSum();
    int h4w = 0;
    for (unsigned int i = 0; i < _pages.size(); i++)
        h4w = std::max(h4w, _pages.at(i).widget->heightForWidth(width - used));

    if (h4w > 0)
        return h4w + _canvasOffsetY + _margin.vSum();
    return -1;
}

Size
TabPanel::preferredSize() const
{
    int w = 0, h = 0;
    // find max. size
    for (unsigned int i = 0; i < _pages.size(); i++)
    {
        Size wS = _pages[i].widget->preferredSize();
        if (wS.width() > w)
            w = wS.width();
        if (wS.height() > h)
            h = wS.height();
    }

    return Size(
            stylist()->defaultParameter(StyleHint::FrameOffsetLR)
                    + _margin.hSum() + w,
            stylist()->defaultParameter(StyleHint::FrameOffsetTB)
                    + _margin.vSum() + h + _canvasOffsetY);
}

int
TabPanel::canvasY() const
{
    return _margin.top()
            + stylist()->defaultParameter(StyleHint::FrameOffsetTop)
            + _canvasOffsetY;
}

int
TabPanel::canvasHeight() const
{
    return height() - _margin.vSum()
            - stylist()->defaultParameter(StyleHint::FrameOffsetTB)
            - _canvasOffsetY;
}

int
TabPanel::count() const
{
    return _pages.size();
}

void
TabPanel::clear()
{
    _pages.clear();
}

int
TabPanel::currentIndex() const
{
    return _currentIndex;
}

Widget*
TabPanel::currentWidget() const
{
    return _pages.at(_currentIndex).widget;
}

int
TabPanel::indexOf(Widget* widget) const
{
    for (unsigned int i = 0; i < _pages.size(); i++)
    {
        if (_pages.at(i).widget == widget)
            return i;
    }
    return -1;
}

bool
TabPanel::pageEnabled(int index) const
{
    return _pages.at(index).widget->enabled();
}

std::string
TabPanel::pageLabel(int index) const
{
    return _pages.at(index).button->text();
}

void
TabPanel::addPage(Widget* widget, std::string label)
{
    if (!widget)
        return;

    if (_currentIndex != -1)
    {
        _pages.at(_currentIndex).widget->setVisible(false);
        _pages.at(_currentIndex).button->setChecked(false);
    }

    _currentIndex++;
    TabPage page;
    page.widget = widget;
    page.button = new TabPanelButton(label, this);
    page.button->setChecked(true);
    page.button->sigClicked.connect(
            sigc::bind<int>(sigc::mem_fun(this, &TabPanel::setCurrentPage),
                            _currentIndex));

    _pages.push_back(page);

    addChild(page.button);
    addChild(widget);

    setSurfaceGeometryModified();
}

void
TabPanel::insertPage(int index, Widget* widget, std::string label,
                     std::string iconPath)
{
    //  widget->setParent(this);
    //  TabPage page;
    //  page.widget = widget;
    //  page.button = new TabPanelButton(label, this);
    //  if (!iconPath.empty())
    //    page.button->setIcon(iconPath, Size(stylist()->hint(TabPanelButtonHeight)
    //        - 6, stylist()->hint(TabPanelButtonHeight) - 6));
    //
    //  if (_currentIndex != -1)
    //    _pages.at(_currentIndex).widget->setVisible(false);
    //
    //  if (index < _pages.size())
    //    {
    //      _currentIndex = index;
    //      PageList::iterator it = _pages.begin() + index;
    //      _pages.insert(it, page);
    //    }
    //  else
    //    {
    //      _pages.push_back(page);
    //      _currentIndex = _pages.size() - 1;
    //    }
    //
    //  addChild(page.button);
    //  setSurfaceGeometryChanged();
    //  update();
}

void
TabPanel::removePage(int index)
{
    if (index < _pages.size())
    {
        PageList::iterator it = _pages.begin() + index;
        removeChild(it->button);
        _pages.erase(it);
        setSurfaceGeometryModified();
        update();
    }
}

void
TabPanel::setCurrentPage(int index)
{
    if (index == _currentIndex)
        return;

    if (index >= 0 && index < _pages.size())
    {
        // hide previous page.
        if (_currentIndex != -1)
        {
            _pages.at(_currentIndex).widget->setVisible(false);
            _pages.at(_currentIndex).button->setChecked(false);
        }

        // show current page.
        _currentIndex = index;
        _pages.at(_currentIndex).widget->setVisible(true);
        _pages.at(_currentIndex).button->setChecked(true);
        update();
    }
}

void
TabPanel::setPageLabel(int index, std::string label)
{
    _pages.at(index).button->setText(label);
}

void
TabPanel::setPageEnabled(int index, bool enabled)
{
    if (enabled)
        _pages.at(index).widget->setEnabled();
    else
        _pages.at(index).widget->setDisabled();
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
    stylist()->drawTabPanel(&painter, this, _canvasOffsetY);
    painter.end();
}

void
TabPanel::updateChildrenFrameGeometry()
{

    _canvasOffsetY = 0;
    for (unsigned int i = 0; i < _pages.size(); i++)
        _canvasOffsetY = std::max(_canvasOffsetY,
                                  _pages[i].button->preferredSize().height());
    int buttonX = 30;
    int buttonW = 0;
    int buttonH = _canvasOffsetY;

    int pageWidth = canvasWidth();
    int pageHeight = canvasHeight();

    for (unsigned int i = 0; i < _pages.size(); i++)
    {
        // set button
        buttonW = _pages[i].button->preferredSize().width();
        _pages[i].button->setGeometry(buttonX, 0, buttonW, buttonH);
        buttonX += buttonW;

        // set page
        _pages[i].widget->moveTo(canvasX(), canvasY());

        _pages[i].widgetSize = _pages[i].widget->preferredSize();

        if (_pages[i].widgetSize.width() < pageWidth
                && ((_pages[i].widget->xConstraint() & GrowPolicy)
                        || (_pages[i].widget->xConstraint() & ExpandPolicy)))
            _pages[i].widget->setWidth(pageWidth);
        else if (_pages[i].widgetSize.width() > pageWidth
                && (_pages[i].widget->xConstraint() & ShrinkPolicy))
            _pages[i].widget->setWidth(pageWidth);
        else
            _pages[i].widget->setWidth(_pages[i].widgetSize.width());

        if (_pages[i].widgetSize.height() < pageHeight
                && ((_pages[i].widget->yConstraint() & GrowPolicy)
                        || (_pages[i].widget->yConstraint() & ExpandPolicy)))
            _pages[i].widget->setHeight(pageHeight);
        else if (_pages[i].widgetSize.height() > pageHeight
                && (_pages[i].widget->yConstraint() & ShrinkPolicy))
            _pages[i].widget->setHeight(pageHeight);
        else
            _pages[i].widget->setHeight(_pages[i].widgetSize.height());
    }
}

void
TabPanel::paintChildren(const Rectangle& rect)
{
    for (unsigned int i = 0; i < _pages.size(); i++)
    {
        _pages[i].button->paint(rect);
        if (i == _currentIndex)
            _pages[i].widget->paint(rect);
    }
}

} /* namespace ilixi */
