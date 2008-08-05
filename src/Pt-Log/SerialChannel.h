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
#ifndef Pt_Log_SerialChannel_h
#define Pt_Log_SerialChannel_h

#include <Pt/Log/Api.h>
#include <Pt/Log/Channel.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Thread.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/SerialDevice.h>
#include <string>

namespace Pt {

namespace Log {

class PT_LOG_API WriteEvent : public Pt::Event
{
    public:
        WriteEvent(const std::string& msg)
        : _msg(msg)
        {
        }

        const std::string& message() const
        { return _msg; }

        ~WriteEvent()
        {
        }

        const std::type_info& typeInfo() const
        {
            return typeid(WriteEvent);
        }

	    Pt::Event& clone(Pt::Allocator& allocator) const
	    {
	        void* m = allocator.allocate( sizeof(WriteEvent) );
	        return *( new (m)WriteEvent(*this) );
	    }

	    void destroy(Pt::Allocator& allocator)
	    {
	        allocator.deallocate(this, sizeof(WriteEvent));
	    }

    private:
        std::string _msg;
};


class PT_LOG_API SerialChannel : public Pt::System::Thread
                               , public Pt::Connectable
                               , public Channel
{
    public:
        SerialChannel();

        ~SerialChannel();

    protected:
        void processEvent(const Pt::Event& ev);

        virtual void _open(const std::string& url);

        virtual void _close();

        virtual void _write(const std::string& message, bool isAsync);

    private:
        size_t _n;
        Pt::System::Mutex _mutex;
        Pt::System::EventLoop _threadLoop;
        Pt::System::SerialDevice _device;
};

} // namespace Log

} // namespace Pt


#endif


