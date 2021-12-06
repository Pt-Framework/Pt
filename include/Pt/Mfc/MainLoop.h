/* 
 * Copyright (C) 2014 Marc Boris Duerner
 * Copyright (C) 2014 Laurentiu-Gheorghe Crisan
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

#ifndef Pt_Mfc_MainLoop_h
#define Pt_Mfc_MainLoop_h

#include <Pt/Mfc/Api.h>
#include <Pt/System/EventLoop.h>
#include <Pt/WinVer.h>
#include <vector>

#include <WinDef.h>

namespace Pt {

namespace Mfc {

class PT_MFC_API MainLoop : public System::EventLoop
{
    public:
        MainLoop();

        virtual ~MainLoop();

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

        Pt::System::Selector& selector()
        { return *_selector; }

        /** @brief Waits for next message.
        */
        bool pumpMessage();

    private:
        void processTimers();

        void handleWake();
    
    private:
        Pt::System::Selector* _selector;
        System::Mutex _mutex;
        System::TimerQueue _timerQueue;
        System::EventQueue _eventQueue;
        std::vector<System::Selectable*> _avail;  
        HANDLE _timer;
};

} // namespace

} // namespace

#endif
