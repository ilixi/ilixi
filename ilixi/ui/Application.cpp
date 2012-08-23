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

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPLICATION, "ilixi/ui/Application", "Application");

Application::Application(int* argc, char*** argv, AppOptions opts)
        : AppBase(argc, argv, opts),
          WindowWidget(),
          _backgroundImage(NULL)
{
    // parse app-meta file...
    ILOG_TRACE_W(ILX_APPLICATION);
    initDFB(argc, argv);
    setStylist(new Stylist());
    setBackgroundFilled(false);
    setMargins(0, 0, 0, 0);

    setTitle("Untitled");
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
    setAppState(APS_TERM);
}

void
Application::exec()
{
    ILOG_INFO(ILX_APPLICATION, "Starting...\n");

    if (__options & OptExclusive)
    {
        if (__layer->SetCooperativeLevel(__layer, DLSCL_EXCLUSIVE))
            ILOG_ERROR(ILX_APPLICATION, "Error while setting EXLUSIVE mode!\n");
        else
            ILOG_INFO(ILX_APPLICATION, "Now running in exclusive mode.\n");
    }

    show();

    setLayerSize(width(), height());
    if (_backgroundImage)
        _backgroundImage->setSize(width(), height());

    while (true)
    {
        if (__state & APS_TERM)
            break;
        else
        {
            handleEvents(runCallbacks());
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
    if (__state & APS_HIDDEN)
    {
        showWindow();
        __state = APS_VISIBLE;
        sigVisible();
    }
}

void
Application::hide()
{
    if (__state & APS_VISIBLE)
    {
        closeWindow();
        __state = APS_HIDDEN;
        sigHidden();
    }
}

void
Application::setStylist(Stylist* stylist)
{
    if (!stylist)
        return;
    // TODO we will allow setting custom stylist in the future.
    if (_stylist)
        return;
    _stylist = stylist;
}

void
Application::setPaletteFromFile(const char* palette)
{
    if (_stylist)
        _stylist->setPaletteFromFile(palette);
}

void
Application::setStyleFromFile(const char* style)
{
    if (_stylist)
        _stylist->setStyleFromFile(style);
}

void
Application::postUserEvent(unsigned int type, void* data)
{
    DFBUserEvent event;
    event.clazz = DFEC_USER;
    event.type = type;
    event.data = data;

    __buffer->PostEvent(__buffer, DFB_EVENT(&event));
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
