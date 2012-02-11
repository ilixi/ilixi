/*
 Copyright 2011 Tarik Sekmen.

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

#include "core/IFusion.h"
#include "core/Logger.h"
#include <sys/stat.h>
#include <spawn.h>
#include <sys/wait.h>
#include <fstream>
#include <syslog.h>
#include <string.h>

#include "AppManager.h"

using namespace ilixi;
using namespace IMaestro;
/*
 * Maestro provides a simple mechanism to communicate foreground permissions to applications. Upon start, Maestro
 * creates a Fusion world and a shared memory pool for storing a record of each active application inside a vector (AppVector).
 * A Fusion arena is also set up and used for providing access to the pool and a message reactor by all applications.
 *
 * Each application is responsible for creating and setting up its own record and placing the record in AppVector when it
 * is initialised. Therefore, it is also an application's responsibility to remove its record when it is destroyed. This
 * is done automatically in classes derived from ilixi::Application. During the course of operation these records are
 * accessed by Maestro for controlling application behaviour, e.g. sending them to background or setting properties of their windows.
 * (At most one application can have foreground permission and receive incoming DirectFB input events at any time.)
 *
 * A Fusion reactor is used for sending display commands to active applications and receiving back status information.
 * A command could be sent to a single application using the channel which corresponds to the application's FusionID
 * or globally using channel 1. Applications should use channel 0 to send messages to Maestro.
 */

// Configuration parameters of maestro.
Parameters config;
// Pointer to ilixi dfb-fusion world
FusionWorld *world = NULL;
// Pointer to ilixi dfb-fusion shared memory pool
FusionSHMPoolShared *pool = NULL;
// Pointer to ilixi dfb-fusion arena
FusionArena *arena = NULL;

// Pointer to MaestroObject instance inside shared memory pool
MaestroObject *maestro = NULL;
// Pointer to shared dfb-FusionVector instance inside shared memory pool
FusionVector *appVector = NULL;

// Record pointing to app with foreground permission.
AppRecord* currentApp = NULL;
// Record pointing to app with OSK widget.
AppRecord* oskTarget = NULL;
// Home application.
AppRecord* home = NULL;

// Fusion call object
//FusionCall call;
// Reaction to incoming messages
Reaction reaction;

// DirectFB interface
IDirectFB* dfb = NULL;
// DirectFB layer interface
IDirectFBDisplayLayer* layer = NULL;
// Background surface of application
IDirectFBSurface* bgSurface = NULL;
// This property stores an interface to busy state window.
IDirectFBWindow* busyWindow = NULL;
// Surface of busyWindow.
IDirectFBSurface* busyWSurface = NULL;
// This property stores surface for busy image.
IDirectFBSurface* busyImage = NULL;
// This loads busy image.
IDirectFBVideoProvider* busyProvider = NULL;

// Fusion
bool
initFusion(bool slaveMode = false);

void
exitFusion(bool slaveMode = false);

int
initArenaCB(FusionArena*, void*);

int
joinArenaCB(FusionArena *arena, void *ctx);

int
leaveArenaCB(FusionArena *arena, void *ctx, bool emergency);

int
cleanArenaCB(FusionArena *arena, void *ctx, bool emergency);

// Reactor related
ReactionResult
reactionCB(const void *msgData, void *ctx);

FusionCallHandlerResult
dispatchCB(int caller, int call_arg, void *call_ptr, void *ctx,
    unsigned int serial, int *ret_val);

ReactionResult
handleSB(IMaestro::MaestroMessage* msg);

ReactionResult
handleOSK(IMaestro::MaestroMessage* msg);

ReactionResult
handleApp(IMaestro::MaestroMessage* msg);

void
callApp(int channel, ReactorMessageType type, AppState mode,
    unsigned int appIndex = 0);

// DirectFB
bool
initDFB();

void
releaseDFB();

void
showBusy();

void
hideBusy();

void
renderBusyCB(void *cdata);

// Other
AppRecord*
getAppRecord(int index);

void
start(int argc, char **argv);

void
stop();

void
status();

void
parseConfig();

void
createFork(int argc, char **argv);

void
runApp(char* path, pid_t* id);

//*****************************************************************
// Connects to fusion world and creates objects in shared
// memory.
//
// If slaveMode is true and process is master returns false.
//*****************************************************************
bool
initFusion(bool slaveMode)
{
  ILOG_DEBUG("Attempting to enter DFB-World as %s...", slaveMode ? "Slave"
      : "Master");
  // FIXME Fusion ABI version = 46
  DirectResult result = fusion_enter(config.WORLD_INDEX, 46, FER_ANY, &world);
  if (result == DR_OK)
    {
      bool master = fusion_master(world);
      ILOG_INFO("Entered DFB-World[%d] with ID: %lu, pid: %d as %s",
          fusion_world_index(world), fusion_id(world), getpid(),
          master ? "Master" : "Slave");

      if (slaveMode && master)
        {
          ILOG_WARNING("Maestro is not running.");
          exitFusion(true);
          return false;
        }

      if (master)
        {
          ILOG_DEBUG("Maestro-Master is creating DFB-Pool...");
          if (fusion_shm_pool_create(world, "ilixiPool", config.POOL_SIZE, 0,
              &pool) != DR_OK)
            {
              ILOG_FATAL("fusion_shm_pool_create() failed!");
              return false;
            }
        }

      int ret;
      if (fusion_arena_enter(world, "ilixiArena", initArenaCB, joinArenaCB,
          NULL, &arena, &ret) != DR_OK)
        {
          ILOG_FATAL("fusion_arena_enter() failed!");
          return false;
        }

      if (master)
        {
          // TODO App will ask for bounds using call mechanism...
          if (fusion_call_init(maestro->getAppBounds, dispatchCB, NULL, world)
              != DR_OK)
            {
              ILOG_FATAL("fusion_call_init() failed!");
              return false;
            }

          //      // Have the call executed when a dispatched message has been processed by all recipients.
          //      if (fusion_reactor_set_dispatch_callback(maestro->reactor, &call, NULL)
          //          != DR_OK)
          //        {
          //          ILOG_FATAL("fusion_reactor_set_dispatch_callback() failed!");
          //          return false;
          //        }

          // Attach a local reaction to the reactor at channel 0
          if (fusion_reactor_attach(maestro->reactor, reactionCB, NULL,
              &reaction) != DR_OK)
            {
              ILOG_FATAL("fusion_reactor_attach() failed!");
              return false;
            }
        }
      return true;
    }
  else
    {
      ILOG_FATAL("Could not create DFB-World!");
      return false;
    }
}

//*****************************************************************
// Leaves world and arena.
//
// If slaveMode is false (i.e. master process) destroys pool.
//*****************************************************************
void
exitFusion(bool slaveMode)
{
  if (arena)
    {
      ILOG_DEBUG("Maestro-%s (pid: %d) is leaving DFB-Arena...", slaveMode ? "Slave"
          : "Master", getpid());
      int ret;
      void *ctx;
      if (slaveMode)
        fusion_arena_exit(arena, leaveArenaCB, NULL, &ctx, 0, &ret);
      else
        fusion_arena_exit(arena, cleanArenaCB, NULL, &ctx, 0, &ret);
      arena = NULL;
    }
  if (pool && !slaveMode)
    {
      ILOG_DEBUG("Maestro-Master (pid: %d) is destroying DFB-Pool...", getpid());
      fusion_shm_pool_destroy(world, pool);
      pool = NULL;
    }
  if (world)
    {
      ILOG_DEBUG("Maestro-%s (pid: %d) is leaving DFB-World...", slaveMode ? "Slave"
          : "Master", getpid());
      fusion_exit(world, false);
      world = NULL;
    }
  ILOG_INFO("Maestro (pid: %d) has left DFB-Fusion!", getpid());
}

//*****************************************************************
// Allocate shared memory space and create fields.
//*****************************************************************
int
initArenaCB(FusionArena *arena, void *ctx)
{
  ILOG_DEBUG("Maestro-Master is initialising DFB-Arena...");
  maestro = (MaestroObject*) SHMALLOC(pool, sizeof(MaestroObject));
  fusion_arena_add_shared_field(arena, "MaestroObject", (void *) maestro);

  maestro->pool = pool;
  maestro->reactor = fusion_reactor_new(sizeof(MaestroMessage), "ilixiReactor",
      world);

  fusion_reactor_direct(maestro->reactor, false);

  maestro->mLock = (FusionSkirmish*) SHMALLOC(pool, sizeof(FusionSkirmish));
  fusion_skirmish_init(maestro->mLock, "AppVectorLock", world);

  maestro->getAppBounds = (FusionCall*) SHMALLOC(pool, sizeof(FusionCall));

  maestro->OSK = (char*) SHMALLOC(pool, sizeof(char) * config.OSK_BUFFER_SIZE);
  sprintf(maestro->OSK, "ilixi");
  maestro->OSK_BUFFER_SIZE = config.OSK_BUFFER_SIZE;

  // TODO remove AppVector if using SaWMan.
  appVector = (FusionVector*) SHMALLOC(pool, sizeof(FusionVector));
  fusion_arena_add_shared_field(arena, "AppVector", (void *) appVector);
  fusion_vector_init(appVector, config.APP_VECTOR_SIZE, pool);

  ILOG_INFO("Maestro-Master initialised DFB-Arena!");
  return 0;
}

//*****************************************************************
// Map shared memory objects.
//*****************************************************************
int
joinArenaCB(FusionArena *arena, void *ctx)
{
  ILOG_DEBUG("Maestro-Slave is joining DFB-Arena...");
  fusion_arena_get_shared_field(arena, "MaestroObject", (void **) &maestro);
  fusion_arena_get_shared_field(arena, "AppVector", (void **) &appVector);
  ILOG_INFO("Maestro-Slave joined DFB-Arena!");
  return 0;
}

//*****************************************************************
// Leave arena for slaves... Does nothing...
//*****************************************************************
int
leaveArenaCB(FusionArena *arena, void *ctx, bool emergency)
{
}

//*****************************************************************
// Deallocate objects in shared memory.
//*****************************************************************
int
cleanArenaCB(FusionArena *arena, void *ctx, bool emergency)
{
  ILOG_DEBUG("Maestro-Master is cleaning DFB-Arena...");

  DirectResult ret = fusion_reactor_detach(maestro->reactor, &reaction);
  if (ret != DR_OK)
    ILOG_ERROR("Error while detaching local reaction: %X", ret);

  ret = fusion_reactor_destroy(maestro->reactor);
  if (ret != DR_OK)
    ILOG_ERROR("Error while destroying reactor: %X", ret);

  ret = fusion_reactor_free(maestro->reactor);
  if (ret != DR_OK)
    ILOG_ERROR("Error while freeing reactor: %X", ret);

  ret = fusion_call_destroy(maestro->getAppBounds);
  if (ret != DR_OK)
    ILOG_ERROR("Error while destroying call: %X", ret);

  fusion_vector_destroy(appVector);
  SHFREE(pool, appVector);
  SHFREE(pool, maestro->getAppBounds);
  SHFREE(pool, maestro->mLock);
  SHFREE(pool, maestro->OSK);
  SHFREE(pool, maestro);
  ILOG_INFO("Maestro-Master cleaned DFB-Arena!");
  return 0;
}

//*****************************************************************
// Called when there is a new message to be consumed in reactor.
//*****************************************************************
ReactionResult
reactionCB(const void *msgData, void *ctx)
{
  MaestroMessage* msg = (MaestroMessage*) msgData;
  if (msg->senderID == 2)
    return handleSB(msg);
  //  else if (msg->senderID == 3)
  //    return handleOSK(msg);
  else
    return handleApp(msg);
}

//*****************************************************************
// Called when a dispatched message has been processed by all recipients.
//*****************************************************************
FusionCallHandlerResult
dispatchCB(int caller, int call_arg, void *call_ptr, void *ctx,
    unsigned int serial, int *ret_val)
{
  return FCHR_RETURN;
  if (call_arg != 1)
    {
      ILOG_DEBUG("Message received by fusionee.");
      return FCHR_RETURN;
    }
  return FCHR_RETAIN;
}

//*****************************************************************
// Handle incoming messages from StatusBar.
//*****************************************************************
ReactionResult
handleSB(MaestroMessage* msg)
{
  switch (msg->type)
    {
  case ModeRequest:
    {
      ILOG_DEBUG("Received ModeRequest (%d) from StatusBar.", msg->state);
      switch (msg->state)
        {
      case Visible:
        {
          callApp(msg->senderID, SwitchMode, Visible);
          return RS_OK;
        }
      default:
        ILOG_WARNING("Requested state is not supported for StatusBar!");
        return RS_DROP;
        }
    }

    // StatusBar's message is forwarded to target Application by Maestro.
    // Here, msg->appIndex refers to target Fusion ID.
  case SwitchMode:
    {
      ILOG_DEBUG("Received SwitchMode (%d) from StatusBar, target: %d.", msg->state, msg->appIndex);
      switch (msg->state)
        {
      case Visible:
        {
          if (currentApp)
            callApp(currentApp->fusionID, SwitchMode, Quit);
          callApp(msg->appIndex, SwitchMode, Visible);
          return RS_OK;
        }
      case Hidden:
        {
          callApp(msg->appIndex, SwitchMode, Hidden);
          return RS_OK;
        }
      case Quit:
        {
          callApp(msg->appIndex, SwitchMode, Quit);
          return RS_OK;
        }
      default:
        ILOG_WARNING("Requested state is not supported for StatusBar!");
        return RS_DROP;
        }
    }

  case Notification:
    {
      ILOG_DEBUG("Received Notification (%d) from StatusBar.", msg->state);
      switch (msg->state)
        {
      case Terminating:
        {
          AppRecord* app = getAppRecord(appVector, msg->senderID);
          fusion_skirmish_prevail(app->lock);
          app->state = (AppState) (app->state | Quit);
          fusion_skirmish_dismiss(app->lock);
          return RS_OK;
        }
      case Visible:
        {
          AppRecord* app = getAppRecord(appVector, msg->senderID);
          fusion_skirmish_prevail(app->lock);
          app->state = (AppState) (app->state & ~Idle);
          fusion_skirmish_dismiss(app->lock);
          return RS_OK;
        }
      default:
        ILOG_WARNING("Notification state is not supported for StatusBar!");
        return RS_DROP;
        }
    }

  default:
    ILOG_ERROR("StatusBar message type (%d) is not supported!", msg->type);
    return RS_DROP;
    }
}

//*****************************************************************
// Handle incoming messages from OSK.
//*****************************************************************
ReactionResult
handleOSK(MaestroMessage* msg)
{
  AppRecord* app = getAppRecord(appVector, msg->senderID);

  switch (msg->type)
    {
  case ModeRequest:
    {
      ILOG_DEBUG("Received ModeRequest (%d) from OSK.", msg->state);
      switch (msg->state)
        {
      case Visible:
        {
          return RS_OK;
        }
      case Hidden:
        {
          callApp(3, SwitchMode, Hidden);
          return RS_OK;
        }
      default:
        ILOG_WARNING("Requested state is not supported for OSK!");
        return RS_DROP;
        }
    }

  case Notification:
    {
      switch (msg->state)
        {
      case Terminating:
        {
          fusion_skirmish_prevail(app->lock);
          app->state = (AppState) (app->state | Quit);
          fusion_skirmish_dismiss(app->lock);
          ILOG_DEBUG( "OSK is terminating.");
          return RS_OK;
        }
      case Visible:
        {
          //          hideBusy();
          fusion_skirmish_prevail(app->lock);
          app->state = (AppState) (app->state & ~Idle);
          fusion_skirmish_dismiss(app->lock);
          ILOG_DEBUG( "%s is visible.", app->title);
          return RS_OK;
        }
      case Hidden:
        {
          fusion_skirmish_prevail(app->lock);
          app->state = (AppState) (app->state | Idle);
          fusion_skirmish_dismiss(app->lock);
          ILOG_DEBUG( "%s is hidden.", app->title);
          return RS_OK;
        }
      default:
        ILOG_WARNING("Notification state is not supported for OSK!");
        return RS_DROP;
        }
    }

  case OSKEvent:
    {
      ILOG_DEBUG("Received OSKEvent from OSK.");
      if (oskTarget)
        callApp(oskTarget->fusionID, OSKEvent, Ready);
      return RS_OK;
    }

  default:
    ILOG_WARNING("OSK message type (%d) is not supported!", msg->type);
    return RS_DROP;
    }
}

//*****************************************************************
// Handle incoming messages from application.
//*****************************************************************
ReactionResult
handleApp(MaestroMessage* msg)
{
  AppRecord* app = getAppRecord(appVector, msg->senderID);

  if (!app)
    return RS_DROP;

  if (!home)
    home = app;

  switch (msg->type)
    {
  case ModeRequest:
    {
      ILOG_DEBUG("Received ModeRequest (%d) from %s.", msg->state, app->title);
      switch (msg->state)
        {
      case Visible:
        {
          showBusy();
          if (currentApp)
            {

              fusion_skirmish_prevail(currentApp->lock);
              currentApp->state = (AppState) (currentApp->state | Idle);
              fusion_skirmish_dismiss(currentApp->lock);
              callApp(currentApp->fusionID, SwitchMode, Hidden);
            }

          callApp(msg->senderID, SwitchMode, Visible);
          return RS_OK;
        }
      default:
        ILOG_WARNING("Requested state is not supported!");
        return RS_DROP;
        }
    } // end ModeRequest

    // TODO Remove SwitchMode for apps.
  case SwitchMode:
    {
      if (app != home) // This mode is for Home only.
        {
          ILOG_WARNING("Message type is not supported!");
          return RS_DROP;
        }

      switch (msg->state)
        {
      case Visible:
        {
          // NOTE Home should hide itself.
          callApp(msg->appIndex, SwitchMode, Visible);
          return RS_OK;
        }
      default:
        ILOG_WARNING("Requested state is not supported for Home!");
        return RS_DROP;
        }
    } // end SwitchMode

  case Notification:
    {
      ILOG_DEBUG("Received Notification (%d) from %s.", msg->state, app->process);
      switch (msg->state)
        {
      case Initialising:
        {
          // _vm->busy();
          ILOG_DEBUG( "ID: %d is initialising.", msg->senderID);
          return RS_OK;
        }
      case Ready:
        {
          // _vm->ready();
          ILOG_DEBUG( "%s is ready.", app->process);
          return RS_OK;
        }
      case Terminating:
        {
          if (oskTarget == app) // hide OSK if application is the oskTarget
            {
              callApp(3, SwitchMode, Hidden);
              oskTarget = NULL;
            }

          if (currentApp == app)
            currentApp = NULL;

          ILOG_DEBUG( "%s is terminating.", app->title);

          if (home != app)
            callApp(home->fusionID, SwitchMode, Visible);
          else
            home = NULL;

          fusion_skirmish_prevail(app->lock);
          app->state = (AppState) (app->state | Quit);
          fusion_skirmish_dismiss(app->lock);

          return RS_OK;
        }
      case Visible:
        {
          fusion_skirmish_prevail(app->lock);
          app->state = (AppState) (app->state & ~Idle);
          fusion_skirmish_dismiss(app->lock);

          currentApp = app;
          callApp(2, Notification, Visible, currentApp->fusionID); // notify status bar...

          hideBusy();
          ILOG_DEBUG( "%s is visible.", app->title);
          return RS_OK;
        }
      case Hidden:
        {
          if (oskTarget == app) // hide OSK if application is the oskTarget
            {
              callApp(3, SwitchMode, Hidden);
              oskTarget = NULL;
            }

          fusion_skirmish_prevail(app->lock);
          app->state = (AppState) (app->state | Idle);
          fusion_skirmish_dismiss(app->lock);

          ILOG_DEBUG( "%s is hidden.", app->title);
          return RS_OK;
        }
      default:
        ILOG_WARNING("Notification state (%d) is not supported for %s!", msg->state, app->process);
        return RS_DROP;
        }
    } // end Notification

  case OSKEvent:
    {
      if (currentApp != app)
        {
          ILOG_WARNING("OSKEvent state is not supported!");
          return RS_DROP;
        }

      switch (msg->state)
        {
      case Visible:
        {
          //          showBusy();
          oskTarget = app;
          ILOG_DEBUG("%s is asking for OSK to be visible.", app->title);
          callApp(3, SwitchMode, Visible);
          return RS_OK;
        }
        //      case Hidden:
        //        {
        //          oskTarget = NULL;
        //          ILOG_DEBUG("%s is asking for OSK to be hidden.", app->title);
        //          callApp(3, SwitchMode, Hidden);
        //          return RS_OK;
        //        }
      default:
        ILOG_WARNING("OSKEvent state is not supported!");
        return RS_DROP;
        }
    } // end OSKEvent

  default:
    ILOG_WARNING("Message type is not supported!");
    return RS_DROP;
    }
}

//*****************************************************************
// Sends a message to an application.
//*****************************************************************
void
callApp(int channel, ReactorMessageType type, AppState mode,
    unsigned int appIndex)
{
  ILOG_DEBUG("Sending %s mode via reactor using channel %d...",
      (mode==Visible) ? "Visible" : (mode == Hidden) ? "Hidden": "Quit", channel);
  MaestroMessage message(1, type, mode, appIndex);
  fusion_reactor_dispatch_channel(maestro->reactor, channel, &message,
      sizeof(message), false, NULL);
}

//*****************************************************************
// Initialises DirectFB interfaces.
//*****************************************************************
bool
initDFB()
{
  if (DirectFBInit(NULL, NULL) != DFB_OK)
    {
      ILOG_ERROR("Error while initialising DirectFB!");
      return false;
    }

  if (DirectFBCreate(&dfb) != DFB_OK)
    {
      ILOG_ERROR("Error while creating DirectFB interface!");
      return false;
    }

  if (dfb->GetDisplayLayer(dfb, DLID_PRIMARY, &layer) != DFB_OK)
    {
      ILOG_ERROR("Error while getting primary layer!");
      return false;
    }

  // ViewportManager will have administrative access and control windows from now on.
  layer->SetCooperativeLevel(layer, DLSCL_ADMINISTRATIVE);

  DFBGraphicsDeviceDescription deviceDescription;
  DFBDisplayLayerConfig layerConfig;

  // FIXME buffer mode setting of layer is DLBM_BACKSYSTEM.
  dfb->GetDeviceDescription(dfb, &deviceDescription);
  if (!((deviceDescription.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL)
      && (deviceDescription.blitting_flags & DSBLIT_BLEND_COLORALPHA)))
    {
      layerConfig.flags = DLCONF_BUFFERMODE;
      layerConfig.buffermode = DLBM_BACKSYSTEM; // Backbuffer in system memory...
      layer->SetConfiguration(layer, &layerConfig);
    }
  layer->GetConfiguration(layer, &layerConfig);

  layer->EnableCursor(layer, config.CURSOR_ENABLED);

  if (config.BG_MODE_IMAGE)
    {
      DFBSurfaceDescription bgsurfaceDesc;
      IDirectFBImageProvider *provider;
      if (dfb->CreateImageProvider(dfb, config.BG_IMAGE, &provider) != DFB_OK)
        {
          ILOG_ERROR("Error while loading background image!");
          return false;
        }

      bgsurfaceDesc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_WIDTH
          | DSDESC_HEIGHT | DSDESC_CAPS);
      bgsurfaceDesc.width = layerConfig.width;
      bgsurfaceDesc.height = layerConfig.height;
      bgsurfaceDesc.caps = DSCAPS_SHARED;

      if (dfb->CreateSurface(dfb, &bgsurfaceDesc, &bgSurface) != DFB_OK)
        {
          ILOG_ERROR("Error while creating background image surface!");
          return false;
        }

      provider->RenderTo(provider, bgSurface, NULL);
      provider->Release(provider);

      // TODO Add background tiling (DLBM_TILE) option
      layer->SetBackgroundImage(layer, bgSurface);
      layer->SetBackgroundMode(layer, DLBM_IMAGE);
    }
  else
    {
      layer->SetBackgroundColor(layer, config.BG_COLOR_R, config.BG_COLOR_G,
          config.BG_COLOR_B, 1);
      layer->SetBackgroundMode(layer, DLBM_COLOR);
    }

  //  // Create busy window and set busy image.
  //    {
  //      if (dfb->CreateVideoProvider(dfb, config.BUSY_IMAGE, &busyProvider)
  //          != DFB_OK)
  //        {
  //          ILOG_ERROR("Error while loading busy image!");
  //          return false;
  //        }
  //
  //      DFBSurfaceDescription busySurfaceDesc;
  //      busyProvider->GetSurfaceDescription(busyProvider, &busySurfaceDesc);
  //      if (dfb->CreateSurface(dfb, &busySurfaceDesc, &busyImage) != DFB_OK)
  //        {
  //          ILOG_ERROR("Error while creating background image surface!");
  //          return false;
  //        }
  //      busyProvider->SetPlaybackFlags(busyProvider, DVPLAY_LOOPING);
  //
  //      // create window
  //      int w = busySurfaceDesc.width;
  //      int h = busySurfaceDesc.height;
  //      int x = (layerConfig.width - w) / 2.0;
  //      int y = (layerConfig.height - h) / 2.0;
  //
  //      DFBWindowDescription desc;
  //      desc.flags = (DFBWindowDescriptionFlags) (DWDESC_POSX | DWDESC_POSY
  //          | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_SURFACE_CAPS
  //          | DWDESC_PIXELFORMAT | DWDESC_OPTIONS);
  //
  //      desc.posx = x;
  //      desc.posy = y;
  //      desc.width = w;
  //      desc.height = h;
  //      desc.pixelformat = DSPF_ARGB;
  //      desc.caps = (DFBWindowCapabilities) (DWCAPS_DOUBLEBUFFER
  //          | DWCAPS_ALPHACHANNEL | DWCAPS_NODECORATION);
  //      desc.surface_caps = (DFBSurfaceCapabilities) (DSCAPS_PREMULTIPLIED
  //          | DSCAPS_FLIPPING);
  //      desc.options = (DFBWindowOptions) (DWOP_ALPHACHANNEL | DWOP_SCALE);
  //
  //      if (layer->CreateWindow(layer, &desc, &busyWindow) != DFB_OK)
  //        {
  //          ILOG_ERROR("Error while creating DirectFB window!");
  //          return false;
  //        }
  //
  //      busyWindow->SetStackingClass(busyWindow, DWSC_UPPER);
  //      busyWindow->SetOpacity(busyWindow, 0);
  //      busyWindow->GetSurface(busyWindow, &busyWSurface);
  //    }
  ILOG_INFO("DirectFB is initialised.");
  return true;
}

//*****************************************************************
// Release DirectFB interfaces.
//*****************************************************************
void
releaseDFB()
{
  if (busyWindow)
    {
      busyProvider->Release(busyProvider);
      busyImage->Release(busyImage);
      busyWSurface->Release(busyWSurface);
      busyWindow->Destroy(busyWindow);
      busyWindow->Release(busyWindow);
      busyWindow = NULL;
    }

  if (bgSurface)
    bgSurface->Release(bgSurface);

  if (layer)
    layer->Release(layer);

  if (dfb)
    {
      dfb->Release(dfb);
      ILOG_INFO("DFB interfaces are released.");
    }
}

//*****************************************************************
// Shows busyWindow
//*****************************************************************
void
showBusy()
{
  if (busyWindow)
    {
      busyWindow->GrabKeyboard(busyWindow);
      busyWindow->SetOpacity(busyWindow, 255);
      busyProvider->PlayTo(busyProvider, busyImage, NULL, renderBusyCB, NULL);
    }
}

//*****************************************************************
// Hides busyWindow
//*****************************************************************
void
hideBusy()
{
  if (busyWindow)
    {
      busyProvider->Stop(busyProvider);
      busyWindow->SetOpacity(busyWindow, 0);
      busyWindow->UngrabKeyboard(busyWindow);
    }
}

//*****************************************************************
// Blits new busy image frame to busyWindow
//*****************************************************************
void
renderBusyCB(void *cdata)
{
  busyWSurface->Blit(busyWSurface, busyImage, NULL, 0, 0);
  busyWSurface->Flip(busyWSurface, NULL, DSFLIP_BLIT);
}

//*****************************************************************
// Start Maestro.
//*****************************************************************
void
start(int argc, char **argv)
{
  parseConfig();
  if (initFusion(true))
    {
      printf("Maestro is already running!\n");
      exitFusion(true);
      ILOG_CLOSE();
    }
  else
    createFork(argc, argv);
}

//*****************************************************************
// Stop Maestro.
//*****************************************************************
void
stop()
{
  parseConfig();
  if (initFusion(true))
    {
      callApp(1, SwitchMode, Quit);
      exitFusion(true);
    }
}

//*****************************************************************
// Print list of running applications.
//*****************************************************************
void
status()
{
  parseConfig();
  if (initFusion(true))
    {
      // prints DFBUptime taken from dfbdump tool...
      long long millis;
      long int seconds, minutes, hours, days;
      millis = direct_clock_get_millis();

      seconds = millis / 1000;
      millis %= 1000;

      minutes = seconds / 60;
      seconds %= 60;

      hours = minutes / 60;
      minutes %= 60;

      days = hours / 24;
      hours %= 24;

      switch (days)
        {
      case 0:
        printf("\nDirectFB uptime: %02ld:%02ld:%02ld\n", hours, minutes,
            seconds);
        break;

      case 1:
        printf("\nDirectFB uptime: %ld day, %02ld:%02ld:%02ld\n", days, hours,
            minutes, seconds);
        break;

      default:
        printf("\nDirectFB uptime: %ld days, %02ld:%02ld:%02ld\n", days, hours,
            minutes, seconds);
        break;
        }
      // print a list of running ilixi applications...
      fusion_skirmish_prevail(maestro->mLock);
      int appCount = fusion_vector_size(appVector);
      if (appCount > 0)
        {
          printf("\n#\tPID\t%-20s\tStatus\n", "Title");
          printf(
              "------------------------------------------------------------\n");
          for (int i = 0; i < appCount; i++)
            {
              AppRecord *app = (AppRecord *) fusion_vector_at(appVector, i);
              printf("%d\t%d\t%-30s\t%s\n", i, app->pid, app->title,
                  (app->state & Visible) ? "Visible" : "Hidden");
            }
        }
      else
        printf("There aren't any applications running...\n");
      fusion_skirmish_dismiss(maestro->mLock);
    }
  else
    printf("Please start Maestro\n");
  exitFusion(true);
  ILOG_CLOSE();
}

//*****************************************************************
// Create a background process and run Maestro daemon.
//*****************************************************************
void
createFork(int argc, char **argv)
{
  pid_t child_pid = fork();
  if (child_pid < 0)
    {
      exitFusion(true);
      ILOG_FATAL("Unable to start Maestro using fork()");
      exit(EXIT_FAILURE);
    }
  else if (child_pid > 0)
    {
      exitFusion(true);
      ILOG_DEBUG("Maestro-Slave is terminating.");
      exit(EXIT_SUCCESS);
    }
  else
    {
      sleep(1);
      umask(0);
      pid_t sid = setsid();
      if (sid < 0)
        {
          ILOG_FATAL("Unable to create a Session ID for Maestro!");
          exit(EXIT_FAILURE);
        }

      if ((chdir(ILIXI_DATADIR)) < 0)
        {
          ILOG_FATAL("Unable to change current working directory to %s!", ILIXI_DATADIR);
          exit(EXIT_FAILURE);
        }

      close(STDIN_FILENO);
      //      close(STDOUT_FILENO);
      //      close(STDERR_FILENO);

      if (initFusion())
        {
          ILOG_NOTICE("Maestro-Master (pid: %d) is running!", getpid());
          initDFB();

          //          AppManager appMan;
          //          appMan.AddApplication("StatusBar", config.STATUSBAR, NULL);
          //          appMan.AddApplication("Home", config.HOME, NULL);
          //          appMan.AddApplication("OSK", config.OSK, NULL);
          //          appMan.init();

          int err;
          pid_t statusbar_id, osk_id, home_id;
          bool loop = true;

          //          appMan.startRequest("StatusBar", &statusbar_id);

          // Comment this bit if you wish to try app manager..
          runApp(config.STATUSBAR, &statusbar_id);

          while (loop)
            {
              usleep(2000);
              fusion_skirmish_prevail(maestro->mLock);
              if (appVector->count == 1)
                {
                  AppRecord* app = (AppRecord *) appVector->elements[0];
                  if (app->state & Ready)
                    loop = false;
                }
              fusion_skirmish_dismiss(maestro->mLock);
            }
          // Comment ends

          //          char *oskArg[] =
          //            { config.OSK, NULL };
          //          if ((err = posix_spawn(&osk_id, oskArg[0], NULL, NULL, oskArg,
          //              environ)) != 0)
          //            {
          //              ILOG_ERROR("posix_spawn() error: %d", err);
          //              exit(EXIT_FAILURE);
          //            }
          //
          //          loop = true;
          //          while (loop)
          //            {
          //              usleep(2000);
          //              fusion_skirmish_prevail(maestro->mLock);
          //              if (appVector->count == 2)
          //                {
          //                  AppRecord* app = (AppRecord *) appVector->elements[1];
          //                  if (app->state & Ready)
          //                    loop = false;
          //                }
          //              fusion_skirmish_dismiss(maestro->mLock);
          //            }
          //

//          runApp(config.HOME, &home_id);
          runApp(config.HOME, &home_id);

          // wait until statusbar quits...
          int childExitStatus;
          pid_t ws = waitpid(statusbar_id, &childExitStatus, 0);
          if (WIFEXITED(childExitStatus))
            ILOG_ERROR("StatusBar terminated normally with: %d", WEXITSTATUS(childExitStatus));
          else if (WIFSIGNALED(childExitStatus))
            ILOG_ERROR("StatusBar terminated due to a signal: %d", WTERMSIG(childExitStatus));
          else
            ILOG_NOTICE("StatusBar is terminated; now terminating remaining applications.");

          // Send message to all applications and wait until they are all terminated.
          showBusy();
          callApp(1, SwitchMode, Quit);
          loop = true;
          int step = 0;
          while (loop)
            {
              usleep(1000);
              ++step;
              if (step == 100)
                loop = false;
              fusion_skirmish_prevail(maestro->mLock);
              if (appVector->count == 0)
                loop = false;
              fusion_skirmish_dismiss(maestro->mLock);
            }
          hideBusy();

          //          appMan.deinit();

          releaseDFB();
          exitFusion();
          ILOG_NOTICE("Maestro ends.");
        }
      else
        exit(EXIT_FAILURE);
      ILOG_CLOSE();
    }
}

void
runApp(char* path, pid_t* id)
{
  int err;
  char *sArg[] =
    { path, NULL };
  if ((err = posix_spawn(id, sArg[0], NULL, NULL, sArg, environ)) != 0)
    {
      ILOG_ERROR("posix_spawn() error: %d", err);
      exit(EXIT_FAILURE);
    }
}

//*****************************************************************
// Parse configuration from maestro.conf
//*****************************************************************
void
parseConfig()
{
  // initialise all parameters before parsing them...
  config.WORLD_INDEX = 0;
  config.POOL_SIZE = 1024;
  config.APP_VECTOR_SIZE = 10;
  config.OSK_BUFFER_SIZE = 10;
  config.CLIPBOARD_SIZE = 10;
  config.BG_MODE_IMAGE = false;
  sprintf(config.BG_IMAGE, "");
  config.BG_COLOR_R = 0;
  config.BG_COLOR_G = 0;
  config.BG_COLOR_B = 0;
  sprintf(config.BUSY_IMAGE, "");
  config.CURSOR_ENABLED = 0;
  sprintf(config.CURSOR_IMAGE, "");
  config.CURSOR_HOT_X = 0;
  config.CURSOR_HOT_Y = 0;
  sprintf(config.STATUSBAR, ILIXI_BINDIR"ilixi_statusbar");
  config.STATUSBAR_HEIGHT = 50;
  sprintf(config.OSK, ILIXI_BINDIR"ilixi_osk");
  sprintf(config.HOME, ILIXI_BINDIR"ilixi_home");

  char lineBuffer[1024], tag[24], value[1000];
  std::ifstream infile;
  std::string file = ILIXI_DATADIR"maestro.conf";
  infile.open(file.c_str(), std::ifstream::in);
  if (infile.good())
    ILOG_DEBUG("Parsing configuration file: %s", file.c_str());
  else
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
          ILOG_DEBUG("%s: %s", tag, value);
          if (strcmp(tag, "WORLD_INDEX") == 0)
            config.WORLD_INDEX = atoi(value);
          else if (strcmp(tag, "POOL_SIZE") == 0)
            config.POOL_SIZE = atoi(value);
          else if (strcmp(tag, "APP_VECTOR_SIZE") == 0)
            config.APP_VECTOR_SIZE = atoi(value);
          else if (strcmp(tag, "OSK_SIZE") == 0)
            config.OSK_BUFFER_SIZE = atoi(value);
          else if (strcmp(tag, "CLIPBOARD_SIZE") == 0)
            config.CLIPBOARD_SIZE = atoi(value);

          else if (strcmp(tag, "BACKGROUND_MODE") == 0)
            {
              if (strcmp(value, "IMAGE") == 0)
                config.BG_MODE_IMAGE = true;
              else
                config.BG_MODE_IMAGE = false;
            }
          else if (strcmp(tag, "BACKGROUND_IMAGE") == 0)
            sprintf(config.BG_IMAGE, ILIXI_DATADIR"%s", value);
          else if (strcmp(tag, "BACKGROUND_COLOR_R") == 0)
            config.BG_COLOR_R = atoi(value);
          else if (strcmp(tag, "BACKGROUND_COLOR_G") == 0)
            config.BG_COLOR_G = atoi(value);
          else if (strcmp(tag, "BACKGROUND_COLOR_B") == 0)
            config.BG_COLOR_B = atoi(value);

          else if (strcmp(tag, "BUSY_IMAGE") == 0)
            sprintf(config.BUSY_IMAGE, ILIXI_DATADIR"%s", value);

          else if (strcmp(tag, "CURSOR_ENABLED") == 0)
            config.CURSOR_ENABLED = atoi(value);
          else if (strcmp(tag, "CURSOR_IMAGE") == 0)
            sprintf(config.CURSOR_IMAGE, ILIXI_DATADIR"%s", value);
          else if (strcmp(tag, "CURSOR_HOT_X") == 0)
            config.CURSOR_HOT_X = atoi(value);
          else if (strcmp(tag, "CURSOR_HOT_Y") == 0)
            config.CURSOR_HOT_Y = atoi(value);

          else if (strcmp(tag, "STATUSBAR") == 0)
            sprintf(config.STATUSBAR, ILIXI_BINDIR"%s", value);
          else if (strcmp(tag, "STATUSBAR_HEIGHT") == 0)
            config.STATUSBAR_HEIGHT = atoi(value);

          else if (strcmp(tag, "HOME") == 0)
            sprintf(config.HOME, ILIXI_BINDIR"%s", value);

          else if (strcmp(tag, "OSK") == 0)
            sprintf(config.OSK, ILIXI_BINDIR"%s", value);
        }
    }
  infile.close();
}

//*****************************************************************
// Prints a list of command line options
//*****************************************************************
void
printUsage()
{
  printf("\nUsage: ilixi_maestro [OPTION...]\n");
  printf("      --start                              Starts Maestro\n");
  printf("      --stop                               Stops Maestro\n");
  printf("      --restart                            Restarts Maestro\n");
  printf(
      "      --status                             List running applications\n");
}

//*****************************************************************
int
main(int argc, char **argv)
{
  if (argc < 2)
    printUsage();
  else
    {
#if ILIXI_LOGGER_ENABLED
      ilixi_log_init(basename(argv[0]), LOG_DAEMON);
#endif
      if (strcmp(argv[1], "--start") == 0)
        start(argc, argv);
      else if (strcmp(argv[1], "--stop") == 0)
        {
          stop();
          ILOG_CLOSE();
        }
      else if (strcmp(argv[1], "--restart") == 0)
        {
          stop();
          sleep(1);
          start(argc, argv);
        }
      else if (strcmp(argv[1], "--status") == 0)
        status();
      else
        {
          printUsage();
          ILOG_CLOSE();
        }
    }
}
