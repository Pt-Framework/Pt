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
#ifndef SYMBAPP_H_
#define SYMBAPP_H_

#include <aknapp.h>

namespace Pt {
namespace Gui {
class ApplicationImpl;
}
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// IMPORTANT NOTE:
// ALL Symbian classes have to reside in the global namespace
// otherwise results are undefined
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class SymbApp : public CAknApplication
{
public:
    SymbApp(Pt::Gui::ApplicationImpl* appImpl) 
    : _appImpl(appImpl)
    , _document(0)
    {
    }
    
    virtual TFileName ResourceFileName() const;
    
    class SymbDoc& Document() { return *_document; }

    Pt::Gui::ApplicationImpl* ApplicationImpl() { return _appImpl; }
    
    bool HasInitialized() const { return _document != 0; }

private:        
    CApaDocument* CreateDocumentL();
    
    // TODO: Find solution for delivering/setting UID
    TUid AppDllUid() const;

    Pt::Gui::ApplicationImpl* _appImpl;
    SymbDoc* _document;    
};

#endif /*SYMBAPP_H_*/
