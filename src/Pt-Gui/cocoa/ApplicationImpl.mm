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
#import "ApplicationImpl.h"

#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#import <Foundation/NSGeometry.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSRunLoop.h>

@interface PtGuiApplication : NSApplication
{
    NSAutoreleasePool* pool;
}

- (void) initPool;

- (void) dealloc;

@end

@implementation PtGuiApplication

- (void) initPool
{
    pool = [[NSAutoreleasePool alloc] init];
}

- (void) dealloc
{
    [super dealloc];
    [pool release];
}

@end


namespace Pt {

namespace Gui {

void MainLoopImplOnWake(void* p)
{
    MainLoopImpl* impl = reinterpret_cast<MainLoopImpl*>(p);

    NSEvent* event = nil;

    // process cocoa events
    do
    {
        event = [NSApp nextEventMatchingMask: NSAnyEventMask
                                   untilDate: nil
                                      inMode: NSDefaultRunLoopMode
                                     dequeue: YES];
    
        [NSApp sendEvent:event];
    }
    while(event != nil);

    impl->processAvail();

    // process Pt events
    bool isActive = impl->processEvents();

    // handle loop exit
    if( ! isActive)
    {
        // set stop flag
        [NSApp stop: nil];

        // post fake event so NSApp notices the stop flag
        event = [NSEvent otherEventWithType: NSApplicationDefined
                                   location: NSMakePoint(0,0)
                              modifierFlags: 0
                                  timestamp: 0.0
                               windowNumber: 0
                                    context: nil
                                    subtype: 0
                                      data1: 0
                                      data2: 0];

        [NSApp postEvent: event atStart: false];
    }
}


void MainLoopImplOnTimer(CFRunLoopTimerRef timer, void *p)
{
    MainLoopImpl* impl = reinterpret_cast<MainLoopImpl*>(p);
    impl->processTimers();

}


void MainLoopImplOnKQueue(CFFileDescriptorRef f, CFOptionFlags callBackTypes, void *p)
{
    MainLoopImpl* impl = reinterpret_cast<MainLoopImpl*>(p);
    impl->processKQueue();
}


MainLoopImpl::MainLoopImpl(Signal<const Pt::Event&>& eventSignal)
: _event(&eventSignal)
{
    [PtGuiApplication sharedApplication];
    [NSApp initPool];

    // NSRunLoop, CFRunLoop, CFFileDescriptor
    CFRunLoopSourceContext ctx;
    ctx.version = 0;
    ctx.info = this;
    ctx.retain = NULL;
    ctx.release = NULL;
    ctx.copyDescription = NULL;
    ctx.equal = NULL;
    ctx.hash = NULL;
    ctx.schedule = NULL;
    ctx.cancel = NULL;
    ctx.perform = &MainLoopImplOnWake;

    _wakeSource = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &ctx);

    CFFileDescriptorRef fdref = CFFileDescriptorCreate(kCFAllocatorDefault, _selector.kd(), false, 
                                                       &MainLoopImplOnKQueue, NULL);
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorReadCallBack);
    _kqueueSource = CFFileDescriptorCreateRunLoopSource(kCFAllocatorDefault, fdref, 0);

    CFRunLoopTimerContext timerCtx;
    timerCtx.version = 0;
    timerCtx.info = this;
    timerCtx.retain = NULL;
    timerCtx.release = NULL;
    timerCtx.copyDescription = NULL;

    _masterTimer = CFRunLoopTimerCreate( kCFAllocatorDefault,
                                         CFAbsoluteTimeGetCurrent(),
                                         10000.0, 0, 0, 
                                         &MainLoopImplOnTimer, &timerCtx);

    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopAddSource(rl, _wakeSource, kCFRunLoopCommonModes);
    CFRunLoopAddSource(rl, _kqueueSource, kCFRunLoopCommonModes);
    CFRunLoopAddTimer(rl, _masterTimer, kCFRunLoopCommonModes);
}


MainLoopImpl::MainLoopImpl(Signal<const Pt::Event&>& eventSignal, Allocator& a)
: _event(&eventSignal)
{
    [PtGuiApplication sharedApplication];
    [NSApp initPool];
    
    // NSRunLoop, CFRunLoop, CFFileDescriptor
    CFRunLoopSourceContext ctx;
    ctx.version = 0;
    ctx.info = this;
    ctx.retain = NULL;
    ctx.release = NULL;
    ctx.copyDescription = NULL;
    ctx.equal = NULL;
    ctx.hash = NULL;
    ctx.schedule = NULL;
    ctx.cancel = NULL;
    ctx.perform = &MainLoopImplOnWake;

    _wakeSource = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &ctx);

    CFFileDescriptorRef fdref = CFFileDescriptorCreate(kCFAllocatorDefault, _selector.kd(), false, 
                                                       &MainLoopImplOnKQueue, NULL);
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorReadCallBack);
    _kqueueSource = CFFileDescriptorCreateRunLoopSource(kCFAllocatorDefault, fdref, 0);

    CFRunLoopTimerContext timerCtx;
    timerCtx.version = 0;
    timerCtx.info = this;
    timerCtx.retain = NULL;
    timerCtx.release = NULL;
    timerCtx.copyDescription = NULL;

    _masterTimer = CFRunLoopTimerCreate( kCFAllocatorDefault,
                                         CFAbsoluteTimeGetCurrent(),
                                         10000.0, 0, 0, 
                                         &MainLoopImplOnTimer, &timerCtx);


    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopAddSource(rl, _wakeSource, kCFRunLoopCommonModes);
    CFRunLoopAddSource(rl, _kqueueSource, kCFRunLoopCommonModes);
    CFRunLoopAddTimer(rl, _masterTimer, kCFRunLoopCommonModes);
}


MainLoopImpl::~MainLoopImpl()
{
    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopRemoveSource(rl, _wakeSource, kCFRunLoopCommonModes);
    CFRelease(_wakeSource);

    CFRunLoopRemoveSource(rl, _kqueueSource, kCFRunLoopCommonModes);
    CFRelease(_kqueueSource);

    CFRunLoopRemoveTimer(rl, _masterTimer, kCFRunLoopCommonModes);
    CFRelease(_masterTimer);

    [NSApp release];
}


void MainLoopImpl::run()
{
    [NSApp run];
}


void MainLoopImpl::wake()
{
    CFRunLoopSourceSignal(_wakeSource);
    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopWakeUp(rl);
}


void MainLoopImpl::exit()
{
    _eventQueue.exit();
    wake();
}


void MainLoopImpl::commitEvent(const Pt::Event& event)
{
    _eventQueue.pushEvent(event); 
    wake();
}


void MainLoopImpl::queueEvent(const Pt::Event& event)
{
    _eventQueue.pushEvent(event);
}


bool MainLoopImpl::processEvents()
{ 
    return _eventQueue.processEvents(*_event);
}


void MainLoopImpl::processKQueue()
{ 
    _selector.waitForWake(0);
}


void MainLoopImpl::processAvail()
{
    while(true)
    {
        System::MutexLock lock(_mutex);
        if( _avail.empty() )
            break;

        System::Selectable* selectable = _avail.back();
        _avail.pop_back();
        lock.unlock();

        selectable->run();
    }
}


void MainLoopImpl::attach(System::Timer& timer)
{ 
    _timerQueue.addTimer(timer); 
    this->processTimers();
}


void MainLoopImpl::detach(System::Timer& timer)
{ 
    _timerQueue.removeTimer(timer);
    this->processTimers();
}


void MainLoopImpl::processTimers()
{ 
    CFTimeInterval nextTimer = _timerQueue.processTimers();

    if(nextTimer == System::EventLoop::WaitInfinite)
    {
        nextTimer = std::numeric_limits<CFTimeInterval>::max();
    }

    CFTimeInterval interval = nextTimer / 1000.0;
    CFAbsoluteTime fireDate = CFAbsoluteTimeGetCurrent() + interval;
    CFRunLoopTimerSetNextFireDate (_masterTimer, fireDate);
}


void MainLoopImpl::attach(System::Selectable& s)
{
    _selector.attach(s);
}


void MainLoopImpl::detach(System::Selectable& s)
{
    _selector.detach(s);
}


void MainLoopImpl::idle(System::Selectable& s)
{ 
    System::MutexLock lock(_mutex);

    std::vector<System::Selectable*>::iterator it = _avail.begin();
    while( it != _avail.end() )
    {
        if(*it == &s)
            it = _avail.erase(it);
        else
            ++it;
    }
}


void MainLoopImpl::avail(System::Selectable& s)
{  
    System::MutexLock lock(_mutex);
    _avail.push_back(&s);

    CFRunLoopSourceSignal(_wakeSource);
}


void MainLoopImpl::cancel(System::IOHandle& h)
{
    _selector.cancel(h);
}


void MainLoopImpl::beginRead(System::IOHandle* h)
{  
    _selector.beginRead(h);
    CFRunLoopSourceSignal(_kqueueSource);
}


void MainLoopImpl::endRead(System::IOHandle* h)
{  
    _selector.endRead(h);
    CFRunLoopSourceSignal(_kqueueSource);
}


void MainLoopImpl::beginWrite(System::IOHandle* h)
{  
    _selector.beginWrite(h);
    CFRunLoopSourceSignal(_kqueueSource);
}


void MainLoopImpl::endWrite(System::IOHandle* h)
{  
    _selector.endWrite(h);
    CFRunLoopSourceSignal(_kqueueSource);
}


bool MainLoopImpl::isReadable(System::IOHandle* h)
{  
    return _selector.isReadable(h);
}


bool MainLoopImpl::isWritable(System::IOHandle* h)
{
    return _selector.isWritable(h);  
}


bool MainLoopImpl::isError(System::IOHandle* h)
{
    return _selector.isError(h);
}

//
// MainLoop
//

MainLoop::MainLoop()
: System::EventLoop()
, _impl( event() )
{
}


MainLoop::~MainLoop()
{
}


void MainLoop::onAttachSelectable(System::Selectable& s)
{
    _impl.attach(s);
}


void MainLoop::onDetachSelectable(System::Selectable& s)
{
    _impl.detach(s);
}


void MainLoop::onIdle(System::Selectable& s)
{
    _impl.idle(s);
}


void MainLoop::onReady(System::Selectable& s)
{
    _impl.avail(s);
}


void MainLoop::onRun()
{
    _impl.run();
}


void MainLoop::onExit()
{
    _impl.exit();
}


void MainLoop::onCommitEvent(const Pt::Event& ev)
{
    _impl.commitEvent(ev);
}


void MainLoop::onQueueEvent(const Pt::Event& ev)
{
    _impl.queueEvent(ev);
}


void MainLoop::onProcessEvents()
{
    _impl.processEvents();
}


void MainLoop::onWake()
{
    _impl.wake();
}


void MainLoop::onAttachTimer(System::Timer& timer)
{
    _impl.attach(timer);
}


void MainLoop::onDetachTimer(System::Timer& timer)
{
    _impl.detach(timer);
}

} // namespace Gui

} // namespace Pt

