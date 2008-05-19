/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2008 Peter Barth                                        *
 *   Copyright (C) 2006-2008 PTV AG                                        *
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

#include "ApplicationImpl.h"
#include "WidgetImpl.h"

#include "Pt/Gui/Application.h"
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/CloseEvent.h>
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Gui/MoveEvent.h>
#include <Pt/Gui/MouseMoveEvent.h>

#include <iostream>
#include <assert.h>

// Symbian APIs
#include <eikstart.h>
// Our own classes
#include "SymbAppUi.h"
#include "SymbDoc.h"
#include "SymbApp.h"

using namespace std;

// this is used as symbian application factory
static CApaApplication* NewApplication()
{
    return Pt::Gui::ApplicationImpl::_self->_symbApp;
}

namespace Pt {

namespace Gui {

ApplicationImpl::ApplicationImpl(Application& app) 
: _app(app), /*_eventLoopThread(_eventLoop), */_symbApp(new SymbApp(this))
{
    connect(eventQueueSignal, app.event);
    lockAppInstance();
    assert(ApplicationImpl::_self == 0);
    ApplicationImpl::_self = this;
}

ApplicationImpl::~ApplicationImpl()
{
    //_eventLoop.exit();
    //_eventLoopThread.wait();
    ApplicationImpl::_self = 0;
    unlockAppInstance();
}

void ApplicationImpl::commitEvent(const Pt::Event& e)
{
    //_eventLoop.commitEvent(e);
}


void ApplicationImpl::queueEvent(const Pt::Event& e)
{
    //_eventLoop.queueEvent(e);
}


int ApplicationImpl::run()
{
    //_eventLoopThread.start();    
    return EikStart::RunApplication(NewApplication);
}


int ApplicationImpl::exit()
{
    _symbApp->GetDocument().GetAppUi().Exit();
    //_eventLoop.exit();
    //_eventLoopThread.wait();
    // TODO: Find return code
    return 0;
}

void ApplicationImpl::processEvents()
{
    //_eventLoop.processEvents();
}

void ApplicationImpl::dispatchEvent(Pt::Event& event)
{
    eventQueueSignal.send(event);
}

// assuming that there is only one Application instance at a time
ApplicationImpl* ApplicationImpl::_self = 0;
System::Mutex ApplicationImpl::_mutex(System::Mutex::Normal);

void ApplicationImpl::lockAppInstance()
{
    _mutex.lock();
}

void ApplicationImpl::unlockAppInstance()
{
    _mutex.unlock();    
}

void ApplicationImpl::constructBackendWidgets()
{
    for (unsigned int i = 0; i < _widgets.size(); ++i)
    {
        Widget* widget = _widgets.at(i);
        widget->impl().construct();
    }
    _widgets.clear();
}

} // namespace Gui

} // namespace Pt
