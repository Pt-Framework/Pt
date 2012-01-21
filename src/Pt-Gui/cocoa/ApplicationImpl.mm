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


void MainLoopImplOnFd(CFFileDescriptorRef f, CFOptionFlags flags, void *p)
{
    System::IOHandle* h = reinterpret_cast<System::IOHandle*>(p);

    if(flags & kCFFileDescriptorReadCallBack)
        h->enableFilters = System::IOHandle::FilterRead;
    else if(flags & kCFFileDescriptorWriteCallBack)
        h->enableFilters = System::IOHandle::FilterWrite;

    System::Selectable* s = h->sel;
    s->run();
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
    CFRunLoopAddTimer(rl, _masterTimer, kCFRunLoopCommonModes);
}


MainLoopImpl::~MainLoopImpl()
{
    while( ! _selectables.empty() )
    {
        _selectables.first()->detach();
    }

    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopRemoveSource(rl, _wakeSource, kCFRunLoopCommonModes);
    CFRelease(_wakeSource);

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
    _selectables.insert(s);
}


void MainLoopImpl::detach(System::Selectable& s)
{
    System::SelectableList::unlink(s);
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
    size_t id = h.id;

    if(id == System::IOHandle::InvalidId)
        return;

    h.enableFilters = 0;
    IOEntry& entry = _iotable[id];

    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopRemoveSource(rl, entry.source, kCFRunLoopCommonModes);
    CFRelease(entry.source);
    CFRelease(entry.fd);

    if(_iotable.size() > 1)
    {
        _iotable.back().iohandle->id = id;
        _iotable[id] = _iotable.back();
    }

    h.id = System::IOHandle::InvalidId;
    _iotable.resize(_iotable.size() -1);

}


MainLoopImpl::IOEntry& MainLoopImpl::enableIOHandle(System::IOHandle* h)
{
    if(h->id == System::IOHandle::InvalidId)
    {
        CFFileDescriptorContext ctx;
        ctx.version = 0;
        ctx.info = h;
        ctx.retain = NULL;
        ctx.release = NULL;
        ctx.copyDescription = NULL;

        CFFileDescriptorRef fdref = CFFileDescriptorCreate(kCFAllocatorDefault, h->fd, false, 
                                                           &MainLoopImplOnFd, NULL);

        CFRunLoopSourceRef fdsource = CFFileDescriptorCreateRunLoopSource(kCFAllocatorDefault, fdref, 0);
    
        CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
        CFRunLoopAddSource(rl, fdsource, kCFRunLoopCommonModes);

        IOEntry entry(*h, fdsource, fdref);

        size_t id = _iotable.size();
        _iotable.push_back(entry);
        h->id = id;
        return _iotable.back();
    }

    return _iotable[h->id];
}


void MainLoopImpl::beginRead(System::IOHandle* h)
{  
    CFFileDescriptorRef fdref = enableIOHandle(h).fd;
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorReadCallBack);
}


void MainLoopImpl::endRead(System::IOHandle* h)
{
    CFFileDescriptorRef fdref = _iotable[h->id].fd;
    CFFileDescriptorDisableCallBacks(fdref, kCFFileDescriptorReadCallBack);
}


void MainLoopImpl::beginWrite(System::IOHandle* h)
{  
    CFFileDescriptorRef fdref = enableIOHandle(h).fd;
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorWriteCallBack);
}


void MainLoopImpl::endWrite(System::IOHandle* h)
{
    CFFileDescriptorRef fdref = _iotable[h->id].fd;
    CFFileDescriptorDisableCallBacks(fdref, kCFFileDescriptorWriteCallBack);
}


bool MainLoopImpl::isReadable(System::IOHandle* h)
{  
    bool isReady = h->enableFilters == System::IOHandle::FilterRead;
    if(isReady)
        h->enableFilters = 0;
    return isReady;
}


bool MainLoopImpl::isWritable(System::IOHandle* h)
{
    bool isReady = h->enableFilters == System::IOHandle::FilterWrite;
    if(isReady)
        h->enableFilters = 0;
    return isReady;
}


bool MainLoopImpl::isError(System::IOHandle* h)
{
    return false;
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

