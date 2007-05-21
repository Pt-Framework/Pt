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
#include <Pt/Log/Message.h>
#include <Pt/NonCopyable.h>
#include <string>


namespace Pt {

namespace Log {

/** @brief Logging channel
    @ingroup Logging
*/
class PT_LOG_API Channel : protected Pt::NonCopyable
{
    protected:
        Channel()
        {}

    public:
        virtual ~Channel()
        {}

        void open(const std::string url)
        { this->_open(url); }

        void close()
        { this->_close(); }

        void write(const Message& message)
        {
            std::string level = toString( message.logLevel() );
            this->_write(message.timestamp().toIsoString() + " [" + message.target() + "] " + level + " - "  + message.text() + "\n" );
        }

    protected:
        virtual void _open(const std::string& url) = 0;

        virtual void _close() = 0;

        virtual void _write(const std::string& message) = 0;
};

}

}


#endif


