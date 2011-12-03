/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
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
#ifndef Pt_Gui_cocoa_ApplicationImpl_h
#define Pt_Gui_cocoa_ApplicationImpl_h

#include <Pt/Gui/Api.h>
#include <Pt/Gui/Event.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Singleton.h>

#import <CoreFoundation/CFRunLoop.h>

namespace Pt {

namespace Gui {

class MainLoopImpl : public System::EventLoopImpl
{
    public:
        MainLoopImpl();

        MainLoopImpl(Allocator& a);

        virtual ~MainLoopImpl();

        void attach( System::Selectable& s )
        {}

        void detach( System::Selectable& s )
        {}

        void enable( System::Selectable& dev )
        {}

        void disable( System::Selectable& dev )
        {}

        void changed(System::Selectable& s)
        {}

    protected:
        virtual void onRun();

        virtual void onWake();

    private:
        CFRunLoopSourceRef _wakeSource;
};


class MainLoop : public Pt::System::EventLoop
               , public Pt::Singleton<MainLoop>
{
    public:
        MainLoop();

        ~MainLoop();

    protected:
        virtual void onAttach(System::Selectable&);

        virtual void onDetach(System::Selectable&);

        virtual void onEnable(System::Selectable& s);

        virtual void onDisable(System::Selectable& s);

        virtual void onReinit(System::Selectable& s);

        virtual void onChanged(System::Selectable& s);


    private:
        MainLoopImpl _impl;
};

} // namespace Gui

} // namespace Pt

#endif

