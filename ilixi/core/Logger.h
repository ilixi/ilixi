/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include <ilixiConfig.h>
#include <directfb.h>
#include <stdexcept>
#include <lib/Util.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX, "ilixi", "ilixi");
D_DEBUG_DOMAIN( ILX_CORE, "ilixi/core", "Important stuff");
D_DEBUG_DOMAIN( ILX_GRAPHICS, "ilixi/graphics", "Primarily graphics related");
D_DEBUG_DOMAIN( ILX_LIB, "ilixi/lib", "Non-essential helpers");
D_DEBUG_DOMAIN( ILX_TYPES, "ilixi/types", "Types");
D_DEBUG_DOMAIN( ILX_UI, "ilixi/ui", "Widgets");

#ifdef ILIXI_LOGGER_ENABLED
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
#define ILOG_TRACE(Domain)              D_DEBUG_AT(Domain, "[%p] %s()\n", this, __FUNCTION__)
#define ILOG_TRACE_F(Domain)            D_DEBUG_AT(Domain, "%s()\n", __FUNCTION__)
#define ILOG_TRACE_W(Domain)            D_DEBUG_AT(Domain, "[%d:%p] %s()\n", id(), this, __FUNCTION__)
#define ILOG_DEBUG( Domain, _fmt... )   D_DEBUG_AT(Domain, _fmt)
#define ILOG_INFO( Domain, _fmt...)     D_LOG(Domain, INFO, _fmt)
#define ILOG_NOTICE( Domain, _fmt...)   D_LOG(Domain, NOTICE, _fmt)
#define ILOG_WARNING( Domain, _fmt...)  D_LOG(Domain, WARNING, _fmt)
#else // ILIXI_LOG_DEBUG_ENABLED = 0
#define ILOG_TRACE(Domain)              do {} while(0)
#define ILOG_TRACE_F(Domain)            do {} while(0)
#define ILOG_TRACE_W(Domain)            do {} while(0)
#define ILOG_DEBUG(_fmt...)             do {} while(0)
#define ILOG_INFO(_fmt...)              do {} while(0)
#define ILOG_NOTICE(_fmt...)            do {} while(0)
#define ILOG_WARNING(_fmt...)           do {} while(0)
#endif // end ILIXI_LOG_DEBUG_ENABLED
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
#define ILOG_ERROR(Domain, _fmt...)     D_LOG(Domain, ERROR, _fmt)
#define ILOG_FATAL(Domain, _fmt...)     D_LOG(Domain, FATAL, _fmt)
#else
#define ILOG_ERROR(Domain, _fmt...)     D_ERROR(_fmt)
#define ILOG_FATAL(Domain, _fmt...)     D_ERROR(_fmt)
#endif
#else // ILIXI_LOGGER_ENABLED = 0
#define ILOG_TRACE(Domain)              do {} while(0)
#define ILOG_TRACE_F(Domain)            do {} while(0)
#define ILOG_TRACE_W(Domain)            do {} while(0)
#define ILOG_DEBUG(_fmt...)             do {} while(0)
#define ILOG_INFO(_fmt...)              do {} while(0)
#define ILOG_NOTICE(_fmt...)            do {} while(0)
#define ILOG_WARNING(_fmt...)           do {} while(0)
#define ILOG_ERROR(Domain,_fmt...)      do { printf(" -> ERROR (%s): ", Domain.name); printf(_fmt); } while(0)
#define ILOG_FATAL(Domain,_fmt...)      do { printf(" -> FATAL (%s): ", Domain.name); printf(_fmt); } while(0)
#define ILOG_OPEN(ident)                do {} while(0)
#define ILOG_CLOSE()                    do {} while(0)
#endif // end ILIXI_LOGGER_ENABLED
#define ILOG_THROW(Domain, _msg)        do { ILOG_FATAL(Domain, _msg); throw std::runtime_error( _msg ); } while(0)
}
#endif /* ILIXI_LOGGER_H_ */
