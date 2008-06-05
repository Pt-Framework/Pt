/***************************************************************************
 *   Copyright (C) 2008 Peter Barth                                        *
 *   Copyright (C) 2008 PTV AG                                             *
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
#ifndef SYMBEVENTLOOP_H_
#define SYMBEVENTLOOP_H_

#include <Pt/Event.h>
#include <Pt/System/Selector.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Condition.h>

#include <e32base.h>
#include <e32std.h> 
#include <pthread.h>

namespace Pt {

class Event;

namespace Gui {
class ApplicationImpl;
}

}

class SymbEventLoop : public CActive
{
public:
    static SymbEventLoop* NewL(Pt::Gui::ApplicationImpl& _appImpl);
    ~SymbEventLoop();    
    
    bool Start();    
    void Watch();
    
    bool Stop();

    void CommitEvent(const Pt::Event& e);

    void QueueEvent(const Pt::Event& e);    

    void ProcessEvents();
    
protected:
    // from CActive
    void RunL();
    void DoCancel();
    
private:
    void ProcessQueuedEvents();
    
    void Wake();
    
    SymbEventLoop(Pt::Gui::ApplicationImpl& _appImpl);
    void ConstructL();    

    bool CreateThread();
    static void* EventLoopThreadEntry(void* threadID);
    void EventLoopThread();

    Pt::Gui::ApplicationImpl& _appImpl;
    
    TThreadId _mainThreadId;
    
    bool _running;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    pthread_cond_t _startCond;

    pthread_t _thread;

    std::list<Pt::Event*>   _eventQueue;
    Pt::System::Mutex       _queueMutex;
    
    //Pt::System::Mutex       _wakeMutex;
    //Pt::System::Condition   _wakeMutexCond;
    Pt::System::Mutex       _processMutex;
    Pt::System::Selector    _selector;
};

#endif /*SYMBEVENTLOOP_H_*/
