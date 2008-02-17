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
#include <iostream>

namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at, const Math::Size& size)
: _apiWidget(apiWidget)
{
    _window = nil;
    _view = [[WidgetView alloc] initWithWidget: &apiWidget];

    if(parent == 0)
    {
        _window = [[NSWindow alloc] initWithContentRect:NSMakeRect(at.x(), at.y(), size.width(), size.height())      
                                                        styleMask:NSTitledWindowMask | 
                                                                  NSClosableWindowMask | 
                                                                  NSMiniaturizableWindowMask |
                                                                  NSResizableWindowMask
                                                         backing:NSBackingStoreBuffered
                                                         defer:NO];
    
        [_window setAcceptsMouseMovedEvents:YES];   
        [_window setContentView: _view];
        [_window setDelegate: _view];
    }
    else
    {
        [parent->impl()._view addSubview:_view];
        [_view setFrame:NSMakeRect(at.x(), at.y(), size.width(), size.height())];
    }
    
    _painter.setView(_view);
}


WidgetImpl::~WidgetImpl()
{
    [_window release];
    [_view release];
}


Pt::String WidgetImpl::title() const
{ 
/*    
    NSString* str = [window title];
    [str getBytes:(void *)buffer 
                  maxLength:(NSUInteger)
                  usedLength:(NSUInteger *)
                  encoding:(NSStringEncoding)
                  options:(NSStringEncodingConversionOptions)options 
                  range:(NSRange)range 
                  remainingRange:(NSRangePointer)leftover
*/
                  
    return L""; 
}


void WidgetImpl::setTitle(const Pt::String& text)
{
    //TODO: use Byteorder.h to determine endianess of encoding
    NSString* str = [[NSString alloc] initWithBytes: text.c_str() 
                                                     length: text.size() * sizeof(Pt::Char)
                                                     encoding: NSUTF32LittleEndianStringEncoding];
    [_window setTitle:str];
    [str release];
}


Painter WidgetImpl::painter()
{
	return Painter( &_painter );
}


void WidgetImpl::show()
{
    [_window makeKeyAndOrderFront:nil];
    [_view setHidden:NO];
}


void WidgetImpl::hide()
{
    [_window orderOut:nil];
    [_view setHidden:YES];
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
