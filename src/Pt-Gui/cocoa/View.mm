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
#import "View.h"
#import "Application.h"
#include "Pt/Gui/CloseEvent.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/MouseMoveEvent.h"
#import <Foundation/NSString.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#include <iostream>

@implementation PtGuiView
- (PtGuiView*) initWithWidget: (Pt::Gui::Widget*) widget
{
    self = [super init];
    if(self)
    {
        _widget = widget;
    }
    
    return self;
}
- (void) mouseDown:(NSEvent*)ev{
    NSPoint local_point = [self convertPoint:[ev locationInWindow] fromView:nil];

    Pt::Gui::MouseEvent mev(*_widget, local_point.x, local_point.y, 
                            Pt::Gui::MouseEvent::LeftButton, 
                            Pt::Gui::MouseEvent::Press, 
                            0); //modifiers
    
    [NSApp processEvent: &mev];
    [super mouseDown: ev];
}

- (void) mouseDragged:(NSEvent*)event
{
    NSPoint local_point = [self convertPoint:[event locationInWindow] fromView:nil];

    Pt::Gui::MouseMoveEvent mev(*_widget, local_point.x, local_point.y, 
                                Pt::Gui::MouseMoveEvent::Moved,  
                                Pt::Gui::MouseMoveEvent::LeftButtonDown);
    
    [NSApp processEvent: &mev];
    [super mouseMoved: event];
}

- (void) mouseMoved:(NSEvent *)event
{
    NSPoint local_point = [self convertPoint:[event locationInWindow] fromView:nil];

    Pt::Gui::MouseMoveEvent mev(*_widget, local_point.x, local_point.y, 
                                Pt::Gui::MouseMoveEvent::Moved,  
                                0);
    
    [NSApp processEvent: &mev];
    [super mouseMoved: event];
}

- (BOOL)windowShouldClose:(id)window
{
    Pt::Gui::CloseEvent cev(*_widget);
    [NSApp processEvent: &cev];
    return YES;
}
@end
