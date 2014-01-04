/***************************************************************************
 *   Copyright (C) 2006-2012 Marc Boris Duerner                            *
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
#ifndef PT_SYSTEM_SELECTABLEIMPL_H
#define PT_SYSTEM_SELECTABLEIMPL_H

#include "Pt/System/IOError.h"

namespace Pt {

namespace System {

class EventLoop;
/*
class FdImpl
{
    public:
        virtual ~FdImpl()
        {}

        int fd() const
        {
            return _fd;
        }

        void setFd(int fd)
        {
            _fd = fd;
        }

        void closeFd();

        void attach(System::EventLoop& s);

        void detach(System::EventLoop& s);

        void avail();

        virtual int initSelect(fd_set& rfds, fd_set& wfds, fd_set& efds);

        virtual void exitSelect();

        virtual int checkEvent(fd_set& rfds, fd_set& wfds, fd_set& efds);

    protected:
        FdImpl()
        : _fd(-1)
        , _rfds(0)
        {
        }

        virtual void onInput() = 0;

    private:
        
        fd_set* _rfds;
};


inline void FdImpl::closeFd()
{
  if (_fd < 0)
      return;

    ::close(_fd);
    _fd = -1;
}


inline void FdImpl::attach(System::EventLoop& s)
{
}


inline void FdImpl::detach(System::EventLoop& s)
{
    this->exitSelect();
}


inline void FdImpl::avail()
{
}


inline int FdImpl::initSelect(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    _rfds = &rfds;

    if( this->fd() > 0)
    {
        FD_SET(this->fd(), _rfds);
    }

    return this->fd();
}


inline void FdImpl::exitSelect()
{
    if( _rfds && this->fd() > 0)
    {
        FD_CLR(this->fd(), _rfds);
    }

    _rfds = 0;
}


inline int FdImpl::checkEvent(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    if( this->fd() < 0)
        return 0;

    if( FD_ISSET(this->fd(), &rfds) )
    {
        this->onInput();
        return 1;
    }

    return 0;
}
*/
} // namespace System

} //namespace Pt

#endif
