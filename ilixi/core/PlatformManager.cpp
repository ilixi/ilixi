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

#include <core/PlatformManager.h>
#include <core/Logger.h>
#include <ui/Application.h>
#include <lib/FileSystem.h>
#include <lib/XMLReader.h>
#include <types/FontCache.h>
#include <algorithm>

extern "C"
{
#include <directfb_strings.h>
}

#if ILIXI_HAVE_FUSIONDALE
#include <core/DaleDFB.h>
#endif

#if ILIXI_HAVE_FUSIONSOUND
#include <core/SoundDFB.h>
#endif

#ifdef ILIXI_HAVE_NLS
#include <libintl.h>
#include <types/I18NBase.h>

static void
setI18NLanguage(const char* lang)
{
    setenv("LANGUAGE", lang, 1);
    {
        extern int _nl_msg_cat_cntr;
        ++_nl_msg_cat_cntr;
    }
}
#endif

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_PLATFORMMANAGER, "ilixi/core/PlatformManager", "PlatformManager");
D_DEBUG_DOMAIN( ILX_PLATFORMMANAGER_TRACE, "ilixi/core/PlatformManagerTrace", "PlatformManagerTrace");

static DirectFBPixelFormatNames( format_names );

PlatformManager&
PlatformManager::instance()
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER_TRACE);
    static PlatformManager instance;
    return instance;
}

IDirectFB*
PlatformManager::getDFB() const
{
    return _dfb;
}

IDirectFBDisplayLayer*
PlatformManager::getLayer(const std::string& name) const
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    LogicLayerMap::const_iterator it = _layerMap.find(name);
    if (it != _layerMap.end())
    {
        HardwareLayerMap::const_iterator itHW = _hwLayerMap.find(it->second.id);
        if (itHW != _hwLayerMap.end())
        {
            ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> layer: %p\n", itHW->second.layer);
            return itHW->second.layer;
        }
        ILOG_ERROR(ILX_PLATFORMMANAGER, " -> Cannot find hw layer for logic layer %s!\n", name.c_str());
        return NULL;
    }
    ILOG_ERROR(ILX_PLATFORMMANAGER, " -> Cannot find logic layer %s\n", name.c_str());
    return NULL;
}

DFBDisplayLayerID
PlatformManager::getLayerID(const std::string& name) const
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    LogicLayerMap::const_iterator it = _layerMap.find(name);
    if (it != _layerMap.end())
        return it->second.id;
    ILOG_ERROR(ILX_PLATFORMMANAGER, " -> Cannot find logic layer %s\n", name.c_str());
    return DLID_PRIMARY;
}

IDirectFBSurface*
PlatformManager::getLayerSurface(const std::string& name) const
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    if (_options & OptExclusive)
    {
        LogicLayerMap::const_iterator it = _layerMap.find(name);
        if (it != _layerMap.end())
            return it->second.surface;
    }
    return NULL;
}

Size
PlatformManager::getLayerSize(const std::string& name) const
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    IDirectFBDisplayLayer* layer = getLayer(name);
    if (layer)
    {
        int w, h;
        DFBDisplayLayerConfig config;
        layer->GetConfiguration(layer, &config);
        return Size(config.width, config.height);
    }
    return Size(0, 0);
}

AppOptions
PlatformManager::appOptions() const
{
    return _options;
}

LayerFlipMode
PlatformManager::getLayerFlipMode(const std::string& name) const
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER_TRACE);
    if (_options & OptExclusive)
    {
        LogicLayerMap::const_iterator it = _layerMap.find(name);
        if (it != _layerMap.end())
        {
            HardwareLayerMap::const_iterator itHW = _hwLayerMap.find(it->second.id);
            if (itHW != _hwLayerMap.end())
            {
                ILOG_DEBUG(ILX_PLATFORMMANAGER_TRACE, " -> layer: %p mode: %d\n", itHW->second.layer, itHW->second.flipMode);
                return itHW->second.flipMode;
            }
            return FlipNone;
        }
    } else
        return _windowConf.flipMode;
    return FlipNone;
}

bool
PlatformManager::useFSU(const std::string& name) const
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER_TRACE);
    if (_options & OptExclusive)
    {
        LogicLayerMap::const_iterator it = _layerMap.find(name);
        if (it != _layerMap.end())
        {
            HardwareLayerMap::const_iterator itHW = _hwLayerMap.find(it->second.id);
            if (itHW != _hwLayerMap.end())
            {
                ILOG_DEBUG(ILX_PLATFORMMANAGER_TRACE, " -> layer: %p fsu: %d\n", itHW->second.layer, itHW->second.fsu);
                return itHW->second.fsu;
            }
            return false;
        }
    } else
        return _windowConf.fsu;
    return false;
}

DFBSurfaceCapabilities
PlatformManager::getWindowSurfaceCaps() const
{
    return _windowConf.caps;
}

const std::string&
PlatformManager::getFontPack() const
{
    return _fontPack;
}

const std::string&
PlatformManager::getIconPack() const
{
    return _iconPack;
}

const std::string&
PlatformManager::getPalette() const
{
    return _palette;
}

const std::string&
PlatformManager::getStyle() const
{
    return _style;
}

const std::string&
PlatformManager::getBackground() const
{
    return _background;
}

bool
PlatformManager::cursorVisible() const
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER_TRACE);

    if ((_options & OptExclusive) && _cursorImage)
        return true;

    return false;
}

DFBSurfacePixelFormat
PlatformManager::forcedPixelFormat() const
{
    return _pixelFormat;
}

void
PlatformManager::renderCursor(const DFBPoint& point)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER_TRACE);
    if ((_options & OptExclusive) && _cursorImage)
    {
        if (_cursorTarget)
        {
            _cursorTarget->SetClip(_cursorTarget, NULL);
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
            _cursorTarget->SetStereoEye(_cursorTarget, DSSE_LEFT);
#endif
            _cursorTarget->SetBlittingFlags(_cursorTarget, DSBLIT_BLEND_ALPHACHANNEL);
            _cursorTarget->Blit(_cursorTarget, _cursorImage, NULL, point.x, point.y);
            ILOG_DEBUG(ILX_PLATFORMMANAGER_TRACE, " -> %d, %d\n", point.x, point.y);
        } else
            _cursorLayer->SetScreenPosition(_cursorLayer, point.x, point.y);
    }
}

void
PlatformManager::playSoundEffect(const std::string& id)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
#if ILIXI_HAVE_FUSIONSOUND
#if ILIXI_HAVE_FUSIONDALE
    if (_options & OptExclSoundEffect)
        DaleDFB::playSoundEffect(id);
    else
    {
#endif // ILIXI_HAVE_FUSIONDALE
        SoundMap::const_iterator it = _soundMap.find(id);
        if (it != _soundMap.end())
            it->second->start();
#if ILIXI_HAVE_FUSIONDALE
    }
#endif // ILIXI_HAVE_FUSIONDALE
#endif
}

void
PlatformManager::setSoundEffectLevel(float level)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
#if ILIXI_HAVE_FUSIONSOUND
#if ILIXI_HAVE_FUSIONDALE
    if (_options & OptExclSoundEffect)
        DaleDFB::setSoundEffectLevel(level);
    else if (_soundLevel != level)
    {
#endif // ILIXI_HAVE_FUSIONDALE
        _soundLevel = level;
        for (SoundMap::iterator it = _soundMap.begin(); it != _soundMap.end(); ++it)
            it->second->setVolume(_soundLevel);
#if ILIXI_HAVE_FUSIONDALE
    }
#endif // ILIXI_HAVE_FUSIONDALE
#endif
}

#ifdef ILIXI_HAVE_NLS
bool
PlatformManager::addI18N(I18NBase* tb)
{
    if (!tb)
        return false;

    pthread_mutex_lock(&_tbListMutex);
    I18NBaseList::iterator it = std::find(_tbList.begin(), _tbList.end(), tb);
    if (tb == *it)
    {
        pthread_mutex_unlock(&_tbListMutex);
        ILOG_DEBUG(ILX_PLATFORMMANAGER, "I18NBase %p already added!\n", tb);
        return false;
    }
    _tbList.push_back(tb);
    pthread_mutex_unlock(&_tbListMutex);
    ILOG_DEBUG(ILX_PLATFORMMANAGER, "I18NBase %p is added.\n", tb);
    return true;
}

bool
PlatformManager::removeI18N(I18NBase* tb)
{
    if (!tb)
        return false;

    pthread_mutex_lock(&_tbListMutex);
    I18NBaseList::iterator it = std::find(_tbList.begin(), _tbList.end(), tb);
    if (tb == *it)
    {
        _tbList.erase(it);
        pthread_mutex_unlock(&_tbListMutex);
        ILOG_DEBUG(ILX_PLATFORMMANAGER, "I18NBase %p is removed.\n", tb);
        return true;
    }
    pthread_mutex_unlock(&_tbListMutex);
    return false;
}

void
PlatformManager::setLanguage(const char* lang)
{
    _options = (AppOptions) (_options | OptNoUpdates);
    std::string langFont = _fontPack;

    unsigned uscr = langFont.find("_");
    if (uscr != std::string::npos)
        langFont.replace(uscr + 1, 5, PrintF("%s", lang));

    ILOG_ERROR(ILX_PLATFORMMANAGER, " LANGFONT: %s\n", langFont.c_str());

    if (langFont != _fontPack && FileSystem::fileExists(langFont) && Application::setFontPack(langFont.c_str()))
    {
        _fontPack = langFont;
        ILOG_INFO(ILX_PLATFORMMANAGER, "FontPack changed to %s\n", langFont.c_str());
    }

    setI18NLanguage(lang);
    for (I18NBaseList::iterator it = _tbList.begin(); it != _tbList.end(); ++it)
        ((I18NBase*) *it)->updateI18nText();
    _options = (AppOptions) (_options & ~OptNoUpdates);
}
#endif

PlatformManager::PlatformManager()
        : _options(OptNone),
          _dfb(NULL),
          _cursorLayer(NULL),
          _cursorTarget(NULL),
          _cursorImage(NULL),
          _pixelFormat(DSPF_UNKNOWN)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
#ifdef ILIXI_HAVE_NLS
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&_tbListMutex, &attr);
#endif
}

PlatformManager::~PlatformManager()
{
#ifdef ILIXI_HAVE_NLS
    pthread_mutex_destroy(&_tbListMutex);
#endif
}

void
PlatformManager::initialize(int* argc, char*** argv, AppOptions opts)
{
    if (!_dfb)
    {
        _options = opts;

        if (argc && argv)
        {
            for (int i = 1; i < *argc; i++)
            {
                if (strncmp((*argv)[i], "--ilx:", 6) == 0)
                {
                    parseArgs((*argv)[i] + 6);
                    (*argv)[i] = NULL;
                }
            }

            for (int i = 1; i < *argc; i++)
            {
                int k;
                for (k = i; k < *argc; k++)
                    if ((*argv)[k] != NULL)
                        break;

                if (k > i)
                {
                    k -= i;
                    for (int j = i + k; j < *argc; j++)
                        (*argv)[j - k] = (*argv)[j];
                    *argc -= k;
                }
            }
        }

        ILOG_DEBUG(ILX_PLATFORMMANAGER, "Initialising DirectFB interfaces...\n");

        if (DirectFBInit(argc, argv) != DFB_OK)
            ILOG_THROW(ILX_PLATFORMMANAGER, "DirectFBInit() failed!\n");

        if (DirectFBCreate(&_dfb) != DFB_OK)
            ILOG_THROW(ILX_PLATFORMMANAGER, "DirectFBCreate() failed!\n");

        ILOG_INFO(ILX_PLATFORMMANAGER, "DirectFB interfaces are ready.\n");

#if ILIXI_HAVE_FUSIONSOUND
        if ((_options & OptSound) && SoundDFB::initSound(argc, argv) == DFB_OK)
        {
            ILOG_INFO(ILX_PLATFORMMANAGER, "FusionSound is ready.\n");

            if (!(_options & OptExclusive) && FileSystem::fileExists(FileSystem::homeDirectory().append("/ilx_compositor.pid")))
            {
                ILOG_DEBUG(ILX_PLATFORMMANAGER, "Enabling OptExclSoundEffect.\n");
                _options = (AppOptions) (_options | OptExclSoundEffect | OptDale);
            }
        }
#endif

#if ILIXI_HAVE_FUSIONDALE
        if ((_options & OptDale) && DaleDFB::initDale(argc, argv) == DFB_OK)
            ILOG_INFO(ILX_PLATFORMMANAGER, "FusionDale is ready.\n");
#endif

        if (!parseConfig())
            ILOG_THROW(ILX_PLATFORMMANAGER, "Please modify your configuration file!\n");

    } else
        ILOG_WARNING(ILX_PLATFORMMANAGER, "DirectFB interfaces are already initialised.\n");

#ifdef ILIXI_HAVE_NLS
    ILOG_INFO(ILX_PLATFORMMANAGER, "Locale is: %s\n", setlocale(LC_ALL,""));
#endif
}

void
PlatformManager::release()
{
    if (_dfb)
    {
        ILOG_TRACE_F(ILX_PLATFORMMANAGER);

        FontCache::Instance()->releaseAllEntries();

        if ((appOptions() & OptExclusive) && _cursorImage)
            _cursorImage->Release(_cursorImage);

#if ILIXI_HAVE_FUSIONDALE
        if (_options & OptDale)
        {
            ILOG_DEBUG(ILX_PLATFORMMANAGER, "Releasing FusionDale...\n");
            DaleDFB::releaseDale();
            ILOG_INFO(ILX_PLATFORMMANAGER, "FusionDale interfaces are released.\n");
        }
#endif

#if ILIXI_HAVE_FUSIONSOUND
        if (_options & OptSound)
        {
            ILOG_DEBUG(ILX_PLATFORMMANAGER, "Releasing SoundMap...\n");
            for (SoundMap::iterator it = _soundMap.begin(); it != _soundMap.end(); ++it)
                delete it->second;

            ILOG_DEBUG(ILX_PLATFORMMANAGER, "Releasing FusionSound...\n");
            SoundDFB::releaseSound();
            ILOG_INFO(ILX_PLATFORMMANAGER, "FusionSound interfaces are released.\n");
        }
#endif

        if (_options & OptExclusive)
        {
            for (LogicLayerMap::iterator it = _layerMap.begin(); it != _layerMap.end(); ++it)
            {
                ILOG_DEBUG(ILX_PLATFORMMANAGER, "Releasing logic layer [%s] surface.\n", it->first.c_str());
                it->second.surface->Release(it->second.surface);
                it->second.surface = NULL;
            }
        }

        for (HardwareLayerMap::iterator it = _hwLayerMap.begin(); it != _hwLayerMap.end(); ++it)
        {
            ILOG_DEBUG(ILX_PLATFORMMANAGER, "Releasing layer [%d]\n", it->first);
            it->second.layer->Release(it->second.layer);
            it->second.layer = NULL;
        }

        ILOG_DEBUG(ILX_PLATFORMMANAGER, "Releasing DirectFB interfaces...\n");

        _dfb->Release(_dfb);
        _dfb = NULL;

        ILOG_INFO(ILX_PLATFORMMANAGER, "DirectFB interfaces are released.\n");
    }
}

void
PlatformManager::parseArgs(const char *args)
{
    char* arg = strdup(args);
    char* value;
    char* next;

    while (arg && arg[0])
    {
        if ((next = strchr(arg, ',')) != NULL)
            *next++ = '\0';

        if ((value = strchr(arg, '=')) != NULL)
            *value++ = '\0';

        if (strcmp(arg, "exclusive") == 0)
            _options = (AppOptions) (_options | OptExclusive);
        else if (strcmp(arg, "pixelformat") == 0)
            setPixelFormat(value);

        arg = next;
    }

    free(arg);
}

bool
PlatformManager::parseConfig()
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);

    XMLReader xml;
    if (xml.loadFile((ILIXI_DATADIR"ilixi_config.xml")) == false)
    {
        ILOG_FATAL(ILX_PLATFORMMANAGER, "Could not parse platform configuration!\n");
        return false;
    }

    xmlNodePtr group = xml.currentNode();

    while (group != NULL)
    {
        ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> Parsing %s...\n", group->name);

        if (xmlStrcmp(group->name, (xmlChar*) "HardwareLayers") == 0)
            setHardwareLayers(group->children);
        else if (xmlStrcmp(group->name, (xmlChar*) "LogicLayers") == 0)
            setLogicLayers(group->children);
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
        else if (xmlStrcmp(group->name, (xmlChar*) "Screen") == 0)
            setScreen(group->children);
#endif
        else if (xmlStrcmp(group->name, (xmlChar*) "Window") == 0)
            setWindow(group->children);
        else if (xmlStrcmp(group->name, (xmlChar*) "Theme") == 0)
            setTheme(group->children);
#if ILIXI_HAVE_FUSIONSOUND
        else if (xmlStrcmp(group->name, (xmlChar*) "Sounds") == 0)
            setSounds(group);
#endif
        else if (xmlStrcmp(group->name, (xmlChar*) "Cursor") == 0)
            setCursor(group);

        else if (xmlStrcmp(group->name, (xmlChar*) "PixelFormat") == 0)
        {
            xmlChar* pcDATA = xmlNodeGetContent(group);
            setPixelFormat((char*) pcDATA);
            xmlFree(pcDATA);
        }

        group = group->next;
    }

    ILOG_INFO(ILX_PLATFORMMANAGER, "Parsed platform configuration file.\n");
    return true;
}

void
PlatformManager::setHardwareLayers(xmlNodePtr node)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    while (node != NULL)
    {
        xmlChar* idC = xmlGetProp(node, (xmlChar*) "id");
        xmlChar* fsuC = xmlGetProp(node, (xmlChar*) "fsu");
        xmlChar* flipModeC = xmlGetProp(node, (xmlChar*) "flipMode");
        xmlChar* bufferModeC = xmlGetProp(node, (xmlChar*) "bufferMode");

        unsigned int id = atoi((char*) idC);

        IDirectFBDisplayLayer* layer = NULL;
        DFBResult res = getDFB()->GetDisplayLayer(getDFB(), id, &layer);

        if (res != DFB_OK)
        {
            ILOG_ERROR(ILX_PLATFORMMANAGER, "Cannot get layer with id [%d]!\n", id);
            ILOG_THROW(ILX_PLATFORMMANAGER, "Please fix your platform configuration file.\n");
        } else
        {
            HardwareLayer info;
            info.fsu = false;
            if (xmlStrcmp(fsuC, (xmlChar*) "on") == 0)
                info.fsu = true;
            info.flipMode = FlipNone;
            info.layer = layer;

            std::pair<HardwareLayerMap::iterator, bool> ret = _hwLayerMap.insert(std::pair<unsigned int, HardwareLayer>(id, info));
            if (ret.second == false)
                ILOG_ERROR(ILX_PLATFORMMANAGER, "A layer with id [%d] already exists, cannot add duplicate record!\n", id);
            else if (_options & OptExclusive)
            {
                if (layer->SetCooperativeLevel(layer, DLSCL_EXCLUSIVE))
                    ILOG_ERROR(ILX_PLATFORMMANAGER, "Error while setting EXLUSIVE mode!\n");
                else
                    ILOG_INFO(ILX_PLATFORMMANAGER, " -> Now running in exclusive mode on layer [%d].\n", id);

                DFBDisplayLayerConfig config;
                config.flags = (DFBDisplayLayerConfigFlags) (DLCONF_BUFFERMODE | DLCONF_OPTIONS);

                config.buffermode = DLBM_UNKNOWN;

                if (xmlStrcmp(bufferModeC, (xmlChar*) "frontOnly") == 0)
                    config.buffermode = DLBM_FRONTONLY;
                else if (xmlStrcmp(bufferModeC, (xmlChar*) "backVideo") == 0)
                    config.buffermode = DLBM_BACKVIDEO;
                // wait for sync
                else if (xmlStrcmp(bufferModeC, (xmlChar*) "backSystem") == 0)
                    config.buffermode = DLBM_BACKSYSTEM;
                else if (xmlStrcmp(bufferModeC, (xmlChar*) "triple") == 0)
                    config.buffermode = DLBM_TRIPLE;
                // onsync
                else if (xmlStrcmp(bufferModeC, (xmlChar*) "windows") == 0)
                    config.buffermode = DLBM_WINDOWS;

                if (config.buffermode == DLBM_UNKNOWN)
                {
                    DFBGraphicsDeviceDescription deviceDesc;
                    getDFB()->GetDeviceDescription(getDFB(), &deviceDesc);
                    if (deviceDesc.acceleration_mask == DFXL_NONE)
                    {
                        config.buffermode = DLBM_BACKSYSTEM;
                        ret.first->second.fsu = false;
                        ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> Not using full-screen updates\n");
                    } else
                    {
                        config.buffermode = DLBM_BACKVIDEO;
                        //setAppOption(OptTripleAccelerated);
                    }
                }

                if (config.buffermode != DLBM_FRONTONLY)
                {
                    if (xmlStrcmp(flipModeC, (xmlChar*) "onSync") == 0)
                        ret.first->second.flipMode = FlipOnSync;
                    else if (xmlStrcmp(flipModeC, (xmlChar*) "waitForSync") == 0)
                        ret.first->second.flipMode = FlipWaitForSync;
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,7,0)
                    else if (xmlStrcmp(flipModeC, (xmlChar*) "new") == 0)
                        ret.first->second.flipMode = FlipNew;
#endif
                }

#ifdef ILIXI_STEREO_OUTPUT
                config.options = DLOP_STEREO;
#else
                config.options = DLOP_NONE;
#endif

                res = layer->SetConfiguration(layer, &config);
                if (res != DFB_OK)
                {
                    ILOG_ERROR(ILX_PLATFORMMANAGER, "Cannot set buffermode: 0x%08x on layer [%d] - %s\n", config.buffermode, id, DirectFBErrorString(res));

                    if (config.buffermode != DLBM_BACKSYSTEM)
                    {
                        config.buffermode = DLBM_BACKSYSTEM;
                        ILOG_INFO(ILX_PLATFORMMANAGER, "Setting buffermode to BACKSYSTEM...\n");
                        res = layer->SetConfiguration(layer, &config);
                        if (res != DFB_OK)
                        {
                            ILOG_ERROR(ILX_PLATFORMMANAGER, "Cannot set buffermode: 0x%08x on layer [%d] - %s\n", config.buffermode, id, DirectFBErrorString(res));
                            ILOG_THROW(ILX_PLATFORMMANAGER, "Please fix your platform configuration file.\n");
                        }
                    } else
                        ILOG_THROW(ILX_PLATFORMMANAGER, "Please fix your platform configuration file.\n");

                } else
                    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> buffermode: 0x%08x\n", config.buffermode);
            }
        }

        xmlFree(idC);
        xmlFree(fsuC);
        xmlFree(flipModeC);
        xmlFree(bufferModeC);

        node = node->next;
    }

}

void
PlatformManager::setLogicLayers(xmlNodePtr node)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    while (node != NULL)
    {
        xmlChar* nameC = xmlGetProp(node, (xmlChar*) "name");
        xmlChar* refC = xmlGetProp(node, (xmlChar*) "hwID");

        LogicLayer info;
        unsigned int id = atoi((char*) refC);
        info.id = id;

        HardwareLayerMap::iterator it = _hwLayerMap.find(id);
        if (it == _hwLayerMap.end())
            ILOG_ERROR(ILX_PLATFORMMANAGER, "Cannot add %s because there is no hw layer with id [%d]!\n", (char*) nameC, id);
        else
        {
            std::pair<LogicLayerMap::iterator, bool> ret = _layerMap.insert(std::pair<std::string, LogicLayer>((char*) nameC, info));
            if (ret.second == false)
                ILOG_ERROR(ILX_PLATFORMMANAGER, "A logic layer with name [%s] already exists, cannot add duplicate record!\n", (char*) nameC);
            else if (_options & OptExclusive)
            {
                if (it->second.layer->GetSurface(it->second.layer, &ret.first->second.surface) != DFB_OK)
                    ILOG_THROW(ILX_PLATFORMMANAGER, "Error while getting layer surface!\n");

                ret.first->second.surface->Clear(ret.first->second.surface, 0, 0, 0, 0);
                ret.first->second.surface->Flip(ret.first->second.surface, NULL, DSFLIP_NONE);
            }
            ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> Added logic layer [%s] \n", (char*)nameC);
        }

        xmlFree(refC);
        xmlFree(nameC);

        node = node->next;
    }
}

#if ILIXI_DFB_VERSION >= VERSION_CODE(1, 6, 0)

void PlatformManager::setScreen(xmlNodePtr node)
{
    if (!(_options & OptExclusive))
    return;

    ILOG_TRACE_F(ILX_PLATFORMMANAGER);

    IDirectFBScreen* pScreen;
    DFBScreenEncoderConfig sEncoderCfg;
    DFBScreenEncoderDescription sEncoderDesc;
    sEncoderCfg.flags = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_SCANMODE | DSECONF_FREQUENCY | DSECONF_CONNECTORS | DSECONF_RESOLUTION | DSECONF_FRAMING);

    while (node != NULL)
    {
        xmlChar* pcDATA = xmlNodeGetContent(node);

        if (xmlStrcmp(node->name, (xmlChar*) "TvStandard") == 0)
        setScreenTvStandard(&sEncoderCfg.tv_standard, pcDATA);
        else if (xmlStrcmp(node->name, (xmlChar*) "OutConnectors") == 0)
        setScreenOutConnectors(&sEncoderCfg.out_connectors, pcDATA);
        else if (xmlStrcmp(node->name, (xmlChar*) "ScanMode") == 0)
        setScreenScanMode(&sEncoderCfg.scanmode, pcDATA);
        else if (xmlStrcmp(node->name, (xmlChar*) "Resolution") == 0)
        setScreenResolution(&sEncoderCfg.resolution, pcDATA);
        else if (xmlStrcmp(node->name, (xmlChar*) "Frequency") == 0)
        setScreenFrequency(&sEncoderCfg.frequency, pcDATA);

        xmlFree(pcDATA);
        node = node->next;
    }

    getLayer()->GetScreen(getLayer(), &pScreen);

    // Find the most appropriate framing encoder configuration
    pScreen->GetEncoderDescriptions(pScreen, &sEncoderDesc);

    if (sEncoderDesc.all_framing & DSEPF_STEREO_FRAME_PACKING)
    sEncoderCfg.framing = DSEPF_STEREO_FRAME_PACKING;
    else if (sEncoderDesc.all_framing & DSEPF_STEREO_SIDE_BY_SIDE_FULL)
    sEncoderCfg.framing = DSEPF_STEREO_SIDE_BY_SIDE_FULL;
    else if (sEncoderDesc.all_framing & DSEPF_STEREO_SIDE_BY_SIDE_HALF)
    sEncoderCfg.framing = DSEPF_STEREO_SIDE_BY_SIDE_HALF;
    else if (sEncoderDesc.all_framing & DSEPF_STEREO_TOP_AND_BOTTOM)
    sEncoderCfg.framing = DSEPF_STEREO_TOP_AND_BOTTOM;
    else
    sEncoderCfg.framing = DSEPF_MONO;

    DFBResult err = pScreen->SetEncoderConfiguration(pScreen, 0, &sEncoderCfg);
    if (err == DFB_UNSUPPORTED)
    {
        ILOG_WARNING(ILX_PLATFORMMANAGER, "Cannot set screen encoder configuration!\n");
        ILOG_WARNING(ILX_PLATFORMMANAGER, "Setting encoder framing to DSEPF_MONO...\n");
        sEncoderCfg.framing = DSEPF_MONO;
        pScreen->SetEncoderConfiguration(pScreen, 0, &sEncoderCfg);
    }

    pScreen->Release(pScreen);
}

void
PlatformManager::setScreenTvStandard(DFBScreenEncoderTVStandards* cfg, xmlChar* standards)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> pcDATA: %s\n", standards);
    *cfg = DSETV_UNKNOWN;
    char* pch = strtok((char*) standards, " ,");
    while (pch != NULL)
    {
        if (strcmp(pch, "PAL") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_PAL);
        else if (strcmp(pch, "NTSC") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_NTSC);
        else if (strcmp(pch, "SECAM") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_SECAM);
        else if (strcmp(pch, "PAL_60") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_PAL_60);
        else if (strcmp(pch, "PAL_BG") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_PAL_BG);
        else if (strcmp(pch, "PAL_I") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_PAL_I);
        else if (strcmp(pch, "PAL_M") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_PAL_M);
        else if (strcmp(pch, "PAL_N") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_PAL_N);
        else if (strcmp(pch, "PAL_NC") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_PAL_NC);
        else if (strcmp(pch, "NTSC_M_JPN") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_NTSC_M_JPN);
        else if (strcmp(pch, "DIGITAL") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_DIGITAL);
        else if (strcmp(pch, "NTSC_443") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_NTSC_443);
        else if (strcmp(pch, "ALL") == 0)
            *cfg = (DFBScreenEncoderTVStandards) (*cfg | DSETV_ALL);

        pch = strtok(NULL, " ,");
    }
    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> tv_standard: 0x%08x\n", *cfg);
}

void
PlatformManager::setScreenOutConnectors(DFBScreenOutputConnectors* cfg, xmlChar* connectors)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> pcDATA: %s\n", connectors);
    *cfg = DSOC_UNKNOWN;
    char* pch = strtok((char*) connectors, " ,");
    while (pch != NULL)
    {
        if (strcmp(pch, "VGA") == 0)
            *cfg = (DFBScreenOutputConnectors) (*cfg | DSOC_VGA);
        else if (strcmp(pch, "SCART") == 0)
            *cfg = (DFBScreenOutputConnectors) (*cfg | DSOC_SCART);
        else if (strcmp(pch, "YC") == 0)
            *cfg = (DFBScreenOutputConnectors) (*cfg | DSOC_YC);
        else if (strcmp(pch, "CVBS") == 0)
            *cfg = (DFBScreenOutputConnectors) (*cfg | DSOC_CVBS);
        else if (strcmp(pch, "SCART2") == 0)
            *cfg = (DFBScreenOutputConnectors) (*cfg | DSOC_SCART2);
        else if (strcmp(pch, "COMPONENT") == 0)
            *cfg = (DFBScreenOutputConnectors) (*cfg | DSOC_COMPONENT);
        else if (strcmp(pch, "HDMI") == 0)
            *cfg = (DFBScreenOutputConnectors) (*cfg | DSOC_HDMI);
        else if (strcmp(pch, "656") == 0)
            *cfg = (DFBScreenOutputConnectors) (*cfg | DSOC_656);

        pch = strtok(NULL, " ,");
    }

    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> out_connectors: 0x%08x\n", *cfg);
}

void
PlatformManager::setScreenScanMode(DFBScreenEncoderScanMode* cfg, xmlChar* scanmode)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> pcDATA: %s\n", scanmode);
    *cfg = DSESM_UNKNOWN;

    if (xmlStrcmp(scanmode, (xmlChar*) "INTERLACED") == 0)
        *cfg = DSESM_INTERLACED;
    else if (xmlStrcmp(scanmode, (xmlChar*) "PROGRESSIVE") == 0)
        *cfg = DSESM_PROGRESSIVE;

    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> scanmode: 0x%08x\n", *cfg);
}

void
PlatformManager::setScreenResolution(DFBScreenOutputResolution* cfg, xmlChar* resolution)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> pcDATA: %s\n", resolution);
    *cfg = DSOR_UNKNOWN;

    if (xmlStrcmp(resolution, (xmlChar*) "640_480") == 0)
        *cfg = DSOR_640_480;
    else if (xmlStrcmp(resolution, (xmlChar*) "720_480") == 0)
        *cfg = DSOR_720_480;
    else if (xmlStrcmp(resolution, (xmlChar*) "720_576") == 0)
        *cfg = DSOR_720_576;
    else if (xmlStrcmp(resolution, (xmlChar*) "800_600") == 0)
        *cfg = DSOR_800_600;
    else if (xmlStrcmp(resolution, (xmlChar*) "1024_768") == 0)
        *cfg = DSOR_1024_768;
    else if (xmlStrcmp(resolution, (xmlChar*) "1152_864") == 0)
        *cfg = DSOR_1152_864;
    else if (xmlStrcmp(resolution, (xmlChar*) "1280_720") == 0)
        *cfg = DSOR_1280_720;
    else if (xmlStrcmp(resolution, (xmlChar*) "1280_768") == 0)
        *cfg = DSOR_1280_768;
    else if (xmlStrcmp(resolution, (xmlChar*) "1280_960") == 0)
        *cfg = DSOR_1280_960;
    else if (xmlStrcmp(resolution, (xmlChar*) "1280_1024") == 0)
        *cfg = DSOR_1280_1024;
    else if (xmlStrcmp(resolution, (xmlChar*) "1400_1050") == 0)
        *cfg = DSOR_1400_1050;
    else if (xmlStrcmp(resolution, (xmlChar*) "1600_1200") == 0)
        *cfg = DSOR_1600_1200;
    else if (xmlStrcmp(resolution, (xmlChar*) "1920_1080") == 0)
        *cfg = DSOR_1920_1080;
    else if (xmlStrcmp(resolution, (xmlChar*) "960_540") == 0)
        *cfg = DSOR_960_540;
    else if (xmlStrcmp(resolution, (xmlChar*) "1440_540") == 0)
        *cfg = DSOR_1440_540;
    else if (xmlStrcmp(resolution, (xmlChar*) "ALL") == 0)
        *cfg = DSOR_ALL;

    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> resolution: 0x%08x\n", *cfg);
}

void
PlatformManager::setScreenFrequency(DFBScreenEncoderFrequency* cfg, xmlChar* freq)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> pcDATA: %s\n", freq);
    *cfg = DSEF_UNKNOWN;

    if (xmlStrcmp(freq, (xmlChar*) "25HZ") == 0)
        *cfg = DSEF_25HZ;
    else if (xmlStrcmp(freq, (xmlChar*) "29_97HZ") == 0)
        *cfg = DSEF_29_97HZ;
    else if (xmlStrcmp(freq, (xmlChar*) "50HZ") == 0)
        *cfg = DSEF_50HZ;
    else if (xmlStrcmp(freq, (xmlChar*) "59_94HZ") == 0)
        *cfg = DSEF_59_94HZ;
    else if (xmlStrcmp(freq, (xmlChar*) "60HZ") == 0)
        *cfg = DSEF_60HZ;
    else if (xmlStrcmp(freq, (xmlChar*) "75HZ") == 0)
        *cfg = DSEF_75HZ;
    else if (xmlStrcmp(freq, (xmlChar*) "30HZ") == 0)
        *cfg = DSEF_30HZ;
    else if (xmlStrcmp(freq, (xmlChar*) "24HZ") == 0)
        *cfg = DSEF_24HZ;
    else if (xmlStrcmp(freq, (xmlChar*) "23_976HZ") == 0)
        *cfg = DSEF_23_976HZ;

    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> frequency: 0x%08x\n", *cfg);
}

#endif // version > 1.6
void
PlatformManager::setWindow(xmlNodePtr node)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    _windowConf.fsu = false;
    _windowConf.flipMode = FlipNew;
    _windowConf.caps = DSCAPS_DOUBLE;

    while (node != NULL)
    {
        xmlChar* pcDATA = xmlNodeGetContent(node);
        if (xmlStrcmp(node->name, (xmlChar*) "FullScreenUpdate") == 0)
        {
            if (xmlStrcmp(pcDATA, (xmlChar*) "on") == 0)
                _windowConf.fsu = true;
        } else if (xmlStrcmp(node->name, (xmlChar*) "FlipMode") == 0)
        {
            if (xmlStrcmp(pcDATA, (xmlChar*) "none") == 0)
                _windowConf.flipMode = FlipNone;
            else if (xmlStrcmp(pcDATA, (xmlChar*) "onSync") == 0)
                _windowConf.flipMode = FlipOnSync;
            else if (xmlStrcmp(pcDATA, (xmlChar*) "waitForSync") == 0)
                _windowConf.flipMode = FlipWaitForSync;
        } else if (xmlStrcmp(node->name, (xmlChar*) "BufferMode") == 0)
        {
            if (xmlStrcmp(pcDATA, (xmlChar*) "single") == 0)
                _windowConf.caps = DSCAPS_NONE;
            else if (xmlStrcmp(pcDATA, (xmlChar*) "triple") == 0)
                _windowConf.caps = DSCAPS_TRIPLE;
        }

        xmlFree(pcDATA);
        node = node->next;
    }

    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> fsu: %d\n", _windowConf.fsu);
    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> flipMode: %d\n", _windowConf.flipMode);
    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> caps: %x\n", _windowConf.caps);
}

void
PlatformManager::setTheme(xmlNodePtr node)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    while (node != NULL)
    {
        xmlChar* pcDATA = xmlNodeGetContent(node);
        std::string file = (char*) pcDATA;

        if (xmlStrcmp(node->name, (xmlChar*) "Background") == 0)
        {
            size_t found = file.find("@THEMEDIR:");
            if (found != std::string::npos)
            {
                _background = ILIXI_DATADIR"themes/";
                _background.append(file.substr(found + 10, std::string::npos));
            } else
                _background = file;
            ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> Background: %s\n", _background.c_str());
        } else if (xmlStrcmp(node->name, (xmlChar*) "Style") == 0)
        {
            size_t found = file.find("@THEMEDIR:");
            if (found != std::string::npos)
            {
                _style = ILIXI_DATADIR"themes/";
                _style.append(file.substr(found + 10, std::string::npos));
            } else
                _style = file;
            ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> Style: %s\n", _style.c_str());
        } else if (xmlStrcmp(node->name, (xmlChar*) "Palette") == 0)
        {
            size_t found = file.find("@THEMEDIR:");
            if (found != std::string::npos)
            {
                _palette = ILIXI_DATADIR"themes/";
                _palette.append(file.substr(found + 10, std::string::npos));
            } else
                _palette = file;
            ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> Palette: %s\n", _palette.c_str());
        } else if (xmlStrcmp(node->name, (xmlChar*) "FontPack") == 0)
        {
            size_t found = file.find("@THEMEDIR:");
            if (found != std::string::npos)
            {
                _fontPack = ILIXI_DATADIR"themes/";
                _fontPack.append(file.substr(found + 10, std::string::npos));
            } else
                _fontPack = file;
            ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> FontPack: %s\n", _fontPack.c_str());
        } else if (xmlStrcmp(node->name, (xmlChar*) "IconPack") == 0)
        {
            size_t found = file.find("@THEMEDIR:");
            if (found != std::string::npos)
            {
                _iconPack = ILIXI_DATADIR"themes/";
                _iconPack.append(file.substr(found + 10, std::string::npos));
            } else
                _iconPack = file;
            ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> IconPack: %s\n", _iconPack.c_str());
        }

        xmlFree(pcDATA);
        node = node->next;
    }
}

#if ILIXI_HAVE_FUSIONSOUND
void
PlatformManager::setSounds(xmlNodePtr node)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    if (!(_options & OptSound))
        return;

    xmlChar* level = xmlGetProp(node, (xmlChar*) "level");
    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> Sound level: %s\n", (char*) level);
    _soundLevel = atoi((char*) level);
    xmlFree(level);

    xmlNodePtr element = node->children;
    while (element != NULL)
    {
        xmlChar* nameC = xmlGetProp(element, (xmlChar*) "id");
        xmlChar* pcDATA = xmlNodeGetContent(element);
        std::string path = (char*) pcDATA;
        std::string file;
        size_t found = path.find("@SOUNDDIR:");
        if (found != std::string::npos)
        {
            file = ILIXI_DATADIR"sounds/";
            file.append(path.substr(found + 10, std::string::npos));
        } else
            file = path;

        Sound* sound = new Sound(file);

        std::pair<SoundMap::iterator, bool> ret = _soundMap.insert(std::pair<std::string, Sound*>((char*) nameC, sound));
        if (ret.second == false)
        {
            delete sound;
            ILOG_ERROR(ILX_PLATFORMMANAGER, "A sound with name [%s] already exists, cannot add duplicate record!\n", (char*) nameC);
        } else
        {
            sound->setVolume(_soundLevel);
            ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> Added %s - %s\n", (char*) nameC, file.c_str());
        }

        xmlFree(pcDATA);
        xmlFree(nameC);

        element = element->next;
    }
    for (SoundMap::iterator it = _soundMap.begin(); it != _soundMap.end(); ++it)
        it->second->setVolume(_soundLevel);
}
#endif // ILIXI_HAVE_FUSIONSOUND
void
PlatformManager::setCursor(xmlNodePtr node)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);

    if (!(_options & OptExclusive))
        return;

    xmlChar* visible = xmlGetProp(node, (xmlChar*) "visible");
    xmlChar* useLayer = xmlGetProp(node, (xmlChar*) "useLayer");

    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> visible: %s useLayer: %s\n", (char*) visible, (char*) useLayer);

    if (xmlStrcmp(visible, (xmlChar*) "no") == 0)
        return;

    xmlChar* pcDATA = xmlNodeGetContent(node);
    std::string path = (char*) pcDATA;
    std::string file;
    size_t found = path.find("@IMGDIR:");
    if (found != std::string::npos)
    {
        file = ILIXI_DATADIR"images/";
        file.append(path.substr(found + 8, std::string::npos));
    } else
        file = path;

    // setup cursor image
    IDirectFBImageProvider* provider;
    DFBSurfaceDescription desc;
    if (getDFB()->CreateImageProvider(getDFB(), file.c_str(), &provider) != DFB_OK)
        ILOG_THROW(ILX_PLATFORMMANAGER, "Error while creating image provider for cursor!\n");

    provider->GetSurfaceDescription(provider, &desc);
    desc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
    desc.caps = DSCAPS_PREMULTIPLIED;
    desc.pixelformat = DSPF_ARGB;

    if (getDFB()->CreateSurface(getDFB(), &desc, &_cursorImage) != DFB_OK)
        ILOG_THROW(ILX_PLATFORMMANAGER, "Error while creating cursor surface!\n");

    provider->RenderTo(provider, _cursorImage, NULL);
    provider->Release(provider);

    if (xmlStrcmp(useLayer, (xmlChar*) "yes") == 0)
    {
        // if cursor and ui are on same layer, use cursorTarget (i.e. not using individual hw layer)
        if (getLayer("cursor") == getLayer("ui"))
        {
            _cursorTarget = getLayerSurface("cursor");
            ILOG_INFO(ILX_PLATFORMMANAGER, " -> using cursor/ui logic layer for cursor.\n");
        } else // else if cursor is on an individual layer, try to use cursorLayer
        {
            ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> Will try to use cursor logic layer...\n");
            _cursorLayer = getLayer("cursor");
            if (_cursorLayer)
            {
                ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> setting layer rectangle (0, 0, %d, %d)\n", desc.width, desc.height);
                _cursorLayer->SetScreenRectangle(_cursorLayer, 0, 0, desc.width, desc.height);
                IDirectFBSurface* surface = getLayerSurface("cursor");
                if (surface)
                {
                    surface->Blit(surface, _cursorImage, NULL, 0, 0);
                    surface->Flip(surface, NULL, DSFLIP_NONE);
                    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> now using cursor logic layer for cursor.\n");
                } else
                    ILOG_ERROR(ILX_PLATFORMMANAGER, " -> cannot get cursor logic layer surface!\n");
            } else
                ILOG_ERROR(ILX_PLATFORMMANAGER, " -> cannot get cursor logic layer!\n");
        }

        if (!_cursorTarget && !_cursorLayer)
        {
            _cursorTarget = getLayerSurface("ui");
            ILOG_INFO(ILX_PLATFORMMANAGER, " -> using ui logic layer for cursor.\n");
        }
    }

    xmlFree(pcDATA);
    xmlFree(visible);
    xmlFree(useLayer);
}

bool
PlatformManager::setPixelFormat(const char* format)
{
    ILOG_TRACE_F(ILX_PLATFORMMANAGER);
    if (_pixelFormat != DSPF_UNKNOWN)
    {
        ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> pixelformat is already set.\n", _pixelFormat);
        return true;
    }

    if (strcasecmp(format, "DEFAULT") == 0)
        return true;

    int i = 0;
    ILOG_DEBUG(ILX_PLATFORMMANAGER, " -> %s\n", format);
    while (format_names[i].format != DSPF_UNKNOWN)
    {
        if (!strcasecmp(format, format_names[i].name))
        {
            _pixelFormat = format_names[i].format;
            return true;
        }
        ++i;
    }

    ILOG_WARNING(ILX_PLATFORMMANAGER, "Specified pixel format (%s) is invalid!\n", format);
    return false;
}

void
PlatformManager::setAppOption(AppOptions option)
{
    ILOG_DEBUG(ILX_PLATFORMMANAGER, "Setting option: 0x%02x\n", option);
    _options = (AppOptions) (_options | option);
}

void
PlatformManager::unSetAppOption(AppOptions option)
{
    ILOG_DEBUG(ILX_PLATFORMMANAGER, "Clearing option: 0x%02x\n", option);
    _options = (AppOptions) (_options & ~option);
}

} /* namespace ilixi */
