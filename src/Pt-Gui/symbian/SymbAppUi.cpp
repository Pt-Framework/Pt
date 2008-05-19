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
#include "ApplicationImpl.h"
#include <assert.h>
#include "Widget.h"

void SymbAppUi::ConstructL()
{
    BaseConstructL(ENoAppResourceFile);
    SetKeyBlockMode(ENoKeyBlock);
    
    // there must be an application Impl running, 
    // otherwise something is wrong at this point
    Pt::Gui::ApplicationImpl* appImpl = Pt::Gui::ApplicationImpl::_self;    
    assert(appImpl);
    appImpl->constructBackendWidgets();
}

SymbAppUi::~SymbAppUi()
{

}

void SymbAppUi::CloseApp() 
{ 
    Exit(); 
}

void SymbAppUi::SetParentDoc(SymbDoc* parentDoc)
{
    _parentDoc = parentDoc;
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
            Exit();
            return EKeyWasConsumed;
        }
        break;
    default:
        return EKeyWasNotConsumed;
    }
    return EKeyWasNotConsumed;
}
