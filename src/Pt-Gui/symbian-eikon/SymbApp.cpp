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

#include "SymbApp.h"
#include "SymbDoc.h"
#include "ApplicationImpl.h"

const TUid KUidsymbian = { 0x0D7113C1 };

// TODO: Find solution for delivering/setting UID
TUid SymbApp::AppDllUid() const 
{ 
    return KUidsymbian; 
}

CApaDocument* SymbApp::CreateDocumentL() 
{ 
    // there is only one document, we should remember it for further
    // reference, but note that this is used as a factory 
    // there is no reason to delete all the documents created here
    _document = SymbDoc::NewL(*this);
    return _document; 
}

TFileName SymbApp::ResourceFileName() const
{
    return TFileName();
}    
