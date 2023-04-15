/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2013 Marc Boris D�rner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA  02110-1301  USA
 */

#import "WindowView.h"

#include <Pt/Hmi/Application.h>

#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSTrackingArea.h>

#include <CoreGraphics/CoreGraphics.h>


@implementation WindowView

- (WindowView*) initWithImpl: (Pt::Hmi::WindowImpl*) window
{
    self = [super init];
    _windowImpl = window;
    
    int opts = (NSTrackingActiveAlways | 
                NSTrackingInVisibleRect | 
                NSTrackingMouseEnteredAndExited | 
                NSTrackingMouseMoved);
    
    NSTrackingArea* area = [[NSTrackingArea alloc] initWithRect:[self bounds] 
                                                   options:opts 
                                                   owner:self 
                                                   userInfo:nil];
    [self addTrackingArea:area];
    
    return self;
}


- (BOOL) acceptsFirstResponder
{
    return TRUE;
}


- (BOOL) acceptsFirstMouse: (NSEvent*) ev
{
    return TRUE;
}


- (BOOL) resignFirstResponder 
{
    return TRUE;
}


- (void) keyDown: (NSEvent*) ev
{
    NSString* chars = [ev characters];
    unsigned keyCode = [ev keyCode];

    // TODO: convert from UTF-16
    unichar u16Char = [chars characterAtIndex: 0];

    // tab character
    if(keyCode == 48)
        u16Char = 9;

    // return character
    if(keyCode == 36)
        u16Char = 13;

    // backspace character
    if(keyCode == 51)
        u16Char = 8;

    Pt::Char ch(u16Char);
    
    _windowImpl->onKeyDown(keyCode, ch);
}


- (void) keyUp: (NSEvent*) ev
{
    NSString* chars = [ev characters];   
    unsigned keyCode = [ev keyCode];

    // TODO: convert from UTF-16
    unichar u16Char = [chars characterAtIndex: 0];

    // tab character
    if(keyCode == 48)
        u16Char = 9;

    // return character
    if(keyCode == 36)
        u16Char = 13;

    // backspace character
    if(keyCode == 51)
        u16Char = 8;

    Pt::Char ch(u16Char);
    
    _windowImpl->onKeyUp(keyCode, ch);
}


- (void) flagsChanged: (NSEvent*) ev
{
    unsigned int mod = [ev modifierFlags];
    _windowImpl->onKeyModifier(mod);
}


- (void) drawRect: (NSRect) rect
{
    _windowImpl->onPaint(rect);
}


- (void)setFrameOrigin: (NSPoint) origin
{
    //std::clog << "FRAME ORIGIN: " << origin.x << "," 
    //                              << origin.y << std::endl;

    [super setFrameOrigin:origin];
    //_windowImpl->onMove(origin);
}


- (void) setFrameSize: (NSSize) frameSize
{
    //std::clog << "FRAME SIZE : " << frameSize.width << "x"
    //                             << frameSize.height << std::endl;
    
    [super setFrameSize:frameSize];
    
    _windowImpl->onResize(frameSize);
}


- (void) mouseDown: (NSEvent*) ev
{
    //std::clog << "MOUSE DOWN " << std::endl;

    // NSPoint location = [ev locationInWindow];
    // NSWindow* window = [ev window];
    // NSPoint point = [_windowImpl->view() convertPoint:location fromView:window];
    // _windowImpl->onLMouseDown(point.x, point.y);

    NSPoint mp = [ev locationInWindow];
    _windowImpl->onLMouseDown(mp.x, mp.y);
}


- (void) mouseUp: (NSEvent*) ev
{
    //std::clog << "MOUSE UP" << std::endl;
    NSPoint mp = [ev locationInWindow];
    _windowImpl->onLMouseUp(mp.x, mp.y);
}


- (void) mouseDragged: (NSEvent*) ev
{
    //std::clog << "MOUSE DRAGGED" << std::endl;
    NSPoint mp = [ev locationInWindow];
    _windowImpl->onMouseMove(mp.x, mp.y);
}


- (void) mouseMoved: (NSEvent *) ev
{
    // static int nnn = 0;
    // std::clog << "MOUSE MOVED " << nnn++ << std::endl;
    NSPoint mp = [ev locationInWindow];
    _windowImpl->onMouseMove(mp.x, mp.y);
}


- (void) viewDidUnhide;
{
    //std::clog << "VIEW UNHIDE" << std::endl;
    _windowImpl->onShow(true);
}


- (void) viewDidHide;
{
    //std::clog << "VIEW HIDE" << std::endl;
    _windowImpl->onShow(false);
}


- (void) windowDidExpose: (NSNotification*) notification
{
    // only called for windows with nonretained backing store
    std::clog << "EXPOSE" << std::endl;
}


- (void) windowDidMove: (NSNotification*) notification
{
    _windowImpl->onMove();
}


- (NSSize) windowWillResize: (NSWindow*) sender 
                     toSize: (NSSize) frameSize


{
    //std::clog << "WINDOW RESIZE : " << frameSize.width << "x"
    //                                << frameSize.height << std::endl;
    
    return frameSize;
}


- (void) windowDidResize: (NSNotification*) notification
{
    //std::clog << "WINDOW RESIZE : " << frameSize.width << "x"
    //                                << frameSize.height << std::endl;
}


- (void) windowDidBecomeKey:(NSNotification*) notification
{
    //std::clog << "WINDOW BECAME KEY" << std::endl;
    _windowImpl->onActivate(true);
}


- (void) windowDidResignKey:(NSNotification*) notification
{
    //std::clog << "WINDOW RESIGNED KEY" << std::endl;
    _windowImpl->onActivate(false);
}


- (void) windowDidBecomeMain: (NSNotification*) notification
{
    //std::clog << "WINDOW BECAME MAIN" << std::endl;
}


- (void) windowDidResignMain: (NSNotification*) notification
{
    //std::clog << "WINDOW RESIGNED MAIN" << std::endl;
}


- (BOOL) windowShouldClose: (id) sender 
{
    //std::clog << "WINDOW SHOULD CLOSE" << std::endl;
    _windowImpl->onClosing();
    return FALSE;
}


- (void) windowDidChangeBackingProperties: (NSNotification*) notification
{
    CGFloat scale = [ _windowImpl->window() backingScaleFactor ];
    std::clog << "BACKING SCALE FACTOR: " << scale << std::endl;
}

@end
