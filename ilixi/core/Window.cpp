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

#include <core/Window.h>
#include <core/AppBase.h>
#include <core/Logger.h>
#include <algorithm>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_WINDOW, "ilixi/core/Window", "Window");

Window::Window()
        : _dfbWindow(NULL),
          _windowSurface(NULL)
{
}

Window::~Window()
{
    releaseDFBWindow();
    ILOG_DEBUG(ILX_WINDOW, "~Window\n");
}

DFBWindowID
Window::windowID() const
{
    DFBWindowID windowID;
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
    int w = 0, h = 0;
    if (_dfbWindow)
        _dfbWindow->GetSize(_dfbWindow, &w, &h);
    ILOG_TRACE_F(ILX_WINDOW);
    ILOG_DEBUG(ILX_WINDOW, " -> Size(%d, %d)\n", w, h);
    return Size(w, h);
}

void
Window::showWindow()
{
    if (!_dfbWindow)
        return;

    _dfbWindow->SetOpacity(_dfbWindow, 255);
    _dfbWindow->RaiseToTop(_dfbWindow);
    ILOG_DEBUG(ILX_WINDOW, "Window is %p visible.\n", this);
}

void
Window::hideWindow()
{
    if (!_dfbWindow)
        return;

    _dfbWindow->SetOpacity(_dfbWindow, 0);
    _dfbWindow->LowerToBottom(_dfbWindow);
    ILOG_DEBUG(ILX_WINDOW, "Window is %p hidden.\n", this);
}

bool
Window::initDFBWindow(const Size& size)
{
    if (!AppBase::getDFB())
    {
        ILOG_FATAL(ILX_WINDOW, "DirectFB interface is not initialised!\n");
        exit(EXIT_FAILURE);
    }

    DFBResult ret;
    DFBWindowDescription desc;
    desc.flags = (DFBWindowDescriptionFlags) (DWDESC_POSX | DWDESC_POSY
            | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_SURFACE_CAPS
            | DWDESC_PIXELFORMAT | DWDESC_OPTIONS | DWDESC_STACKING);

    desc.pixelformat = DSPF_ARGB;
    desc.surface_caps = DSCAPS_DOUBLE;
    desc.options = (DFBWindowOptions) (DWOP_ALPHACHANNEL | DWOP_SCALE);
#ifdef ILIXI_STEREO_OUTPUT
    desc.caps = (DFBWindowCapabilities) (DWCAPS_DOUBLEBUFFER | DWCAPS_ALPHACHANNEL
            | DWCAPS_STEREO);
#else
    desc.caps = (DFBWindowCapabilities) (DWCAPS_DOUBLEBUFFER
            | DWCAPS_ALPHACHANNEL);
#endif

    DFBDisplayLayerConfig conf;
    ret = AppBase::getLayer()->GetConfiguration(AppBase::getLayer(), &conf);
    if (ret != DFB_OK)
    {
        ILOG_ERROR(
                ILX_WINDOW,
                "Error while getting primary layer configuration (%s)!\n", DirectFBErrorString(ret));
        return false;
    }

    if (!AppBase::activeWindow())
    {
        ILOG_DEBUG(ILX_WINDOW, "Main window is initialising...\n");
        desc.posx = 0;
        desc.posy = 0;
        if (AppBase::appOptions() & OptStatusBar)
        {
            desc.width = conf.width;
            desc.height = 54;
        } else
        {
            desc.width = conf.width;
            desc.height = conf.height;
        }
        desc.stacking = DWSC_MIDDLE;
    } else // other windows
    {
        ILOG_DEBUG(ILX_WINDOW, "Window is initialising...\n");
        int w = size.width();
        int h = size.height();
        if (w > (conf.width - 20))
            w = conf.width - 20;

        if (h > (conf.height - 20))
            h = conf.height - 20;

        int x = (conf.width - w) / 2.0;
        int y = (conf.height - h) / 2.0;

        desc.posx = x;
        desc.posy = y;
        desc.width = w;
        desc.height = h;
        desc.stacking = DWSC_UPPER;

        ILOG_DEBUG(ILX_WINDOW, " -> (%d, %d, %d, %d)\n", x, y, w, h);
    }

    ret = AppBase::getLayer()->CreateWindow(AppBase::getLayer(), &desc,
                                            &_dfbWindow);
    if (ret != DFB_OK)
    {
        ILOG_ERROR(
                ILX_WINDOW,
                "Error while creating DirectFB window! (%s)!\n", DirectFBErrorString(ret));
        return false;
    }

    ret = _dfbWindow->GetSurface(_dfbWindow, &_windowSurface);
    if (ret != DFB_OK)
    {
        ILOG_ERROR(
                ILX_WINDOW,
                "Unable to acquire surface from application window. (%s)!\n", DirectFBErrorString(ret));
        return false;
    }

#ifdef ILIXI_STEREO_OUTPUT
    _dfbWindow->SetStereoDepth(_dfbWindow, 0);
#endif

    ILOG_INFO(ILX_WINDOW, "Window %p is initialised.\n", this);
    return true;
}

void
Window::releaseDFBWindow()
{
    if (_dfbWindow)
    {
        ILOG_DEBUG(ILX_WINDOW, "Releasing DirectFB window %p...\n", _dfbWindow);

        _windowSurface->Release(_windowSurface);
        _dfbWindow->Close(_dfbWindow);
        _dfbWindow->Destroy(_dfbWindow);
        _dfbWindow->Release(_dfbWindow);
        _dfbWindow = NULL;

        ILOG_INFO(ILX_WINDOW, "DirectFB window interface is released.\n");
    }
}

} /* namespace ilixi */
