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
#include "WidgetImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Gui/Widget.h>
//#include <Pt/Gui/PaintEvent.h>

#import <Foundation/NSString.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>

@implementation PtGuiView- (void)mouseDown:(NSEvent*)ev{
    NSPoint local_point = [self convertPoint:[ev locationInWindow] fromView:nil];
    
    Pt::Gui::Widget* widget = 0;
    Pt::Gui::MouseEvent mev(*widget, local_point.x, local_point.y, 
                            Pt::Gui::MouseEvent::LeftButton, 
                            Pt::Gui::MouseEvent::Press, 
                            0); //modifiers
    
    [NSApp processEvent: &mev];
    
    [super mouseDown: ev];
}
@end

namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(/*Widget& apiWidget, Widget* parent,*/ const Math::Point& at, const Math::Size& size)
//: _apiWidget(apiWidget)
{

    window = [[NSWindow alloc] initWithContentRect:NSMakeRect(at.x(), at.y(), size.width(), size.height())      
                                                   styleMask:NSTitledWindowMask | 
                                                             NSClosableWindowMask | 
                                                             NSMiniaturizableWindowMask |
                                                             NSResizableWindowMask
                                                   backing:NSBackingStoreBuffered
                                                   defer:YES];
    [window autorelease];
    [window setTitle:@"Unknown Window"];
    [window setDelegate: window];
    [window setAcceptsMouseMovedEvents:YES];

    
    view = [[PtGuiView alloc] init];
    [window setContentView: view];
}


WidgetImpl::~WidgetImpl()
{

}


Pt::String WidgetImpl::title() const
{ 
    return L""; 
}


void WidgetImpl::setTitle(const Pt::String& text)
{
    NSString* str = [[NSString alloc] initWithBytes: text.data() 
                                                     length: text.size()
                                                     encoding: NSUTF32StringEncoding];
    [str autorelease];
    [window setTitle:str];
}


//Painter WidgetImpl::painter()
//{
//	return Painter(&_painter);
//}


void WidgetImpl::show()
{
    [window makeKeyAndOrderFront:nil];
    [view setHidden:NO];
}


void WidgetImpl::hide()
{
    [window orderOut:nil];
    [view setHidden:YES];
}


void WidgetImpl::setParent(Widget* parent)
{

}


void WidgetImpl::move(size_t x, size_t y)
{

}


void WidgetImpl::resize(size_t width, size_t height)
{

}

} // namespace Gui

} // namespace Pt
