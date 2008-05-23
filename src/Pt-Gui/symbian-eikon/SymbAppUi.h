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
#ifndef SYMBAPPUI_H_
#define SYMBAPPUI_H_

#include <aknappui.h>

namespace Pt
{
namespace Gui
{
class Widget;
class ApplicationImpl;
}
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// IMPORTANT NOTE:
// ALL Symbian classes have to reside in the global namespace
// otherwise results are undefined
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class SymbAppUi : public CAknAppUi
{
public:     
    void ConstructL();    
    ~SymbAppUi();
    void CloseApp();

    void SetParentDoc(class SymbDoc* parentDoc);
    
    Pt::Gui::ApplicationImpl& GetApplicationImpl();
    
private:
    void DynInitMenuPaneL(TInt, CEikMenuPane*);

    void HandleCommandL(TInt);
    
    virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, 
            TEventCode aType);

    void handleExit();
    
    class SymbDoc* _parentDoc;
    Pt::Gui::Widget* _widget;
};

#endif /*SYMBAPPUI_H_*/
