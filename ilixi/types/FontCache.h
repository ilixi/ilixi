/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
 Denis Oliver Kropp <dok@directfb.org>.

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

#ifndef ILIXI_FONTCACHE_H_
#define ILIXI_FONTCACHE_H_

#include <pthread.h>
#include <map>
#include <directfb.h>
#include <string>

namespace ilixi
{

class FontCache
{
public:
    static FontCache*
    Instance();

    unsigned int
    getKey(const std::string& name, int size, DFBFontAttributes attr);

    unsigned int
    getEntry(const std::string& name, int size, DFBFontAttributes attr, IDirectFBFont** font);

    void
    releaseEntry(unsigned int key);

    void
    releaseEntry(const char* name, int size, DFBFontAttributes attr);

private:
    pthread_mutex_t _lock;

    struct FontData
    {
        FontData(IDirectFBFont* f)
                : font(f),
                  ref(1)
        {
        }
        IDirectFBFont* font;
        unsigned int ref;
    };

    typedef std::map<unsigned int, FontData> CacheMap;
    CacheMap _cache;

    FontCache();

    FontCache(FontCache const&);

    FontCache&
    operator=(FontCache const&);

    virtual
    ~FontCache();

    IDirectFBFont*
    getEntryFromFile(unsigned int key, const std::string& file, int size, DFBFontAttributes attr);

    std::string
    getFCFileName(const char* name, const char* style, double size, int slant);

    static FontCache* __instance;

};

} /* namespace ilixi */
#endif /* ILIXI_FONTCACHE_H_ */
