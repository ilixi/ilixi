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

#ifndef ILIXI_CLIPBOARD_H_
#define ILIXI_CLIPBOARD_H_

#include <ilixiConfig.h>
#include <directfb.h>
#include <string>

namespace ilixi
{
//! Provides means to share data between processes using DirectFB.
/*!
 * API is still experimental and it can change in the future.
 */
class Clipboard
{
public:
    /*!
     * Returns an instance to Clipboard.
     */
    static Clipboard&
    instance();

    /*!
     * Clears the contents of clipboard.
     */
    void
    clear();

    /*!
     * Returns mime type of clipboard data.
     */
    std::string
    mimeType();

    /*!
     * Returns true if clipboard has data.
     */
    bool
    hasData();

    /*!
     * Use to get clipboard data.
     */
    void
    getData(void** data, unsigned int* size);

    /*!
     * Returns a pointer to a DirectFB surface if clipboard has data with "application/directfb-surface" mime type.
     * Otherwise returns NULL.
     */
    IDirectFBSurface*
    image();

    /*!
     * Returns text stored in clipboard.
     */
    std::string
    text();

    /*!
     * Sets the clipboard data.
     */
    void
    setData(const void* data, unsigned int size, const char* mimeType = "application/octet-stream");

    /*!
     * Sets the DirectFB surface to share via clipboard.
     */
    void
    setImage(IDirectFBSurface* image);

    /*!
     * Sets the clipboard text.
     */
    void
    setText(const std::string& text, const char* mimeType = "text/plain");

private:
    Clipboard();

    virtual
    ~Clipboard();

    //! Updates clipbard contents if modified by other process.
    void
    refreshClipboard();

    void* _data;
    unsigned int _size;
    std::string _mimeType;
    struct timeval _timeStamp;
};

} /* namespace ilixi */
#endif /* ILIXI_CLIPBOARD_H_ */
