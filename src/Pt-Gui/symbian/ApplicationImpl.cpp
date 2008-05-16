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

#include "Pt/Gui/Application.h"
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/CloseEvent.h>
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Gui/MoveEvent.h>
#include <Pt/Gui/MouseMoveEvent.h>

#include <iostream>

// Symbian APIs
#include <aknapp.h>
#include <akndoc.h>
#include <aknappui.h>
#include <eikstart.h>

using namespace std;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// IMPORTANT NOTE:
// The Symbian all classes have to reside in the global namespace
// otherwise results are undefined
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
const TUid KUidsymbian = { 0x0D7113C1 };

class AppUi : public CAknAppUi
{
public:     
    void ConstructL()
    {
        BaseConstructL(ENoAppResourceFile);
        SetKeyBlockMode(ENoKeyBlock);
        //iEikonEnv->AppUiFactory()->StatusPane()->MakeVisible( EFalse );
    }
    
    ~AppUi()
    {
        
    }

    void CloseApp() 
    { 
        Exit(); 
    }

    void SetParentDoc(class Document* parentDoc)
    {
        _parentDoc = parentDoc;
    }
    
private:
    void DynInitMenuPaneL(TInt, CEikMenuPane*) 
    {
        
    }

    void HandleCommandL( TInt )
    {
        
    }
    
    virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, 
            TEventCode aType)
    {
        switch (aType)
        {
            case EEventKeyDown:
                if (aKeyEvent.iScanCode == 0xA5)
                {
                    Exit();
                    return EKeyWasConsumed;
                }
                break;
            default:
                return EKeyWasNotConsumed;
        }
        return EKeyWasNotConsumed;
    }
    
    class Document* _parentDoc;
};

class Document : public CAknDocument
{
public:
    static Document* NewL(CEikApplication& aApp)
    {
        Document* self = new (ELeave)Document(aApp);
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop();
        return self;        
    }
    
    virtual ~Document() 
    {
        
    }
    
    CEikApplication& GetParentApp() { return _parentApp; }
    AppUi& GetAppUi() { return *_appUi; }
  
private:
    Document(CEikApplication& app) : CAknDocument(app), _parentApp(app)
    {        
    }
    
    void ConstructL() 
    {        
    }
    
    CEikAppUi* CreateAppUiL() 
    {
        // there is only one AppUi, we should remember it for further
        // reference, but note that this is used as a factory 
        // there is no reason to delete all the AppUis created here
        _appUi = new (ELeave)AppUi;
        _appUi->SetParentDoc(this);
        return _appUi; 
    }
        
    CEikApplication& _parentApp;
    AppUi* _appUi;
};

class SymbianApp : public CAknApplication
{
private:    
    CApaDocument* CreateDocumentL() 
    { 
        // get the parent Pt::Gui::ApplicationImpl instance
        // this is a bit messy since SymbianApp is created by
        // a static global factory function
        _appImpl = Pt::Gui::ApplicationImpl::_self;
        _appImpl->_symbApp = this;
        Pt::Gui::ApplicationImpl::unlockAppInstance();
        // there is only one document, we should remember it for further
        // reference, but note that this is used as a factory 
        // there is no reason to delete all the documents created here
        _document = Document::NewL(*this);
        return _document; 
    }
    
    TUid AppDllUid() const 
    { 
        return KUidsymbian; 
    }

    Pt::Gui::ApplicationImpl* _appImpl;
    Document* _document;
    
public:
    virtual TFileName ResourceFileName() const
    {
        return TFileName();
    }    
    
    Document& GetDocument() { return *_document; }
};

static CApaApplication* NewApplication()
{
    return new SymbianApp;
}

namespace Pt {

namespace Gui {

ApplicationImpl::ApplicationImpl(Application& app) 
: _app(app), /*_eventLoopThread(_eventLoop), */_symbApp(0)
{
}

ApplicationImpl::~ApplicationImpl()
{
    //_eventLoop.exit();
    //_eventLoopThread.wait();
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
    return ApplicationImpl::createAndRunAppInstance(*this);
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

ApplicationImpl* ApplicationImpl::_self = NULL;
System::Mutex ApplicationImpl::_mutex(System::Mutex::Normal);


void ApplicationImpl::lockAppInstance()
{
    _mutex.lock();
}

void ApplicationImpl::unlockAppInstance()
{
    _mutex.unlock();    
}

int ApplicationImpl::createAndRunAppInstance(ApplicationImpl& impl)
{
    lockAppInstance();
    ApplicationImpl::_self = &impl;  
    return EikStart::RunApplication(NewApplication);
}

} // namespace Gui

} // namespace Pt
