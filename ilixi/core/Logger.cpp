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

#include <core/Logger.h>
#ifdef ILIXI_LOGGER_ENABLED
#include <cstdarg>
#include <syslog.h>

using namespace ilixi;
using namespace std;

void
ilixi::ilixi_log_init(char* ident, int facility)
{
#ifdef ILIXI_LOG_DEBUG_ENABLED
    setlogmask(LOG_UPTO(LOG_NOTICE));
#endif
    if (facility)
        openlog(ident, LOG_NDELAY | LOG_CONS | LOG_PID, facility);
    else
        openlog(ident, LOG_NDELAY | LOG_CONS | LOG_PID, LOG_USER);
}

void
ilixi::ilixi_log_close()
{
    closelog();
}

void
ilixi::ilixi_log(int level, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    vsyslog(level, message, args);
    va_end(args);
}

#endif
