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
#include "Pt/Gui/ResizeEvent.h"
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


- (void) drawRect:(NSRect)rect{    std::cerr << "View::drawRect " << _widget << " " 
              << rect.size.width << ", " << rect.size.height << std::endl;

    Pt::Gui::PaintEvent pev(*_widget, Pt::Math::Point(rect.origin.x, rect.origin.y), 
                                      Pt::Math::Size(rect.size.width, rect.size.height));

    //[NSApp processEvent: &pev];
    //[super drawRect:rect];}


- (void)setFrameOrigin:(NSPoint)origin
{
    //std::cerr << "View::setFrame " << frameRect.size.width << ", " << frameRect.size.height << std::endl;
    [super setFrameOrigin:origin];
    
    //Pt::Gui::ResizeEvent rev(*_widget, frameRect.size.width, frameRect.size.height);
    //[NSApp processEvent: &rev];
}


- (void)setFrameSize:(NSSize)frameSize
{
    std::cerr << "View::setFrameSize " << frameSize.width << ", " << frameSize.height << std::endl;
    [super setFrameSize:frameSize];
    
    Pt::Gui::ResizeEvent rev(*_widget, frameSize.width, frameSize.height);
    [NSApp processEvent: &rev];
}

- (void) mouseDown:(NSEvent*)ev{
    std::cerr << "Mouse Down: " << std::endl;
    NSPoint local_point = [self convertPoint:[ev locationInWindow] fromView:nil];

    Pt::Gui::MouseEvent mev(*_widget, local_point.x, local_point.y, 
                            Pt::Gui::MouseEvent::LeftButton, 
                            Pt::Gui::MouseEvent::Press, 
                            0); //modifiers
    
    [NSApp processEvent: &mev];
    //[super mouseDown: ev];
}

- (void) mouseUp:(NSEvent*)ev{
    std::cerr << "Mouse Up: " << std::endl;
    NSPoint local_point = [self convertPoint:[ev locationInWindow] fromView:nil];

    Pt::Gui::MouseEvent mev(*_widget, local_point.x, local_point.y, 
                            Pt::Gui::MouseEvent::LeftButton, 
                            Pt::Gui::MouseEvent::Release, 
                            0); //modifiers
    
    [NSApp processEvent: &mev];
    //[super mouseUp: ev];
}

- (void) mouseDragged:(NSEvent*)event
{
    std::cerr << "Mouse Dragged: " << std::endl;
    NSPoint local_point = [self convertPoint:[event locationInWindow] fromView:nil];

    Pt::Gui::MouseMoveEvent mev(*_widget, local_point.x, local_point.y, 
                                Pt::Gui::MouseMoveEvent::Moved,  
                                Pt::Gui::MouseMoveEvent::LeftButtonDown);
    
    [NSApp processEvent: &mev];
    //[super mouseDragged: event];
}

- (void) mouseMoved:(NSEvent *)event
{
    std::cerr << "Mouse Moved: " << std::endl;
    NSPoint local_point = [self convertPoint:[event locationInWindow] fromView:nil];

    Pt::Gui::MouseMoveEvent mev(*_widget, local_point.x, local_point.y, 
                                Pt::Gui::MouseMoveEvent::Moved,  
                                0);
    
    [NSApp processEvent: &mev];
    //[super mouseMoved: event];
}

- (BOOL)windowShouldClose:(id)window
{
    Pt::Gui::CloseEvent cev(*_widget);
    [NSApp processEvent: &cev];
    return YES;
}
@end
