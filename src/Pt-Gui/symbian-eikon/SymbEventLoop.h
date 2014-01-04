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
#include <Pt/System/Condition.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Selector.h>
#include <Pt/System/Thread.h>

#include <e32base.h>
#include <e32std.h> 
#include <pthread.h>

// some forward declarations
namespace Pt { 

class Event; 

namespace Gui 
{ 

class ApplicationImpl; 

} 
}

/**
 * @brief This class implements an event loop that can dispatch events from
 * other threads in the main thread using an active object.
 * 
 * Explanation:
 * The Active Scheduler will handle requests from asynchronous services and
 * call the responsible active object which in turn handles the request result
 * within a RunL method. 
 * 
 * Our "asynchronous service" is a worker thread which will act as event loop
 * and sleep until an event is committed. After waking up the event loop it will
 * check whether there is an event in the queue. A request completion will be
 * signaled to the active scheduler which will call RunL in the main application 
 * thread where we can dispatch the event.
 *
 * Certain problems can arise when processing events in a RunL function. 
 * The active scheduler runs active objects in a cooperative fashion, i.e.
 * once a RunL function is called it will not be interrupted. If you block
 * RunL you might block the entire application or cause undesired effects 
 * like panics etc. 
 * 
 * This class is based on work of Rob Cliff from Savage Minds used in his
 * Symbian TightVNC port. His approaches can be read here:
 * http://developer.symbian.com/wiki/download/attachments/1411/Porting+TightVNC+to+Symbian+OS+using+P.I.P.S..pdf?version=1
 * 
 */
class CSymbEventLoop : public CActive
{
public:
    /**
     * @brief Symbian factory method which constructs the event loop instance.
     *
     * @param appImpl Reference to running platform specific application implementation.
     */
    static CSymbEventLoop* NewL(Pt::Gui::ApplicationImpl& appImpl);

    /**
     * @brief Regular destructor.
     */
    ~CSymbEventLoop();    
    
    /**
     * @brief Add event loop to the active scheduler and process events that
     * have been queued until start has been called.
     * @return true if event loop has successfully started, false otherwise.
     */
    bool Start();    

    /**
     * @brief Set active object as active and signal event loop thread to 
     * start waiting for events.
     */
    void WaitForEvents();
    
    /**
     * @brief Stop event loop thread and remove active object from active scheduler.
     * @return true if event loop has successfully stopped, false otherwise.
     */ 
    bool Stop();

    /**
     * Commit event to queue and wake up event loop to deliver queued events.
     * In this implementation this call will block when currently queued events 
     * are being processed.
     * 
     * @see Pt::Application::commitEvent(const Pt::Event&)    
     */
    void CommitEvent(const Pt::Event& e);

    /**
     * Queue event. If event loop is sleeping event will not be delivered.
     * 
     * @see Pt::Application::queueEvent(const Pt::Event&)    
     */
    void QueueEvent(const Pt::Event& e);    

    /**
     * Deliver queued events.
     * @return Returns false if the last processed event was an exit application event. True otherwise.
     * 
     * @see Pt::Application::processEvents()
     */
    bool ProcessEvents();
    
protected:
    /**
     * @brief From CActive: Will be called from active scheduler when there are events in the event queue.
     */
    void RunL();

    TInt RunError(TInt err);
    
    /**
     * @brief From CActive: Will be called upon Cancel() to cancel outstanding request.
     */
    void DoCancel();
    
private:
    /**
     * @brief Private first constructor (Symbian two phase construction).
     * 
     * @param appImpl Reference to running platform specific application implementation.
     */
    CSymbEventLoop(Pt::Gui::ApplicationImpl& appImpl);

    /**
     * @brief Private second constructor (Symbian two phase construction). 
     */
    void ConstructL();    

    /**
     * @brief Destroy events which are still in queue but don't dispatch them.
     */
    void DrainQueue();
    
    /**
     * @brief Process events which have been queued without checking whether event loop is running.
     * @return Returns false if the last processed event was an exit application event. True otherwise.
     */
    bool ProcessQueuedEvents();
    
    /**
     * @brief Wake up event loop in case it is sleeping and waiting for events. 
     * If it's not sleeping this call has no effect.
     */
    void Wake();
    
    /**
     * @brief Create pthread with entry point in EventLoopThreadEntry() and wait for thread to start.
     * 
     * @see EventLoopThreadEntry
     */
    bool CreateThread();
    
    /**
     * @brief Create pthread event loop entry point. This will simply call EventLoopThread()
     * 
     * @see EventLoopThread
     */
    static void* EventLoopThreadEntry(void* threadID);
    
    /**
     * @brief Event loop thread
     */
    void EventLoopThread();

    /**
     * @brief Set state of event thread. Thread-safe.
     */
    void SetRunning(bool running);
    
    /**
     * @brief Query state of event thread. Thread-safe.
     */
    bool IsRunning();
    
    /**
     * @brief Dispatch event to receiver. 
     * @return Returns false if the event was an exit application event. True otherwise.
     */
    bool DispatchEvent(Pt::Event* event);
    
    // This is our event dispatcher.
    Pt::Gui::ApplicationImpl& _appImpl;
    
    // Main thread ID
    TThreadId               _mainThreadId;

    // Event loop thread state
    bool _running;
    Pt::System::Mutex       _runningMutex;

    // Mutex to protect base class iStatus
    Pt::System::Mutex       _mutex;
    Pt::System::Condition   _cond;
    
    // Event thread starting condition.
    Pt::System::Condition   _startCond;

    // pthread handle
    pthread_t _thread;

    // Event queue and mutex for thread safe access.
    std::list<Pt::Event*>   _eventQueue;
    Pt::System::Mutex       _queueMutex;
    
    // Mutex to lock when messages are being processed in main thread
    Pt::System::Mutex       _processMutex;

    // Condition used to sleep until events are queued
    Pt::System::Mutex        _wakeMutex;
    Pt::System::Condition    _wakeCondition;
    
    // TODO: Similar to Pt::System::EventLoop a first version of this
    // event loop used a selector to wait for incoming events.
    // This was not very reliable with the PIPS plugin, after a certain
    // amount of wake calls the selector did wait infinitely.
    // It was replaced with the above condition afterwards.
    //Pt::System::Selector    _selector;
};

#endif /*SYMBEVENTLOOP_H_*/
