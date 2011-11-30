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
#import "Application.h"
#include <iostream>

#import <AppKit/NSEvent.h>
#import <Foundation/NSRunLoop.h>

namespace Pt {

namespace Gui {

ApplicationImpl::ApplicationImpl(Application& a)
: _loop(a)
{
}


ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::commitEvent(const Pt::Event& event)
{
    _loop.commitEvent(event);
}


void ApplicationImpl::queueEvent(const Pt::Event& event)
{
}


void ApplicationImpl::processEvents()
{
}


int ApplicationImpl::run()
{  
    _loop.run();
    return 0;
}


void ApplicationImpl::wake()
{
    _loop.wake();
}


void ApplicationImpl::exit()
{
    _loop.exit();
}




MainLoop::MainLoop(Application& app)
: System::EventLoop(0)
, _impl(0)
{
    _impl = new MainLoopImpl(app);
    EventLoop::init(_impl);
}


MainLoop::MainLoop(Application& app, Allocator& a)
: System::EventLoop(0)
, _impl(0)
{
    _impl = new MainLoopImpl(app, a);
    EventLoop::init(_impl);
}


MainLoop::~MainLoop()
{
    delete _impl;
}


void MainLoop::onAttach(System::Selectable&)
{
}


void MainLoop::onDetach(System::Selectable&)
{
}


void MainLoop::onEnable( System::Selectable& s )
{
}


void MainLoop::onDisable( System::Selectable& s )
{
}


void MainLoop::onReinit(System::Selectable& s)
{
}


void MainLoop::onChanged(System::Selectable& s)
{
}


AppImpl::AppImpl()
{
}


AppImpl::~AppImpl()
{
}


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


MainLoopImpl::MainLoopImpl(Application& app)
{
    [PtGuiApplication sharedApplication];
    [NSApp initWithApplication: &app];

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


MainLoopImpl::MainLoopImpl(Application& app, Allocator& a)
: System::EventLoopImpl(a)
{
    [PtGuiApplication sharedApplication];
    [NSApp initWithApplication: &app];

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
    [NSApp release];
}


void MainLoopImpl::onRun()
{
    [NSApp run];
}


void MainLoopImpl::onWake()
{
    CFRunLoopSourceSignal(_wakeSource);
}


} // namespace Gui

} // namespace Pt
