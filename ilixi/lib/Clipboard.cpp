/*
 Copyright 2010-2015 Tarik Sekmen.

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

#include <lib/Clipboard.h>
#include <core/Logger.h>
#include <core/PlatformManager.h>
#include <sys/time.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_CLIPBOARD, "ilixi/lib/Clipboard", "Clipboard");

bool
timevalCompare(struct timeval *a, struct timeval *b)
{
    return ((a->tv_sec == b->tv_sec) ? (a->tv_usec > b->tv_usec) : (a->tv_sec > b->tv_sec));
}

Clipboard&
Clipboard::instance()
{
    static Clipboard instance;
    return instance;
}

void
Clipboard::clear()
{
    setData(0, 0, "");
}

void
Clipboard::getData(void** data, unsigned int* size)
{
    ILOG_TRACE_F(ILX_CLIPBOARD);
    refreshClipboard();
    *data = _data;
    *size = _size;
}

std::string
Clipboard::mimeType()
{
    ILOG_TRACE_F(ILX_CLIPBOARD);
    refreshClipboard();
    return _mimeType;
}

bool
Clipboard::hasData()
{
    refreshClipboard();
    return _data != NULL;
}

IDirectFBSurface*
Clipboard::image()
{
    ILOG_TRACE_F(ILX_CLIPBOARD);
    if (mimeType() != "application/directfb-surface")
        return NULL;
    void* data;

    unsigned int size;
    getData(&data, &size);

    unsigned int id = *((unsigned int*) data);

    IDirectFBSurface* surface = NULL;
    PlatformManager::instance().getDFB()->GetSurface(PlatformManager::instance().getDFB(), id, &surface);
    return surface;
}

std::string
Clipboard::text()
{
    ILOG_TRACE_F(ILX_CLIPBOARD);
    refreshClipboard();
    if (_size)
    {
        char txt[_size +1];
        snprintf(txt, _size + 1, "%s", (char*) _data);
        D_DEBUG_AT(ILX_CLIPBOARD, " -> text: %s\n", txt);
        return txt;
    }
    return "";
}

void
Clipboard::setData(const void* data, unsigned int size, const char* mimeType)
{
    ILOG_TRACE_F(ILX_CLIPBOARD);
    DFBResult ret = PlatformManager::instance().getDFB()->SetClipboardData(PlatformManager::instance().getDFB(), mimeType, data, size, &_timeStamp);
    if (ret != DFB_OK)
    {
        ILOG_ERROR(ILX_CLIPBOARD, "Cannot set clipboard data[%p] size(%u)!\n", data, size);
        return;
    }
}

void
Clipboard::setImage(IDirectFBSurface* image)
{
    ILOG_TRACE_F(ILX_CLIPBOARD);
    image->AllowAccess(image, "*");
    unsigned int id = 0;
    image->GetID(image, &id);
    void* data = &id;
    setData(&data, sizeof(unsigned int), "application/directfb-surface");
}

void
Clipboard::setText(const std::string& text, const char* mimeType)
{
    ILOG_TRACE_F(ILX_CLIPBOARD);
    DFBResult ret = PlatformManager::instance().getDFB()->SetClipboardData(PlatformManager::instance().getDFB(), mimeType, (void*) text.c_str(), text.size(), &_timeStamp);
    if (ret != DFB_OK)
    {
        ILOG_ERROR(ILX_CLIPBOARD, "Cannot set clipboard text: %s!\n", text.c_str());
        return;
    }
}

Clipboard::Clipboard()
        : _data(NULL),
          _size(0),
          _mimeType("")
{
    ILOG_TRACE_F(ILX_CLIPBOARD);
    gettimeofday(&_timeStamp, NULL);
}

Clipboard::~Clipboard()
{
    ILOG_TRACE_F(ILX_CLIPBOARD);
}

void
Clipboard::refreshClipboard()
{
    ILOG_TRACE_F(ILX_CLIPBOARD);
    struct timeval ts;
    DFBResult ret = PlatformManager::instance().getDFB()->GetClipboardTimeStamp(PlatformManager::instance().getDFB(), &ts);
    if (ret != DFB_OK)
    {
        ILOG_ERROR(ILX_CLIPBOARD, "Cannot get clipboard time stamp!\n");
        return;
    }

    if (timevalCompare(&ts, &_timeStamp))
    {
        char* type;
        DFBResult ret = PlatformManager::instance().getDFB()->GetClipboardData(PlatformManager::instance().getDFB(), &type, &_data, &_size);
        if (ret != DFB_OK)
            ILOG_ERROR(ILX_CLIPBOARD, "Cannot get clipboard data!\n");
        else
            ILOG_DEBUG(ILX_CLIPBOARD, " -> mimeType %s data %p size %u\n", type, _data, _size);
        _mimeType = type;
        _timeStamp = ts;
    }
}

} /* namespace ilixi */
