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
#include "SymbEventLoop.h"

#include <Pt/Gui/Widget.h>
#include "WidgetImpl.h"
#include "ApplicationImpl.h"

#include <assert.h>

#include <coecntrl.h>
#include <w32std.h>

#include "Pt/Gui/CloseEvent.h"

static void Redraw()
{
    std::list<Pt::Gui::WidgetImpl*>& widgets = Pt::Gui::ResourceRegistry::instance().getWidgets();
    for (std::list<Pt::Gui::WidgetImpl*>::iterator i = widgets.begin(); i != widgets.end(); ++i) 
        if (!(*i)->parent())
            (*i)->repaint();       
}

// This class is used to simulate a mouse cursor
// It's not a perfect solution but better than nothing
class CCursorControl : public CCoeControl
{
public:
    // Constructors and destructor
    CCursorControl() 
    {
    }

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

    virtual ~CCursorControl()
    {
        _windowGroup.Close();        
    }
    
private:
    /**
     * From CCoeControl,Draw.
     * @param Specified area for drawing
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

void SymbAppUi::ConstructL()
{
    BaseConstructL(ENoAppResourceFile);
 
    _LIT(Kface,"Arial");
    TFontSpec spec(Kface, 20*3);
    _font = iEikonEnv->CreateScreenFontL(spec);    
        
    iEikonEnv->AppUiFactory()->StatusPane()->MakeVisible( EFalse );
    //SetKeyBlockMode(ENoKeyBlock);
    
    Pt::Gui::ResourceRegistry::instance().constructPixmaps();
    Pt::Gui::ResourceRegistry::instance().constructWidgets();
    
    // after everything has been constructed notify again about sizes
    std::list<Pt::Gui::WidgetImpl*>& widgets = Pt::Gui::ResourceRegistry::instance().getWidgets();
    for (std::list<Pt::Gui::WidgetImpl*>::iterator i = widgets.begin(); i != widgets.end(); ++i) 
        (*i)->synchronize(true);   
    
    //for (std::list<Pt::Gui::WidgetImpl*>::reverse_iterator i = widgets.rbegin(); i != widgets.rend(); ++i) 
    //    (*i)->synchronize(true);   
    
    // Create fake mouse cursor window
    _cursorControl = new (ELeave)CCursorControl();
    _cursorControl->ConstructL(TRect(0,0,15,15));
    AddToStackL(_cursorControl);    
    _cursorControl->SetMopParent(this);
        
    // run event loop 
    ApplicationImpl().eventLoop().Start();
    ApplicationImpl().eventLoop().WaitForEvents();
}

SymbAppUi::~SymbAppUi()
{
    ApplicationImpl().eventLoop().Stop();
    
    RemoveFromStack(_cursorControl);
    delete _cursorControl;
    
    Pt::Gui::ResourceRegistry::instance().destructWidgets();
    Pt::Gui::ResourceRegistry::instance().destructPixmaps();

    //delete _widget;
    iEikonEnv->ReleaseScreenFont(_font);
}

void SymbAppUi::CloseApp() 
{ 
    Exit(); 
}

void SymbAppUi::SetParentDoc(SymbDoc* parentDoc)
{
    _parentDoc = parentDoc;
}

Pt::Gui::ApplicationImpl& SymbAppUi::ApplicationImpl()
{
    Pt::Gui::ApplicationImpl* appImpl = static_cast<SymbApp&>(_parentDoc->ParentApp()).ApplicationImpl();
    assert(appImpl);
    return *appImpl;
}

void SymbAppUi::DynInitMenuPaneL(TInt, CEikMenuPane*) 
{

}

void SymbAppUi::HandleCommandL(TInt commandID)
{

}

#define SYMBIAN_LEFT        0x0e
#define SYMBIAN_RIGHT       0x0f
#define SYMBIAN_UP          0x10
#define SYMBIAN_DOWN        0x11
#define SYMBIAN_SELECT      167

TKeyResponse SymbAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent, 
        TEventCode aType)
{
    switch (aType)
    {
    case EEventKeyDown:
        if (aKeyEvent.iScanCode < 256)
            _firstKey[aKeyEvent.iScanCode] = false;

        if (aKeyEvent.iScanCode == 0xA5)
        {
            handleExit();
            return EKeyWasConsumed;
        }
        else if (HandleFakePointer(aKeyEvent, aType))
            return EKeyWasConsumed;
        break;

    // this is not what sounds like:
    // This is only sent to a key click plug-in DLL (if one is present) to indicate a repeating key event.
    //case EEventKeyRepeat:
    //    if (HandleFakePointer(aKeyEvent, aType))
    //        return EKeyWasConsumed;
    //    break;
    case EEventKey:
        if (aKeyEvent.iScanCode < 256 && !_firstKey[aKeyEvent.iScanCode])
            _firstKey[aKeyEvent.iScanCode] = true;
        else if (aKeyEvent.iScanCode < 256)
            _firstKey[aKeyEvent.iScanCode] = false;
        
        if (aKeyEvent.iScanCode < 256 && _firstKey[aKeyEvent.iScanCode])
            return EKeyWasConsumed;

        if (HandleFakePointer(aKeyEvent, aType, 30))
            return EKeyWasConsumed;
        
    case EEventKeyUp:
        if (HandleFakePointer(aKeyEvent, aType))
            return EKeyWasConsumed;
        break;
        
    default:
        return EKeyWasNotConsumed;
    }
    return EKeyWasNotConsumed;
}

//void SymbAppUi::HandlePointerEventL(const TPointerEvent& aPointerEvent)
//{
//   int i = 0;
//   i++;
//   i--;
//}

void SymbAppUi::handleExit()
{
    Exit();    
}

class CControl : public CCoeControl
{
public:
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
};

bool SymbAppUi::HandleFakePointer(const TKeyEvent& aKeyEvent, TEventCode aType, int offset/* = 5*/)
{
    if (aType == EEventKeyDown || aType == EEventKey)
    {
        if (aType == EEventKeyDown && aKeyEvent.iScanCode == SYMBIAN_SELECT)
        {
            TPointerEvent event;
            memset(&event, 0, sizeof(event));
            event.iType = TPointerEvent::EButton1Down;
            event.iPosition = _cursorControl->Position();
            event.iPosition+=TPoint(2,2);
            event.iParentPosition = event.iPosition;
            
            std::list<Pt::Gui::WidgetImpl*>& widgets = Pt::Gui::ResourceRegistry::instance().getWidgets();
            for (std::list<Pt::Gui::WidgetImpl*>::iterator i = widgets.begin(); i != widgets.end(); ++i) 
            {
                Pt::Gui::WidgetImpl* impl = *i;
                if (!impl->parent())
                {
                    TRect rect(impl->nativeControl()->Position(), impl->nativeControl()->Size());
                    if (rect.Contains(event.iPosition))
                    {
                        event.iPosition-=impl->nativeControl()->Position();
                        impl->nativeControl()->HandlePointerEventL(event);
                    }
                }            
            }
        
            return true;
        }
        else if (aKeyEvent.iScanCode == SYMBIAN_LEFT)
        {
            TPoint pos = _cursorControl->Position();
            pos.iX-=offset;
            _cursorControl->SetPosition(pos);
            Redraw();
            return true;
        }
        else if (aKeyEvent.iScanCode == SYMBIAN_RIGHT)
        {
            TPoint pos = _cursorControl->Position();
            pos.iX+=offset;
            _cursorControl->SetPosition(pos);
            Redraw();
            return true;
        }
        else if (aKeyEvent.iScanCode == SYMBIAN_UP)
        {
            TPoint pos = _cursorControl->Position();
            pos.iY-=offset;
            _cursorControl->SetPosition(pos);
            Redraw();
            return true;
        }
        else if (aKeyEvent.iScanCode == SYMBIAN_DOWN)
        {
            TPoint pos = _cursorControl->Position();
            pos.iY+=offset;
            _cursorControl->SetPosition(pos);
            Redraw();
            return true;
        }
    }
    else if (aType == EEventKeyUp)
    {
        if (aKeyEvent.iScanCode == SYMBIAN_SELECT)
        {
            TPointerEvent event;
            memset(&event, 0, sizeof(event));
            event.iType = TPointerEvent::EButton1Up;
            event.iPosition = _cursorControl->Position();
            event.iPosition+=TPoint(2,2);
            event.iParentPosition = event.iPosition;

            std::list<Pt::Gui::WidgetImpl*>& widgets = Pt::Gui::ResourceRegistry::instance().getWidgets();
            for (std::list<Pt::Gui::WidgetImpl*>::iterator i = widgets.begin(); i != widgets.end(); ++i) 
            {
                Pt::Gui::WidgetImpl* impl = *i;
                if (!impl->parent())
                {
                    TRect rect(impl->nativeControl()->Position(), impl->nativeControl()->Size());
                    if (rect.Contains(event.iPosition))
                    {
                        event.iPosition-=impl->nativeControl()->Position();
                        impl->nativeControl()->HandlePointerEventL(event);
                    }
                }            
            }
                
            return true;
        }
    }
    
    return false;
}
