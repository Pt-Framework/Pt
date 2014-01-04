/*
 * Copyright (C) 2005-2010 by Dr. Marc Boris Duerner
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
#ifndef Pt_System_LogChannel_h
#define Pt_System_LogChannel_h

#include <Pt/System/Api.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <cstddef>

namespace Pt {

namespace System {

/** @brief Logging channel

    This is the base class for all logging channels use by the logging targets
    in the LogManager. Channels are either loaded as a plugin or created by the
    LogManager on startup. Outside of the logging framework the channels are
    opaque and only referred to by their URL's. A channel supports synchronous
    and asynchronous logging, whereby the latter one usually involves a thread
    and a message qeueue.
    
    @ingroup Logging
*/
class PT_SYSTEM_API LogChannel : protected Pt::NonCopyable
{
    protected:
        /** @brief Default constructor
        */
        LogChannel()
        : _refs(0)
        {}

    public:
        /** @brief Destructor
        */
        virtual ~LogChannel()
        {}

        std::size_t ref()
        { return ++_refs; }

        std::size_t unref()
        { return --_refs; }

        const std::string& url() const
        { return _url; }

        /** @brief Open the channel from URL

            The URL is specific to the channel and may contain attributes
            to open it correctly.
        */
        void open(const std::string& urlstr)
        { 
          _url = urlstr;
          this->onOpen(urlstr); 
        }

        /** @brief Closes the channel
        */
        void close()
        { 
            this->onClose(); 
            _url.clear();
        }

        /** @brief Writes data to the channel
        */
        void write(const std::string& message)
        { this->onWrite( message.data(), message.size() ); }

    protected:
        /** @brief Open the channel from URL

            The URL is specific to the channel and may contain attributes
            to open it correctly.
        */
        virtual void onOpen(const std::string& url) = 0;

        /** @brief Closes the channel
        */
        virtual void onClose() = 0;

        /** @brief Writes data to the channel
        */
        virtual void onWrite(const char* msg, std::size_t msglen) = 0;
        
    private:
        std::string _url;
        std::size_t _refs;
};

} // namespace System

} // namespace Pt

#endif // Pt_System_LogChannel_h
