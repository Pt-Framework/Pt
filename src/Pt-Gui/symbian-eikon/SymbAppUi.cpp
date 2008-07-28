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

#include <assert.h>
#include <coecntrl.h>
#include <eikenv.h>
#include <eikspane.h>
#include <w32std.h>

#include <Pt/Gui/Widget.h>

#include "SymbEventLoop.h"
#include "WidgetImpl.h"
#include "ApplicationImpl.h"

/**
 * @brief This class is used to simulate a mouse cursor.
 * It's not a perfect solution but better than nothing.
 */
class CCursorControl : public CCoeControl
{
public:
    /**
     * @brief First phase construction
     */
    CCursorControl() 
    {
    }

    /**
     * @brief Second phase constructor. 
     * Construct mouse cursor window with given size.
     */    
    void ConstructL(const TRect& rect)
    {
        _windowGroup = RWindowGroup(iCoeEnv->WsSession());

        User::LeaveIfError(_windowGroup.Construct((TUint32)&_windowGroup));

        _windowGroup.SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront);
        _windowGroup.EnableReceiptOfFocus(EFalse);

        // we're automatically becoming a window-owning control 
        CreateWindowL(&_windowGroup);

        SetRect(rect);
        EnableDragEvents();
        ActivateL();        
    }

    /**
     * @brief Destructor
     */
    virtual ~CCursorControl()
    {
        _windowGroup.Close();        
    }
    
private:
    /**
     * @brief From CCoeControl: Draw content of window.
     * @param rect Specified area for drawing
     */
    void Draw(const TRect& rect) const
    {
        CWindowGc& gc = SystemGc();
        gc.SetPenStyle(CGraphicsContext::ENullPen);
        gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        gc.SetBrushColor(KRgbBlue);
        gc.DrawEllipse(rect);        
    }
    
    RWindowGroup _windowGroup;
};

void CSymbAppUi::ConstructL()
{
    // No resource file present. 
    BaseConstructL(ENoAppResourceFile);

    memset(_keyDown, 0, sizeof(_keyDown));
 
    // Create some default font.
    _LIT(Kface,"Arial");
    TFontSpec spec(Kface, 20*3);    
    _font = iEikonEnv->CreateScreenFontL(spec);    
    
    // could that happen? Probably not...
    if (!_font)
    {
        _font = const_cast<CFont*>(iEikonEnv->TitleFont());
    }
    
    // Hide status pane? Gives more client area.
    iEikonEnv->AppUiFactory()->StatusPane()->MakeVisible(EFalse);
    
#ifdef __SERIES60_3X__
    // Allow multiple keys being pressed at the same time.
    SetKeyBlockMode(ENoKeyBlock);
#endif
    
    // TODO: Does nothing on series 60
    //iEikonEnv->WsSession().SetPointerCursorArea(ClientRect());
    //iEikonEnv->VirtualCursor().SetCursorStateL(TEikVirtualCursor::EOn, *(iEikonEnv));
    
    // Create fake mouse cursor window
    _cursorControl = new (ELeave)CCursorControl();
    _cursorControl->ConstructL(TRect(0, 0, 15, 15));
    AddToStackL(_cursorControl);    
    _cursorControl->SetMopParent(this);
}

CSymbAppUi::~CSymbAppUi()
{
    RemoveFromStack(_cursorControl);
    delete _cursorControl;
    
    iEikonEnv->ReleaseScreenFont(_font);
}

void CSymbAppUi::DynInitMenuPaneL(TInt, CEikMenuPane*) 
{
    // left empty on purpose
}

void CSymbAppUi::HandleCommandL(TInt commandID)
{
    // left empty on purpose
}

TKeyResponse CSymbAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TKeyCode code;
    
    switch (aType)
    {
        // First key event to happen, keycode is empty, scancode is valid
        case EEventKeyDown:
            if (HandleEventKeyDown(aKeyEvent, aType))
                return EKeyWasConsumed;
            break;
    
        // This event is send repeatedly while the key is pressed
        case EEventKey:
            if (HandleEventKey(aKeyEvent, aType))
                return EKeyWasConsumed;
            break;
         
        // Last key event to happen, keycode is empty, scancode is valid
        case EEventKeyUp:
            if (HandleEventKeyUp(aKeyEvent, aType))
                return EKeyWasConsumed;
            break;
            
        default:
            return EKeyWasNotConsumed;
    }
    
    return EKeyWasNotConsumed;
}

bool CSymbAppUi::HandleEventKeyDown(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    // initialize firstkey flag to false for this key's scancode
    MarkFirstKey(aKeyEvent.iScanCode, false);

    // Handle hardcoded Exit "key"
    // TODO: Find better solution to exit application.
    if (aKeyEvent.iScanCode == EStdKeyDevice1)
    {
        HandleExit();
        return true;
    }
    else if (HandleFakePointer(aKeyEvent, aType, 5))
    {
        return true;
    }

    // Note that after this keydown event follows another event of type EEventKey
    return false;
}

static Pt::Gui::KeyEvent::KeyCode TranslateKeycode(TUint nativeCode)
{
    Pt::Gui::KeyEvent::KeyCode code = Pt::Gui::KeyEvent::Void;    

    switch (nativeCode)
    {
        case EKeyBackspace:     code = Pt::Gui::KeyEvent::Backspace;    break;
        case EKeyTab:           code = Pt::Gui::KeyEvent::Tab;          break;
        case EKeyEnter:         code = Pt::Gui::KeyEvent::Enter;        break;
        case EKeyEscape:        code = Pt::Gui::KeyEvent::Escape;       break;            
        case EKeySpace:         code = Pt::Gui::KeyEvent::Space;        break;        
        case EKeyDelete:        code = Pt::Gui::KeyEvent::Delete;       break;
        case EKeyPrintScreen:   code = Pt::Gui::KeyEvent::PrintScreen;  break;
        case EKeyPause:         code = Pt::Gui::KeyEvent::Pause;        break;
        case EKeyHome:          code = Pt::Gui::KeyEvent::Home;         break;
        case EKeyEnd:           code = Pt::Gui::KeyEvent::End;          break;
        case EKeyPageUp:        code = Pt::Gui::KeyEvent::PageUp;       break;
        case EKeyPageDown:      code = Pt::Gui::KeyEvent::PageDown;     break;
        case EKeyInsert:        code = Pt::Gui::KeyEvent::Insert;       break;
        case EKeyLeftArrow:     code = Pt::Gui::KeyEvent::Left;         break;
        case EKeyRightArrow:    code = Pt::Gui::KeyEvent::Right;        break;
        case EKeyUpArrow:       code = Pt::Gui::KeyEvent::Up;           break;
        case EKeyDownArrow:     code = Pt::Gui::KeyEvent::Down;         break;
        case EKeyLeftAlt:       code = Pt::Gui::KeyEvent::AltL;         break;
        case EKeyRightAlt:      code = Pt::Gui::KeyEvent::AltR;         break;
        case EKeyLeftCtrl:      code = Pt::Gui::KeyEvent::CtrlL;        break;
        case EKeyRightCtrl:     code = Pt::Gui::KeyEvent::CtrlR;        break;
        case EKeyLeftFunc:      code = Pt::Gui::KeyEvent::WindowsL;     break;
        case EKeyRightFunc:     code = Pt::Gui::KeyEvent::WindowsR;     break;
        case EKeyCapsLock:      code = Pt::Gui::KeyEvent::CapsLock;     break;
        case EKeyNumLock:       code = Pt::Gui::KeyEvent::NumLock;      break;
        case EKeyScrollLock:    code = Pt::Gui::KeyEvent::ScrollLock;   break;

        case EKeyF1:            code = Pt::Gui::KeyEvent::F1;           break;
        case EKeyF2:            code = Pt::Gui::KeyEvent::F2;           break;
        case EKeyF3:            code = Pt::Gui::KeyEvent::F3;           break;
        case EKeyF4:            code = Pt::Gui::KeyEvent::F4;           break;
        case EKeyF5:            code = Pt::Gui::KeyEvent::F5;           break;
        case EKeyF6:            code = Pt::Gui::KeyEvent::F6;           break;
        case EKeyF7:            code = Pt::Gui::KeyEvent::F7;           break;
        case EKeyF8:            code = Pt::Gui::KeyEvent::F8;           break;
        case EKeyF9:            code = Pt::Gui::KeyEvent::F9;           break;
        case EKeyF10:           code = Pt::Gui::KeyEvent::F10;          break;
        case EKeyF11:           code = Pt::Gui::KeyEvent::F11;          break;
        case EKeyF12:           code = Pt::Gui::KeyEvent::F12;          break;
        case EKeyF13:           code = Pt::Gui::KeyEvent::F13;          break;
        case EKeyF14:           code = Pt::Gui::KeyEvent::F14;          break;
        case EKeyF15:           code = Pt::Gui::KeyEvent::F15;          break;
        case EKeyF16:           code = Pt::Gui::KeyEvent::F16;          break;
        case EKeyF17:           code = Pt::Gui::KeyEvent::F17;          break;
        case EKeyF18:           code = Pt::Gui::KeyEvent::F18;          break;
        case EKeyF19:           code = Pt::Gui::KeyEvent::F19;          break;
        case EKeyF20:           code = Pt::Gui::KeyEvent::F20;          break;
        case EKeyF21:           code = Pt::Gui::KeyEvent::F21;          break;
        case EKeyF22:           code = Pt::Gui::KeyEvent::F22;          break;
        case EKeyF23:           code = Pt::Gui::KeyEvent::F23;          break;
        case EKeyF24:           code = Pt::Gui::KeyEvent::F24;          break;
        
        case EKeyOff:           code = Pt::Gui::KeyEvent::Meta;         break;               
        case EKeyMenu:          code = Pt::Gui::KeyEvent::ContextMenu;  break;
        // TODO: Handle vendor specific cell phone keyboard scancodes
    }
    
    return code;
}

bool CSymbAppUi::HandleEventKey(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    // remember key code for key being pressed    
    MarkKeyDown(aKeyEvent.iScanCode, aKeyEvent.iCode);
    
    // set first key flag if necessary
    if (!IsFirstKey(aKeyEvent.iScanCode))
    {
        MarkFirstKey(aKeyEvent.iScanCode);
    }
    // otherwise reset it (not the first key)
    else 
    {
        MarkFirstKey(aKeyEvent.iScanCode, false);
    }

    // Handle our fake mouse cursor first
    // if this is the first key we already handled the cursor in HandleEventKeyDown
    if (!IsFirstKey(aKeyEvent.iScanCode))
    {
        if (HandleFakePointer(aKeyEvent, aType, 30))
        {
            return true;
        }
    }
    else
    {
        Pt::Gui::KeyEvent::KeyCode code = TranslateKeycode(aKeyEvent.iCode);
        Pt::Gui::KeyEvent::Type type = Pt::Gui::KeyEvent::Press;
        wchar_t chr = aKeyEvent.iCode < ENonCharacterKeyBase ? aKeyEvent.iCode : 0;        
        DispatchKeyEvent(type, code, chr);        
        return true;
    }

    return false;    
}

bool CSymbAppUi::HandleEventKeyUp(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (HandleFakePointer(aKeyEvent, aType))
    {
        return true;
    }
    else
    {
        TUint nativeCode;
        if (IsKeyDown(aKeyEvent.iScanCode, nativeCode))
        {
            Pt::Gui::KeyEvent::KeyCode code = TranslateKeycode(nativeCode);
            Pt::Gui::KeyEvent::Type type = Pt::Gui::KeyEvent::Release;
            wchar_t chr = nativeCode < ENonCharacterKeyBase ? nativeCode : 0;        
            DispatchKeyEvent(type, code, chr);  
            ResetKeyDown(aKeyEvent.iScanCode);
            return true;
        }
    }

    ResetKeyDown(aKeyEvent.iScanCode);
    return false;    
}

void CSymbAppUi::HandleExit()
{
    Pt::Gui::Environment::instance().stopWaitLoop();    
}

void CSymbAppUi::SynchronizeWidgets()
{
    std::list<Pt::Gui::Resource*>& resources = Pt::Gui::Environment::instance().resources();

    for (std::list<Pt::Gui::Resource*>::iterator i = resources.begin(); i != resources.end(); ++i) 
    {
        Pt::Gui::Resource* resource = *i;
        if (resource->Type() == Pt::Gui::Resource::TypeWidget)
        {
            Pt::Gui::WidgetImpl* impl = dynamic_cast<Pt::Gui::WidgetImpl*>(resource);
            assert(impl);
            impl->synchronize(true);
        }
    }
}

// Dirty trick: Declare CControl with public HandlePointerEventL
class CControl : public CCoeControl
{
public:
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
};

void CSymbAppUi::DispatchFakePointerEvent(const TPointerEvent& event)
{
    // dispatch fake pointer event to all main windows
    std::list<Pt::Gui::Resource*>& resources = Pt::Gui::Environment::instance().resources();

    for (std::list<Pt::Gui::Resource*>::iterator i = resources.begin(); i != resources.end(); ++i) 
    {
        Pt::Gui::Resource* resource = *i;
        if (resource->Type() == Pt::Gui::Resource::TypeWidget)
        {
            Pt::Gui::WidgetImpl* impl = dynamic_cast<Pt::Gui::WidgetImpl*>(resource);
            assert(impl);
            if (!impl->parent())
            {
                TRect rect(impl->nativeControl()->Position(), impl->nativeControl()->Size());
                if (rect.Contains(event.iPosition))
                {
                    TPointerEvent newEvent = event;
                    newEvent.iPosition-=impl->nativeControl()->Position();
                    impl->nativeControl()->HandlePointerEventL(newEvent);
                }
            }
        }
    }    
}

bool CSymbAppUi::HandleFakePointer(const TKeyEvent& aKeyEvent, TEventCode aType, int offset/* = 5*/)
{
    if (aType == EEventKeyDown || aType == EEventKey)
    {
        TPoint oldPos = _cursorControl->Position();
        bool handled = false;
        
        // If key event is down and code is EStdKeyDevice3
        // we simulate a mouse button down event
        if (aType == EEventKeyDown && aKeyEvent.iScanCode == EStdKeyDevice3)
        {
            TPointerEvent event;
            memset(&event, 0, sizeof(event));
            event.iType = TPointerEvent::EButton1Down;
            event.iPosition = _cursorControl->Position();
            event.iPosition+=TPoint(2,2);
            event.iParentPosition = event.iPosition;
            
            DispatchFakePointerEvent(event);
            handled = true;
        }
        // move fake mouse cursor left
        else if (aKeyEvent.iScanCode == EStdKeyLeftArrow)
        {
            TPoint pos = _cursorControl->Position();
            pos.iX-=offset;
            _cursorControl->SetPosition(pos);
            RedrawWindows();
            handled = true;
        }
        // move fake mouse cursor right
        else if (aKeyEvent.iScanCode == EStdKeyRightArrow)
        {
            TPoint pos = _cursorControl->Position();
            pos.iX+=offset;
            _cursorControl->SetPosition(pos);
            RedrawWindows();
            handled = true;
        }
        // move fake mouse cursor up
        else if (aKeyEvent.iScanCode == EStdKeyUpArrow)
        {
            TPoint pos = _cursorControl->Position();
            pos.iY-=offset;
            _cursorControl->SetPosition(pos);
            RedrawWindows();
            handled = true;
        }
        // move fake mouse cursor down
        else if (aKeyEvent.iScanCode == EStdKeyDownArrow)
        {
            TPoint pos = _cursorControl->Position();
            pos.iY+=offset;
            _cursorControl->SetPosition(pos);
            RedrawWindows();
            handled = true;
        }
    
        // if position has changed we simulate mouse move event
        // when EStdKeyDevice3 is down in addition, we simulate drag event
        if (oldPos != _cursorControl->Position())
        {
            TPointerEvent event;
            memset(&event, 0, sizeof(event));
            TUint code;
            event.iType = IsKeyDown(EStdKeyDevice3, code) ? TPointerEvent::EDrag : TPointerEvent::EMove;
            event.iPosition = _cursorControl->Position();
            event.iPosition+=TPoint(2,2);
            event.iParentPosition = event.iPosition;            
            DispatchFakePointerEvent(event);            
        }
        
        return handled;
    }
    else if (aType == EEventKeyUp)
    {
        if (aKeyEvent.iScanCode == EStdKeyDevice3)
        {
            TPointerEvent event;
            memset(&event, 0, sizeof(event));
            event.iType = TPointerEvent::EButton1Up;
            event.iPosition = _cursorControl->Position();
            event.iPosition+=TPoint(2,2);
            event.iParentPosition = event.iPosition;

            DispatchFakePointerEvent(event);                
            return true;
        }
    }
    
    return false;
}

void CSymbAppUi::RedrawWindows()
{
    std::list<Pt::Gui::Resource*>& resources = Pt::Gui::Environment::instance().resources();

    for (std::list<Pt::Gui::Resource*>::iterator i = resources.begin(); i != resources.end(); ++i) 
    {
        Pt::Gui::Resource* resource = *i;
        if (resource->Type() == Pt::Gui::Resource::TypeWidget)
        {
            Pt::Gui::WidgetImpl* impl = dynamic_cast<Pt::Gui::WidgetImpl*>(resource);
            assert(impl);
            if (!impl->parent())
            {
                impl->repaint();       
            }
        }
    }
}

void CSymbAppUi::DispatchKeyEvent(Pt::Gui::KeyEvent::Type type,
        Pt::Gui::KeyEvent::KeyCode code,
        wchar_t chr)
{
    // dispatch key event to all top level windows
    std::list<Pt::Gui::Resource*>& resources = Pt::Gui::Environment::instance().resources();

    for (std::list<Pt::Gui::Resource*>::iterator i = resources.begin(); i != resources.end(); ++i) 
    {
        Pt::Gui::Resource* resource = *i;
        if (resource->Type() == Pt::Gui::Resource::TypeWidget)
        {
            Pt::Gui::WidgetImpl* impl = dynamic_cast<Pt::Gui::WidgetImpl*>(resource);
            assert(impl);
            if (!impl->parent())
            {
                Pt::Gui::KeyEvent keyEvent(impl->apiWidget(), type, code, chr);                        
                Pt::Gui::Environment::instance().dispatchEvent(keyEvent);
            }
        }
    }
}

void CSymbAppUi::MarkKeyDown(TUint scanCode, TUint keyCode)
{
    if (scanCode && scanCode < KMaxScancode)
    {
        _keyDown[scanCode] = keyCode;    
    }
}

void CSymbAppUi::ResetKeyDown(TUint scanCode)
{
    MarkKeyDown(scanCode, 0);
}

bool CSymbAppUi::IsKeyDown(TUint scanCode, TUint& keyCode)
{
    keyCode = 0;
    if (scanCode && scanCode < KMaxScancode)
    {
        keyCode = _keyDown[scanCode];
    }
    
    return keyCode != 0;
}

void CSymbAppUi::MarkFirstKey(TUint scanCode, bool down/* = true*/)
{
    if (scanCode && scanCode < KMaxScancode)
    {
        _firstKey[scanCode] = down;       
    }
}

bool CSymbAppUi::IsFirstKey(TUint scanCode) 
{
    if (scanCode && scanCode < KMaxScancode)
    {
        return _firstKey[scanCode];       
    }
    
    return false;
}
