/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
 *   Copyright (C) 2008 Peter Barth                                        *
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
#include "WidgetImpl.h"
#include "Widget.h"
#include "ApplicationImpl.h"

#include <Pt/Gfx/Region.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/CloseEvent.h>
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Gui/MoveEvent.h>
#include <Pt/Gui/MouseMoveEvent.h>

#include "SymbAppUi.h"
#include "SymbDoc.h"
#include "SymbApp.h"
#include "SymbianTools.h"

#include <limits>
#include <iostream>
#include <assert.h>

// symbian APIs
#include <coecntrl.h>
#include <w32std.h>

class CControl : public CCoeControl
{
public:
    struct GraphicContext
    {
        GraphicContext()
        : _gc(0)
        , _device(0)
        , _nativeFont(0)
        , _drawingActive(false)
        {            
        }
        
        CGraphicsContext* _gc;
        const CGraphicsDevice* _device;
        const CFont* _nativeFont; 
        bool _drawingActive;
    };
    
    // Constructors and destructor
    CControl(Pt::Gui::Widget& parentWidget) 
    : _parentWidget(parentWidget)
    {
    }

    void ConstructL(const TRect& rect)
    {
        _windowGroup=RWindowGroup(iCoeEnv->WsSession());
        
        User::LeaveIfError(_windowGroup.Construct((TUint32)&_windowGroup));
        
        _windowGroup.SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront);
        _windowGroup.EnableReceiptOfFocus(EFalse);
        
        // we're automatically becoming a window-owning control 
        CreateWindowL(&_windowGroup);
        SetRect(rect);
        EnableDragEvents();
        ActivateL();        
    }

    virtual ~CControl()
    {
        _windowGroup.Close();        
    }

    const GraphicContext& BeginDraw()
    {
        _graphicContext._gc = &SystemGc();
        _graphicContext._device = iEikonEnv->ScreenDevice();
        _graphicContext._nativeFont = iEikonEnv->AnnotationFont();
        if (!_graphicContext._drawingActive)
            SystemGc().Activate(Window());  
        _graphicContext._gc->UseFont(_graphicContext._nativeFont);
    
        return _graphicContext;
    }
    
    void EndDraw()
    {
        _graphicContext._gc = 0;
        _graphicContext._device = 0;
        _graphicContext._nativeFont = 0;
        if (!_graphicContext._drawingActive)
            SystemGc().Deactivate();                    
    }
    
private: // Functions from base classes

    /**
     * From CCoeControl,Draw.
     * @param Specified area for drawing
     */
    void Draw(const TRect& rect) const
    {
        _graphicContext._drawingActive = true;
        //CWindowGc& gc = SystemGc();
        //gc.SetPenStyle(CGraphicsContext::ENullPen);
        //gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        //gc.SetBrushColor(KRgbBlue);
        //gc.DrawRect(aRect);        
    
        Pt::Gui::PaintEvent paintEvent(_parentWidget, 
                Pt::Gui::SymbianTools::makeRegion(rect)); 
        
        _parentWidget.impl().dispatchEvent(paintEvent);
        _graphicContext._drawingActive = false;
    }

private:  // Data
    Pt::Gui::Widget& _parentWidget;    
    RWindowGroup _windowGroup;
    mutable GraphicContext _graphicContext;
};

namespace Pt {

namespace Gui {

const ssize_t WidgetImpl::KPositionUnused = std::numeric_limits<ssize_t>::max();

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at, const Math::Size& size)
: _apiWidget(apiWidget)
, _initialLocation(at)
, _initialSize(size)
, _painter(*this)
, _control(0)
{
    ResourceRegistry::instance().registerWidget(this);
}


WidgetImpl::~WidgetImpl()
{
    ResourceRegistry::instance().unregisterWidget(this);
    destruct();
}

Pt::String WidgetImpl::title() const
{ 
    return L""; 
}


void WidgetImpl::setTitle(const Pt::String& text)
{
}


Painter WidgetImpl::painter()
{
    return Painter(&_painter);
}


void WidgetImpl::show()
{
}


void WidgetImpl::hide()
{
}

bool WidgetImpl::isVisible() const
{
    return true;
}

void WidgetImpl::repaint()
{    
}

void WidgetImpl::setParent(Widget* parent)
{
}


void WidgetImpl::move(size_t x, size_t y)
{
}


void WidgetImpl::resize(size_t width, size_t height)
{
    if (isConstructed())
    {
        _control->SetSize(TSize(width, height));
        // will handle resize events
        synchronize();
    }
    else
    {
        _initialSize.setWidth(width);
        _initialSize.setHeight(height);
    }
}

void WidgetImpl::construct()
{
    if (_control)
        destruct();
    
    assert(Pt::Gui::ApplicationImpl::_self);        
    
    if (Pt::Gui::ApplicationImpl::_self->_symbApp->HasInitialized())
    {
        SymbAppUi& ui = Pt::Gui::ApplicationImpl::_self->_symbApp->GetDocument().GetAppUi();
        // TODO: Handle leave
        CControl* control = new (ELeave)CControl(_apiWidget);

        Pt::Math::Point location(_initialLocation);
        Pt::Math::Size size(_initialSize);        
        
        if (location.x() == KPositionUnused)
            location.setX(ui.ClientRect().iTl.iX);
        if (location.y() == KPositionUnused)
            location.setY(ui.ClientRect().iTl.iY);        
        if ((ssize_t)size.width() == KPositionUnused)
            size.setWidth(ui.ClientRect().Width());
        if ((ssize_t)size.height() == KPositionUnused)
            size.setHeight(ui.ClientRect().Height());
        
        control->ConstructL(SymbianTools::makeTRect(location, size));
        control->SetMopParent(&ui);
        ui.AddToStackL(control);
        _control = control;    
        
        synchronize(true);
    }
}

void WidgetImpl::destruct()
{
    assert(Pt::Gui::ApplicationImpl::_self);        

    if (_control && Pt::Gui::ApplicationImpl::_self->_symbApp->HasInitialized())
    {
        SymbAppUi& ui = Pt::Gui::ApplicationImpl::_self->_symbApp->GetDocument().GetAppUi();
        ui.RemoveFromStack(_control); 
    }

    if (_control)
    {
        delete _control;    
        _control = 0;
    }    
}

void WidgetImpl::dispatchEvent(Pt::Event& event)
{    
    assert(Pt::Gui::ApplicationImpl::_self);        
    Pt::Gui::ApplicationImpl::_self->dispatchEvent(event);
}

void WidgetImpl::beginDraw()            
{
    if (!isConstructed())
        return;

    const CControl::GraphicContext& graphicContext = _control->BeginDraw();
    _painter.setGc(graphicContext._gc);
    _painter.setDevice(graphicContext._device);
    _painter.setNativeFont(graphicContext._nativeFont);
}

void WidgetImpl::endDraw()
{
    if (!isConstructed())
        return;

    _painter.setGc(0);    
    _painter.setNativeFont(0);
    _painter.setDevice(0);
    
    _control->EndDraw();
}

void WidgetImpl::synchronize(bool initial/* = false*/)
{
    assert(_control);
    
    // see if we need to inform about changed position
    if (initial || 
        _control->Position().iX != (signed)_apiWidget.x() || 
        _control->Position().iY != (signed)_apiWidget.y())
    {
        MoveEvent moveEvent(_apiWidget, _control->Position().iX, _control->Position().iY);
        dispatchEvent(moveEvent);
    }
    
    // see if we need to inform about changed size
    if (initial || 
        _control->Rect().Width() != (signed)_apiWidget.size().width() || 
        _control->Rect().Height() != (signed)_apiWidget.size().height())
    {
        ResizeEvent::Type resizeType = ResizeEvent::Resize;
        ResizeEvent resizeEvent(_apiWidget, _control->Rect().Width(), _control->Rect().Height(), resizeType);
        dispatchEvent(resizeEvent);
    }
    
}

} // namespace Gui

} // namespace Pt
