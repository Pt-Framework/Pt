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

#include <Pt/Forms/Application.h>

#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSTrackingArea.h>
#import <AppKit/NSAnimationContext.h>

#include <CoreGraphics/CoreGraphics.h>


@implementation WindowView

- (WindowView*) initWithImpl: (Pt::Forms::WindowImpl*) window
                frame: (NSRect) frame
{
    _windowImpl = window;

    self = [super initWithFrame: frame];
    [self setWantsLayer:NO];
    [self setAutoresizingMask: NSViewWidthSizable|NSViewHeightSizable];

    // TODO: move this to a separate class WindowController also implemnting 
    //       WindowDelegate
    [_windowImpl->window() setDelegate: self];

    [_windowImpl->window() addObserver:self
                            forKeyPath:@"visible"
                               options:NSKeyValueObservingOptionNew
                               context:nil];

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


- (void) dealloc 
{
    [_windowImpl->window() removeObserver:self 
                               forKeyPath:@"visible"];

    [super dealloc];
}


- (void) observeValueForKeyPath:(NSString*) keyPath
                       ofObject:(id) object
                         change:(NSDictionary*) change
                        context:(void *) context 
{
    if ([keyPath isEqualToString:@"visible"] && object == _windowImpl->window()) 
    {
        BOOL isVisible = [change[NSKeyValueChangeNewKey] boolValue];
        if( ! isVisible )
        {
            _windowImpl->onViewShow(false);
        } 
        else 
        {
            _windowImpl->onViewShow(true);
        }
    }
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


- (BOOL) isOpaque 
{ 
    return YES; 
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
    
    _windowImpl->onViewKeyDown(keyCode, ch);
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
    
    _windowImpl->onViewKeyUp(keyCode, ch);
}


- (void) flagsChanged: (NSEvent*) ev
{
    unsigned int mod = [ev modifierFlags];
    _windowImpl->onViewKeyModifier(mod);
}


- (void) drawRect: (NSRect) rect
{
    //std::clog << "drawRect BEGIN" << std::endl;
    //std::clog << "_invalidRect: " << _invalidRect.size.width << "x" 
    //                              << _invalidRect.size.height << std::endl;

    // NSRect r = NSUnionRect(_invalidRect, rect);
    // r = NSIntersectionRect(r, [self bounds]);
    // _windowImpl->onViewPaint(r);
    // _invalidRect = NSMakeRect(0, 0, 0, 0);

    _windowImpl->onViewPaint(rect);
}


- (void) setFrameOrigin: (NSPoint) origin
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
    
    //_windowImpl->onViewResize(frameSize);
}


- (void) mouseDown: (NSEvent*) ev
{
    //std::clog << "MOUSE DOWN " << std::endl;

    // NSPoint location = [ev locationInWindow];
    // NSWindow* window = [ev window];
    // NSPoint point = [_windowImpl->view() convertPoint:location fromView:window];
    // _windowImpl->onLMouseDown(point.x, point.y);

    NSPoint mp = [ev locationInWindow];
    _windowImpl->onViewLMouseDown(mp.x, mp.y);
}


- (void) mouseUp: (NSEvent*) ev
{
    //std::clog << "MOUSE UP" << std::endl;
    NSPoint mp = [ev locationInWindow];
    _windowImpl->onViewLMouseUp(mp.x, mp.y);
}


- (void) mouseDragged: (NSEvent*) ev
{
    //std::clog << "MOUSE DRAGGED" << std::endl;

    NSPoint mp = [ev locationInWindow];
    _windowImpl->onViewMouseMove(mp.x, mp.y);
}


- (void) mouseMoved: (NSEvent *) ev
{
    //std::clog << "MOUSE MOVED " << _windowImpl << std::endl;

    NSPoint mp = [ev locationInWindow];
    _windowImpl->onViewMouseMove(mp.x, mp.y);
}


- (void) rightMouseDown:(NSEvent *) ev 
{
    NSPoint mp = [ev locationInWindow];
    _windowImpl->onViewRMouseDown(mp.x, mp.y);
}


- (void) rightMouseUp:(NSEvent *) ev 
{
    NSPoint mp = [ev locationInWindow];
    _windowImpl->onViewRMouseUp(mp.x, mp.y);
}


- (void) viewDidUnhide;
{
    //std::clog << "viewDidUnhide" << std::endl;
}


- (void) viewDidHide;
{
    //std::clog << "viewDidHide" << std::endl;
}


- (void) windowDidExpose: (NSNotification*) notification
{
    // only called for windows with nonretained backing store
    std::clog << "TODO: never recieved windowDidExpose before..." << std::endl;
}


- (void) windowDidMove: (NSNotification*) notification
{
    NSWindow *window = [notification object];
    NSPoint origin = [window frame].origin;

    //std::clog << "windowDidMove BEGIN" << std::endl;
    _windowImpl->onViewMove(origin);
    //std::clog << "windowDidMove END" << std::endl;
}


- (void) windowDidResize: (NSNotification*) notification
{
    NSWindow *window = [notification object];

    NSRect frame = [window frame];
	  NSRect rect = [window contentRectForFrameRect: frame];

    //std::clog << "windowDidResize BEGIN" << std::endl;
    _windowImpl->onViewResize(rect.size);
    //std::clog << "windowDidResize END" << std::endl;
}


- (void) windowDidBecomeKey:(NSNotification*) notification
{
    //std::clog << "WINDOW BECAME KEY" << std::endl;
    _windowImpl->onViewActivate(true);
}


- (void) windowDidResignKey:(NSNotification*) notification
{
    //std::clog << "WINDOW RESIGNED KEY" << std::endl;
    _windowImpl->onViewActivate(false);
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
    _windowImpl->onViewClosing();
    return FALSE;
}


- (void) windowDidChangeBackingProperties: (NSNotification*) notification
{
    _windowImpl->onViewDidRescale();
}

@end
