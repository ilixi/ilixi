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

#include "AppBase.h"
#include "core/Logger.h"
#include <fstream> // Required for reading maestro.conf
#include <string.h>
#include <stdlib.h>

using namespace std;
using namespace ilixi;
using namespace IMaestro;
#if ILIXI_MULTI_ENABLED
AppBase* AppBase::__appInstance = NULL;

//*************************************************************
// Callback wrappers
//*************************************************************
int
AppBase::initArenaCBW(FusionArena *arena, void *ctx)
{
  return 1;
}

int
AppBase::enterArenaCBW(FusionArena *arena, void *ctx)
{
  if (__appInstance)
    return __appInstance->enterArenaCB(arena, ctx);
  return 1;
}

int
AppBase::shutdownArenaCBW(FusionArena *arena, void *ctx, bool emergency)
{
  return 1;
}

int
AppBase::leaveArenaCBW(FusionArena *arena, void *ctx, bool emergency)
{
  if (__appInstance)
    return __appInstance->cleanArenaCB(arena, ctx, emergency);
  return 1;
}

ReactionResult
AppBase::reactionCBW(const void *msgData, void *ctx)
{
  if (__appInstance)
    {
      MaestroMessage *msg = (MaestroMessage*) msgData;
      return __appInstance->maestroCB(msg, ctx);
    }
  return RS_REMOVE;
}

FusionCallHandlerResult
AppBase::dispatchCBW(int caller, int call_arg, void *call_ptr, void *ctx,
    unsigned int serial, int *ret_val)
{
  //  if (call_arg == __appInstance->__appRecord->fusionID)
  //    {
  //      ILOG_DEBUG("Message is received by Maestro.");
  //      return FCHR_RETURN;
  //    }
  return FCHR_RETAIN;
}
#endif
//*************************************************************
// AppBase
//*************************************************************
AppBase::AppBase(int argc, char* argv[])
{
  ILOG_OPEN(basename(argv[0]));
#if ILIXI_MULTI_ENABLED
  if(__appInstance)
    exit(0);
  __appInstance = this;
  __world = NULL;
  __arena = NULL;

  if (joinFusion())
    {
      ILOG_DEBUG( "I am in fusion.");
      __appRecord->process = (char*) SHMALLOC(__mObject->pool,
          sizeof(char) * strlen(argv[0]));
      sprintf(__appRecord->process, "%s", argv[0]);

    }
  else
    {
      printf("Please start Maestro.");
      exit(EXIT_FAILURE);
    }
#endif
}

AppBase::~AppBase()
{
#if ILIXI_MULTI_ENABLED
  leaveFusion();
  __appInstance = NULL;
#endif
  ILOG_CLOSE();
}

#if ILIXI_MULTI_ENABLED
std::string
AppBase::title() const
{
  return __appRecord->title;
}

void
AppBase::setTitle(std::string title)
{
  fusion_skirmish_prevail(__appRecord->lock);
  if (__appRecord->title)
    SHFREE(__mObject->pool, __appRecord->title);
  __appRecord->title = (char*) SHMALLOC(__mObject->pool,
      sizeof(char) * (title.length() + 1));
  sprintf(__appRecord->title, "%s", title.c_str());
  fusion_skirmish_dismiss(__appRecord->lock);
}

AppState
AppBase::appState() const
{
  fusion_skirmish_prevail(__appRecord->lock);
  AppState s = __appRecord->state;
  fusion_skirmish_dismiss(__appRecord->lock);
  return s;
}

void
AppBase::setAppState(AppState state)
{
  fusion_skirmish_prevail(__appRecord->lock);
  __appRecord->state = (AppState) (__appRecord->state | state);
  fusion_skirmish_dismiss(__appRecord->lock);
}

void
AppBase::clearAppState(AppState state)
{
  fusion_skirmish_prevail(__appRecord->lock);
  __appRecord->state = (AppState) (__appRecord->state & ~state);
  fusion_skirmish_dismiss(__appRecord->lock);
}

AppRecord*
AppBase::appRecord() const
{
  return __appRecord;
}

std::string
AppBase::getOSKText() const
{
  fusion_skirmish_prevail(__mObject->mLock);
  std::string txt(__mObject->OSK);
  fusion_skirmish_dismiss(__mObject->mLock);
  return txt;
}

void
AppBase::setOSKText(const std::string& text)
{
  fusion_skirmish_prevail(__mObject->mLock);
  snprintf(__mObject->OSK, __mObject->OSK_BUFFER_SIZE, "%s", text.c_str());
  fusion_skirmish_dismiss(__mObject->mLock);
}

AppRecord* const
AppBase::getAppRecord(unsigned int appID) const
{
  AppRecord* record;
  for (int i = 0; i < __appVector->count; i++)
    {
      record = (AppRecord *) fusion_vector_at(__appVector, i);
      if (record->fusionID == appID)
        return record;
    }
  return NULL;
}

AppRecord* const
AppBase::getAppRecord(std::string process) const
{
  AppRecord* record;
  for (int i = 0; i < __appVector->count; i++)
    {
      record = (AppRecord *) fusion_vector_at(__appVector, i);
      if (strcmp(record->process, process.c_str()) == 0)
        return record;
    }
  return NULL;
}

void
AppBase::callMaestro(ReactorMessageType type, AppState state,
    unsigned int appIndex)
{
  if (type == Notification)
    ILOG_DEBUG("Sending Notification (state: %d) via reactor...", state);
  else if (type == ModeRequest)
    ILOG_DEBUG("Sending ModeRequest (state: %d) via reactor...", state);
  else if (type == OSKEvent)
    ILOG_DEBUG("Sending OSKEvent (state %d) via reactor...", state);
  else
    ILOG_DEBUG("Sending SwitchMode (state: %d) via reactor...", state);

  if (appIndex == 0)
    {
      MaestroMessage message(__appRecord->fusionID, type, state,
          __appRecord->fusionID);
      fusion_reactor_dispatch(__mObject->reactor, &message, false, NULL);
    }
  else
    {
      MaestroMessage message(__appRecord->fusionID, type, state, appIndex);
      fusion_reactor_dispatch(__mObject->reactor, &message, false, NULL);
    }
}

void
AppBase::callMaestro(const IMaestro::MaestroMessage& message)
{
  MaestroMessage m = message;
  m.senderID = __appRecord->fusionID;
  fusion_reactor_dispatch(__mObject->reactor, &m, false, NULL);
}

int
AppBase::enterArenaCB(FusionArena *arena, void *ctx)
{
  ILOG_DEBUG( "Joining ilixiArena...");
  if (fusion_arena_get_shared_field(arena, "MaestroObject",
      (void **) &__mObject) != DR_OK)
    {
      ILOG_ERROR("Error getting MaestroObject");
      return 1;
    }

  if (fusion_arena_get_shared_field(arena, "AppVector", (void **) &__appVector)
      != DR_OK)
    {
      ILOG_ERROR("Error getting AppVector");
      return 1;
    }

  if (__appVector->count < __appVector->capacity)
    {
      __appRecord = (AppRecord*) SHMALLOC(__mObject->pool, sizeof(AppRecord));
      __appRecord->state = Initialising;
      __appRecord->fusionID = fusion_id(__world);
      __appRecord->pid = getpid();
      __appRecord->title = NULL;
      __appRecord->process = NULL;

      __appRecord->lock
          = (FusionSkirmish*) SHMALLOC(__mObject->pool, sizeof(FusionSkirmish));
      char name[5];
      snprintf(name, 5, "L%d", __appRecord->fusionID);
      fusion_skirmish_init(__appRecord->lock, name, __world);

      fusion_skirmish_prevail(__mObject->mLock);
      fusion_vector_add(__appVector, __appRecord);
      fusion_skirmish_dismiss(__mObject->mLock);
    }
  else
    {
      ILOG_ERROR("Error adding record to AppVector");
      return 1;
    }

  ILOG_INFO( "Joined ilixiArena.");
  return 0;
}

int
AppBase::cleanArenaCB(FusionArena *arena, void *ctx, bool emergency)
{
  ILOG_DEBUG( "Cleaning ilixiArena...");
  if (__appRecord)
    {
      fusion_skirmish_prevail(__mObject->mLock);
      int appIndex = fusion_vector_index_of(__appVector, __appRecord);
      fusion_skirmish_destroy(__appRecord->lock);
      SHFREE(__mObject->pool, __appRecord->lock);
      SHFREE(__mObject->pool, __appRecord->title);
      SHFREE(__mObject->pool, __appRecord);
      ILOG_DEBUG( "Freed SHM...");
      DirectResult ret = fusion_vector_remove(__appVector, appIndex);
      if (ret != DR_OK)
        ILOG_ERROR("Error while removing appRecord: %X", ret);
      else
        ILOG_DEBUG( "Removed record %d ...", appIndex);
      fusion_skirmish_dismiss(__mObject->mLock);

      //      ret = fusion_call_destroy(&__call);
      //      if (ret != DR_OK)
      //        {
      //          ILOG_ERROR("Error while destroying call: %X", ret);
      //          return 1;
      //        }
      //      else
      //        ILOG_DEBUG( "Destroyed call...");

      ret = fusion_reactor_detach(__mObject->reactor, &__lReaction);
      if (ret != DR_OK)
        {
          ILOG_ERROR("Error while detaching local reaction: %X", ret);
          return 1;
        }
      else
        ILOG_DEBUG( "Detached local reaction...");

      ret = fusion_reactor_detach(__mObject->reactor, &__gReaction);
      if (ret != DR_OK)
        {
          ILOG_ERROR("Error while detaching global reaction: %X", ret);
          return 1;
        }
      else
        ILOG_DEBUG( "Detached global reaction...");
    }
  ILOG_INFO( "ilixiArena is cleaned.");
  return 0;
}

ReactionResult
AppBase::maestroCB(MaestroMessage *msg, void *ctx)
{
  if (msg->senderID != 1)
    {
      ILOG_ERROR("Sender (%d) is not authorised!", msg->senderID);
      return RS_DROP;
    }

  switch (msg->type)
    {

  case SwitchMode:
    {
      if (msg->state == Quit)
        {
          setAppState(Terminating);
          return RS_OK;
        }
      else
        ILOG_WARNING("SwitchMode state (%d) is not supported!", msg->state);
      return RS_DROP;
    } // end SwitchMode

  case Notification:
    {
      if (msg->state == Alive)
        {
          setAppState(Alive);
          return RS_OK;
        }
      else
        ILOG_WARNING("Notification state (%d) is not supported!", msg->state);
      return RS_DROP;
    } // end Notification

  default:
    ILOG_WARNING("Message type (%d) is not supported!", msg->type);
    return RS_DROP;
    } // end type
}

bool
AppBase::joinFusion()
{
  // parse maestro config file and find out id of ilixiWorld.
  int ilixiWorld = 0;
  char lineBuffer[1024], tag[24], value[1000];
  std::ifstream infile;
  std::string file = ILIXI_DATADIR"maestro.conf";
  infile.open(file.c_str(), std::ifstream::in);
  if (!infile.good())
    {
      ILOG_FATAL("Unable to open configuration file: %s", file.c_str());
      exit(EXIT_FAILURE);
    }
  while (infile.good())
    {
      infile.getline(lineBuffer, 1024);
      if (lineBuffer[0] != '#' && lineBuffer[0] != '\n' && lineBuffer[0]
          != '\0')
        {
          sscanf(lineBuffer, "%s %*s %s", tag, value);
          if (strcmp(tag, "WORLD_INDEX") == 0)
            {
              ilixiWorld = atoi(value);
              break;
            }
        }
    }
  infile.close();

  ILOG_DEBUG( "Attempting to enter world (%d) as Slave", ilixiWorld);
  // Fixme Fusion ABI version = 46
  DirectResult result = fusion_enter(ilixiWorld, 46, FER_SLAVE, &__world);
  if (result == DR_OK)
    {
      bool master = fusion_master(__world);
      ILOG_INFO("Entered world[%d] with FusionID: %lu, pid: %d as %s",
          fusion_world_index(__world), fusion_id(__world), getpid(),
          master ? "Master" : "Slave");

      if (master)
        {
          ILOG_FATAL("Maestro is not running.");
          leaveFusion();
          return false;
        }

      int ret;
      if (fusion_arena_enter(__world, "ilixiArena", initArenaCBW,
          enterArenaCBW, NULL, &__arena, &ret) != DR_OK)
        {
          ILOG_FATAL("Could not enter ilixiArena!");
          return false;
        }

      //      ILOG_DEBUG( "Initialising call mechanism...");
      //      if (fusion_call_init(&__call, dispatchCBW, NULL, __world) != DR_OK)
      //        {
      //          ILOG_ERROR("Unable to initialise dispatch_callback!");
      //          return false;
      //        }

      //      ILOG_DEBUG("Setting reactor dispatch callback...");
      //      if (fusion_reactor_set_dispatch_callback(__mObject->reactor, &__call,
      //          NULL) != DR_OK)
      //        {
      //          ILOG_ERROR("Unable to set dispatch_callback!");
      //          return false;
      //        }
      ILOG_DEBUG("Attaching reaction to global channel: 1");
      if (fusion_reactor_attach_channel(__mObject->reactor, 1, reactionCBW,
          NULL, &__gReaction) != DR_OK)
        {
          ILOG_ERROR("Unable to attach reaction to channel!");
          return false;
        }

      ILOG_DEBUG("Attaching reaction to local channel: %d", __appRecord->fusionID);
      if (fusion_reactor_attach_channel(__mObject->reactor,
          __appRecord->fusionID, reactionCBW, NULL, &__lReaction) != DR_OK)
        {
          ILOG_ERROR("Unable to attach reaction to channel!");
          return false;
        }
      ILOG_DEBUG( "Application is in ilixiWorld.");
      callMaestro(Notification, Initialising);
      return true;
    }
  else
    {
      ILOG_ERROR("Unable to enter fusion! Do you have read/write access to fusion node?");
      return false;
    }
}

void
AppBase::leaveFusion()
{
  if (__arena)
    {
      int ret;
      void *ctx;
      DirectResult result = fusion_arena_exit(__arena, shutdownArenaCBW,
          leaveArenaCBW, &ctx, 0, &ret);
      if (result != DR_OK)
        ILOG_FATAL("Unable to exit ilixiArena");
      else
        ILOG_INFO("Application exited from ilixiArena.");
    }
  if (__world)
    {
      DirectResult result = fusion_exit(__world, false);
      if (result != DR_OK)
        ILOG_FATAL("Unable to exit from ilixiWorld");
      else
        ILOG_INFO("Application left ilixiWorld.");
    }
}
#else
std::string
AppBase::title() const
{
  return (__title);
}

void
AppBase::setTitle(std::string title)
{
  __title = title;
}

AppState
AppBase::appState() const
{
  return (__state);
}

void
AppBase::setAppState(AppState state)
{
  __state = (AppState) (__state | state);
}

void
AppBase::clearAppState(AppState state)
{
  __state = (AppState) (__state & ~state);
}
#endif
