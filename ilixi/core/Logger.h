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

#ifndef ILIXI_LOGGER_H_
#define ILIXI_LOGGER_H_

#include "ilixiConfig.h"
#include "directfb.h"

namespace ilixi
{
#ifdef ILIXI_LOGGER_ENABLED
  D_DEBUG_DOMAIN( ILX, "ilixi", "ilixi");
  D_DEBUG_DOMAIN( ILX_CORE, "ilixi/core", "Important stuff");
  D_DEBUG_DOMAIN( ILX_GRAPHICS, "ilixi/graphics", "Graphics related");
  D_DEBUG_DOMAIN( ILX_LIB, "ilixi/lib", "For fun");
  D_DEBUG_DOMAIN( ILX_TYPES, "ilixi/types", "Types we like to use");
  D_DEBUG_DOMAIN( ILX_UI, "ilixi/ui", "Widgets");

  D_DEBUG_DOMAIN( ILX_APPBASE, "ilixi/core/AppBase", "AppBase");
  D_DEBUG_DOMAIN( ILX_EVENTMANAGER, "ilixi/core/EventManager", "EventManager");
  D_DEBUG_DOMAIN( ILX_THREAD, "ilixi/core/Thread", "Thread");
  D_DEBUG_DOMAIN( ILX_WINDOW, "ilixi/core/Window", "Window");

  D_DEBUG_DOMAIN( ILX_PAINTER, "ilixi/graphics/Painter", "Painter");
  D_DEBUG_DOMAIN( ILX_PALETTE, "ilixi/graphics/Palette", "Palette Parser");
  D_DEBUG_DOMAIN( ILX_STYLE, "ilixi/graphics/Style", "Style Parser");
  D_DEBUG_DOMAIN( ILX_SURFACE, "ilixi/graphics/Surface", "Surface");

  D_DEBUG_DOMAIN( ILX_BRUSH, "ilixi/types/Brush", "Brush");
  D_DEBUG_DOMAIN( ILX_FONT, "ilixi/types/Font", "Font");
  D_DEBUG_DOMAIN( ILX_IMAGE, "ilixi/types/Image", "Image");
  D_DEBUG_DOMAIN( ILX_PEN, "ilixi/types/Pen", "Pen");

  D_DEBUG_DOMAIN( ILX_APPLICATION, "ilixi/ui/Application", "Application");
  D_DEBUG_DOMAIN( ILX_BORDERBASE, "ilixi/ui/BorderBase", "BorderBase");
  D_DEBUG_DOMAIN( ILX_BUTTON, "ilixi/ui/Button", "Button");
  D_DEBUG_DOMAIN( ILX_CHECKBOX, "ilixi/ui/CheckBox", "CheckBox");
  D_DEBUG_DOMAIN( ILX_COMBOBOX, "ilixi/ui/ComboBox", "ComboBox");
  D_DEBUG_DOMAIN( ILX_CONTAINER, "ilixi/ui/ContainerBase", "ContainerBase");
  D_DEBUG_DOMAIN( ILX_DIALOG, "ilixi/ui/Dialog", "Dialog");
  D_DEBUG_DOMAIN( ILX_FRAME, "ilixi/ui/Frame", "Frame");
  D_DEBUG_DOMAIN( ILX_HBOX, "ilixi/ui/HBox", "HBox");
  D_DEBUG_DOMAIN( ILX_ICON, "ilixi/ui/Icon", "Icon");
  D_DEBUG_DOMAIN( ILX_LABEL, "ilixi/ui/Label", "Label");
  D_DEBUG_DOMAIN( ILX_LAYOUT, "ilixi/ui/Layout", "Layout");
  D_DEBUG_DOMAIN( ILX_PUSHBUTTON, "ilixi/ui/PushButton", "PushButton");
  D_DEBUG_DOMAIN( ILX_RADIOBUTTON, "ilixi/ui/RadioButton", "RadioButton");
  D_DEBUG_DOMAIN( ILX_SCROLLAREA, "ilixi/ui/ScrollArea", "ScrollArea");
  D_DEBUG_DOMAIN( ILX_SPACER, "ilixi/ui/Spacer", "Spacer");
  D_DEBUG_DOMAIN( ILX_SURFACEVIEW, "ilixi/ui/SurfaceView", "SurfaceView");
  D_DEBUG_DOMAIN( ILX_TEXTBASE, "ilixi/ui/TextBase", "TextBase");
  D_DEBUG_DOMAIN( ILX_TOOLBUTTON, "ilixi/ui/ToolButton", "ToolButton");
  D_DEBUG_DOMAIN( ILX_VBOX, "ilixi/ui/VBox", "VBox");
  D_DEBUG_DOMAIN( ILX_WIDGET, "ilixi/ui/Widget", "Widget");
  D_DEBUG_DOMAIN( ILX_WINDOWWIDGET, "ilixi/ui/WindowWidget", "WindowWidget");

  /*!
   * Opens a connection to Syslog in preparation for submitting messages.
   */
  void
  ilixi_log_init(char* ident, int facility = 0);

  /*!
   * Closes the current Syslog connection.
   */
  void
  ilixi_log_close();

  /*!
   * Submits a message to the Syslog facility on the same system.
   */
  void
  ilixi_log(int level, const char* message, ...);

#ifdef ILIXI_LOG_DEBUG_ENABLED
#define ILOG_DEBUG( Domain, _fmt... )   D_DEBUG_AT(Domain, _fmt)
#define ILOG_INFO( Domain, _fmt...)     D_LOG(Domain, INFO, _fmt)
#define ILOG_NOTICE( Domain, _fmt...)   D_LOG(Domain, NOTICE, _fmt)
#define ILOG_WARNING( Domain, _fmt...)  D_LOG(Domain, WARNING, _fmt)

//#define ILOG_DEBUG(_fmt...)     ilixi_log(7, _fmt)
//#define ILOG_INFO(_fmt...)      ilixi_log(6, _fmt)
//#define ILOG_NOTICE(_fmt...)    ilixi_log(5, _fmt)
//#define ILOG_WARNING(_fmt...)   ilixi_log(4, _fmt)

#else // ILIXI_LOG_DEBUG_ENABLED = 0
#define ILOG_DEBUG(_fmt...)     do {} while(0)
#define ILOG_INFO(_fmt...)      do {} while(0)
#define ILOG_NOTICE(_fmt...)    do {} while(0)
#define ILOG_WARNING(_fmt...)   do {} while(0)
#endif // end ILIXI_LOG_DEBUG_ENABLED
#define ILOG_ERROR(Domain, _fmt...)     D_LOG(Domain, ERROR, _fmt)
#define ILOG_FATAL(Domain, _fmt...)     D_LOG(Domain, FATAL, _fmt)

//#define ILOG_ERROR(_fmt...)     ilixi_log(3, _fmt)
//#define ILOG_FATAL(_fmt...)     ilixi_log(2, _fmt)
#define ILOG_OPEN(ident)        ilixi_log_init(ident)
#define ILOG_CLOSE()            ilixi_log_close()
#else // ILIXI_LOGGER_ENABLED = 0
#define ILOG_DEBUG(_fmt...)     do {} while(0)
#define ILOG_INFO(_fmt...)      do {} while(0)
#define ILOG_NOTICE(_fmt...)    do {} while(0)
#define ILOG_WARNING(_fmt...)   do {} while(0)
#define ILOG_ERROR(_fmt...)     do {} while(0)
#define ILOG_FATAL(_fmt...)     do {} while(0)
#define ILOG_OPEN(ident)        do {} while(0)
#define ILOG_CLOSE()            do {} while(0)
#endif // end ILIXI_LOGGER_ENABLED
}
#endif /* ILIXI_LOGGER_H_ */
