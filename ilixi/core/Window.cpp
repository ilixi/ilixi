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

#include <core/Window.h>

#include <core/Application.h>
#include <core/Logger.h>
#include <core/PlatformManager.h>
#include <algorithm>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_WINDOW, "ilixi/core/Window", "Window");

Window::Window()
        : _dfbWindow(NULL),
          _windowSurface(NULL)
{
    ILOG_TRACE_F(ILX_WINDOW);
}

Window::~Window()
{
    ILOG_TRACE_F(ILX_WINDOW);
    releaseDFBWindow();
}

DFBWindowID
Window::windowID() const
{
    DFBWindowID windowID = 0;
    _dfbWindow->GetID(_dfbWindow, &windowID);
    return windowID;
}

IDirectFBSurface*
Window::dfbSurface() const
{
    return _windowSurface;
}

Size
Window::windowSize() const
{
    ILOG_TRACE_F(ILX_WINDOW);
    int w = 0, h = 0;
    if (_dfbWindow)
        _dfbWindow->GetSize(_dfbWindow, &w, &h);
    ILOG_DEBUG(ILX_WINDOW, " -> Size(%d, %d)\n", w, h);
    return Size(w, h);
}

Point
Window::windowPosition() const
{
    int x = 0, y = 0;
    if (_dfbWindow)
        _dfbWindow->GetPosition(_dfbWindow, &x, &y);
    return Point(x, y);
}

void
Window::showWindow()
{
    ILOG_TRACE_F(ILX_WINDOW);
    if (!_dfbWindow)
        return;

    _dfbWindow->SetOpacity(_dfbWindow, 255);
    _dfbWindow->RaiseToTop(_dfbWindow);
    ILOG_DEBUG(ILX_WINDOW, " -> Window %p is visible.\n", this);
}

void
Window::hideWindow()
{
    ILOG_TRACE_F(ILX_WINDOW);
    if (!_dfbWindow)
        return;

    _dfbWindow->SetOpacity(_dfbWindow, 0);
    _dfbWindow->LowerToBottom(_dfbWindow);
    ILOG_DEBUG(ILX_WINDOW, " -> Window %p is hidden.\n", this);
}

bool
Window::initDFBWindow(const Size& size)
{
    ILOG_TRACE_F(ILX_WINDOW);
    if (!PlatformManager::instance().getDFB())
    {
        ILOG_FATAL(ILX_WINDOW, "DirectFB interface is not initialised!\n");
        exit(EXIT_FAILURE);
    }

    DFBResult ret;
    DFBWindowDescription desc;
    desc.flags = (DFBWindowDescriptionFlags) (DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_SURFACE_CAPS | DWDESC_PIXELFORMAT | DWDESC_OPTIONS | DWDESC_STACKING);

    desc.pixelformat = DSPF_ARGB;
    desc.surface_caps = PlatformManager::instance().getWindowSurfaceCaps();
    desc.options = (DFBWindowOptions) (DWOP_ALPHACHANNEL);
#ifdef ILIXI_STEREO_OUTPUT
    desc.caps = (DFBWindowCapabilities) (DWCAPS_DOUBLEBUFFER | DWCAPS_ALPHACHANNEL | DWCAPS_STEREO);
#else
    desc.caps = (DFBWindowCapabilities) (DWCAPS_DOUBLEBUFFER | DWCAPS_ALPHACHANNEL);
#endif

    DFBDisplayLayerConfig conf;
    ret = PlatformManager::instance().getLayer()->GetConfiguration(PlatformManager::instance().getLayer(), &conf);
    if (ret != DFB_OK)
    {
        ILOG_ERROR( ILX_WINDOW, "Error while getting primary layer configuration (%s)!\n", DirectFBErrorString(ret));
        return false;
    }

    if (!Application::activeWindow())
    {
        ILOG_DEBUG(ILX_WINDOW, " -> Main window is initialising...\n");
        desc.posx = 0;
        desc.posy = 0;
        desc.width = conf.width;
        desc.height = conf.height;
        desc.stacking = DWSC_MIDDLE;
        ILOG_DEBUG(ILX_WINDOW, " -> Rect(%d, %d, %d, %d)\n", desc.posx, desc.posy, desc.width, desc.height);
    } else // other windows
    {
        Size appSize = Application::appSize();
        ILOG_DEBUG(ILX_WINDOW, " -> Child window is initialising...\n");
        int w = size.width();
        int h = size.height();
        if (w > (appSize.width() - 20))
            w = appSize.width() - 20;

        if (h > (appSize.height() - 20))
            h = appSize.height() - 20;

        int x = (appSize.width() - w) / 2.0;
        int y = (appSize.height() - h) / 2.0;

        desc.posx = x;
        desc.posy = y;
        desc.width = w;
        desc.height = h;
        desc.stacking = DWSC_UPPER;

        ILOG_DEBUG(ILX_WINDOW, " -> Rect(%d, %d, %d, %d)\n", desc.posx, desc.posy, desc.width, desc.height);
    }

    ret = PlatformManager::instance().getLayer("ui")->CreateWindow(PlatformManager::instance().getLayer("ui"), &desc, &_dfbWindow);
    if (ret != DFB_OK)
    {
        ILOG_ERROR( ILX_WINDOW, "Error while creating DirectFB window! (%s)!\n", DirectFBErrorString(ret));
        return false;
    }

    ret = _dfbWindow->GetSurface(_dfbWindow, &_windowSurface);
    if (ret != DFB_OK)
    {
        ILOG_ERROR( ILX_WINDOW, "Unable to acquire surface from application window. (%s)!\n", DirectFBErrorString(ret));
        return false;
    }

#ifdef ILIXI_STEREO_OUTPUT
    _dfbWindow->SetStereoDepth(_dfbWindow, 0);
#endif

    ILOG_INFO(ILX_WINDOW, " -> Window %p is initialised.\n", this);
    return true;
}

void
Window::releaseDFBWindow()
{
    ILOG_TRACE_F(ILX_WINDOW);
    if (_dfbWindow)
    {
        ILOG_DEBUG(ILX_WINDOW, " -> Releasing DirectFB window %p...\n", _dfbWindow);

        _windowSurface->Release(_windowSurface);
        _dfbWindow->Close(_dfbWindow);
        _dfbWindow->Destroy(_dfbWindow);
        _dfbWindow->Release(_dfbWindow);
        _dfbWindow = NULL;

        ILOG_INFO(ILX_WINDOW, "DirectFB window interface is released.\n");
    }
}

} /* namespace ilixi */
