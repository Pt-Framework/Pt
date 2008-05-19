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

#include "SymbDoc.h"
#include "SymbAppUi.h"

SymbDoc* SymbDoc::NewL(CEikApplication& aApp)
{
    SymbDoc* self = new (ELeave)SymbDoc(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;        
}

SymbDoc::~SymbDoc() 
{

}

SymbDoc::SymbDoc(CEikApplication& app) 
: CAknDocument(app), _parentApp(app)
{        
}

void SymbDoc::ConstructL() 
{        
}

CEikAppUi* SymbDoc::CreateAppUiL() 
{
    // there is only one AppUi, we should remember it for further
    // reference, but note that this is used as a factory 
    // there is no reason to delete all the AppUis created here
    _appUi = new (ELeave)SymbAppUi;
    _appUi->SetParentDoc(this);
    return _appUi; 
}

