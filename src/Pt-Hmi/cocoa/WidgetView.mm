/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2013 Marc Boris Dürner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#import "WidgetView.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PointingEvent.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSTrackingArea.h>
#include <Pt/Hmi/PaintSurface.h>
#include "PaintSurfaceImpl.h"
#include <CoreGraphics/CoreGraphics.h>

@implementation WidgetView

- (WidgetView*) init : (Pt::Hmi::WindowImpl*) device
{
    self = [super init];
    _outDevice = device;
    
    int opts = (NSTrackingActiveAlways | NSTrackingInVisibleRect | NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved);
    
    NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:[self bounds] options:opts owner:self userInfo:nil];
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
    _outDevice->onSpezialKeyEvent(mod);
}

- (void)keyDown:(NSEvent *)ev
{
    NSString* chars = [ev characters];
    unichar character = [chars characterAtIndex: 0];
    
    //Emulate tab character on shift pressed.
    if(character == 25)
        character = 9;
    
    _outDevice->onKeyDown(character);
}

- (void)keyUp:(NSEvent *)ev
{
    NSString* chars = [ev characters];
    unichar character = [chars characterAtIndex: 0];
    
    //Emulate tab charecter on shift pressed.
    if(character == 25)
        character = 9;
    
    _outDevice->onKeyUp(character);
}

- (void) drawRect:(NSRect)rect
{
    
    Pt::Hmi::PaintSurface* surface = _outDevice->paintSurface();
    
    Pt::Hmi::PaintSurfaceImpl* impl = surface->impl();
    
    CGContextRef context = impl->context();
    
    CGImageRef image =  CGBitmapContextCreateImage(context);
    
    CGContextRef currentContext = ( CGContextRef ) [[NSGraphicsContext currentContext] graphicsPort];

    CGContextDrawImage(currentContext,rect,image);
    
    CGImageRelease(image);

}

- (void)setFrameOrigin:(NSPoint)origin
{
    [super setFrameOrigin:origin];
    
    _outDevice->onPosition();
}

- (void)setFrameSize:(NSSize)frameSize
{
    [super setFrameSize:frameSize];
    
    _outDevice->onSize();
}

- (void) mouseDown:(NSEvent*)ev
{
    NSPoint mp = [ev locationInWindow];
    _outDevice->onLMouseDown(mp.x,mp.y);
}

- (void) mouseUp:(NSEvent*)ev
{
    NSPoint mp = [ev locationInWindow];
    _outDevice->onLMouseUp(mp.x,mp.y);
}

- (void) mouseDragged:(NSEvent*)ev
{
    
    NSPoint mp = [ev locationInWindow];
    _outDevice->onMouseMove(mp.x,mp.y);
}

- (void) mouseMoved:(NSEvent *)ev
{
    NSPoint mp = [ev locationInWindow];
    _outDevice->onMouseMove(mp.x,mp.y);
}

- (BOOL) windowShouldClose:(id)window
{
    _outDevice->onClosing();
	return false;
}

@end
