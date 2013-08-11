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
#import "WidgetView.h"
#include "Pt/Hmi/Application.h"
#include "Pt/Hmi/PointingEvent.h"

#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>

@implementation WidgetView

- (WidgetView*) init
{
    self = [super init];
    return self;
}


- (void) drawRect:(NSRect)rect
{

}


- (void)setFrameOrigin:(NSPoint)origin
{

    std::cerr << "View::setFrame " << std::endl;
    [super setFrameOrigin:origin];


}


- (void)setFrameSize:(NSSize)frameSize
{
    std::cerr << "View::setFrameSize " << frameSize.width << ", " << frameSize.height << std::endl;
    [super setFrameSize:frameSize];

    //Pt::Gui::ResizeEvent rev(*_widget, frameSize.width, frameSize.height);
    //Pt::Gui::MainLoop::instance().event().send(rev);

    //[NSApp processEvent: &rev];
}


- (void) mouseDown:(NSEvent*)ev
{
    std::cerr << "Mouse Down: " << std::endl;
    NSPoint local_point = [self convertPoint:[ev locationInWindow] fromView:nil];

    //Pt::Gui::MouseEvent mev(*_widget, local_point.x, local_point.y,
    //                        Pt::Gui::MouseEvent::LeftButton,
    //                        Pt::Gui::MouseEvent::Press,
    //                        0); //modifiers

    //Pt::Gui::MainLoop::instance().event().send(mev);
}

- (void) mouseUp:(NSEvent*)ev
{
    std::cerr << "Mouse Up: " << std::endl;
    /*NSPoint local_point = [self convertPoint:[ev locationInWindow] fromView:nil];

    Pt::Gui::MouseEvent mev(*_widget, local_point.x, local_point.y,
                            Pt::Gui::MouseEvent::LeftButton,
                            Pt::Gui::MouseEvent::Release,
                            0); //modifiers

    Pt::Gui::MainLoop::instance().event().send(mev);*/
    //[NSApp processEvent: &mev];

    //[super mouseUp: ev];
}

- (void) mouseDragged:(NSEvent*)event
{
    std::cerr << "Mouse Dragged: " << std::endl;
    /*NSPoint local_point = [self convertPoint:[event locationInWindow] fromView:nil];

    Pt::Gui::MouseMoveEvent mev(*_widget, local_point.x, local_point.y,
                                Pt::Gui::MouseMoveEvent::Moved,
                                Pt::Gui::MouseMoveEvent::LeftButtonDown);

    Pt::Gui::MainLoop::instance().event().send(mev);*/
    //[NSApp processEvent: &mev];

    //[super mouseDragged: event];
}

- (void) mouseMoved:(NSEvent *)event
{
    std::cerr << "Mouse Moved: " << std::endl;
    /*NSPoint local_point = [self convertPoint:[event locationInWindow] fromView:nil];*/

	Pt::Hmi::PointingEvent mev;
	
    Pt::Hmi::Application::instance().pointerEvent().send(0, mev);
    //[NSApp processEvent: &mev];

    //[super mouseMoved: event];
}

- (BOOL) windowShouldClose:(id)window
{
	std::cerr << "Closed: " << std::endl;

    //Pt::Gui::CloseEvent cev(*_widget);

    //Pt::Gui::MainLoop::instance().event().send(cev);
    //[NSApp processEvent: &cev];

    return YES;
}

@end
