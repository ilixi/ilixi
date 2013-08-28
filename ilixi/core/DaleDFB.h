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

#ifndef ILIXI_DALEDFB_H_
#define ILIXI_DALEDFB_H_

#include <directfb.h>
extern "C"
{
#include <fusiondale.h>
}
#include <lib/Notify.h>
#include <types/Rectangle.h>
#include <list>
namespace ilixi
{

//! Provides access to FusionDale interfaces.
class DaleDFB
{
    friend class PlatformManager;
    friend class Notify;
public:

    /*!
     * Returns Coma interface.
     */
    static IComa*
    getComa();

    /*!
     * Gets a Coma component with given name.
     *
     * @param[in] name Coma component name.
     * @param[out] component interface.
     */
    static DFBResult
    comaGetComponent(const char* name, IComaComponent** component);

    /*!
     * Gets a shared memory block.
     *
     * @param[in] bytes
     * @param[out] ret
     */
    static DFBResult
    comaGetLocal(unsigned int bytes, void** ret);

    /*!
     * Performs method invocation.
     */
    static DFBResult
    comaCallComponent(IComaComponent* component, ComaMethodID method, void* arg);

    /*!
     * Shows on-screen-keyboard.
     */
    static DFBResult
    showOSK(const Rectangle& rect);

    /*!
     * Hides on-screen-keyboard.
     */
    static DFBResult
    hideOSK();

private:
    static IFusionDale* __dale;
    static IComa* __coma;
    static IComaComponent* __oskComp;
    static IComaComponent* __compComp;
    static IComaComponent* __soundComp;

    struct OSKRequest
    {
        DFBRectangle inputRect;
        int mode;
        pid_t process;
    };

    typedef std::list<Notify*> Notifications;
    static Notifications __nots;

    /*!
     * Constructor.
     */
    DaleDFB();

    /*!
     * Destructor.
     */
    virtual
    ~DaleDFB();

    /*!
     * Initialises FusionDale and component manager interfaces.
     */
    static DFBResult
    initDale(int* argc, char*** argv);

    /*!
     * Releases FusionDale interfaces.
     */
    static void
    releaseDale();

    /*!
     * Get OSK COMA Component.
     */
    static DFBResult
    getOSKComp();

    /*!
     * Get Compositor COMA Component.
     */
    static DFBResult
    getCompComp();

    /*!
     * Get SoundMixer COMA Component.
     */
    static DFBResult
    getSoundComp();

    /*!
     * Sends a notification to compositor via Compositor Component.
     */
    static DFBResult
    addNotification(Notify*, void* data);

    static DFBResult
    removeNotification(Notify*);

    static void
    notificationListener(void* ctx, void* arg);

    /*!
     * Plays a sound effect via SoundMixer component.
     */
    static DFBResult
    playSoundEffect(const std::string& id);

    /*!
     * Sets sound effect master volume via SoundMixer component.
     */
    static DFBResult
    setSoundEffectLevel(float level);

};

} /* namespace ilixi */
#endif /* DALEDFB_H_ */
