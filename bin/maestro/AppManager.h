/*
 * AppManager.h
 *
 *  Created on: 8 Oct 2011
 *      Author: tarik
 */

#ifndef APPMANAGER_H_
#define APPMANAGER_H_

#include <sawman.h>
#include "Layout.h"

class AppManager
{

public:
  AppManager();

  virtual
  ~AppManager();

  bool
  init();

  void
  deinit();

  /*!
   * Starts an application.
   *
   * @param name application's name.
   * @param ret_pid
   */
  DirectResult
  startRequest(const char *name, pid_t *ret_pid);

  /*!
   * Stops an application.
   *
   * @param pid
   * @param caller
   */
  DirectResult
  stopRequest(pid_t pid, FusionID caller);

  /*!
   * Called when a process utilising DirectFB starts.
   *
   * @param process
   */
  virtual DirectResult
  processAdded(SaWManProcess *process);

  /*!
   * Called when a process utilising DirectFB stops.
   *
   * @param process
   */
  virtual DirectResult
  processRemoved(SaWManProcess *process);

  /*!
   * Consume an event before it is passed on to applications.
   *
   * @param event
   */
  virtual DirectResult
  inputFilter(DFBInputEvent *event);

  virtual DirectResult
  windowPreconfig(SaWManWindowConfig *config);

  virtual DirectResult
  windowAdded(SaWManWindowInfo *info);

  virtual DirectResult
  windowRemoved(SaWManWindowInfo *info);

  virtual DirectResult
  windowReconfig(SaWManWindowReconfig *reconfig);

  virtual DirectResult
  windowReStack(SaWManWindowHandle handle, SaWManWindowHandle relative,
      SaWManWindowRelation relation);

  virtual DirectResult
  stackResized(const DFBDimension *size);

  virtual DirectResult
  switchFocus(SaWManWindowHandle window);

  virtual DirectResult
  layerReconfig(SaWManLayerReconfig *reconfig);

  virtual DirectResult
  appIDChanged(SaWManWindowInfo *info);

  /*!
   * Add an application to allowed list.
   *
   * @param name
   * @param program
   * @param args
   */
  void
  AddApplication(const char *name, const char *program, const char *args);

private:

  struct Application
  {
    DirectLink link;
    int magic;
    const char* name;
    const char* program;
    const char* args;
    long long start_time;bool started;
    pid_t pid;
    SaWManProcess* process;
  };

  ISaWMan* _saw;
  ISaWManManager* _manager;
  DirectLink* applications;
  SaWManScalingMode _scalingMode;
  SaWManWindowHandle _focusWindow;

  SaWManCallbacks callbacks;

  Layout* _layout;

  static pthread_mutex_t __appManLock;
  static AppManager* __instance;

  Application*
  LookupApplication(const char *name);

  Application*
  LookupApplicationByPID(pid_t pid);

  // SaWMan callbacks...

  friend DirectResult
  start_request(void *context, const char *name, pid_t *ret_pid);

  friend DirectResult
  stop_request(void *context, pid_t pid, FusionID caller);

  friend DirectResult
  process_added(void *context, SaWManProcess *process);

  friend DirectResult
  process_removed(void *context, SaWManProcess *process);

  friend DirectResult
  input_filter(void *context, DFBInputEvent *event);

  friend DirectResult
  window_preconfig(void *context, SaWManWindowConfig *config);

  friend DirectResult
  window_added(void *context, SaWManWindowInfo *info);

  friend DirectResult
  window_removed(void *context, SaWManWindowInfo *info);

  friend DirectResult
  window_reconfig(void *context, SaWManWindowReconfig *reconfig);

  friend DirectResult
  window_restack(void *context, SaWManWindowHandle handle,
      SaWManWindowHandle relative, SaWManWindowRelation relation);

  friend DirectResult
  stack_resized(void *context, const DFBDimension *size);

  friend DirectResult
  switch_focus(void *context, SaWManWindowHandle window);

  friend DirectResult
  layer_reconfig(void *context, SaWManLayerReconfig *reconfig);

  friend DirectResult
  application_id_changed(void *context, SaWManWindowInfo *info);

};

#endif /* APPMANAGER_H_ */
