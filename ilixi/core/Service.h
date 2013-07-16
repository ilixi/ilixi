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

#ifndef ILIXI_SERVICE_H_
#define ILIXI_SERVICE_H_

#include <lib/Util.h>
#include <core/Callback.h>
#include <lib/Timer.h>

#include <types/Enums.h>
#include <directfb.h>
#include <list>
#include <sigc++/signal.h>

namespace ilixi
{

class Service: public sigc::trackable
{
public:
    Service(int* argc, char*** argv, AppOptions opts = OptNone);

    virtual
    ~Service();

    /*!
     * You can reimplement this method to create your own custom main loop.
     */
    static void
    exec();

    /*!
     * Wakes up main loop.
     */
    void
    wakeUp();

    /*!
     * Terminates application.
     */
    void
    quit();

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

    sigc::signal<void> sigPerformWork;

protected:
    /*!
     * Custom user events are handled in this function.
     * \sa postUserEvent
     */
    virtual void
    handleUserEvent(const DFBUserEvent& event);

    /*!
     * Executes each timer and returns a timeout for next interval in ms.
     */
    int32_t
    runTimers();

    /*!
     * Executes callbacks.
     */
    void
    runCallbacks();

    /*!
     * All events are handled using this function.
     *
     * @param timeout in ms.
     * @param forceWait causes to wait even if there is a pending UI update
     */
    void
    handleEvents(int32_t timeout, bool forceWait = false);

private:
    //! Service instance.
    static Service* __instance;
    //! Event buffer for Service.
    static IDirectFBEventBuffer* __buffer;

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

    void
    initEventBuffer();

    void
    releaseEventBuffer();

    /*!
     * Adds callback.
     */
    static bool
    addCallback(Callback* cb);

    /*!
     * Removes callback.
     */
    static bool
    removeCallback(Callback* cb);

    /*!
     * Adds a new timer to be monitored.
     */
    static bool
    addTimer(Timer* timer);

    /*!
     * Removes timer.
     */
    static bool
    removeTimer(Timer* timer);
};

} /* namespace ilixi */
#endif /* ILIXI_SERVICE_H_ */
