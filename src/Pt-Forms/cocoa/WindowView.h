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
 * */

#include <Pt/Forms/Api.h>
#include "WindowImpl.h"

#import <AppKit/NSView.h>
#import <AppKit/NSWindowController.h>

@interface WindowView : NSView<NSWindowDelegate>
{
    Pt::Forms::WindowImpl* _windowImpl;
}
    
- (WindowView*) initWithImpl: (Pt::Forms::WindowImpl*) window 
                frame: (NSRect) frame;

- (void) dealloc;

- (BOOL) acceptsFirstResponder;

- (BOOL) acceptsFirstMouse: (NSEvent *) ev;

- (BOOL) resignFirstResponder;

- (BOOL) isOpaque;

- (void) drawRect: (NSRect) rect;

- (void) setFrameOrigin: (NSPoint) origin;

- (void) setFrameSize: (NSSize) frameSize;

- (void) mouseDown: (NSEvent*) ev;

- (void) mouseUp: (NSEvent*) ev;

- (void) mouseDragged: (NSEvent*) ev;

- (void) mouseMoved: (NSEvent*) ev;

- (void) rightMouseDown:(NSEvent *) ev;

- (void) rightMouseUp:(NSEvent *) ev;

- (void) keyDown: (NSEvent*) ev;

- (void) keyUp: (NSEvent*) ev;

- (void) flagsChanged: (NSEvent*) ev;

- (void) viewDidUnhide;

- (void) viewDidHide;

- (void) windowDidExpose: (NSNotification*) notification;

- (void) windowDidMove: (NSNotification*) notification;

- (void) windowDidResize: (NSNotification*) notification;

- (void) windowDidBecomeKey:(NSNotification*) notification;

- (void) windowDidResignKey:(NSNotification*) notification;

- (void) windowDidBecomeMain: (NSNotification*) notification;

- (void) windowDidResignMain: (NSNotification*) notification;

- (BOOL) windowShouldClose: (id) sender;

- (void) windowDidChangeBackingProperties: (NSNotification*) notification;

@end
