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

#include <ui/AppWindow.h>

#include <core/Application.h>
#include <core/Logger.h>
#include <core/PlatformManager.h>
#include <graphics/Painter.h>
#include <graphics/Stylist.h>
#include <ui/ToolBar.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_APPWINDOW, "ilixi/ui/AppWindow", "AppWindow");

AppWindow::AppWindow(Application* owner)
        : WindowWidget(),
          _backgroundImage(NULL),
          _backgroundImageTiled(false),
          _toolbar(NULL),
          _toolbarNorth(true),
          _customCompose(false),
          _owner(owner)
{
    ILOG_TRACE_W(ILX_APPWINDOW);
    setBackgroundImage(PlatformManager::instance().getBackground());
    setBackgroundFilled(true);
    setMargins(0, 0, 0, 0);
}

AppWindow::~AppWindow()
{
    ILOG_TRACE_W(ILX_APPWINDOW);
    delete _backgroundImage;
}

Image*
AppWindow::background() const
{
    return (_backgroundImage);
}

bool
AppWindow::backgroundTiled() const
{
    return _backgroundImageTiled;
}

int
AppWindow::canvasX() const
{
    return _margin.left();
}

int
AppWindow::canvasY() const
{
    if (_toolbar && _toolbarNorth)
        return _toolbar->preferredSize().height() + _margin.top();
    return _margin.top();
}

int
AppWindow::canvasHeight() const
{
    if (_toolbar)
        return height() - _margin.vSum() - _toolbar->preferredSize().height();
    return height() - _margin.vSum();
}

int
AppWindow::canvasWidth() const
{
    return width() - _margin.hSum();
}

const ToolBar*
AppWindow::toolbar() const
{
    return _toolbar;
}

void
AppWindow::setBackgroundImage(const std::string& imagePath, bool tile)
{
    if (_backgroundImage)
        delete _backgroundImage;

    _backgroundImage = new Image(imagePath);
    _backgroundImage->setSize(size());
    _backgroundImageTiled = tile;
    setBackgroundFilled(true);

    ILOG_DEBUG(ILX_APPWINDOW, "Background is set [%s]\n", imagePath.c_str());
}

bool
AppWindow::setToolbar(ToolBar* toolbar, bool positionNorth)
{
    if (addChild(toolbar))
    {
        removeChild(_toolbar);
        _toolbar = toolbar;
        _toolbarNorth = positionNorth;
        doLayout();
        return true;
    }
    return false;
}

void
AppWindow::setCustomCompose(bool useCustomCompose)
{
    _customCompose = useCustomCompose;
}

void
AppWindow::updateLayoutGeometry()
{
    if (_toolbar)
    {
        int h = _toolbar->preferredSize().height();
        if (_toolbarNorth)
        {
            _toolbar->setGeometry(0, 0, width(), h);
            _toolbar->setNeighbour(Down, _layout);
            _layout->setNeighbour(Up, _toolbar);
        } else
        {
            _toolbar->setGeometry(0, height() - h, width(), h);
            _toolbar->setNeighbour(Up, _layout);
            _layout->setNeighbour(Down, _toolbar);
        }
    }
    _layout->setGeometry(canvasX(), canvasY(), canvasWidth(), canvasHeight());
}

void
AppWindow::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_APPWINDOW);
    if (_customCompose)
        _owner->compose(event);
    else
    {
        Painter painter(this);
        painter.begin(event);
        stylist()->drawAppFrame(&painter, this);
        painter.end();
    }
}

} /* namespace ilixi */
