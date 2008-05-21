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

WidgetRegistry::WidgetRegistry() 
{
    //_widgets = new Widget*[RegistrySize];
    //reset();
}

WidgetRegistry::~WidgetRegistry()
{
    //delete[] _widgets;
}

void WidgetRegistry::registerWidget(Widget* widget)
{
    _widgets.push_back(widget);
    
//    for (int i = 0; i < RegistrySize; ++i)
//    {
//        if (!_widgets[i])
//        {
//            _widgets[i] = widget;
//            return;
//        }
//    }
//    
//    assert(false);
}

void WidgetRegistry::unregisterWidget(Widget* widget)
{
    // make new list and remove ourselves
    std::vector<Widget*> newList;
    for (unsigned int i = 0; i < _widgets.size(); ++i)
    {
        if (_widgets.at(i) != widget)
            newList.push_back(_widgets.at(i));
    }

    _widgets = newList;

//    for (int i = 0; i < RegistrySize; ++i)
//    {
//        if (_widgets[i] == widget)
//        {
//            _widgets[i] = 0;
//            return;
//        }
//    }
}

// Widget backend construction becomes delayed until 
// application instance is running and MVC hierachy is built
void WidgetRegistry::constructBackendControls()
{
    for (unsigned int i = 0; i < _widgets.size(); ++i)
    {
        Widget* widget = _widgets.at(i);
        widget->impl().construct();
    }
    
//    for (int i = 0; i < RegistrySize; ++i)
//    {
//        if (_widgets[i])
//            _widgets[i]->impl().construct();
//    }
}

void WidgetRegistry::destructBackendControls()
{
    for (unsigned int i = 0; i < _widgets.size(); ++i)
    {
        Widget* widget = _widgets.at(i);
        widget->impl().destruct();
    }
    
//    for (int i = 0; i < RegistrySize; ++i)
//    {
//        if (_widgets[i])
//            _widgets[i]->impl().destruct();
//    }
}

void WidgetRegistry::reset()
{
    _widgets.clear();
    //memset(_widgets, 0, RegistrySize*sizeof(Widget*));
}

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
    
    // Note:
    // It is important to make sure the WidgetRegistry singleton instance
    // is existing before entering EikStart::RunApplication
    // If it's created from a constructor the Symbian kernel will panic
    // with a memory leak error code.
    // The following call will not do anything except ensuring the
    // singleton instance has been created
    WidgetRegistry::instance().destructBackendControls();
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

} // namespace Gui

} // namespace Pt
