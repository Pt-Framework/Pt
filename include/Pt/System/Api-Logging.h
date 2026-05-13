/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_SYSTEM_API_LOGGING_H
#define PT_SYSTEM_API_LOGGING_H

/** @defgroup Logging Logging

    @brief Efficient multi-threaded logging with configurable output channels.

    The logging framework offers an efficient, extensible system to
    log messages from programs with multiple threads to a number of
    channels. Logging can be completely disabled at compile time, when the
    logging macros are used. At runtime, log messages are filtered by
    a level of severity. Filtering is very efficient, because log messages
    are not even built if their log level is too low.
    Currently three types of output channels exist, logging to files
    with file rolling, to the console and to the serial port. The logging
    framework can be extended by new channels.
*/

#endif
