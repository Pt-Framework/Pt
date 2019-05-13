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
 MA  02110-1301  USA
 */

#import "MainWindowView.h"

#include <Pt/Hmi/Application.h>

#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSTrackingArea.h>

#include <CoreGraphics/CoreGraphics.h>

@implementation MainWindowView

- (MainWindowView*) init : (Pt::Hmi::MainWindowImpl*) window
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


- (void) flagsChanged:(NSEvent*)ev
{
    unsigned int mod = [ev modifierFlags];
    _windowImpl->onSpezialKeyEvent(mod);
}


- (void) keyDown:(NSEvent *)ev
{
    NSString* chars = [ev characters];
    unichar character = [chars characterAtIndex: 0];
    
    //Emulate tab character on shift pressed.
    if(character == 25)
        character = 9;
    
    _windowImpl->onKeyDown(character);
}


- (void) keyUp:(NSEvent *)ev
{
    NSString* chars = [ev characters];
    unichar character = [chars characterAtIndex: 0];
    
    //Emulate tab charecter on shift pressed.
    if(character == 25)
        character = 9;
    
    _windowImpl->onKeyUp(character);
}


- (void) drawRect:(NSRect)rect
{
    _windowImpl->onPaint(rect);
}


- (void)setFrameOrigin:(NSPoint)origin
{
    [super setFrameOrigin:origin];
    
    _windowImpl->onPosition();
}


- (void)setFrameSize:(NSSize)frameSize
{
    [super setFrameSize:frameSize];
    
    _windowImpl->onSize();
}


- (void) mouseDown:(NSEvent*)ev
{
    NSPoint mp = [ev locationInWindow];
    _windowImpl->onLMouseDown(mp.x,mp.y);
}


- (void) mouseUp:(NSEvent*)ev
{
    NSPoint mp = [ev locationInWindow];
    _windowImpl->onLMouseUp(mp.x,mp.y);
}


- (void) mouseDragged:(NSEvent*)ev
{
    
    NSPoint mp = [ev locationInWindow];
    _windowImpl->onMouseMove(mp.x,mp.y);
}


- (void) mouseMoved:(NSEvent *)ev
{
    NSPoint mp = [ev locationInWindow];
    _windowImpl->onMouseMove(mp.x,mp.y);
}


- (BOOL) windowShouldClose:(id)window
{
    _windowImpl->onClosing();
	return false;
}

@end
