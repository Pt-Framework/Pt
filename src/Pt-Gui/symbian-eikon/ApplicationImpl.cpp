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
#include <eikproc.h>
#include <eikenv.h>
// Our own classes
#include "SymbAppUi.h"
#include "SymbEventLoop.h"

using namespace std;

namespace Pt {

namespace Gui {

Environment::Environment() 
: _coe(0)
, _ui(0)
{
}

Environment::~Environment()
{
    destroyFramework();
}

void Environment::registerResource(Resource* resource)
{
    _resources.push_back(resource);
    if (_resources.size() == 1 && !_coe)
        initFramework();
}

void Environment::unregisterResource(Resource* resource)
{
    size_t size = _resources.size();
    std::list<Resource*>::iterator where;
    where = std::remove(_resources.begin(), _resources.end(), resource);
    _resources.erase(where, _resources.end());
    assert(_resources.size() == size-1);

    // TODO: Destroy framework when resource count drops to zero?
    // For now we'll better leave it
    //if (_resources.size() == 0)
    //    destroyFramework();
}

void Environment::initFramework()
{
    assert(!_coe);
    assert(!_ui);
    
    _coe = new CEikonEnv();

    TRAPD(err, _coe->ConstructL());
    __ASSERT_ALWAYS(!err, User::Panic(_L("PPR_PANIC1"), err));
    
    _coe->RootWin().SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront);

    _ui = new CSymbAppUi();    
    TRAP(err, _ui->ConstructL());
    
    __ASSERT_ALWAYS(!err, User::Panic(_L("PPR_PANIC2"), err));    

    _coe->SetAppUi(_ui);
}

void Environment::destroyFramework()
{   
    if (_ui)
    {
        delete _ui;
        _ui = 0;        
    }
    
    if (_coe)
    {
        if (0 == _ui)
            _coe->SetAppUi(0);
        _coe->DestroyEnvironment();
        // TODO: Sure above is enough?
        //delete _coe;
        _coe = 0;
    }
}

void Environment::startWaitLoop()
{
    // owned by CEikonEnv
    CActiveScheduler::Start();
}

void Environment::stopWaitLoop()
{
    // owned by CEikonEnv
    CActiveScheduler::Stop();
}

CSymbAppUi& Environment::symbAppUi() const 
{ 
    return *_ui; 
}

void Environment::dispatchEvent(const Pt::Event& event)
{
    eventQueueSignal.send(event);
}

ApplicationImpl::ApplicationImpl(Application& app) 
: _app(app)
, _eventLoop(0)
{
    Environment::instance().registerResource(this);
    
    connect(Environment::instance().eventQueueSignal, app.event);
    
    // In case somebody tries to create another Application instance from
    // another thread, this is simply not possible
    lockAppInstance();

    // This only created the eventloop, it's not being run until
    // the view has been created. The view will also stop the loop.
    TRAPD(createError, _eventLoop = CSymbEventLoop::NewL(*this));
    if (createError != KErrNone)
    {
        unlockAppInstance();
        throw std::runtime_error("Eventloop creation failed" + PT_SOURCEINFO);
    }
}

ApplicationImpl::~ApplicationImpl()
{
    unlockAppInstance();
    delete _eventLoop;
    Environment::instance().unregisterResource(this);
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
    // If widgets are created prior to application instantiation
    // not all events were delivered. 
    // Make sure widgets get the important events (e.g. resize/move)
    Environment::instance().symbAppUi().SynchronizeWidgets();
    
    if (!_eventLoop->Start())
    {
        _eventLoop->Stop();        
        return 0;
    }
    
    _eventLoop->WaitForEvents();
    Environment::instance().startWaitLoop();
    _eventLoop->Stop();

    // TODO: Deliver some exit code?
    return 0;
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

// assuring that there is only one Application instance at a time
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
