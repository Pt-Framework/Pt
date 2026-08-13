/*
 * Copyright (C) 2014 Marc Boris Duerner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 * 02110-1301, USA.
 */

#ifndef Pt_Qt_MainLoop_h
#define Pt_Qt_MainLoop_h

#include <Pt/Qt/Api.h>
#include <Pt/System/EventLoop.h>
#include <QtCore/QCoreApplication>

namespace Pt {

namespace Qt {

class PT_QT_API MainLoop : public Pt::System::EventLoop
{
    public:
        MainLoop(QCoreApplication& app);
        
        virtual ~MainLoop();
        
        //! @internal
        Pt::System::Selector& selector();
   
    protected:
        virtual void onAttachSelectable(System::Selectable&);

        virtual void onDetachSelectable(System::Selectable&);

        virtual void onCancel(System::Selectable& s);

        virtual void onReady(System::Selectable& s);

        virtual void onRun();

        virtual void onExit();

        virtual void onCommitEvent(const Pt::Event& ev);

        virtual void onQueueEvent(const Pt::Event& ev);

        virtual void onProcessEvents();

        virtual void onWake();

        virtual void onAttachTimer(System::Timer& timer);

        virtual void onDetachTimer(System::Timer& timer);

    private:     
        class MainLoopImpl* _impl; 
};

} // namespace

} // namespace

#endif
