/*
 Copyright 2010-2015 Tarik Sekmen.

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

#include <types/FontCache.h>
#include <lib/Util.h>
#include <core/PlatformManager.h>
#include <core/Logger.h>
#include <ilixiConfig.h>
#include <sstream>
#include <fontconfig/fontconfig.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_FONTCACHE, "ilixi/types/FontCache", "FontCache");

FontCache* FontCache::__instance = NULL;

FontCache*
FontCache::Instance()
{
    if (!__instance)
    {
        __instance = new FontCache;
        FcConfig* config = FcConfigGetCurrent();
        FcConfigAppFontAddDir(config, (FcChar8*) ILIXI_DATADIR"fonts");
    }
    return __instance;
}

FontCache::FontCache()
{
    pthread_mutex_init(&_lock, NULL);
}

FontCache::FontCache(FontCache const&)
{
}

FontCache&
FontCache::operator=(FontCache const&)
{
}

FontCache::~FontCache()
{
    releaseAllEntries();
    pthread_mutex_destroy(&_lock);
}

unsigned int
FontCache::getKey(const std::string& name, int size, DFBFontAttributes attr)
{
    ILOG_TRACE_F(ILX_FONTCACHE);
    std::stringstream ss;
    ss << name << size << attr;
    return createHash(ss.str());
}

unsigned int
FontCache::getEntry(const std::string& name, int size, DFBFontAttributes attr, IDirectFBFont** font)
{
    ILOG_TRACE_F(ILX_FONTCACHE);
    ILOG_DEBUG(ILX_FONTCACHE, " -> name: %s\n", name.c_str());
    ILOG_DEBUG(ILX_FONTCACHE, " -> size: %d\n", size);
    std::string style = "regular";
    int slant = 0;

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    if (attr & DFFA_STYLE_BOLD)
        style = "bold";

    if (attr & DFFA_STYLE_ITALIC)
        slant = FC_SLANT_ITALIC;
#endif
    ILOG_DEBUG(ILX_FONTCACHE, " -> style: %s\n", style.c_str());
    unsigned int key = getKey(name, size, attr);
    ILOG_DEBUG(ILX_FONTCACHE, " -> key: %u\n", key);
    *font = getEntryFromFile(key, name, style, slant, size, attr);
    return key;
}

void
FontCache::releaseEntry(unsigned int key)
{
    ILOG_TRACE_F(ILX_FONTCACHE);
    pthread_mutex_lock(&_lock);
    CacheMap::iterator it = _cache.find(key);
    if (it != _cache.end())
    {
        if (--(it->second.ref))
        {
            ILOG_DEBUG(ILX_FONTCACHE, " -> Decrement ref counter for entry (%u)\n", key);
            pthread_mutex_unlock(&_lock);
            return;
        }
        // remove entry...
        ILOG_DEBUG(ILX_FONTCACHE, " -> Release font for entry (%u)\n", key);
        it->second.font->Release(it->second.font);
        _cache.erase(it);
    } else
        ILOG_DEBUG(ILX_FONTCACHE, " -> Key (%u) not found.\n", key);
    pthread_mutex_unlock(&_lock);
}

void
FontCache::releaseEntry(const char* name, int size, DFBFontAttributes attr)
{
    releaseEntry(getKey(name, size, attr));
}

void
FontCache::logEntries()
{
    ILOG_TRACE_F(ILX_FONTCACHE);
    pthread_mutex_lock(&_lock);
    ILOG_DEBUG(ILX_FONTCACHE, " -> Map size: %d\n", _cache.size());
    for (CacheMap::iterator it = _cache.begin(); it != _cache.end(); ++it)
        ILOG_DEBUG(ILX_FONTCACHE, "   -> %u: %p\n", it->first, it->second.font);
    pthread_mutex_unlock(&_lock);
}

IDirectFBFont*
FontCache::getEntryFromFile(unsigned int key, const std::string& name, const std::string& style, int slant, int size, DFBFontAttributes attr)
{
    ILOG_TRACE_F(ILX_FONTCACHE);

    pthread_mutex_lock(&_lock);
    CacheMap::iterator it = _cache.find(key);
    if (it == _cache.end())
    {
        // load and insert
        IDirectFBFont* font;
        DFBFontDescription desc;

        desc.flags = (DFBFontDescriptionFlags) (DFDESC_HEIGHT | DFDESC_ATTRIBUTES);
        desc.height = size;
        desc.attributes = attr;
        std::string file = getFCFileName(name.c_str(), style.c_str(), size, slant);
        DFBResult ret = PlatformManager::instance().getDFB()->CreateFont(PlatformManager::instance().getDFB(), file.c_str(), &desc, &font);
        if (ret)
        {
            ILOG_WARNING(ILX_FONTCACHE, " -> Loading failed for (%s, %d)!\n", file.c_str(), size);
            ILOG_WARNING(ILX_FONTCACHE, " -> Error: %s\n", DirectFBErrorString(ret));
            pthread_mutex_unlock(&_lock);
            return NULL;
        }

        CacheMap::iterator it2 = _cache.begin();
        FontData data(font);
        _cache.insert(it2, std::pair<unsigned int, FontData>(key, data));
        ILOG_DEBUG(ILX_FONTCACHE, " -> Cached key (%u) for (%s, %d)\n", key, file.c_str(), size);
        pthread_mutex_unlock(&_lock);

        return font;

    } else
    {
        ILOG_DEBUG(ILX_FONTCACHE, " -> Got from cache using key: %u\n", key);
        it->second.ref++;
        pthread_mutex_unlock(&_lock);
        return it->second.font;
    }
}

std::string
FontCache::getFCFileName(const char* name, const char* style, double size, int slant)
{
    ILOG_TRACE_F(ILX_FONTCACHE);
    FcPattern *pat, *match;
    FcResult result;

    ILOG_DEBUG(ILX_FONTCACHE, " -> name: %s\n", name);
    ILOG_DEBUG(ILX_FONTCACHE, " -> style: %s\n", style);
    ILOG_DEBUG(ILX_FONTCACHE, " -> size: %f\n", size);
    pat = FcPatternCreate();
    FcPatternAddString(pat, FC_FAMILY, (const FcChar8 *) name);
    FcPatternAddString(pat, FC_STYLE, (const FcChar8 *) style);
    FcPatternAddDouble(pat, FC_SIZE, size);
    FcPatternAddInteger(pat, FC_SLANT, slant);

    FcConfigSubstitute(NULL, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);
    match = FcFontMatch(NULL, pat, &result);

    FcChar8* fileName;
    FcPatternGetString(match, FC_FILE, 0, &fileName);
    ILOG_DEBUG(ILX_FONTCACHE, " -> Matched: %s\n", (char*) fileName);
    std::string font_match = (char*) fileName;
    FcPatternDestroy(match);
    FcPatternDestroy(pat);

    return font_match;
}

void
FontCache::releaseAllEntries()
{
    ILOG_TRACE_F(ILX_FONTCACHE);
    pthread_mutex_lock(&_lock);
    for (CacheMap::iterator it = _cache.begin(); it != _cache.end(); ++it)
        it->second.font->Release(it->second.font);
    _cache.clear();
    pthread_mutex_unlock(&_lock);
}

} /* namespace ilixi */
