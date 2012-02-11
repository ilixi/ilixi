/*
 * AppManager.cpp
 *
 *  Created on: 8 Oct 2011
 *      Author: tarik
 */

#include "AppManager.h"
#include <wait.h>
#include <spawn.h>
#include <directfb_util.h>

#include "core/Logger.h"

using namespace ilixi;

#define MAX_WINDOWS 4

AppManager* AppManager::__instance = NULL;
pthread_mutex_t AppManager::__appManLock = PTHREAD_MUTEX_INITIALIZER;

DirectResult
start_request(void *context, const char *name, pid_t *ret_pid)
{
  if (AppManager::__instance)
    return AppManager::__instance->startRequest(name, ret_pid);
  return DR_FAILURE;
}

DirectResult
stop_request(void *context, pid_t pid, FusionID caller)
{
  if (AppManager::__instance)
    return AppManager::__instance->stopRequest(pid, caller);
  return DR_FAILURE;
}

DirectResult
process_added(void *context, SaWManProcess *process)
{
  if (AppManager::__instance)
    return AppManager::__instance->processAdded(process);
  return DR_FAILURE;
}

DirectResult
process_removed(void *context, SaWManProcess *process)
{
  if (AppManager::__instance)
    return AppManager::__instance->processRemoved(process);
  return DR_FAILURE;
}

DirectResult
input_filter(void *context, DFBInputEvent *event)
{
  if (AppManager::__instance)
    return AppManager::__instance->inputFilter(event);
  return DR_FAILURE;
}

DirectResult
window_preconfig(void *context, SaWManWindowConfig *config)
{
  if (AppManager::__instance)
    return AppManager::__instance->windowPreconfig(config);
  return DR_FAILURE;
}

DirectResult
window_added(void *context, SaWManWindowInfo *info)
{
  if (AppManager::__instance)
    return AppManager::__instance->windowAdded(info);
  return DR_FAILURE;
}

DirectResult
window_removed(void *context, SaWManWindowInfo *info)
{
  if (AppManager::__instance)
    return AppManager::__instance->windowRemoved(info);
  return DR_FAILURE;
}

DirectResult
window_reconfig(void *context, SaWManWindowReconfig *reconfig)
{
  if (AppManager::__instance)
    return AppManager::__instance->windowReconfig(reconfig);
  return DR_FAILURE;
}

DirectResult
window_restack(void *context, SaWManWindowHandle handle,
    SaWManWindowHandle relative, SaWManWindowRelation relation)
{
  if (AppManager::__instance)
    return AppManager::__instance->windowReStack(handle, relative, relation);
  return DR_FAILURE;
}

DirectResult
stack_resized(void *context, const DFBDimension *size)
{
  if (AppManager::__instance)
    return AppManager::__instance->stackResized(size);
  return DR_FAILURE;
}

DirectResult
switch_focus(void *context, SaWManWindowHandle window)
{
  if (AppManager::__instance)
    return AppManager::__instance->switchFocus(window);
  return DR_FAILURE;
}

DirectResult
layer_reconfig(void *context, SaWManLayerReconfig *reconfig)
{
  if (AppManager::__instance)
    return AppManager::__instance->layerReconfig(reconfig);
  return DR_FAILURE;
}

DirectResult
application_id_changed(void *context, SaWManWindowInfo *info)
{
  if (AppManager::__instance)
    return AppManager::__instance->appIDChanged(info);
  return DR_FAILURE;
}

//*********************************************************************

AppManager::AppManager() :
  _saw(NULL), _manager(NULL), applications(NULL), _scalingMode(SWMSM_STANDARD),
      _focusWindow(SAWMAN_WINDOW_NONE)
{
}

AppManager::~AppManager()
{
  if (__instance)
    deinit();
}

bool
AppManager::init()
{
  pthread_mutex_lock(&__appManLock);
  if (__instance)
    {
      if (__instance != this)
        ILOG_ERROR("Another AppManager is in use, remove it first!");
      else
        ILOG_ERROR("AppManager is already initialised!");
      pthread_mutex_unlock(&__appManLock);
      return false;
    }

  ILOG_DEBUG("Initialising AppManager...");
  __instance = this;

  SaWManInit(NULL, NULL);
  SaWManCreate(&_saw);

  callbacks.Start = start_request;
  callbacks.Stop = stop_request;
  callbacks.ProcessAdded = process_added;
  callbacks.ProcessRemoved = process_removed;
  callbacks.InputFilter = input_filter;
  callbacks.WindowPreConfig = window_preconfig;
  callbacks.WindowAdded = window_added;
  callbacks.WindowRemoved = window_removed;
  callbacks.WindowReconfig = window_reconfig;
  callbacks.WindowRestack = window_restack;
  //  callbacks.StackResized = stack_resized;
  callbacks.SwitchFocus = switch_focus;
  callbacks.LayerReconfig = layer_reconfig;
  callbacks.ApplicationIDChanged = application_id_changed;

  _saw->CreateManager(_saw, &callbacks, NULL, &_manager);
  _manager->SetScalingMode(_manager, _scalingMode);

  _layout = new Layout(_manager);

  ILOG_INFO("AppManager is initialised and ready.");
  pthread_mutex_unlock(&__appManLock);
  return true;
}

void
AppManager::deinit()
{
  pthread_mutex_lock(&__appManLock);
  delete _layout;

  if (_manager)
    _manager->Release(_manager);

  if (_saw)
    _saw->Release(_saw);

  __instance = NULL;
  pthread_mutex_unlock(&__appManLock);
  D_INFO("Deinitialised SaWMan\n");
}

DirectResult
AppManager::startRequest(const char *name, pid_t *ret_pid)
{
  Application *app;
  pid_t pid;

  D_INFO( "Start request for '%s'!\n", name );

  D_ASSERT( name != NULL );
  D_ASSERT( ret_pid != NULL );

  app = LookupApplication(name);
  if (!app)
    {
      D_ERROR("Application [%s] is not found.\n", name);
      return DR_ITEMNOTFOUND;
    }

  if (app->started && !waitpid(app->pid, NULL, WNOHANG))
    {
      D_DEBUG( "Already running '%s' (%d)!", name, app->pid );
      return DR_BUSY;
    }

  int err;
  char *args[3];
  args[0] = const_cast<char*> (app->program);
  args[1] = const_cast<char*> (app->args);
  args[2] = NULL;

  if ((err = posix_spawn(&pid, args[0], NULL, NULL, args, environ)) != 0)
    {
      ILOG_ERROR("posix_spawn() error: %d", err);
      return DR_FAILURE;
    }

  // TODO wait here until app sets its state to READY...
  app->started = true;
  app->start_time = direct_clock_get_millis();
  app->pid = pid;
  *ret_pid = pid;

  return DR_OK;
}

DirectResult
AppManager::stopRequest(pid_t pid, FusionID caller)
{
  Application *app;

  D_INFO( "Stop request from Fusion ID 0x%lx for pid %d!\n", caller, pid );

  app = LookupApplicationByPID(pid);
  if (!app)
    {
      D_ERROR("Application [%s] is not found.\n", app->name);
      return DR_ITEMNOTFOUND;
    }

  /* Already died before attaching? */
  if (waitpid(app->pid, NULL, WNOHANG))
    {
      app->started = false;
      app->pid = 0;

      return DR_OK;
    }

  /* Not attached yet? */
  if (!app->process)
    {
      D_ERROR( "Application with pid %d did not attach yet!\n", app->pid );
      return DR_NOCONTEXT;
    }

  // TODO Kill application using ilixi reactor

  /* FIXME: avoid signals */
  kill(app->pid, 9);

  return DR_OK;
}

DirectResult
AppManager::processAdded(SaWManProcess *process)
{
  Application *app;

  D_INFO( "Process added (%d) [%lu]!\n", process->pid, process->fusion_id );

  app = LookupApplicationByPID(process->pid);
  if (!app)
    return DR_ITEMNOTFOUND;

  if (app->process)
    {
      D_BUG( "Already attached '%s' (%d)!", app->name, app->pid );
      return DR_BUG;
    }

  app->process = process;

  return DR_OK;
}

DirectResult
AppManager::processRemoved(SaWManProcess *process)
{
  Application *app;

  D_INFO( "Process removed (%d) [%lu]!\n", process->pid, process->fusion_id );

  app = LookupApplicationByPID(process->pid);
  if (!app)
    return DR_ITEMNOTFOUND;

  if (app->process != process)
    {
      D_BUG( "Process mismatch %p != %p of '%s' (%d)!", app->process, process, app->name, app->pid );
      return DR_BUG;
    }

  if (waitpid(app->pid, NULL, 0) < 0)
    perror("waitpid");

  app->process = NULL;
  app->started = false;
  app->pid = 0;

  return DR_OK;
}

DirectResult
AppManager::inputFilter(DFBInputEvent *event)
{
  _manager->Lock(_manager);
  if (event->type == DIET_KEYPRESS)
    {
      switch (event->key_symbol)
        {
      case DIKS_F11:
        _scalingMode = (_scalingMode == SWMSM_SMOOTH_SW) ? SWMSM_STANDARD
            : SWMSM_SMOOTH_SW;
        _manager->SetScalingMode(_manager, _scalingMode);
        _manager->Unlock(_manager);
        return DR_BUSY;

      default:
        break;
        }
    }

  _manager->Unlock(_manager);

  return DR_OK;
}

DirectResult
AppManager::windowPreconfig(SaWManWindowConfig *config)
{
  D_INFO( "Window preconfig (%d,%d-%dx%d)!\n",
      DFB_RECTANGLE_VALS( &config->bounds ) );

  return DR_OK;
}

DirectResult
AppManager::windowAdded(SaWManWindowInfo *info)
{
  D_INFO( "Window[%ld] added (%d,%d-%dx%d)!\n", info->win_id,
      DFB_RECTANGLE_VALS( &info->config.bounds ) );

  if (info->caps & DWCAPS_NODECORATION)
    return DR_NOIMPL; /* to let sawman insert the window */

  /* Already showing window? (reattaching) */
  if (info->config.opacity)
    {
      /* Activate scaling. */
      info->config.options = (DFBWindowOptions) (info->config.options
          | DWOP_SCALE);

      return _layout->addWindow(info->handle);
    }

  return DR_OK;
}

DirectResult
AppManager::windowRemoved(SaWManWindowInfo *info)
{
  D_INFO( "Window[%ld] removed (%d,%d-%dx%d)!\n", info->win_id,
      DFB_RECTANGLE_VALS( &info->config.bounds ) );

  return DR_OK;
}

DirectResult
AppManager::windowReconfig(SaWManWindowReconfig *reconfig)
{
  D_INFO( "Window reconfig (%d,%d-%dx%d)!\n",
      DFB_RECTANGLE_VALS( &reconfig->current.bounds ) );

  if (reconfig->caps & DWCAPS_NODECORATION)
    return DR_OK;

  SaWManWindowConfig* current = &reconfig->current;
  SaWManWindowConfig* request = &reconfig->request;

  if (reconfig->flags & SWMCF_POSITION)
    {
      D_INFO( "Window reconfig - ignoring position (%d,%d)!\n", request->bounds.x, request->bounds.y );
      reconfig->flags = (SaWManWindowConfigFlags) (reconfig->flags
          & ~SWMCF_POSITION);
    }

  if (reconfig->flags & SWMCF_SIZE)
    {
      D_INFO( "Window reconfig - ignoring size (%dx%d)!\n", request->bounds.w, request->bounds.h );
      reconfig->flags = (SaWManWindowConfigFlags) (reconfig->flags
          & ~SWMCF_SIZE);
    }

  if (reconfig->flags & SWMCF_STACKING)
    {
      D_INFO( "Window reconfig - ignoring stacking (%d)!\n", request->stacking );
      reconfig->flags = (SaWManWindowConfigFlags) (reconfig->flags
          & ~SWMCF_STACKING);
    }

  if (reconfig->flags & SWMCF_OPACITY)
    {
      D_INFO( "Window reconfig - opacity change %d\n", request->opacity );
      /* Show? */
      if (request->opacity && !current->opacity)
        {
          /* Activate scaling. */
          if (!(reconfig->flags & SWMCF_OPTIONS))
            request->options = (DFBWindowOptions) 0; /* if a flag is not set, corresponding field may not be initialised */

          reconfig->flags = (SaWManWindowConfigFlags) (reconfig->flags
              | SWMCF_OPTIONS);
          request->options = (DFBWindowOptions) (request->options | DWOP_SCALE);
          return _layout->addWindow(reconfig->handle);
        }
      /* Hide? */
      else if (!request->opacity && current->opacity)
        {
          D_INFO("WindowReConfig - HIDE!!\n");
          return _layout->removeWindow(reconfig->handle);
        }
    }

  return DR_OK;
}

DirectResult
AppManager::windowReStack(SaWManWindowHandle handle,
    SaWManWindowHandle relative, SaWManWindowRelation relation)
{
  D_INFO( "SaWMan/TestMan: Window restack - refusing!\n" );
  return DR_ACCESSDENIED;
}

DirectResult
AppManager::stackResized(const DFBDimension *size)
{
  //XXX not used atm.
  D_INFO("Window restack - refusing!\n");
  return DR_ACCESSDENIED;
}

DirectResult
AppManager::switchFocus(SaWManWindowHandle window)
{
  D_INFO("switchFocus to %p!\n", (void*)(window));
  _focusWindow = window;
  return DR_OK;
}

DirectResult
AppManager::layerReconfig(SaWManLayerReconfig *reconfig)
{
  DFBDisplayLayerConfig* config = &reconfig->config;
  D_INFO( "Layer Reconfig signalled on layer %d:\n", reconfig->layer_id );

  if (reconfig->single)
    {
      D_INFO( "   going to single mode, window: %p\n", (void*)(reconfig->single) );

      SaWManWindowInfo info;
      //      DirectResult res = manager->GetWindowInfo(manager, reconfig->layer_id,
      //          &info);
      //XXX Do we need a SaWManWindowHandle instead of layer_id ?
      DirectResult res = _manager->GetWindowInfo(_manager, _focusWindow, &info);
      if (res)
        D_INFO( "   size: %dx%d\n", info.config.bounds.w, info.config.bounds.h );
    }

  config->width = 800;
  config->height = 450;
  D_INFO( "   requested layer size: %dx%d\n", config->width, config->height );

  return DR_OK;
}

DirectResult
AppManager::appIDChanged(SaWManWindowInfo *info)
{
  D_INFO("Window[%ld] application ID switched to %ld\n", info->win_id, info->application_id );
  return DR_OK;
}

//*****************************************************************************

void
AppManager::AddApplication(const char *name, const char *program,
    const char *args)
{
  Application* app;

  D_ASSERT( name != NULL );
  D_ASSERT( program != NULL );

  app = (Application *) D_CALLOC(1, sizeof(Application));
  if (!app)
    {
      D_OOM();
      return;
    }

  app->name = name;
  app->program = program;
  app->args = args;

  direct_list_append(&applications, &app->link);

  D_MAGIC_SET( app, Application );
}

AppManager::Application*
AppManager::LookupApplication(const char *name)
{
  Application* app;

  D_ASSERT( name != NULL );

  direct_list_foreach (app, applications)
    {
      D_MAGIC_ASSERT( app, Application );

      if (!strcmp(app->name, name))
        return app;
    }

  return NULL;
}

AppManager::Application*
AppManager::LookupApplicationByPID(pid_t pid)
{
  Application *app;

  direct_list_foreach (app, applications)
    {
      D_MAGIC_ASSERT( app, Application );

      if (app->pid == pid)
        return app;
    }

  return NULL;
}
