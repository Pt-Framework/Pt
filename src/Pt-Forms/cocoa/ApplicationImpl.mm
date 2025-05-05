 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#import "ApplicationImpl.h"
#import "PixmapImpl.h"
#import "WindowImpl.h"

#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#import <Foundation/NSGeometry.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSRunLoop.h>

#include <poll.h>

@interface PtGuiApplication : NSApplication
{
    NSAutoreleasePool* pool;
}

- (void) initPool;

- (void) dealloc;

- (void) sendEvent: (NSEvent*) event;

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

- (void) sendEvent: (NSEvent*) event
{
    // static int nnn = 0;

    // switch( [event type] ) 
    // {
    //     case NSEventTypeLeftMouseDown:
    //         std::clog << "### MOUSE DOWN " << nnn++ << std::endl;
    //         break;

    //     case NSEventTypeMouseMoved:
    //         std::clog << "# MOUSE MOVE " << nnn++ << std::endl;
    //         break;

    //     default:
    //         break;
    // }

    [super sendEvent: event];
}

@end


namespace Pt {

namespace Forms {

void MainLoopImplOnWake(void* p)
{
    ApplicationImpl* impl = reinterpret_cast<ApplicationImpl*>(p);
    impl->processEvents();
}

void MainLoopImplOnTimer(CFRunLoopTimerRef timer, void *p)
{
    ApplicationImpl* impl = reinterpret_cast<ApplicationImpl*>(p);
    impl->processTimers();
}

void MainLoopImplOnFd(CFFileDescriptorRef f, CFOptionFlags flags, void *p)
{
    System::IOHandle* h = reinterpret_cast<System::IOHandle*>(p);

    h->ready = 0;

    if(flags & kCFFileDescriptorReadCallBack)
    {
        h->events &= ~System::IONotifier::Read;
        h->ready |= System::IONotifier::Read;
    }
    
    if(flags & kCFFileDescriptorWriteCallBack)
    {
        h->events &= ~System::IONotifier::Write;
        h->ready |= System::IONotifier::Write;
    }

    if(h->events & System::IONotifier::Except)
    {
        pollfd pfd;
        pfd.fd = h->fd;
        pfd.events = POLLPRI;
        pfd.revents = 0;

        int avail = ::poll(&pfd, 1, 0);
        if( avail > 0 && pfd.revents & POLLPRI )
        {
            h->events &= ~System::IONotifier::Except;
            h->ready |= System::IONotifier::Except;
        }
    }

    // NOTE: call back is one-shot, so we do not have to disable it here

    System::Selectable* s = h->sel;
    s->run();
}


ApplicationImpl::ApplicationImpl()
: System::EventLoop()
{
    init();

    // local monitors will only capture events on the window frame

    // [NSEvent addGlobalMonitorForEventsMatchingMask: NSEventMaskAny
    //          handler:^ void (NSEvent* event) 
    //          {
    //             NSEventType eventType = [event type];
    //             if (eventType == NSEventTypeLeftMouseDown ||
    //                 eventType == NSEventTypeRightMouseDown)
    //                 std::clog << "EVENT MOUSE DOWN" << std::endl;
                
    //             return;
    //         }];
}


ApplicationImpl::~ApplicationImpl()
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


void ApplicationImpl::setDefaultFont(const std::string& fontName)
{
    PixmapImpl::setDefaultFont(fontName);
}


void ApplicationImpl::init()
{
    [PtGuiApplication sharedApplication];
    
    // activate the application when launched from shell. 
    // dispatch_async(dispatch_get_main_queue(), ^{
    //     [NSApp activateIgnoringOtherApps:YES];
    // });

    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
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

    _masterTimer = CFRunLoopTimerCreate( kCFAllocatorDefault, CFAbsoluteTimeGetCurrent(), 10000.0, 0, 0,  &MainLoopImplOnTimer, &timerCtx);

    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopAddSource(rl, _wakeSource, kCFRunLoopCommonModes);
    CFRunLoopAddTimer(rl, _masterTimer, kCFRunLoopCommonModes);
}


void ApplicationImpl::nextEvent()
{
    NSEvent* event = nil;

    event = [NSApp nextEventMatchingMask: NSEventMaskAny
                                untilDate: [NSDate distantFuture]
                                    inMode: NSDefaultRunLoopMode
                                    dequeue: YES];
    
    [NSApp sendEvent:event];
}


void ApplicationImpl::processTimers()
{ 
    std::size_t nextTimer = _timerQueue.processTimers();

    if(nextTimer == System::EventLoop::WaitInfinite)
    {
        nextTimer = std::numeric_limits<CFTimeInterval>::max();
    }

    CFTimeInterval interval = nextTimer / 1000.0;
    CFAbsoluteTime fireDate = CFAbsoluteTimeGetCurrent() + interval;
    CFRunLoopTimerSetNextFireDate(_masterTimer, fireDate);
}


void ApplicationImpl::onAttachSelectable(System::Selectable& s)
{
    _selectables.insert(s);
}


void ApplicationImpl::onDetachSelectable(System::Selectable& s)
{
    System::SelectableList::unlink(s);
}


void ApplicationImpl::onCancel(System::Selectable& s)
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


void ApplicationImpl::onReady(System::Selectable& s)
{  
    System::MutexLock lock(_mutex);
    _avail.push_back(&s);

    CFRunLoopSourceSignal(_wakeSource);
}


void ApplicationImpl::onRun()
{
    // NOTE: instead of a master timer we could also iterate using
    //         NSApp runUntil().

    [NSApp run];
}


void ApplicationImpl::onExit()
{
    _eventQueue.exit();
    wake();
}


void ApplicationImpl::onCommitEvent(const Pt::Event& ev)
{
    _eventQueue.pushEvent( ev ); 
    wake();
}

void ApplicationImpl::onQueueEvent(const Pt::Event& ev)
{
    _eventQueue.pushEvent( ev );
}


void ApplicationImpl::onWake()
{
    CFRunLoopSourceSignal(_wakeSource);
    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopWakeUp(rl);
}
    

void ApplicationImpl::onProcessEvents()
{ 
    NSEvent* event = nil;
    
    //
    // process cocoa events
    //
    do {
        
    event = [NSApp nextEventMatchingMask: NSEventMaskAny
                               untilDate: nil
                                  inMode: NSDefaultRunLoopMode
                                 dequeue: YES];
     } while (event != 0);
             
    [NSApp sendEvent:event];
    
    
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
    bool isActive = _eventQueue.processEvents( this->eventReceived() );
    
    //
    // handle loop exit
    //
    if( ! isActive)
    {
        // set stop flag
        [NSApp stop: nil];
        
        // post fake event so NSApp notices the stop flag
        event = [NSEvent otherEventWithType: NSEventTypeApplicationDefined
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


void ApplicationImpl::onAttachTimer(System::Timer& timer)
{ 
    _timerQueue.addTimer(timer); 
    this->processTimers();
}


void ApplicationImpl::onDetachTimer(System::Timer& timer)
{ 
    _timerQueue.removeTimer(timer);
    this->processTimers();
}


void ApplicationImpl::cancel(System::IOHandle& h)
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
    h.events = 0;
}


ApplicationImpl::IOEntry& ApplicationImpl::enableIOHandle(System::IOHandle* h)
{
    if(h->id == System::IOHandle::InvalidId)
    {
        CFFileDescriptorContext ctx;
        ctx.version = 0;
        ctx.info = h;
        ctx.retain = NULL;
        ctx.release = NULL;
        ctx.copyDescription = NULL;

        CFFileDescriptorRef fdref = CFFileDescriptorCreate(kCFAllocatorDefault, 
                                                           h->fd, false, 
                                                           &MainLoopImplOnFd, &ctx);

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


void ApplicationImpl::beginWait(System::IOHandle* h, int flags)
{
    CFFileDescriptorRef fdref = enableIOHandle(h).fd;

    if( flags & System::IONotifier::Read ||
        flags & System::IONotifier::Except )
    {
        CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorReadCallBack);
    }

    if(flags & System::IONotifier::Write)
    {
        CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorWriteCallBack);
    }

    h->events = 0;

    if( flags & System::IONotifier::Read )
    {
        h->events |= System::IONotifier::Read;
    }

    if(flags & System::IONotifier::Write)
    {
        h->events |= System::IONotifier::Write;
    }

    if( flags & System::IONotifier::Except )
    {
        h->events |= System::IONotifier::Except;
    }
}


int ApplicationImpl::endWait(System::IOHandle* h)
{
    CFFileDescriptorRef fdref = _iotable[h->id].fd;

    // disable in case callbacks weren't called
    
    if(h->events & System::IONotifier::Read ||
       h->events & System::IONotifier::Except)
    {
        CFFileDescriptorDisableCallBacks(fdref, kCFFileDescriptorReadCallBack);
    }

    if(h->events & System::IONotifier::Write)
    {
        CFFileDescriptorDisableCallBacks(fdref, kCFFileDescriptorWriteCallBack);
    }

    int flags = h->ready;

    h->ready = 0;
    h->events = 0;

    return flags;
}


void ApplicationImpl::beginRead(System::IOHandle* h)
{  
    CFFileDescriptorRef fdref = enableIOHandle(h).fd;
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorReadCallBack);

    h->events = System::IONotifier::Read;
}


void ApplicationImpl::endRead(System::IOHandle* h)
{
    // disable in case callback wasn't called
    if(h->events & System::IONotifier::Read)
    {
        CFFileDescriptorRef fdref = _iotable[h->id].fd;
        CFFileDescriptorDisableCallBacks(fdref, kCFFileDescriptorReadCallBack);
    }

    h->ready = 0;
    h->events &= ~System::IONotifier::Read;
}


void ApplicationImpl::beginWrite(System::IOHandle* h)
{  
    CFFileDescriptorRef fdref = enableIOHandle(h).fd;
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorWriteCallBack);

    h->events = System::IONotifier::Write;
}


void ApplicationImpl::endWrite(System::IOHandle* h)
{
    // disable in case callback wasn't called
    if(h->events & System::IONotifier::Write)
    {
        CFFileDescriptorRef fdref = _iotable[h->id].fd;
        CFFileDescriptorDisableCallBacks(fdref, kCFFileDescriptorWriteCallBack);
    }

    h->ready = 0;
    h->events &= ~System::IONotifier::Write;
}


bool ApplicationImpl::isReadable(System::IOHandle* h)
{  
    bool isReady = h->ready & System::IONotifier::Read;
    return isReady;
}


bool ApplicationImpl::isWritable(System::IOHandle* h)
{
    bool isReady = h->ready & System::IONotifier::Write;
    return isReady;
}


bool ApplicationImpl::isReady(System::IOHandle* h)
{
    return h->ready & System::IONotifier::Read ||
           h->ready & System::IONotifier::Write ||
           h->ready & System::IONotifier::Except;
}

bool ApplicationImpl::isError(System::IOHandle* h)
{
    return false;
}

} // namespace

} // namespace
