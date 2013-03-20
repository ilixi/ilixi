/*
 * NumPages.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: tarik
 */

#include "NumPages.h"
#include "AppButton.h"
#include <ui/Spacer.h>
#include <core/Logger.h>
#include <sigc++/bind.h>
#include <cmath>
#include <stdlib.h>
#include "Home.h"

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_NUMPAGES, "ilixi/car/Home/pages", "NumPages");

NumPages::NumPages(Widget* parent)
        : Widget(parent),
          _currentPage(0),
          _previousPage(0)
{
    ILOG_TRACE_W(ILX_NUMPAGES);
    _font = new Font("sans");
    _font->setSize(18);
    _font->setStyle(Font::Bold);
    setInputMethod(PointerInput);

    _anim.setDuration(400);
    _anim.sigExec.connect(sigc::mem_fun(this, &NumPages::tweenSlot));

    _tween = new Tween(Tween::CUBIC, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_tween);
}

NumPages::~NumPages()
{
    ILOG_TRACE_W(ILX_NUMPAGES);
    delete _font;
}

void
NumPages::addItem(const char* name, const char* icon)
{
    AppButton* button = new AppButton(name);
    button->setToolButtonStyle(ToolButton::IconAboveText);
    button->setDrawFrame(false);
    button->setFont(_font);
    button->setIcon(icon, Size(96, 96));
    _items.push_back(button);
    button->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(this, &NumPages::runApp), button->text()));
}

void
NumPages::setAppStatus(Compositor::VisibilityData notification)
{
    ILOG_TRACE_W(ILX_NUMPAGES);
    for (int i = 0; i < _items.size(); ++i)
    {
        if (_items[i]->text() == notification.name)
        {
            if (notification.status & Compositor::AppVisible)
                _items[i]->setAppVisible(true);
            else if (notification.status & Compositor::AppHidden)
                _items[i]->setAppVisible(false);
            else if (notification.status & Compositor::AppStarting)
                _items[i]->appStarting();
        }
    }
}

void
NumPages::setAppStarting(Compositor::VisibilityData notification)
{
    ILOG_TRACE_W(ILX_NUMPAGES);
    for (int i = 0; i < _items.size(); ++i)
    {
        if (_items[i]->text() == notification.name)
            _items[i]->appStarting();
    }
}

void
NumPages::initPages()
{
    ILOG_TRACE_W(ILX_NUMPAGES);
    int cols = width() / 150;
    int rows = (height() - 50) / 150;

    int numPages = std::ceil(_items.size() / (cols * rows + .0));
    int buttonOffset = (width() - (numPages * 53)) / 2;
    _previousPage = 0;
    _currentPage = 0;

    GridLayout* grid;
    PageButton* pageButton;
    for (int i = 0; i < numPages; ++i)
    {
        ILOG_DEBUG(ILX_NUMPAGES, " -> page: %d\n", i);
        grid = new GridLayout(rows, cols);

        for (int j = i * (cols * rows); j < (i + 1) * (cols * rows); ++j)
        {
            if (j > _items.size() - 1)
            {
                ILOG_DEBUG(ILX_NUMPAGES, "   -> Spacer: %d\n", j);
                grid->addWidget(new Spacer(Vertical), rows - 1, cols - 1, 1, 1);
                break;
            }
            ILOG_DEBUG(ILX_NUMPAGES, "   -> item: %d (%s)\n", j, _items[j]->text().c_str());
            grid->addWidget(_items[j]);
        }

        grid->setGeometry(i * width(), 0, width(), height() - 50);
        addChild(grid);
        _pages.push_back(grid);

        if (numPages > 1)
        {
            char str[2];
            snprintf(str, 2, "%d", i + 1);
            pageButton = new PageButton(str);
            pageButton->setDrawFrame(false);
            pageButton->setToolButtonStyle(ToolButton::TextOnly);
            pageButton->setGeometry(buttonOffset + i * 53, height() - 50, 48, 48);
            pageButton->sigClicked.connect(sigc::bind<unsigned int>(sigc::mem_fun(this, &NumPages::showPage), i));
            addChild(pageButton);
            _buttons.push_back(pageButton);
        }
    }
    _tween->setEndValue(width());
    if (numPages > 1)
        _buttons[0]->setDisabled();
    update();
}

void
NumPages::compose(const PaintEvent& event)
{
}

void
NumPages::runApp(const std::string& name)
{
    sigAppStart(name.c_str());
}

void
NumPages::showPage(unsigned int index)
{
    ILOG_TRACE_W(ILX_NUMPAGES);
    if (_currentPage == index)
        return;

    _anim.stop();
    _previousPage = _currentPage;
    _currentPage = index;
    _buttons[_previousPage]->setEnabled();
    _buttons[index]->setDisabled();
    _anim.start();
}

void
NumPages::tweenSlot()
{
    if (_previousPage < _currentPage)
    {
        _pages[_previousPage]->setX(-_tween->value());
        _pages[_currentPage]->setX(width() - _tween->value());
    } else if (_previousPage > _currentPage)
    {
        _pages[_previousPage]->setX(_tween->value());
        _pages[_currentPage]->setX(_tween->value() - width());
    }
    update();
}

void
NumPages::updateNumPagesGeometry()
{

}

} /* namespace ilixi */
