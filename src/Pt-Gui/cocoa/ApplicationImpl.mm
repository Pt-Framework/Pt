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
    MainLoop* impl = reinterpret_cast<MainLoop*>(p);
    impl->processEvents();
}


void MainLoopImplOnTimer(CFRunLoopTimerRef timer, void *p)
{
    MainLoop* impl = reinterpret_cast<MainLoop*>(p);
    impl->processTimers();

}


void MainLoopImplOnFd(CFFileDescriptorRef f, CFOptionFlags flags, void *p)
{
    System::IOHandle* h = reinterpret_cast<System::IOHandle*>(p);

    if(flags & kCFFileDescriptorReadCallBack)
    {
        h->events &= ~System::IOHandle::Read;
        h->ready = System::IOHandle::Read;
    }
    else if(flags & kCFFileDescriptorWriteCallBack)
    {
        h->events &= ~System::IOHandle::Write;
        h->ready = System::IOHandle::Write;
    }

    System::Selectable* s = h->sel;
    s->run();
}


MainLoop::MainLoop()
: System::EventLoop()
{
    init();
}


MainLoop::~MainLoop()
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


void MainLoop::init()
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


void MainLoop::onAttachSelectable(System::Selectable& s)
{
    _selectables.insert(s);
}


void MainLoop::onDetachSelectable(System::Selectable& s)
{
    System::SelectableList::unlink(s);
}


void MainLoop::onIdle(System::Selectable& s)
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


void MainLoop::onReady(System::Selectable& s)
{  
    System::MutexLock lock(_mutex);
    _avail.push_back(&s);

    CFRunLoopSourceSignal(_wakeSource);
}


void MainLoop::onRun()
{
    [NSApp run];
}


void MainLoop::onExit()
{
    _eventQueue.exit();
    wake();
}


void MainLoop::onWake()
{
    CFRunLoopSourceSignal(_wakeSource);
    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopWakeUp(rl);
}


void MainLoop::onCommitEvent(const Pt::Event& ev)
{
    _eventQueue.pushEvent( ev ); 
    wake();
}


void MainLoop::onQueueEvent(const Pt::Event& ev)
{
    _eventQueue.pushEvent( ev );
}


void MainLoop::onProcessEvents()
{ 
    NSEvent* event = nil;

    //
    // process cocoa events
    //
    do
    {
        event = [NSApp nextEventMatchingMask: NSAnyEventMask
                                   untilDate: nil
                                      inMode: NSDefaultRunLoopMode
                                     dequeue: YES];
    
        [NSApp sendEvent:event];
    }
    while(event != nil);

    //
    // process available selectables
    //
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

    //
    // process Pt events
    //
    bool isActive = _eventQueue.processEvents( this->event() );

    //
    // handle loop exit
    //
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


void MainLoop::onAttachTimer(System::Timer& timer)
{ 
    _timerQueue.addTimer(timer); 
    this->processTimers();
}


void MainLoop::onDetachTimer(System::Timer& timer)
{ 
    _timerQueue.removeTimer(timer);
    this->processTimers();
}


void MainLoop::cancel(System::IOHandle& h)
{
    size_t id = h.id;

    if(id == System::IOHandle::InvalidId)
        return;

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

    h.ready = 0;
}


MainLoop::IOEntry& MainLoop::enableIOHandle(System::IOHandle* h)
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


void MainLoop::beginRead(System::IOHandle* h)
{  
    CFFileDescriptorRef fdref = enableIOHandle(h).fd;
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorReadCallBack);

    h->events = System::IOHandle::Read;
}


void MainLoop::endRead(System::IOHandle* h)
{
    if(h->events & System::IOHandle::Read)
    {
        CFFileDescriptorRef fdref = _iotable[h->id].fd;
        CFFileDescriptorDisableCallBacks(fdref, kCFFileDescriptorReadCallBack);
    }

    h->ready = 0;
    h->events &= ~System::IOHandle::Read;
}


void MainLoop::beginWrite(System::IOHandle* h)
{  
    CFFileDescriptorRef fdref = enableIOHandle(h).fd;
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorWriteCallBack);

    h->events = System::IOHandle::Write;
}


void MainLoop::endWrite(System::IOHandle* h)
{
    if(h->events & System::IOHandle::Write)
    {
        CFFileDescriptorRef fdref = _iotable[h->id].fd;
        CFFileDescriptorDisableCallBacks(fdref, kCFFileDescriptorWriteCallBack);
    }

    h->ready = 0;
    h->events &= ~System::IOHandle::Write;
}


bool MainLoop::isReadable(System::IOHandle* h)
{  
    bool isReady = h->ready == System::IOHandle::Read;
    return isReady;
}


bool MainLoop::isWritable(System::IOHandle* h)
{
    bool isReady = h->ready == System::IOHandle::Write;
    return isReady;
}


bool MainLoop::isError(System::IOHandle* h)
{
    return false;
}


void MainLoop::processTimers()
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

} // namespace Gui

} // namespace Pt

