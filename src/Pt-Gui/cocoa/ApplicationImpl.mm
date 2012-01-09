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

    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopAddSource(rl, _wakeSource, kCFRunLoopCommonModes);
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

    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopAddSource(rl, _wakeSource, kCFRunLoopCommonModes);
}


MainLoopImpl::~MainLoopImpl()
{
    CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopRemoveSource(rl, _wakeSource, kCFRunLoopCommonModes);
    CFRelease(_wakeSource);

    [ NSApp release];
}


void MainLoopImpl::onRun()
{
    [NSApp run];
}


void MainLoopImpl::onWake()
{
    CFRunLoopSourceSignal(_wakeSource);
}


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


void MainLoop::onEnable(System::Selectable& s)
{
    _impl.enable(s);
}


void MainLoop::onDisable(System::Selectable& s)
{
    _impl.disable(s);
}


void MainLoop::onSignalAvail(System::Selectable&)
{
}


void MainLoop::onSignalIdle(System::Selectable&)
{
}


void MainLoop::onIdle(System::Selectable& s)
{
}


void MainLoop::onReady(System::Selectable& s)
{
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

