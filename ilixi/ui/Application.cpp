/*
 Copyright 2012 Tarik Sekmen.

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

Application::Application(int argc, char* argv[], AppOptions opts) :
    AppBase(argc, argv, opts), WindowWidget(), _backgroundImage(NULL)
{
  // TODO parse command line arguments here...
  // parse app-meta file...
  ILOG_TRACE_W(ILX_APPLICATION);
  initDFB(argc, argv);
  setStylist(new Stylist());
  setBackgroundFilled(false);
  setMargins(5, 5, 5, 5);
  setCanvasPosition(Point(0, 0));
  setBorderStyle(NoBorder);

  setTitle("Untitled");
  sigAbort.connect(sigc::mem_fun(this, &Application::quit));
}

Application::~Application()
{
  delete _backgroundImage;
  delete _stylist;
  ILOG_TRACE_W(ILX_APPLICATION);
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
  setAppState(APS_TERM);
}

void
Application::exec()
{
  ILOG_INFO(ILX_APPLICATION, "Starting...\n");

  if (__options & OptExclusive)
    {
      if (__layer->SetCooperativeLevel(__layer, DLSCL_EXCLUSIVE))
        ILOG_ERROR(ILX_APPLICATION, "Error while setting EXLUSIVE mode!\n");
      else
        ILOG_INFO(ILX_APPLICATION, "Now running in exclusive mode.\n");
    }

  show();

  setLayerSize(width(), height());
  if (_backgroundImage)
    _backgroundImage->setSize(width(), height());

//  fpsInit(&_fps);

  while (true)
    {
      if (__state & APS_TERM)
        break;
      else
        {
          runCallbacks();
          handleEvents();
          updateWindows();
//          ILOG_DEBUG(ILX_APPLICATION, "FPS: %s\n", _fps.fps_string);
//          fpsCount(&_fps, 1000);
        }
    }

  hide();

  ILOG_INFO(ILX_APPLICATION, "Stopping...\n");

  sigQuit();
}

void
Application::setBackgroundImage(std::string imagePath)
{
  if (_backgroundImage)
    delete _backgroundImage;

  _backgroundImage = new Image(imagePath);
  setBackgroundFilled(true);

  ILOG_DEBUG( ILX_APPLICATION, "Background is set [%s]\n", imagePath.c_str());
}

void
Application::show()
{
  if (__state & APS_HIDDEN)
    {
      showWindow();
      __state = APS_VISIBLE;
      sigVisible();
    }
}

void
Application::hide()
{
  if (__state & APS_VISIBLE)
    {
      closeWindow();
      __state = APS_HIDDEN;
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
Application::postUserEvent(unsigned int type, void* data)
{
  DFBUserEvent event;
  event.clazz = DFEC_USER;
  event.type = type;
  event.data = data;

  __buffer->PostEvent(__buffer, DFB_EVENT(&event));
}

void
Application::compose(const PaintEvent& event)
{
  ILOG_TRACE_W(ILX_APPLICATION);
  Painter painter(this);
  painter.begin(event.rect);
  stylist()->drawAppFrame(&painter, this);
  painter.end();
}
