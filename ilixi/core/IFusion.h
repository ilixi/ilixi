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

#ifndef ILIXI_FUSION_HPP_
#define ILIXI_FUSION_HPP_

// DirectFB-Fusion
extern "C"
{
#include <fusion/fusion.h>
#include <fusion/shmalloc.h>
#include <fusion/call.h>
#include <fusion/arena.h>
#include <fusion/vector.h>
#include <fusion/reactor.h>
}

// DirectFB
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <directfb.h>

namespace ilixi
{
  //! Maestro creates a unique instance of this object in ilixi fusion world.
  struct MaestroObject
  {
    //! Pointer to shared memory pool
    FusionSHMPoolShared* pool;
    //! Pointer to reactor
    FusionReactor* reactor;
    //! Lock for this object.
    FusionSkirmish* mLock;
    //! Ask for bounds of application.
    FusionCall* getAppBounds;

    //! OSK input buffer.
    char* OSK;
    //! Length of OSK input buffer in bytes.
    int OSK_BUFFER_SIZE;
  };

  namespace IMaestro
  {
    /*!
     * This enum is used to specify the type of message that is sent over reactor.
     * Some messages are only meaningful between predefined parties. For example,
     * SwitchMode messages sent to an application by another application are ignored.
     */
    enum ReactorMessageType
    {
      SwitchMode, //!< This is a command message sent by Maestro to an application.
      ModeRequest, //!< This is a request sent to Maestro by an application.
      Notification, //!< This is a notification of a state change.
      OSKEvent, //!< This is a notification of an application's OSK usage.
      WindowRequest
    //!< This a request sent to Maestro by an application regarding a window.
    };

    /*!
     * This enum is used to specify the state of an application.
     */
    enum AppState
    {
      None = 0x00000000, //!< Not set.
      Busy = 0x00000001, //!< Application is busy until it notifies that it is ready.
      Idle = 0x00000002, //!< Application is idle.
      Alive = 0x00000004, //!< Application is alive.
      Initialising = 0x00000008, //!< Application is initialising shared resources.
      Ready = 0x00000010, //!< Application successfully initialised and ready to execute.
      Terminating = 0x00000020, //!< Application is about to terminate shortly.
      Visible = 0x00000040, //!< Application has a visible window and has access to events.
      Hidden = 0x00000080, //!< Application has no window and has no access to events.
      Quit = 0x00000100,    //!< Application should terminate.
    };

    /*!
     * An active application has a record in fusion world.
     */
    struct AppRecord
    {
      //! Current state of application.
      AppState state;
      //! ID of owner application in fusion world.
      unsigned int fusionID;
      //! pid of task.
      pid_t pid;
      //! Title of application.
      char* title;
      //! Path to application.
      char* process;
      //!< Lock for safe access/modification.
      FusionSkirmish* lock;
    };

    /*!
     * Maestro and applications communicate over reactor using this message format.
     */
    struct MaestroMessage
    {
      /*!
       * Constructor.
       *
       * @param sender corresponds to fusionID of sender.
       * @param messageType type of message.
       * @param appMode makes sense only if type is SwitchMode or ModeRequest.
       * @param index of application inside appVector.
       */
      MaestroMessage(unsigned int sender, ReactorMessageType messageType,
          AppState appState, unsigned int index = 0) :
        senderID(sender), type(messageType), state(appState), appIndex(index)
      {
      }

      MaestroMessage(ReactorMessageType messageType, AppState appState) :
        senderID(0), type(messageType), state(appState)
      {
      }

      MaestroMessage(ReactorMessageType messageType, AppState appState,
          DFBWindowID windowID) :
        senderID(0), type(messageType), state(appState)
      {
        payload.windowID = windowID;
      }

      //! FusionID of sender.
      unsigned int senderID;
      //! Type of this message.
      ReactorMessageType type;
      //! If message type has an effect on state, this field should not be null.
      AppState state;
      //! This is the index of sender in appVector. If sender is Maestro it denotes another application.
      unsigned int appIndex;

      union Payload
      {
        unsigned int appIndex;
        DFBWindowID windowID;
      } payload;
    };

    // This structure is used to store parameters.
    struct Parameters
    {
      int WORLD_INDEX;
      int POOL_SIZE;
      int APP_VECTOR_SIZE;
      int OSK_BUFFER_SIZE;
      int CLIPBOARD_SIZE;

      bool BG_MODE_IMAGE;
      char BG_IMAGE[150];
      short BG_COLOR_R;
      short BG_COLOR_G;
      short BG_COLOR_B;

      char BUSY_IMAGE[150];

      bool CURSOR_ENABLED;
      char CURSOR_IMAGE[150];
      int CURSOR_HOT_X;
      int CURSOR_HOT_Y;

      char STATUSBAR[150];
      short STATUSBAR_HEIGHT;

      char HOME[150];

      char OSK[150];
    };

    AppRecord*
    getAppRecord(FusionVector *appVector, int fusionID);
  }
}
#endif /* ILIXI_FUSION_HPP_ */
