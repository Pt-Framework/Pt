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
#import "ApplicationImpl.h"
#import "Application.h"

namespace Pt {

namespace Gui {

ApplicationImpl::ApplicationImpl(Application& a)
: app(&a)
{
    [PtGuiApplication sharedApplication];
    [NSApp initWithApplication: app];

    // NSRunLoop, CFRunLoop, CFFileDescriptor
}


ApplicationImpl::~ApplicationImpl()
{
    [NSApp release];
}


void ApplicationImpl::commitEvent(const Pt::Event& event)
{
}


void ApplicationImpl::queueEvent(const Pt::Event& event)
{
}


void ApplicationImpl::processEvents()
{
}


int ApplicationImpl::run()
{  
	[NSApp run];
	return 0;
}


void ApplicationImpl::wake()
{

}


void ApplicationImpl::exit()
{
    [NSApp terminate: nil];
}

} // namespace Gui

} // namespace Pt
