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
#include <Pt/Gui/Widget.h>
#include "ApplicationImpl.h"

#include <Pt/Gfx/Region.h>
#include <Pt/Gui/MouseEvent.h>
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
        , _drawingActive(0)
        , _references(0)
        {            
        }
        
        CGraphicsContext* _gc;
        const CGraphicsDevice* _device;
        const CFont* _nativeFont; 
        int _drawingActive;
        int _references;
    };
    
    // Constructors and destructor
    CControl(Pt::Gui::WidgetImpl& owner) 
    : _apiWidgetImpl(owner)
    , _pointerEventConsumed(false)
    , _allowParentRedrawing(false)
    {
    }

    void SetAllowParentRedrawing(bool allowParentRedrawing) { _allowParentRedrawing = allowParentRedrawing; }
    
    void ConstructL(const TRect& rect)
    {
        if (!_apiWidgetImpl.parent())
        {
            _windowGroup = RWindowGroup(iCoeEnv->WsSession());

            User::LeaveIfError(_windowGroup.Construct((TUint32)&_windowGroup));

            _windowGroup.SetOrdinalPosition(0, /*ECoeWinPriorityAlwaysAtFront*/ECoeWinPriorityHigh);
            _windowGroup.EnableReceiptOfFocus(EFalse);

            // we're automatically becoming a window-owning control 
            CreateWindowL(&_windowGroup);
        }
        else
        {
            // get parent native control
            Pt::Gui::WidgetImpl& impl = _apiWidgetImpl.parent()->impl();
            CControl* parentControl = impl.nativeControl();
            // this is our container
            SetContainerWindowL(*parentControl);
            // add ourselves to parent
            parentControl->AddControl(this);
        }
        
        SetRect(rect);
        EnableDragEvents();
        ActivateL();        
    }

    virtual ~CControl()
    {
        if (!_apiWidgetImpl.parent())
            _windowGroup.Close();        
    }
    
    // Get absolute position of widget within parent widget
    TPoint AbsolutePosition() const
    {
        return _apiWidgetImpl.parent() ? Position() : TPoint(0,0);
    }   
    
    // Get relative position of widget within parent widget
    // Don't call this function if there is no parent, it will not work 
    void SetRelativePosition(const TPoint& position)
    {
        TPoint relativePos(0,0);
        if (_apiWidgetImpl.parent())
        {
            assert(_apiWidgetImpl.parent()->impl().nativeControl());
            relativePos = _apiWidgetImpl.parent()->impl().nativeControl()->AbsolutePosition();
            relativePos+=position;
            SetPosition(relativePos);
        }
        else
        {
            assert(false);
        }
    }

    // Get relative position of widget within parent widget
    TPoint RelativePosition()
    {
        TPoint relativePos(0,0);
        if (_apiWidgetImpl.parent())
        {
            relativePos = Position();
            assert(_apiWidgetImpl.parent()->impl().nativeControl());
            TPoint parentPos(_apiWidgetImpl.parent()->impl().nativeControl()->AbsolutePosition());
            relativePos-=parentPos;
        }
        
        return relativePos;
    }
        
    const GraphicContext& BeginDraw()
    {       
        GraphicContext& graphicContext = WorkingContext();
        
        // no context active
        if (!graphicContext._gc)
        {            
            graphicContext._gc = &SystemGc();
            graphicContext._device = iEikonEnv->ScreenDevice();
            graphicContext._nativeFont = Pt::Gui::ApplicationImpl::_self->_symbApp->GetDocument().GetAppUi().Font();
            if (!graphicContext._drawingActive)
            {
                ActivateGc();
                Window().Invalidate(Rect());
                Window().BeginRedraw(Rect());                
            }
            graphicContext._gc->UseFont(_graphicContext._nativeFont);
            if (RootControl() != this)
            {
                TPoint p = AbsolutePosition();
                TRect rc(p, Rect().Size());
                graphicContext._gc->SetClippingRect(rc);
            }
            else
                graphicContext._gc->CancelClippingRect();
            graphicContext._references = 1;
        }
        else
        {
            graphicContext._references++;
        }
        
        return graphicContext;
    }
    
    void EndDraw()
    {
        GraphicContext& graphicContext = WorkingContext();

        if (graphicContext._references > 0)
        {
            graphicContext._references--;
            if (graphicContext._references == 0)
            {
                graphicContext._gc->DiscardFont();
                graphicContext._gc = 0;
                graphicContext._device = 0;
                graphicContext._nativeFont = 0;
                if (!graphicContext._drawingActive)
                {
                    Window().EndRedraw();
                    DeactivateGc();                    
                }
            }
        }
        else
        {
            assert(!graphicContext._gc);
        }
    }

    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
        // Make sure event is routed to children
        CCoeControl::HandlePointerEventL(aPointerEvent);

        CControl* root = RootControl();                
        if (root != this && root->_pointerEventConsumed)
            return;
        
        if ((root == this && !root->_pointerEventConsumed) || (root != this))
        {
            TranslateMouseEvent(aPointerEvent);
        }
        
        if (root != this)
            root->_pointerEventConsumed = true;
        else
            root->_pointerEventConsumed = false;
    }
            
    void AddControl(CControl* control)
    {
        _controls.push_back(control);
    }

    void RemoveControl(CControl* control)
    {
        size_t size = _controls.size();
        std::vector<CControl*>::iterator where;
        where = std::remove(_controls.begin(), _controls.end(), control);
        _controls.erase(where, _controls.end());
        assert(_controls.size() == size-1);
    }
    
protected:
    virtual CCoeControl* ComponentControl(TInt aIndex) const
    {
        return _controls.at(aIndex);
    }
    
    virtual TInt CountComponentControls() const
    {
        return _controls.size();
    }

    virtual void SizeChanged()
    {
        // If size has changed redraw our parent
        if (_allowParentRedrawing && _apiWidgetImpl.parent() && 
            _apiWidgetImpl.parent()->impl().nativeControl())
        {
            _apiWidgetImpl.parent()->impl().nativeControl()->DrawDeferred();
        }
    }
    
private:
    // Traverse widget hierarchy up to the root widget
    CControl* RootControl() const
    {
        // find root window
        Pt::Gui::Widget* parent = apiWidget().parent();
        Pt::Gui::Widget* this_ = &apiWidget();
        while (parent)
        {
            this_ = parent;
            parent = parent->parent();
        }        
        
        // this_ is root widget
        return this_->impl().nativeControl();
    }
    
    // When accessing the graphic context information
    // we always take it from the root widget
    GraphicContext& WorkingContext() const
    {
        return RootControl()->_graphicContext;        
    }

    /**
     * From CCoeControl,Draw.
     * @param Specified area for drawing
     */
    void Draw(const TRect& rect) const
    {
        GraphicContext& graphicContext = WorkingContext();
        
        graphicContext._drawingActive++;        
        
        TRect rc(rect);
        // if we're having a parent we need to adjust the update rectangle
        // to be located in 0/0
        TPoint absolutePos(AbsolutePosition());
        if (_apiWidgetImpl.parent())
            rc.Move(-absolutePos.iX, -absolutePos.iY);
        
        Pt::Gui::PaintEvent paintEvent(apiWidget(), 
                Pt::Gui::SymbianTools::makeRegion(rc)); 
        
        _apiWidgetImpl.dispatchEvent(paintEvent);

        graphicContext._drawingActive--;        
    }
    
    void TranslateMouseEvent(const TPointerEvent& aPointerEvent)
    {
        unsigned int modifiers = 0;

        Pt::Gui::MouseEvent::Button button;
        Pt::Gui::MouseEvent::Action action;

        TPoint pt = AbsolutePosition();
        int x = aPointerEvent.iPosition.iX - pt.iX;
        int y = aPointerEvent.iPosition.iY - pt.iY;

        switch (aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Down:
            button = Pt::Gui::MouseEvent::LeftButton;
            action = Pt::Gui::MouseEvent::Press;                
            break;

        case TPointerEvent::EButton1Up:
            button = Pt::Gui::MouseEvent::LeftButton;
            action = Pt::Gui::MouseEvent::Release;                
            break;
        default:
            return;
        }

        Pt::Gui::MouseEvent mouseEvent(apiWidget(), x, y, button, action, modifiers);

        _apiWidgetImpl.dispatchEvent(mouseEvent);        
    }
    
    Pt::Gui::Widget& apiWidget() const { return _apiWidgetImpl.apiWidget(); }

    Pt::Gui::WidgetImpl& _apiWidgetImpl;    
    RWindowGroup _windowGroup;
    mutable GraphicContext _graphicContext;
    bool _pointerEventConsumed;
    bool _allowParentRedrawing;
    
    std::vector<CControl*> _controls;
};

namespace Pt {

namespace Gui {

const ssize_t WidgetImpl::KUnused = std::numeric_limits<ssize_t>::max();

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at, const Math::Size& size)
: _apiWidget(apiWidget)
, _parent(parent)
, _initialLocation(at)
, _initialSize(size)
, _initialVisibility(true)
, _painter(*this)
, _control(0)
{
    // register widget
    ResourceRegistry::instance().registerWidget(this);
    
    // application instance is running, it's ok to construct the widget right now
    if (Pt::Gui::ApplicationImpl::_self && Pt::Gui::ApplicationImpl::_self->_symbApp->HasInitialized())
        construct();
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
    if (isConstructed())
        _control->MakeVisible(ETrue);
    else
        _initialVisibility = true;
}


void WidgetImpl::hide()
{
    if (isConstructed())
        _control->MakeVisible(EFalse);
    else
        _initialVisibility = false;
}

bool WidgetImpl::isVisible() const
{
    if (isConstructed())
        return _control->IsVisible() != EFalse;
   
    return _initialVisibility;
}

void WidgetImpl::repaint()
{    
    if (isConstructed())
        _control->DrawNow();
}

void WidgetImpl::setParent(Widget* parent)
{
    if (!isConstructed())
        return;
        
    Widget* currentParent = this->parent();
    
    // Do we have a parent widget?
    if (currentParent)
    {
        // Remove ourselves from parent
        CControl* parentControl = currentParent->impl().nativeControl();
        parentControl->RemoveControl(this->nativeControl());
        // We should update the old parent, the window server won't do it
        parentControl->DrawDeferred();
    }
    // we don't have a parent and we're going to have one
    else if (parent)
    {
        // TODO: Change window owning status
    }
    
    if (parent)
    {
        CControl* parentControl = parent->impl().nativeControl();
        parentControl->AddControl(this->nativeControl());
    }
    else
    {
        // TODO: Make it window owning
    }
}


void WidgetImpl::move(size_t x, size_t y)
{
    if (isConstructed())
    {
        if (_parent)
            _control->SetRelativePosition(TPoint(x, y));
        else
            _control->SetPosition(TPoint(x, y));
        synchronize(false);
    }
    else
    {
        _initialLocation = Pt::Math::Point(x, y);
    }
}


void WidgetImpl::resize(size_t width, size_t height)
{
    if (isConstructed())
    {
        _control->SetSize(TSize(width, height));
        synchronize(false);
    }
    else
    {
        _initialSize = Pt::Math::Size(width, height);
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
        CControl* control = new (ELeave)CControl(*this);

        Pt::Math::Point location(_initialLocation);
        Pt::Math::Size size(_initialSize);        
        
        if (_parent)
        {
            assert(_parent->impl().nativeControl());
            
            if (location.x() == KUnused)
                location.setX(0);
            if (location.y() == KUnused)
                location.setY(0);        
            if ((ssize_t)size.width() == KUnused)
                size.setWidth(_parent->impl().nativeControl()->Size().iWidth);
            if ((ssize_t)size.height() == KUnused)
                size.setHeight(_parent->impl().nativeControl()->Size().iHeight);            

            int x = location.x() + _parent->impl().nativeControl()->AbsolutePosition().iX;
            int y = location.y() + _parent->impl().nativeControl()->AbsolutePosition().iY;
            
            location.set(x, y);            
        }
        else
        {
            if (location.x() == KUnused)
                location.setX(ui.ClientRect().iTl.iX);
            if (location.y() == KUnused)
                location.setY(ui.ClientRect().iTl.iY);        
            if ((ssize_t)size.width() == KUnused)
                size.setWidth(ui.ClientRect().Width());
            if ((ssize_t)size.height() == KUnused)
                size.setHeight(ui.ClientRect().Height());            
        }
                
        control->ConstructL(SymbianTools::makeTRect(location, size));
        // TODO: Handle leave
        if (!_parent)
            ui.AddToStackL(control);
        
        control->SetMopParent(&ui);
        control->MakeVisible(_initialVisibility ? ETrue : EFalse);
        _control = control;    
        
        synchronize(true);
        
        _control->SetAllowParentRedrawing(true);
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

    TPoint offset(0, 0);
    if (isConstructed())
        offset = _control->AbsolutePosition();
    
    _painter.setOffset(offset);
}

void WidgetImpl::endDraw()
{
    _painter.setGc(0);    
    _painter.setNativeFont(0);
    _painter.setDevice(0);
    _painter.setOffset(TPoint(0,0));

    if (!isConstructed())
        return;

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
        TPoint pos = _parent ? _control->RelativePosition() : _control->Position();
        
        MoveEvent moveEvent(_apiWidget, pos.iX, pos.iY);
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
