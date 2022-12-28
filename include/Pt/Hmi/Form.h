/* Copyright (C) 2022 Marc Boris Duerner 
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef PT_HMI_FORM_H
#define PT_HMI_FORM_H

#include <Pt/Hmi/Visual.h>
#include <Pt/Gfx/PaintRegion.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>

#include <vector>
#include <map>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////
// TODO: move base functionality to Visual API
//       surface in Form
//       responder in Form or Window
///////////////////////////////////////////////////////////////////////

//
// TODO:
//  - alignment for move/resize of window
//  - WindowImpl for native and framework windows
//  - set Decorator on Window to translate positions
//

class PT_HMI_API Form : public View
                      , public Pt::Connectable
{
    friend class Widget;

    public:
        Form();

        virtual ~Form();
    
        void setSurface(Gfx::PaintSurface* surface);

        void setNextResponder(Responder* r);

    public:
        Widget* content();

        const Widget* content()  const;

        void setContent(Widget* widget);

        Widget* findWidget(const Gfx::PointF& pos);

        Widget* findWidget(const std::string& name);

        Widget* findWidget(Pt::uint64_t vid);

    protected:
        virtual void onInvalidateRequest();

    public:
        void relayout();

        Gfx::SizeF measure(const SizePolicy& policy);

        void layout(const Gfx::RectF& rect);

    public:
        //bool acceptsInput() const;

        //bool isEnabled() const;

    public:
        //const Gfx::RectF& geometry() const;

        //const Gfx::RectF& bounds() const;



        //const Gfx::PointF& position() const;

        //void move(const Gfx::PointF&);

        //const Gfx::SizeF& size() const;

        //void resize(const Gfx::SizeF& s);

    public:
        Widget* focusWidget();

        void focusNext();

        void focusPrev();

    //
    // Visual
    //
    protected:
        //virtual Visual* onGetParent() const;

        virtual Visual* onHitTest(const Gfx::PointF& pos);

        //virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        //virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual void onEvent(const Pt::Event& ev);

        virtual void onRelease();

    //
    // Responder
    //
    protected:
        virtual Responder* onNextResponder();

        virtual bool onMouseEvent(const MouseEvent& ev);
        
        virtual bool onTouchEvent(const TouchEvent& ev);
        
        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onEnterEvent(const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

    //
    // View
    //
    protected:
        virtual void onAttach(Widget& widget);

        virtual void onDetach(Widget& widget);

        virtual void onInit(Widget& widget);

        virtual void onRelease(Widget& widget);

        virtual void onRegister(Widget& widget);

        virtual void onDeregister(Widget& widget);

        virtual Gfx::PointF onToWidget(const Widget& widget, 
                                        const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromWidget(const Widget& widget, 
                                          const Gfx::PointF& pos) const;

        virtual void onRepaint(Widget& widget, const Gfx::RectF& rect);

        virtual void onRelayout(Widget& widget);

        virtual void onEnable(Widget& widget, bool isEnable);

        virtual void onActivate(Widget& w, bool active);

        virtual void onShow(Widget& widget, bool isShown);

        virtual void onMove(Widget& widget, const Gfx::PointF& pos);

        virtual void onResize(Widget& widget, const Gfx::SizeF& size);

        virtual void onRaise(Widget& widget);

    //
    // Form
    //
    protected:
        virtual void onSetFocusPolicy(Widget& w, FocusPolicy policy);

        virtual void onSetFocusIndex(Widget& w, unsigned index);

        virtual void onSetFocus(Widget& w);

        virtual void onSetShortcut(Widget& w, const Key* key);

        virtual void onSetMnemonic(Widget& w, const Char* ch);

    //
    // invalidation
    //
    protected:
        virtual void onProcessInvalidateEvent(const InvalidateEvent& ev);

        virtual void onInvalidateEvent(const InvalidateEvent& ev);
    
        virtual void onInvalidate();

    //
    // painting
    //
    protected:
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);

    //
    // layouting
    //
    protected:
        virtual void onProcessRelayoutEvent(const RelayoutEvent& ev);
    
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

    //
    // scaling
    //
    protected:
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onRescale(double scaling);
    
    //
    // geometry
    //
    protected:
        //virtual void onProcessMoveEvent(const MoveEvent& ev);

        virtual void onMoveEvent(const MoveEvent& ev);

        //virtual void onProcessResizeEvent(const ResizeEvent& ev);
        
        virtual void onResizeEvent(const ResizeEvent& ev);

    //
    // enabling
    //
    protected:
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        virtual void onEnableEvent(const EnableEvent& ev);

        virtual void onEnable(bool e);
    
    //
    // input
    //
    protected:
        virtual void onProcessMouseEvent(const MouseEvent& ev);
        
        virtual void onProcessTouchEvent(const TouchEvent& ev);

        virtual void onProcessScrollEvent(const ScrollEvent& sev);

        virtual void onProcessEnterEvent(const EnterEvent& ev);

        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        virtual void onProcessKeyEvent(const KeyEvent& ev);

    private:
        template <typename Iter>
        void moveFocus(Iter begin, Iter end);
    
    private:
        Pt::Signal<const Pt::Event&> _eventReceived;
        Widget*                      _mainWidget;

        Gfx::PaintSurface*           _surface;
        Responder*                   _nextResponder;

        int                          _invalidates;
        int                          _layouts;

        bool                         _enabled;
        bool                         _enabledState;

        //Gfx::RectF                   _alignedGeometry;
        //Gfx::RectF                   _bounds;
        //Gfx::RectF                   _requestedGeometry;
        
        Visual*                      _pointer;
        Widget*                      _active;

        std::vector<Widget*>         _focusList;
        Widget*                      _focusWidget;

        std::map<Key, Widget*>       _shortcuts;
        std::map<Pt::Char, Widget*>  _mnemonics;
};

} // namespace

} // namespace

#endif // include guard
