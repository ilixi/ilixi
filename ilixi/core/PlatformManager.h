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

#ifndef ILIXI_PLATFORMMANAGER_H_
#define ILIXI_PLATFORMMANAGER_H_

#include <ilixiConfig.h>
#include <directfb.h>
#include <string>
#include <libxml/tree.h>
#include <map>
#include <types/Enums.h>
#include <types/Point.h>
#include <types/Size.h>
#ifdef ILIXI_HAVE_FUSIONSOUND
#include <types/Sound.h>
#endif
#include <lib/Util.h>

#ifdef ILIXI_HAVE_NLS
#include <list>
namespace ilixi
{
class I18NBase;
}
#endif

namespace ilixi
{

//! Configures and manages DirectFB interfaces.
/*!
 * PlatformManager helps with configuration and management of DirectFB interfaces.
 *
 * Settings are parsed from ilixi_config.xml file in DATADIR.
 */
class PlatformManager
{
    friend class Application;
    friend class Service;

public:
    /*!
     * Returns an instance of PlatformManager.
     */
    static PlatformManager&
    instance();

    /*!
     * Returns main DirectFB interface.
     */
    IDirectFB*
    getDFB() const;

    /*!
     * Returns a DFBLayer if logic layer name is matched, otherwise returns default ui layer.
     */
    IDirectFBDisplayLayer*
    getLayer(const std::string& name = "ui") const;

    /*!
     * Returns a DFBLayerID logic layer name is matched, otherwise returns DLID_PRIMARY.
     */
    DFBDisplayLayerID
    getLayerID(const std::string& name = "ui") const;

    /*!
     * Returns the layer surface only in exclusive mode, otherwise returns NULL.
     */
    IDirectFBSurface*
    getLayerSurface(const std::string& name = "ui") const;

    /*!
     * Returns the dimensions of given logic layer.
     */
    Size
    getLayerSize(const std::string& name = "ui") const;

    /*!
     * Returns application options.
     */
    AppOptions
    appOptions() const;

    /*!
     * Returns the flip mode used for given logic layer.
     */
    LayerFlipMode
    getLayerFlipMode(const std::string& name) const;

    /*!
     * Returns whether full screen updates are used for given logic layer.
     */
    bool
    useFSU(const std::string& name) const;

    /*!
     * Returns window surface capabilities.
     */
    DFBSurfaceCapabilities
    getWindowSurfaceCaps() const;

    /*!
     * Returns path to default font pack definition file.
     */
    const std::string&
    getFontPack() const;

    /*!
     * Returns path to default icon pack definition file.
     */
    const std::string&
    getIconPack() const;

    /*!
     * Returns path to default palette definition file.
     */
    const std::string&
    getPalette() const;

    /*!
     * Returns path to default style definition file.
     */
    const std::string&
    getStyle() const;

    /*!
     * Returns path to default backgroung image.
     */
    const std::string&
    getBackground() const;

    /*!
     * Returns if cursor is visible.
     */
    bool
    cursorVisible() const;

    /*!
     * Returns the pixel format used for all images.
     */
    DFBSurfacePixelFormat
    forcedPixelFormat() const;

    /*!
     * Render cursor using given position.
     */
    void
    renderCursor(const DFBPoint& point);

    /*!
     * Plays a sound from SoundMap.
     */
    void
    playSoundEffect(const std::string& id = "Click");

    /*!
     * Sets level for sound effects.
     *
     * @param level [0-1].
     */
    void
    setSoundEffectLevel(float level);

#ifdef ILIXI_HAVE_NLS
    bool
    addI18N(I18NBase* tb);

    bool
    removeI18N(I18NBase* tb);

    void
    setLanguage(const char* lang);
#endif

private:
    //! This property stores various options for application behaviour.
    AppOptions _options;

    //! DirectFB interface.
    IDirectFB* _dfb;

    IDirectFBDisplayLayer* _cursorLayer;
    IDirectFBSurface* _cursorTarget;
    IDirectFBSurface* _cursorImage;

    //! Forced pixel format for all images
    DFBSurfacePixelFormat _pixelFormat;

    struct HardwareLayer
    {
        bool fsu;                           // FullScreenUpdates
        LayerFlipMode flipMode;             // Controls surface flip behaviour
        IDirectFBDisplayLayer* layer;
    };

    struct LogicLayer
    {
        unsigned int id;                // Id of layer
        IDirectFBSurface* surface;
    };

    typedef std::map<unsigned int, HardwareLayer> HardwareLayerMap;
    HardwareLayerMap _hwLayerMap;

    typedef std::map<std::string, LogicLayer> LogicLayerMap;
    LogicLayerMap _layerMap;

    struct WindowConf
    {
        bool fsu;
        LayerFlipMode flipMode;
        DFBSurfaceCapabilities caps;
    };

#ifdef ILIXI_HAVE_FUSIONSOUND
    typedef std::map<std::string, Sound*> SoundMap;
    SoundMap _soundMap;
    //! Master sound effect level
    float _soundLevel;
#endif

#ifdef ILIXI_HAVE_NLS
    typedef std::list<I18NBase*> I18NBaseList;
    I18NBaseList _tbList;
    pthread_mutex_t _tbListMutex;
    std::string _fontPackDefault;
#endif

    WindowConf _windowConf;
    std::string _background;
    std::string _fontPack;
    std::string _iconPack;
    std::string _style;
    std::string _palette;

    PlatformManager();

    ~PlatformManager();

    PlatformManager(PlatformManager const& copy);

    PlatformManager&
    operator=(PlatformManager const& copy);

    /*!
     * Initialise DirectFB interfaces.
     *
     * This method is executed only once by main Application during its construction.
     */
    void
    initialize(int* argc, char*** argv, AppOptions opts);

    /*!
     * Releases DirectFB resources.
     */
    void
    release();

    void
    parseArgs(const char *args);

    bool
    parseConfig();

    void
    setHardwareLayers(xmlNodePtr node);

    void
    setLogicLayers(xmlNodePtr node);

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)

    void
    setScreen(xmlNodePtr node);

    void
    setScreenTvStandard(DFBScreenEncoderTVStandards* cfg, xmlChar* standards);

    void
    setScreenOutConnectors(DFBScreenOutputConnectors* cfg, xmlChar* connectors);

    void
    setScreenScanMode(DFBScreenEncoderScanMode* cfg, xmlChar* scanmode);

    void
    setScreenResolution(DFBScreenOutputResolution* cfg, xmlChar* resolution);

    void
    setScreenFrequency(DFBScreenEncoderFrequency* cfg, xmlChar* freq);

#endif

    void
    setWindow(xmlNodePtr node);

    void
    setTheme(xmlNodePtr node);

#ifdef ILIXI_HAVE_FUSIONSOUND
    void
    setSounds(xmlNodePtr node);
#endif

    void
    setCursor(xmlNodePtr node);

    bool
    setPixelFormat(const char* format);

    void
    setAppOption(AppOptions option);

    void
    unSetAppOption(AppOptions option);

    // what about service apps?
    // if there is a compositor, make video layer request via component.
};

} /* namespace ilixi */
#endif /* ILIXI_PLATFORMMANAGER_H_ */
