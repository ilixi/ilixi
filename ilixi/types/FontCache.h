/*
 Copyright 2010-2013 Tarik Sekmen.

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
//! Application wide font cache.
/*!
 * FontCache stores a map of loaded fonts for ease of .
 *
 * Fontconfig is used for finding fonts with given parameters.
 */
class FontCache
{
    friend class PlatformManager;
public:
    /*!
     * Returns singleton instance.
     */
    static FontCache*
    Instance();

    /*!
     * Returns a hash value for given font parameters.
     *
     * @param name Font name, e.g. Sans.
     * @param size Font size, e.g. 12.
     * @param attr DirectFB font attributes.
     */
    unsigned int
    getKey(const std::string& name, int size, DFBFontAttributes attr);

    /*!
     * Returns a font and a hash value for given font parameters.
     *
     * @param name Font name, e.g. Sans.
     * @param size Font size, e.g. 12.
     * @param attr DirectFB font attributes.
     * @param font This parameter is set with requested font.
     */
    unsigned int
    getEntry(const std::string& name, int size, DFBFontAttributes attr, IDirectFBFont** font);

    /*!
     * Releases reference to font with given hash value.
     */
    void
    releaseEntry(unsigned int key);

    /*!
     * Releases reference to font with given parameters.
     *
     * @param name Font name, e.g. Sans.
     * @param size Font size, e.g. 12.
     * @param attr DirectFB font attributes.
     */
    void
    releaseEntry(const char* name, int size, DFBFontAttributes attr);

private:
    //! This mutex locks cache map for access.
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

    void
    releaseAllEntries();

    static FontCache* __instance;

};

} /* namespace ilixi */
#endif /* ILIXI_FONTCACHE_H_ */
