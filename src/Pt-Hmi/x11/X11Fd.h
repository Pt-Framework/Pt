/***************************************************************************
 *   Copyright (C) 2013 Marc Boris Duerner                                 *
 *   Copyright (C) 2013 Laurentiu-Ghoerghe Crisan                          *
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
#ifndef Pt_Hmi_X11Fd_ApplicationImpl_h
#define Pt_Hmi_X11Fd_ApplicationImpl_h

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

// X11 defines these two globally, which conflicts with enum values in Pt/Char.h
#undef Above
#undef Below

#include <Pt/Hmi/Api.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Singleton.h>
#include <Pt/Event.h>
#include "posix/Selector.h"
#include <map>
#include <unistd.h>

namespace Pt {
namespace Hmi {

class X11Fd : public System::Selectable
{
public:
    X11Fd(Display* display);

    ~X11Fd();

    void begin()
    {      
      if( ! _loop )
        throw std::logic_error("I/O device not active");

        Pt::System::Selector& selector = _loop->selector();
        selector.beginRead(&_ioh);
    }

    void close()
    { 
        if(_ioh.fd != -1)
        {
            ::close(_ioh.fd); 
            _ioh.fd = -1;
        }
    }

    void flush()
    { this->onRun(); }
    
protected:
    virtual bool onRun();

    virtual void onCancel()
    { throw std::logic_error("not implemented"); }
    

    void onAttach(System::EventLoop& loop);

    void onDetach(System::EventLoop& loop);    

private:
    Display* _display;
    XEvent _xev;
    Pt::System::IOHandle _ioh;
    Pt::System::EventLoop* _loop;
};

}}

#endif
