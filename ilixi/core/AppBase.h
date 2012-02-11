/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#ifndef ILIXI_APPBASE_H_
#define ILIXI_APPBASE_H_

#include <string>
#include "core/IFusion.h"

namespace ilixi
{
  //! Base class for ilixi applications.
  /*!
   * AppBase manages a connection to the ilixi dfb-fusion world using
   * joinFusion() and leaveFusion() methods. If your intention is to create a simple
   * ilixi background process without access to event buffer (e.g. input events) and
   * any visual elements, you could derive from this class.
   *
   * AppBase initiates a connection attempt to ilixi dfb-fusion world during construction;
   * the process is terminated if the attempt is unsuccessful; otherwise a new
   * application record is created inside shared memory pool and initialised using joinFusionCB().
   */
  class AppBase
  {
    friend class EventManager;
    friend class Window;
  public:
    /*!
     * Constructor.
     */
    AppBase(int argc, char* argv[]);

    /*!
     * Destructor.
     */
    virtual
    ~AppBase();

    /*!
     * Returns title of application.
     */
    std::string
    title() const;

    /*!
     * Sets application title.
     */
    void
    setTitle(std::string title);

  protected:
    /*!
     * Returns current state of application.
     */
    IMaestro::AppState
    appState() const;

    /*!
     * Sets application state flag.
     */
    void
    setAppState(IMaestro::AppState state);

    /*!
     * Clears application state flag.
     */
    void
    clearAppState(IMaestro::AppState state);
#if ILIXI_MULTI_ENABLED
    /*!
     * Returns application's own record in fusion.
     */
    IMaestro::AppRecord*
    appRecord() const;

    /*!
     * Returns text stored in OSK buffer.
     */
    std::string
    getOSKText() const;

    /*!
     * Sets text stored in OSK area. Text can be truncated if it is
     * longer than buffer length.
     */
    void
    setOSKText(const std::string& text);

    /*!
     * Returns an AppRecord with given id number or NULL if there is no such id in ilixi-world.
     */
    IMaestro::AppRecord* const
    getAppRecord(unsigned int fusionID) const;

    /*!
     * Returns an AppRecord with given process name or NULL if there is no such process in ilixi-world.
     */
    IMaestro::AppRecord* const
    getAppRecord(std::string process) const;

    /*!
     * This method is used to send a mode request or other notify messages from
     * the application to Maestro.
     */
    void
    callMaestro(IMaestro::ReactorMessageType type, IMaestro::AppState state,
        unsigned int appIndex = 0);

    void
    callMaestro(const IMaestro::MaestroMessage& message);
#endif
  private:
#if ILIXI_MULTI_ENABLED
    //! Pointer to ilixi dfb-fusion world
    FusionWorld *__world;
    //! Pointer to ilixi dfb-fusion arena
    FusionArena *__arena;
    //! Fusion call object
//    FusionCall __call;
    //! Fusion local reaction using channel: fusionID.
    Reaction __lReaction;
    //! Fusion global reaction using channel: 1.
    Reaction __gReaction;
    //! Pointer to MaestroObject instance inside shared memory pool
    MaestroObject *__mObject;
    //! Pointer to shared dfb-FusionVector instance inside shared memory pool
    FusionVector *__appVector;
    //! Pointer to AppRecord instance of application inside shared memory pool
    IMaestro::AppRecord *__appRecord;
    //! Used by callback wrappers for forwarding.
    static AppBase* __appInstance;

    //! This wrapper is for initialising arena (Does nothing atm).
    static int
    initArenaCBW(FusionArena *arena, void *ctx);

    //! This wrapper is called when application enters arena.
    static int
    enterArenaCBW(FusionArena *arena, void *ctx);

    //! This  wrapper is for shutting down arena (Does nothing atm).
    static int
    shutdownArenaCBW(FusionArena *arena, void *ctx, bool emergency);

    //! This wrapper is called when application leaves arena.
    static int
    leaveArenaCBW(FusionArena *arena, void *ctx, bool emergency);

    //! This wrapper is called when a message arrives over reactor.
    static ReactionResult
    reactionCBW(const void *msgData, void *ctx);

    //! This wrapper is called when sent message is received by Maestro.
    static FusionCallHandlerResult
    dispatchCBW(int caller, int call_arg, void *call_ptr, void *ctx,
        unsigned int serial, int *ret_val);

    /*!
     * This method is called as application joins ilixiArena.
     * Memory for storing an application record is allocated inside the shared pool
     * and the record is initialised.
     */
    int
    enterArenaCB(FusionArena* arena, void* ctx);

    /*!
     * This method is called as the application leaves ilixiArena.
     * Previously allocated memory in shared pool is freed and the application
     * record is removed.
     */
    int
    cleanArenaCB(FusionArena* arena, void* ctx, bool emergency);

    /*!
     * This method is called if application receives a message either on
     * global or local channel from Maestro.
     */
    virtual ReactionResult
    maestroCB(IMaestro::MaestroMessage *msg, void *ctx);

    /*!
     * This method is used to connect to the fusion world as a slave.
     * Upon successful connection an application record is formed inside
     * the shared pool. Furthermore, RPC mechanism, which is used for
     * controlling the application's state, is set up.
     *
     * \sa enterArenaCB
     *
     * @return true if successful, otherwise false.
     */
    bool
    joinFusion();

    /*!
     * Leaves ilixi dfb-fusion world and arena.
     *
     * \sa cleanArenaCB
     */
    void
    leaveFusion();
#else
    std::string __title;
    IMaestro::AppState __state;
#endif


  };
}
#endif /* ILIXI_APPBASE_H_ */
