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

#include "ui/Application.h"
#include "graphics/Painter.h"
#include "graphics/Stylist.h"
#include "core/Logger.h"
#include <string.h>

using namespace ilixi;
using namespace IMaestro;

Application::Application(int argc, char* argv[]) :
    AppBase(argc, argv), WindowWidget(), _fullscreen(false), _backgroundImage(
        NULL)
{
  // TODO parse command line arguments here...
  // parse app-meta file...
  initDFB(NULL, NULL);
  setStylist(new Stylist());
  setBackgroundFilled(false);
  setMargins(5, 5, 5, 5);
  setCanvasPosition(Point(0, 0));
  setBorderStyle(NoBorder);
#if ILIXI_MULTI_ENABLED
  setAppState(Ready);
  callMaestro(Notification, Ready);
#endif
  setTitle("Untitled");
  sigAbort.connect(sigc::mem_fun(this, &Application::quit));
  ILOG_DEBUG("I am ready.");
  show();
}

Application::~Application()
{
  delete _backgroundImage;
  delete _stylist;
#if ILIXI_MULTI_ENABLED
  while (true)
    {
      usleep(1000);
      if (appState() & Quit)
      break;
    }
#endif
}

Image*
Application::background() const
{
  return (_backgroundImage);
}

void
Application::quit()
{
  setVisible(false);
  setAppState(Terminating);
}

void
Application::exec()
{
  ILOG_INFO( "Starting...");
#if ILIXI_MULTI_ENABLED
  setAppState((AppState) (Idle | Hidden));
  callMaestro(ModeRequest, Visible);
#else
  setAppState(Visible);
#endif
  // enter event loop
  DFBWindowEvent event;
  AppState state;
  while (true)
    {
      state = appState();
      if (state & Terminating)
        break;
#if ILIXI_MULTI_ENABLED
      else if (state & Idle || state & Hidden)
      usleep(10000);
#endif
      else
        {
          // Run callbacks...
          pthread_mutex_lock(&__cbMutex);
          for (CallbackList::iterator it = __callbacks.begin();
              it != __callbacks.end(); ++it)
            ((Callback*) *it)->_func(((Callback*) *it)->_data);
          if (!__callbacks.empty())
            __buffer->WakeUp(__buffer);
          pthread_mutex_unlock(&__cbMutex);

          // Handle events
          __buffer->WaitForEventWithTimeout(__buffer, 0, 100);
          while (__buffer->GetEvent(__buffer, DFB_EVENT(&event)) == DFB_OK)
            __activeWindow->handleWindowEvent(event);

          // Paint pending window updates for all windows...
          pthread_mutex_lock(&__windowMutex);
          for (WindowList::iterator it = __windowList.begin();
              it != __windowList.end(); ++it)
            ((Window*) *it)->updateWindow();
          pthread_mutex_unlock(&__windowMutex);
        }
    }
  ILOG_DEBUG( "Stopping...");
  sigQuit();
#if ILIXI_MULTI_ENABLED
  callMaestro(Notification, Terminating);
#endif
}

void
Application::setBackgroundImage(std::string imagePath)
{
  if (_backgroundImage)
    delete _backgroundImage;
  _backgroundImage = new Image(imagePath);
  _backgroundImage->setSize(getWindowSize());
  setBackgroundFilled(true);
  ILOG_DEBUG("Setting background to [%s]", imagePath.c_str());
}

void
Application::setFullScreen(bool fullscreen)
{
  _fullscreen = fullscreen;
}

void
Application::show()
{
  if (appState() & Hidden)
    {
      setVisible(true);

      initWindow();
      showWindow();

      clearAppState(Hidden);
      setAppState(Visible);
#if ILIXI_MULTI_ENABLED
      callMaestro(Notification, Visible);
#endif
      sigVisible();
    }
}

void
Application::hide()
{
  if (appState() & Visible)
    {
      setVisible(false);
      clearAppState(Visible);
      setAppState(Hidden);

      closeWindow();
#if ILIXI_MULTI_ENABLED
      callMaestro(Notification, Hidden);
#endif
      sigHidden();
    }
}

void
Application::setStylist(Stylist* stylist)
{
  if (!stylist)
    return;
  // TODO we will allow setting custom stylist in the future.
  if (_stylist)
    return;
  _stylist = stylist;
}

void
Application::compose()
{
  Painter painter(this);
  painter.begin();
  stylist()->drawAppFrame(&painter, this);
  painter.end();
}
#if ILIXI_MULTI_ENABLED
ReactionResult
Application::maestroCB(MaestroMessage *msg, void *ctx)
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
            switch (msg->state)
              {
                case Visible:
                ILOG_DEBUG("Received SwitchMode::Visible from Maestro.");
                show();
                return RS_OK;

                case Hidden:
                ILOG_DEBUG("Received SwitchMode::Hidden from Maestro.");
                hide();
                return RS_OK;

                case Quit:
                ILOG_DEBUG("Received SwitchMode::Quit from Maestro.");
                quit();
                return RS_OK;

                default:
                ILOG_WARNING("SwitchMode state (%d) is not supported!", msg->state);
                return RS_DROP;
              }
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

        case OSKEvent:
          {
            if (__activeWindow && msg->state == Ready)
              {
                __activeWindow->_eventManager->setOSKWidgetText(getOSKText());
                return RS_OK;
              }
            return RS_DROP;
          } // end OSKEvent

        default:
        ILOG_WARNING("Message type (%d) is not supported!", msg->type);
        return RS_DROP;
      }
  }
#endif
