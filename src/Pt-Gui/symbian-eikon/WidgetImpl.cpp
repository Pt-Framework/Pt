/*
 * Copyright (C) 2008 Marc Boris Duerner
 * Copyright (C) 2008 Peter Barth
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "WidgetImpl.h"

#include <limits>
#include <iostream>
#include <assert.h>

#include <Pt/Gui/Widget.h>
#include <Pt/Gui/MouseEvent.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/CloseEvent.h>
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Gui/MoveEvent.h>
#include <Pt/Gui/MouseMoveEvent.h>
#include <Pt/Gfx/Region.h>

#include "ApplicationImpl.h"
#include "SymbAppUi.h"
#include "SymbianTools.h"

// symbian APIs
#include <eikenv.h>
#include <coecntrl.h>
#include <w32std.h>

/**
 * @brief This is the symbian backend control class.
 * It represents functionality to be either a window owning (top level) control
 * or a lodging control which is nested into another control.
 *
 * When speaking of the "root" control the top level control which owns the
 * window is meant.
 *
 * <b>Note</b> that a nested control still uses the graphic context of the root
 * window owning control. If you force redraw the content of the root window
 * without redrawing the children you are likely to overdraw the nested controls.
 *
 * Also accessing controls is NOT thread-safe. A Panic occurs when trying to do so.
 */
class CControl : public CCoeControl
{
public:
    /**
     * @brief Class to hold graphic context information used to begin
     * drawing into the window or a control.
     */
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

        // Context to draw to
        CGraphicsContext* _gc;
        // Device that holds the context
        CGraphicsDevice* _device;
        // The default font which will be activated for this context
        const CFont* _nativeFont;
        // References of Draw() in current redraw callstack
        int _drawingActive;
        // References to current open context
        int _references;
        TRect _clipRect;
    };

    /**
     * @brief First phase constructor.
     *
     * @param owner Widget implementation which owns this control.
     * @param defaultFont Reference to an existing default font.
     */
    CControl(Pt::Gui::WidgetImpl& owner, const CFont& defaultFont)
    : _apiWidgetImpl(owner)
    , _defaultFont(defaultFont)
    , _pointerEventConsumed(false)
    , _allowParentRedrawing(false)
    {
    }

    /**
     * @brief Second phase constructor.
     *
     * @param rect Construction rectangle.
     */
    void ConstructL(const TRect& rect)
    {
        ReparentL(false);

        SetRect(rect);
        EnableDragEvents();
        ActivateL();
    }

    /**
     * @brief Regular destructor.
     */
    virtual ~CControl()
    {
        if (!_apiWidgetImpl.parent())
            _windowGroup.Close();
    }

    /**
     * @brief Release the backend window if we are a window owning control.
     */
    void ReleaseWindow()
    {
        assert(OwnsWindow() != EFalse);
        _windowGroup.Close();
        CloseWindow();
    }

    /**
     * @brief Call this if the parent of this control has changed.
     * Also Recursively adjust window references in case of nesting.
     *
     * @param readjustChildren A flag to indicate whether to adjust the children or not.
     */
    void ReparentL(bool readjustChildren = true)
    {
        // no parent? We're becoming a window owning control
        if (!_apiWidgetImpl.parent())
        {
            _windowGroup = RWindowGroup(iCoeEnv->WsSession());

            User::LeaveIfError(_windowGroup.Construct((TUint32)&_windowGroup));

            _windowGroup.SetOrdinalPosition(0, /*ECoeWinPriorityAlwaysAtFront*/ECoeWinPriorityHigh);
            _windowGroup.EnableReceiptOfFocus(EFalse);

            // we're becoming a window-owning control now
            CreateWindowL(&_windowGroup);

            // All children need to adjust their window
            if (readjustChildren)
            {
                std::vector<CControl*>::iterator it;
                for (it = _controls.begin(); it != _controls.end(); ++it)
                    (*it)->ReadjustWindowOwning();
            }
        }
        else
        {
            // get parent native control
            Pt::Gui::WidgetImpl& impl = _apiWidgetImpl.parent()->impl();
            CControl* parentControl = impl.nativeControl();
            // add ourselves to parent
            parentControl->AddControl(this);

            // All children need to adjust their window
            if (readjustChildren)
                ReadjustWindowOwning();
            else
            {
                SetContainerWindowL(*parentControl);
            }
        }
    }

    /**
     * @brief Setting this flag will allow the control to redraw its parent
     * in case of a size change.
     */
    void SetAllowParentRedrawing(bool allowParentRedrawing) { _allowParentRedrawing = allowParentRedrawing; }

    /**
     * @brief Get absolute position of widget within parent widget.
     * If the control is a window owning control the returned position will be
     * at the origin.
     */
    TPoint AbsolutePosition() const
    {
        return _apiWidgetImpl.parent() ? Position() : TPoint(0,0);
    }

    /**
     * @brief Set relative position of widget within parent widget.
     * Don't call this function if there is no parent, it will not work.
     * <b>Note</b> that this function will not work in a recursive fashion i.e.
     * children positions are unaffected.
     *
     * @param position New position.
     */
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

    /**
     * @brief Get relative position of widget within parent widget.
     */
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

    /**
     * @brief Start drawing to this control.
     * This function will return all context information that is necessary
     * when control is about being drawn into.
     */
    const GraphicContext& BeginDraw()
    {
        GraphicContext& graphicContext = WorkingContext();

        // no context active
        if (!graphicContext._gc)
        {
            // the window handle of the root window needs to be the same
            // with the current oen, otherwise something went wrong
            assert(&Window() == &RootControl()->Window());

            graphicContext._gc = &SystemGc();
            graphicContext._device = iEikonEnv->ScreenDevice();
            graphicContext._nativeFont = &_defaultFont;

            // we're not invoked out of Draw()
            // we need to activate the context
            if (!graphicContext._drawingActive)
            {
                ActivateGc();
                Window().Invalidate(Rect());
                Window().BeginRedraw(Rect());
            }

            graphicContext._gc->UseFont(_graphicContext._nativeFont);

            // if we're not the window owning control (root)
            // we setup a clipping rectangle too
            if (RootControl() != this)
            {
                TPoint p = AbsolutePosition();
                TRect rc(p, Rect().Size());
                rc.iBr.iX++;
                rc.iBr.iY++;
                graphicContext._clipRect = rc;
                graphicContext._gc->SetClippingRect(rc);
            }
            else
            {
                graphicContext._clipRect = TRect(0, 0, 0, 0);
                graphicContext._gc->CancelClippingRect();
            }

            graphicContext._references = 1;
        }
        else
        {
            graphicContext._references++;
        }

        return graphicContext;
    }

    /**
     * @brief Call this when you are finished with drawing into the control.
     * All context information will be cleaned up.
     */
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
                // not invoked out of Draw()
                // deactivate context
                if (!graphicContext._drawingActive)
                {
                    Window().EndRedraw();
                    DeactivateGc();

                    // whenever the window content has been accessed we redraw the children
                    // to make sure they're still visible and nobody has drawn over them.
                    // Must only be done when we were not invoked from the Draw() method
                    // because Draw() will ensure the children are properly redrawn.
                    for (int i = 0; i < CountComponentControls(); i++)
                    {
                        ComponentControl(i)->DrawDeferred();
                    }
                }
            }
        }
        else
        {
            assert(!graphicContext._gc);
        }

        // TODO: whenever the client area has changed we redraw the children
        // to make sure they're visible
        //for (int i = 0; i < CountComponentControls(); i++)
        //    ComponentControl(i)->DrawDeferred();
    }

    /**
     * @brief Deal with "mouse" events.
     * Don't forget to call CCoeControl::HandlePointerEventL otherwise
     * the event is not delivered to the children.
     */
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
        // start
        if (RootControl() == this)
            _pointerEventConsumed = false;

        // Make sure event is routed to children
        CCoeControl::HandlePointerEventL(aPointerEvent);

        CControl* root = RootControl();
        // pointer event has been consumed
        if (root != this && root->_pointerEventConsumed)
            return;

        // we're the root window and pointer event has not been consumed
        // or we're not the root window
        // => dispatch pointer events
        if ((root == this && !root->_pointerEventConsumed) || (root != this))
        {
            TranslateMouseEvent(aPointerEvent);
        }

        // we're not the root window, mark pointer event as consumed
        if (root != this)
            root->_pointerEventConsumed = true;
        else
            root->_pointerEventConsumed = false;
    }

    /**
     * @brief Add a child control.
     */
    void AddControl(CControl* control)
    {
        _controls.push_back(control);
    }

    /**
     * @brief Remove a child control.
     * Note that removing a child will not cause the window reference to be released.
     */
    void RemoveControl(CControl* control)
    {
        size_t size = _controls.size();
        std::vector<CControl*>::iterator where;
        where = std::remove(_controls.begin(), _controls.end(), control);
        _controls.erase(where, _controls.end());
        assert(_controls.size() == size-1);
    }

    /**
     * @brief Recursively adjust the relative positions of a control.
     *
     * @param childrenOnly Flag to start with the children and to ignore the current position.
     */
    void ReadjustRelativePosition(bool childrenOnly)
    {
        if (!childrenOnly && _apiWidgetImpl.parent())
        {
            Pt::Gui::WidgetImpl& impl = _apiWidgetImpl.parent()->impl();
            CControl* parentControl = impl.nativeControl();
            // this is our container
            TPoint pos = parentControl->AbsolutePosition();

            pos.iX+=apiWidget().region().topLeft().x();
            pos.iY+=apiWidget().region().topLeft().y();

            SetPosition(pos);
        }

        std::vector<CControl*>::iterator it;
        for (it = _controls.begin(); it != _controls.end(); ++it)
            (*it)->ReadjustRelativePosition(false);
    }

protected:
    /**
     * @brief From CCoeControl: Return a nested control.
     * @see CountComponentControls
     *
     * @param aIndex Index to control.
     */
    virtual CCoeControl* ComponentControl(TInt aIndex) const
    {
        return _controls.at(aIndex);
    }

    /**
     * @brief From CCoeControl: Return the number of nested controls.
     * @see ComponentControl
     */
    virtual TInt CountComponentControls() const
    {
        return _controls.size();
    }

    /**
     * @brief From CCoeControl: Is called when the size of this control is changed.
     */
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
    /**
     * @brief Get the root control of this control
     * The root is the control which is the top level control which owns the window.
     * => Traverse widget hierarchy up to the root widget.
     */
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

    /**
     * @brief Provide access to the context information of the root control.
     * When accessing the graphic context information we always take it
     * from the root widget.
     * @see BeginDraw
     * @see RootControl
     */
    GraphicContext& WorkingContext() const
    {
        return RootControl()->_graphicContext;
    }

    /**
     * @brief From CCoeControl: Draw control content.
     * @param rect Specified area for drawing.
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

    /**
     * @brief Internally used to translate a symbian pointer event
     * into a Pt event and dispatch it.
     *
     * @param aPointerEvent Symbian pointer event.
     */
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
            {
                button = Pt::Gui::MouseEvent::LeftButton;
                action = Pt::Gui::MouseEvent::Press;
                Pt::Gui::MouseEvent mouseEvent(
                        apiWidget(), x, y,
                        button, action, modifiers
                        );
                _apiWidgetImpl.dispatchEvent(mouseEvent);
                break;
            }

            case TPointerEvent::EButton1Up:
            {
                button = Pt::Gui::MouseEvent::LeftButton;
                action = Pt::Gui::MouseEvent::Release;
                Pt::Gui::MouseEvent mouseEvent(apiWidget(), x, y, button, action, modifiers);
                _apiWidgetImpl.dispatchEvent(mouseEvent);
                break;
            }

            case TPointerEvent::EDrag:
            {
                Pt::Gui::MouseMoveEvent mouseEvent(
                        apiWidget(), x, y,
                        Pt::Gui::MouseMoveEvent::Moved,
                        Pt::Gui::MouseMoveEvent::LeftButtonDown
                        );
                _apiWidgetImpl.dispatchEvent(mouseEvent);
                break;
            }

            case TPointerEvent::EMove:
            {
                Pt::Gui::MouseMoveEvent mouseEvent(
                        apiWidget(), x, y,
                        Pt::Gui::MouseMoveEvent::Moved,
                        0
                        );
                _apiWidgetImpl.dispatchEvent(mouseEvent);
                break;
            }

            default:
                // TODO: Handle more mouse events
                return;
        }

    }

    /**
     * @brief Adjust window owning of nested control.
     */
    void ReadjustWindowOwning()
    {
        Pt::Gui::WidgetImpl& impl = _apiWidgetImpl.parent()->impl();
        CControl* parentControl = impl.nativeControl();
        // this is our container
        SetContainerWindowL(*parentControl);

        std::vector<CControl*>::iterator it;
        for (it = _controls.begin(); it != _controls.end(); ++it)
            (*it)->ReadjustWindowOwning();
    }

    /**
     * @brief Provide access to the ApiWidget.
     */
    Pt::Gui::Widget& apiWidget() const { return _apiWidgetImpl.apiWidget(); }

    Pt::Gui::WidgetImpl& _apiWidgetImpl;
    RWindowGroup _windowGroup;
    const CFont& _defaultFont;
    mutable GraphicContext _graphicContext;
    bool _pointerEventConsumed;
    bool _allowParentRedrawing;

    std::vector<CControl*> _controls;
};

namespace Pt {

namespace Gui {

const ssize_t WidgetImpl::KUnused = std::numeric_limits<ssize_t>::max();

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Gfx::Point& at, const Gfx::Size& size)
: _apiWidget(apiWidget)
, _parent(parent)
, _initialLocation(at)
, _initialSize(size)
, _initialVisibility(true)
, _painter(*this)
, _control(0)
{
    // register widget
    Environment::instance().registerResource(this);
    construct();
}


WidgetImpl::~WidgetImpl()
{
    destruct();
    Environment::instance().unregisterResource(this);
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
    // We don't have a parent widget but we're going to have one
    else if (parent)
    {
        _control->ReleaseWindow();
    }

    _parent = parent;
    TRAPD(errorCode, _control->ReparentL());
    if (errorCode != KErrNone)
        throw std::runtime_error("Could not set new parent." + PT_SOURCEINFO);

    _control->ReadjustRelativePosition(false);
    _control->DrawDeferred();
}


void WidgetImpl::move(size_t x, size_t y)
{
    if (isConstructed())
    {
        // since nested controls are positioned in an absolute
        // fashion we need to make a difference between
        // child controls and top level windows here
        if (_parent)
            _control->SetRelativePosition(TPoint(x, y));
        else
            _control->SetPosition(TPoint(x, y));

        // adjust all children, but only children, as we already adjusted
        // our own position
        _control->ReadjustRelativePosition(true);

        synchronize(false);
    }
    else
    {
        _initialLocation = Pt::Gfx::Point(x, y);
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
        _initialSize = Pt::Gfx::Size(width, height);
    }
}

void WidgetImpl::construct()
{
    if (_control)
        destruct();

    CSymbAppUi& ui = Pt::Gui::Environment::instance().symbAppUi();

    CControl* control = 0;
    TRAPD(errorCode, control = new (ELeave)CControl(*this, ui.Font()));
    if (errorCode != KErrNone)
        throw std::bad_alloc();

    Pt::Gfx::Point location(_initialLocation);
    Pt::Gfx::Size size(_initialSize);

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

    TRAP(errorCode, control->ConstructL(SymbianTools::makeTRect(location, size)));
    if (errorCode != KErrNone)
        throw std::runtime_error("Widget control creation failed." + PT_SOURCEINFO);

    // Only top level windows are added to the control stack
    if (!_parent)
    {
        TRAP(errorCode, ui.AddToStackL(control));
        if (errorCode != KErrNone)
            throw std::runtime_error("Failed to add control to control stack." + PT_SOURCEINFO);
    }

    control->SetMopParent(&ui);
    control->MakeVisible(_initialVisibility ? ETrue : EFalse);
    _control = control;

    synchronize(true);

    _control->SetAllowParentRedrawing(true);
}

void WidgetImpl::destruct()
{
    if (_control)
    {
        CSymbAppUi& ui = Pt::Gui::Environment::instance().symbAppUi();
        // it doesn't matter whether we have been added to the stack or not
        ui.RemoveFromStack(_control);
    }

    if (_control)
    {
        delete _control;
        _control = 0;
    }

    _painter.destructResources();
}

void WidgetImpl::dispatchEvent(Pt::Event& event)
{
    Pt::Gui::Environment::instance().dispatchEvent(event);
}

PainterImpl::ContextInfo WidgetImpl::beginDraw()
{
    PainterImpl::ContextInfo contextInfo;

    if (!isConstructed())
        return contextInfo;

    const CControl::GraphicContext& graphicContext = _control->BeginDraw();

    contextInfo._gc = graphicContext._gc;
    contextInfo._device = graphicContext._device;
    contextInfo._nativeFont = graphicContext._nativeFont;
    contextInfo._coeEnv = CEikonEnv::Static();
    contextInfo._offset = _control->AbsolutePosition();
    contextInfo._clipRect = graphicContext._clipRect;

    return contextInfo;
}

void WidgetImpl::endDraw()
{
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
