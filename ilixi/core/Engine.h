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

#ifndef ILIXI_ENGINE_H_
#define ILIXI_ENGINE_H_

#include <core/Callback.h>
#include <lib/Timer.h>
#include <lib/Util.h>
#include <types/Event.h>

#include <directfb.h>
#include <list>
#include <sigc++/signal.h>

#if ILIXI_HAS_SURFACEEVENTS
#include <core/SurfaceEventListener.h>
#endif

namespace ilixi
{

//! This class provides a mechanism to run callbacks, timers and custom work items inside main loop.
class Engine : public sigc::trackable
{
    friend class Application;
    friend class Service;
    friend class SurfaceEventListener;  // add/remove SurfaceEventListener
public:
    static Engine&
    instance();

    /*!
     * Runs callbacks, timers and custom work items.
     *
     * @return next timeout in milliseconds.
     */
    int32_t
    cycle();

    /*!
     * Wakes up main loop.
     */
    void
    wakeUp();

    /*!
     * Returns whether engine should run another cycle.
     */
    bool
    stopped() const;

    /*!
     * Stops engine cycles.
     */
    void
    stop();

    /*!
     * Adds callback.
     */
    bool
    addCallback(Callback* cb);

    /*!
     * Removes callback.
     */
    bool
    removeCallback(Callback* cb);

    /*!
     * Returns number of registered callbacks.
     */
    unsigned int
    numCallbacks() const;

    /*!
     * Adds a new timer to be monitored.
     */
    bool
    addTimer(Timer* timer);

    /*!
     * Removes timer.
     */
    bool
    removeTimer(Timer* timer);

    /*!
     * Post a universal event to main event buffer.
     *
     * @param target Widget.
     * @param type of event
     * @param data if not NULL, a pointer to your data.
     */
    void
    postUniversalEvent(Widget* target, unsigned int type, void* data = NULL);

    /*!
     * Post a custom user event.
     *
     * @param type of event [0-999].
     * @param data if not NULL, a pointer to your data.
     *
     * \warning make sure your event type is lower than 1000.
     */
    void
    postUserEvent(unsigned int type, void* data = NULL);

    /*!
     * Post a DFBEvent.
     */
    void
    postEvent(DFBEvent* event);

    /*!
     * This signal is emitted once each cycle.
     */
    sigc::signal<void> sigPerformWork;

protected:
    /*!
     * Executes callbacks.
     */
    void
    runCallbacks();

    /*!
     * Executes each timer and returns a timeout for next interval in ms.
     */
    int32_t
    runTimers();

private:
    //! Event buffer for Service.
    IDirectFBEventBuffer* __buffer;

    //! Termination flag.
    bool _terminate;

    typedef std::list<Callback*> CallbackList;
    //! List of callbacks
    CallbackList __callbacks;
    //! Serialises access to __callbacks.
    pthread_mutex_t __cbMutex;

    typedef std::list<Timer*> TimerList;
    TimerList _timers;
    pthread_mutex_t __timerMutex;

#if ILIXI_HAS_SURFACEEVENTS
    typedef std::list<SurfaceEventListener*> SurfaceListenerList;
    //! List of surface event listeners.
    SurfaceListenerList __selList;
    //! Serialises access to _selList.
    pthread_mutex_t __selMutex;
#endif // end ILIXI_HAS_SURFACEEVENTS
    Engine();

    virtual
    ~Engine();

    void
    initialise();

    void
    release();

    void
    initEventBuffer();

    void
    releaseEventBuffer();

    DFBResult
    resetBuffer();

    DFBResult
    attachWindow(IDirectFBWindow* _dfbWindow);

    DFBResult
    detachWindow(IDirectFBWindow* _dfbWindow);

    DFBResult
    getNextEvent(DFBEvent* event);

    void
    waitForEvents(int32_t timeout);

#if ILIXI_HAS_SURFACEEVENTS
    /*!
     * Adds surface event listener.
     */
    bool
    addSurfaceEventListener(SurfaceEventListener* sel);

    /*!
     * Removes surface event listener.
     */
    bool
    removeSurfaceEventListener(SurfaceEventListener* sel);

    /*!
     * Forwards incoming surface event to corresponding surface event listener object.
     */
    void
    consumeSurfaceEvent(const DFBSurfaceEvent& event);
#endif // end ILIXI_HAS_SURFACEEVENTS
};

} /* namespace ilixi */
#endif /* ILIXI_ENGINE_H_ */
