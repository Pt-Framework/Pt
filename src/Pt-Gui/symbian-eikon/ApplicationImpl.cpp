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
#include "PixmapImpl.h"

#include "Pt/Gui/Application.h"
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/Pixmap.h>
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
#include "SymbEventLoop.h"

using namespace std;

// this is used as symbian application factory
static CApaApplication* NewApplication()
{
    return Pt::Gui::ApplicationImpl::_self->_symbApp;
}

namespace Pt {

namespace Gui {

ResourceRegistry::ResourceRegistry() 
{
}

ResourceRegistry::~ResourceRegistry()
{
}

void ResourceRegistry::registerWidget(WidgetImpl* widget)
{
    registerResource<WidgetImpl>(_widgets, widget);
}

void ResourceRegistry::unregisterWidget(WidgetImpl* widget)
{
    unregisterResource<WidgetImpl>(_widgets, widget);
}

void ResourceRegistry::constructWidgets()
{
    constructResources<WidgetImpl>(_widgets);
}

void ResourceRegistry::destructWidgets()
{
    destructResources<WidgetImpl>(_widgets);
}

void ResourceRegistry::registerPixmap(PixmapImpl* pixmap)
{
    registerResource<PixmapImpl>(_pixmaps, pixmap);
}

void ResourceRegistry::unregisterPixmap(PixmapImpl* pixmap)
{
    unregisterResource<PixmapImpl>(_pixmaps, pixmap);
}

void ResourceRegistry::constructPixmaps()
{
    constructResources<PixmapImpl>(_pixmaps);
}

void ResourceRegistry::destructPixmaps()
{
    destructResources<PixmapImpl>(_pixmaps);
}

ApplicationImpl::ApplicationImpl(Application& app) 
: _app(app)
, _eventLoop(0)
, _symbApp(new SymbApp(this))
{
    connect(eventQueueSignal, app.event);
    lockAppInstance();
    assert(ApplicationImpl::_self == 0);
    ApplicationImpl::_self = this;

    // TODO: Handle leave
    // This only created an event loop, it's not being run until
    // the view has been created
    // The view will also stop the event loop
    TRAPD(createError, _eventLoop = SymbEventLoop::NewL(*this));
    if (createError != KErrNone)
    {
        delete _symbApp;
        unlockAppInstance();
        throw std::runtime_error("Event loop creation failed" + PT_SOURCEINFO);
    }
}

ApplicationImpl::~ApplicationImpl()
{
    ApplicationImpl::_self = 0;
    unlockAppInstance();
    delete _eventLoop;
}

void ApplicationImpl::commitEvent(const Pt::Event& e)
{
    _eventLoop->CommitEvent(e);
}


void ApplicationImpl::queueEvent(const Pt::Event& e)
{
    _eventLoop->QueueEvent(e);
}


int ApplicationImpl::run()
{
    return EikStart::RunApplication(NewApplication);
}


int ApplicationImpl::exit()
{
    // exit could be called from any thread, but we're only allowed 
    // to exit the app from the creator thread
    // Create an exit event which will cause the application to quit from
    // the creator thread
    ExitEvent event;
    _eventLoop->CommitEvent(event);
    return 0;
}

void ApplicationImpl::processEvents()
{
    _eventLoop->ProcessEvents();
}

void ApplicationImpl::dispatchEvent(const Pt::Event& event)
{
    // If we're having an exit event we simply quit the application
    if (event.typeInfo() == typeid(ExitEvent)) 
    {
        // since the application is going to exit the event we're getting
        // will not be deleted, we NEED to do it here
        delete &event;
        _symbApp->Document().AppUi().Exit(); 
        return;
    }

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
