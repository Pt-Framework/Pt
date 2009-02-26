/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Drner
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
#ifndef Pt_Channel_h
#define Pt_Channel_h

#include <Pt/Log/Api.h>
#include <Pt/NonCopyable.h>
#include <string>


namespace Pt {

namespace Log {

/** @brief Logging channel
    @ingroup Logging

    This is the base class for all logging channels use by the logging targets
    in the LogManager. Channels are either loaded as a plugin or created by the
    LogManager on startup. Outside of the logging framework the channels are
    opaque and only referred to by their URL's. A channel supports synchronous
    and asynchronous logging, whereby the latter one usually involves a thread
    and a message qeueue.
*/
class PT_LOG_API Channel : protected Pt::NonCopyable
{
    protected:
        /** @brief Default constructor
        */
        Channel()
        {}

    public:
        /** @brief Destructor
        */
        virtual ~Channel()
        {}

        /** @brief Open the channel from URL

            The URL is specific to the channel and may contain attributes
            to open it correctly.
        */
        void open(const std::string url)
        { this->_open(url); }

        /** @brief Closes the channel
        */
        void close()
        { this->_close(); }

        /** @brief Writes data to the channel
        */
        void write(const std::string& message)
        { this->_write( message); }

    protected:
        /** @brief Open the channel from URL

            The URL is specific to the channel and may contain attributes
            to open it correctly.
        */
        virtual void _open(const std::string& url) = 0;

        /** @brief Closes the channel
        */
        virtual void _close() = 0;

        /** @brief Writes data to the channel
        */
        virtual void _write(const std::string& message) = 0;
};

}

}


#endif


