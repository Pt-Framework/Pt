/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Drner                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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

            This method writes a message to its physical location. It the
            isAsync flag is true, the method will not block until the message
            is written.
        */
        void write(const std::string& message, bool isAsync = false)
        { this->_write( message, isAsync ); }

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

            This method writes a message to its physical location. It the
            isAsync flag is true, the method will not block until the message
            is written.
        */
        virtual void _write(const std::string& message, bool isAsync) = 0;
};

}

}


#endif


