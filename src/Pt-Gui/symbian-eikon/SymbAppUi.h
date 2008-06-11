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

#include <Pt/Gui/KeyEvent.h>

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
    
    Pt::Gui::ApplicationImpl& ApplicationImpl();
    
    const CFont* Font() const { return _font; }
    
private:
    void DynInitMenuPaneL(TInt, CEikMenuPane*);

    void HandleCommandL(TInt);
    
    virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    bool HandleEventKeyDown(const TKeyEvent& aKeyEvent, TEventCode aType);
    bool HandleEventKey(const TKeyEvent& aKeyEvent, TEventCode aType);
    bool HandleEventKeyUp(const TKeyEvent& aKeyEvent, TEventCode aType);
    
    void HandleExit();
    
    void DispatchFakePointerEvent(const TPointerEvent& event);
    bool HandleFakePointer(const TKeyEvent& aKeyEvent, TEventCode aType, int offset = 5);

    void RedrawWindows();
    void DispatchKeyEvent(Pt::Gui::KeyEvent::Type type,
            Pt::Gui::KeyEvent::KeyCode code,
            wchar_t chr);
    
    void MarkKeyDown(TUint scanCode, TUint keyCode);
    void ResetKeyDown(TUint scanCode);
    bool IsKeyDown(TUint scanCode, TUint& keyCode);
    
    void MarkFirstKey(TUint scanCode, bool down = true);
    bool IsFirstKey(TUint scanCode);    
    
    class SymbDoc* _parentDoc;

    CFont* _font;
    
    class CCursorControl* _cursorControl;

    enum
    {
        KMaxScancode = 256
    };
    // Flag that indicates if this is the first key event being produced
    // (for each scancode)
    bool _firstKey[KMaxScancode];
    // This table holds the virtual key code for each key being pressed
    // (for each scancode)
    TUint _keyDown[KMaxScancode];
};

#endif /*SYMBAPPUI_H_*/
