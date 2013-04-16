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

#include <ui/Application.h>
#include <graphics/Painter.h>
#include <graphics/Stylist.h>
#include <core/Logger.h>
#include <string.h>
#include <core/PlatformManager.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPLICATION, "ilixi/ui/Application", "Application");

Application::Application(int* argc, char*** argv, AppOptions opts)
        : AppBase(argc, argv, opts),
          WindowWidget(),
          _backgroundImage(NULL)
{
    ILOG_TRACE_W(ILX_APPLICATION);

    setStylist(new Stylist());
    setBackgroundImage(PlatformManager::instance().getBackground());
    setBackgroundFilled(false);
    setMargins(0, 0, 0, 0);

    sigAbort.connect(sigc::mem_fun(this, &Application::quit));
}

Application::~Application()
{
    delete _backgroundImage;
    delete _stylist;
    ILOG_TRACE_W(ILX_APPLICATION);
}

Image*
Application::background() const
{
    return (_backgroundImage);
}

int
Application::canvasX() const
{
    return _margin.left();
}

int
Application::canvasY() const
{
    return _margin.top();
}

int
Application::canvasHeight() const
{
    return height() - _margin.vSum();
}

int
Application::canvasWidth() const
{
    return width() - _margin.hSum();
}

void
Application::quit()
{
    setVisible(false);
    setAppFlag(APS_TERM);
}

void
Application::exec()
{
    ILOG_INFO(ILX_APPLICATION, "Starting...\n");

    show();

    while (true)
    {
        if (__flags & APS_TERM)
            break;
        else
        {
            runCallbacks();
            handleEvents(runTimers());
            updateWindows();
        }
    }

    hide();

    ILOG_INFO(ILX_APPLICATION, "Stopping...\n");

    sigQuit();
}

void
Application::setBackgroundImage(std::string imagePath)
{
    if (_backgroundImage)
        delete _backgroundImage;

    _backgroundImage = new Image(imagePath);
    setBackgroundFilled(true);

    ILOG_DEBUG( ILX_APPLICATION, "Background is set [%s]\n", imagePath.c_str());
}

void
Application::show()
{
    if (__flags & APS_HIDDEN)
    {
        _backgroundImage->setSize(size());
        showWindow();
        __flags = APS_VISIBLE;
        sigVisible();
    }
}

void
Application::hide()
{
    if (__flags & APS_VISIBLE)
    {
        closeWindow();
        __flags = APS_HIDDEN;
        sigHidden();
    }
}

void
Application::setStylist(StylistBase* stylist)
{
    // TODO we will allow setting custom stylist in the future.
    if (_stylist)
    {
        ILOG_WARNING(ILX_APPLICATION, "setStylist()  -  Not implemented!");
        return;
    }

    if (!stylist)
        return;

    _stylist = stylist;

    if (!_stylist->setFontPack(PlatformManager::instance().getFontPack().c_str()))
        ILOG_THROW(ILX_APPLICATION, "Please fix your configuration file!\n");

    if (!_stylist->setIconPack(PlatformManager::instance().getIconPack().c_str()))
        ILOG_THROW(ILX_APPLICATION, "Please fix your configuration file!\n");

    if (!_stylist->setPaletteFromFile(PlatformManager::instance().getPalette().c_str()))
        ILOG_THROW(ILX_APPLICATION, "Please fix your configuration file!\n");

    if (!_stylist->setStyleFromFile(PlatformManager::instance().getStyle().c_str()))
        ILOG_THROW(ILX_APPLICATION, "Please fix your configuration file!\n");
}

bool
Application::setFontPack(const char* fontPack)
{
    if (_stylist)
        return _stylist->setFontPack(fontPack);
    return false;
}

bool
Application::setIconPack(const char* iconPack)
{
    if (_stylist)
        return _stylist->setIconPack(iconPack);
    return false;
}

bool
Application::setPaletteFromFile(const char* palette)
{
    if (_stylist)
        return _stylist->setPaletteFromFile(palette);
    return false;
}

bool
Application::setStyleFromFile(const char* style)
{
    if (_stylist)
        return _stylist->setStyleFromFile(style);
    return false;
}

void
Application::postUserEvent(unsigned int type, void* data)
{
    DFBUserEvent event;
    event.clazz = DFEC_USER;
    event.type = type;
    event.data = data;

    __buffer->PostEvent(__buffer, DFB_EVENT(&event) );
}

void
Application::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_APPLICATION);
    Painter painter(this);
    painter.begin(event);
    stylist()->drawAppFrame(&painter, this);
    painter.end();
}

} /* namespace ilixi */
