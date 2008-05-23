/***************************************************************************
 *   Copyright (C) 2008 Peter Barth                                        *
 *   Copyright (C) 2008 PTV AG                                             *
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

#include "SymbAppUi.h"
#include "SymbDoc.h"
#include "SymbApp.h"
#include "ApplicationImpl.h"
#include <assert.h>
#include "Widget.h"
#include "WidgetImpl.h"

void SymbAppUi::ConstructL()
{
    BaseConstructL(ENoAppResourceFile);
    SetKeyBlockMode(ENoKeyBlock);
    
    //_widget = new Pt::Gui::Widget();    
    
    //_widget->impl().construct();
    
    // Important Note:
    // When doing symbian debug builds debug marks are placed on certain
    // call stack levels and there will be panics when the debug marks are not
    // as expected when leaving the call stack.
    // This is apparently a problem when accessing singleton instances
    // in constructors etc.
    // We are just confident that these panics are not caused by real
    // memory leaks.
    Pt::Gui::ResourceRegistry::instance().constructPixmaps();
    Pt::Gui::ResourceRegistry::instance().constructWidgets();
}

SymbAppUi::~SymbAppUi()
{
    Pt::Gui::ResourceRegistry::instance().destructWidgets();
    Pt::Gui::ResourceRegistry::instance().destructPixmaps();

    //delete _widget;
}

void SymbAppUi::CloseApp() 
{ 
    Exit(); 
}

void SymbAppUi::SetParentDoc(SymbDoc* parentDoc)
{
    _parentDoc = parentDoc;
}

Pt::Gui::ApplicationImpl& SymbAppUi::GetApplicationImpl()
{
    Pt::Gui::ApplicationImpl* appImpl = static_cast<SymbApp&>(_parentDoc->GetParentApp()).GetApplicationImpl();
    assert(appImpl);
    return *appImpl;
}

void SymbAppUi::DynInitMenuPaneL(TInt, CEikMenuPane*) 
{

}

void SymbAppUi::HandleCommandL(TInt commandID)
{

}

TKeyResponse SymbAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent, 
        TEventCode aType)
{
    switch (aType)
    {
    case EEventKeyDown:
        if (aKeyEvent.iScanCode == 0xA5)
        {
            handleExit();
            return EKeyWasConsumed;
        }
        break;
    default:
        return EKeyWasNotConsumed;
    }
    return EKeyWasNotConsumed;
}

void SymbAppUi::handleExit()
{
    GetApplicationImpl().exit();    
}