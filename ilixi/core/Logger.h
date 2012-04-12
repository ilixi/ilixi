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
#if ILIXI_LOGGER_ENABLED
  D_DEBUG_DOMAIN( ILX, "ilixi", "ilixi");
  D_DEBUG_DOMAIN( ILX_CORE, "ilixi/core", "Important stuff");
  D_DEBUG_DOMAIN( ILX_GRAPHICS, "ilixi/graphics", "Graphics related");
  D_DEBUG_DOMAIN( ILX_LIB, "ilixi/lib", "For fun");
  D_DEBUG_DOMAIN( ILX_TYPES, "ilixi/types", "Types we like to use");
  D_DEBUG_DOMAIN( ILX_UI, "ilixi/ui", "Widgets");

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

#if ILIXI_LOG_DEBUG_ENABLED
#define ILOG_DEBUG( Domain, _fmt... )   D_LOG(Domain, VERBOSE, _fmt)
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
