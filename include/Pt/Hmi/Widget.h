/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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

#ifndef PT_HMI_WIDGET_H
#define PT_HMI_WIDGET_H

#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/SizePolicy.h>
#include <Pt/Hmi/Spacing.h>

#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/TouchEvent.h>
#include <Pt/Hmi/ScrollEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/EnterEvent.h>
#include <Pt/Hmi/LeaveEvent.h>
#include <Pt/Hmi/EnableEvent.h>
#include <Pt/Hmi/InvalidateEvent.h>
#include <Pt/Hmi/LayoutEvent.h>
#include <Pt/Hmi/ShowEvent.h>
#include <Pt/Hmi/FocusEvent.h>

#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/PaintRegion.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Color.h>

#include <Pt/Signal.h>
#include <Pt/Delegate.h>
#include <vector>

namespace Pt {

namespace Hmi {

class Form;
class Key;

class PT_HMI_API Widget : public View
{
    typedef View Base;

    friend class Form;

    public:
        Widget();

        virtual ~Widget();

    public:
        void setParent(View& parent);

        void unparent();

        // implement add method in derived class
        void add(Widget& widget);

        // implement remove method in derived class
        void remove(Widget& widget);

        const std::vector<Widget*>& widgets() const;

    public:
        Gfx::PaintSurface& surface();

        const Gfx::PaintSurface& surface() const;

        void setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos);

    private:
        void setForm(Form* form);

    public:
        //
        // focus handling
        // 
        FocusPolicy focusPolicy() const;

        void setFocusPolicy(FocusPolicy f);

        size_t focusIndex() const;

        void setFocusIndex(size_t index);  

        bool hasFocus() const;

        void focus();

        //
        // keyboard input
        //
        Key actionKey() const;

        void setActionKey(const Key& ak);

        const Key* shortcut() const;

        void setShortcut(const Key* k);

        const Pt::Char* mnemonic() const; 

        void setMnemonic(const Char& ch);

        String setMnemonic(const String& text);

        void setMnemonicWidget(Widget* w);

        void processShortcut(const Key& key);

        void processMnemonic(Pt::Char m);

      public:
        // TODO: obsolete when processEvent return true/false if consumed
        bool acceptsInput() const;


        void raise();

    //
    // invalidation
    // 
    protected:
        virtual void onInvalidateEvent(const InvalidateEvent& ev);
    
        virtual void onInvalidate();

    //
    // painting
    //  
    protected:
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);
        
        virtual void onPaint(Gfx::PaintSurface& surface, 
                             const Gfx::RectF& rect);

    //
    // layouting
    //
    public:
        const Gfx::RectF geometry() const;

        const SizePolicy& sizePolicy() const;

        void setSizePolicy(const SizePolicy& policy);

        Gfx::SizeF preferredSize() const;

        Gfx::SizeF measure(const SizePolicy& policy);

        void relayout();

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);
       
        virtual void onProcessLayoutEvent(const LayoutEvent& ev);

        virtual void onLayoutEvent(const LayoutEvent& ev);

        virtual void onLayout(const Gfx::RectF& rect);

    public:
        // outer spacing
        const Spacing& margin() const;

        // outer spacing
        void setMargin(const Spacing& s);

        // outer spacing
        void setMargin(double n);

        // inner spacing
        void setMargin(double horiz, double vertical);

        // inner spacing
        const Spacing& padding() const;

        // inner spacing
        void setPadding(const Spacing& p);

        // inner spacing
        void setPadding(double n);

        // inner spacing
        void setPadding(double horiz, double vertical);

    protected:
        virtual void onSetSizeLimits(const Gfx::SizeF& minSize,
                                     const Gfx::SizeF& maxSize);

    protected:
        virtual void onSetSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos);

        virtual void onAddWidget(Widget& w);

        virtual void onRemoveWidget(Widget& w);


        virtual void onActionKey(const KeyEvent& kev);

        virtual void onShortcut(const Key& kev);

        virtual void onMnemonic(Pt::Char m);

    //
    // Visual
    //
    protected:
        virtual Visual* onHitTest(const Gfx::PointF& p);

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;
        
        
        virtual void onRequestRepaint(const Gfx::RectF& rect);

        virtual void onRequestShow(bool isShown);

        virtual void onRequestEnable(bool isEnable);

        virtual void onRequestActivate(bool active);

        virtual void onRequestMove(const Gfx::PointF& pos);

        virtual void onRequestResize(const Gfx::SizeF& s);

        
        virtual void onProcessEvent(const Pt::Event& ev);


        virtual void onProcessEnableEvent(const EnableEvent& ev);

        virtual void onEnableEvent(const EnableEvent& ev);
        
        virtual void onEnable(bool isEnable);


        virtual void onProcessShowEvent(const ShowEvent& ev);

        virtual void onShowEvent(const ShowEvent& ev);

        virtual void onShow(bool visible);


        virtual void onProcessFocusEvent(const FocusEvent& ev);

        virtual void onFocusEvent(const FocusEvent& ev);


        virtual void onProcessRescaleEvent(const RescaleEvent& ev);
        
        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onRescale(double scaling);


        virtual void onProcessMoveEvent(const MoveEvent& ev);

        virtual void onMoveEvent(const MoveEvent& ev);


        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);


        virtual void onProcessMouseEvent(const MouseEvent& ev);

        virtual void onProcessTouchEvent(const TouchEvent& ev);

        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        virtual void onProcessEnterEvent(const EnterEvent& ev);

        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        virtual void onProcessKeyEvent(const KeyEvent& ev);

    //
    // Responder
    //
    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent( const ScrollEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

        virtual bool onEnterEvent( const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

    //
    // View
    //
    protected:
        virtual Gfx::PointF onToWidget(const Widget& widget, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromWidget(const Widget& widget, 
                                         const Gfx::PointF& pos) const;

        virtual void onAttach(Widget& widget);

        virtual void onDetach(Widget& widget);

        virtual void onInit(Widget& widget);

        virtual void onRelease(Widget& widget);

    protected:
        virtual void onRepaintRequest(Widget& widget, const Gfx::RectF& rect);

        virtual void onRelayoutRequest(Widget& widget);

        virtual void onEnableRequest(Widget& widget, bool isEnable);

        virtual void onActivateRequest(Widget& w, bool active);

        virtual void onShowRequest(Widget& widget, bool isShown);

        virtual void onMoveRequest(Widget& widget, const Gfx::PointF& pos);

        virtual void onResizeRequest(Widget& widget, const Gfx::SizeF& size);

        virtual void onRaiseRequest(Widget& w);

        virtual const std::vector<Key> onGetShortcuts();

        virtual const std::vector<Char> onGetMnemonics();

    private:
        Gfx::PaintRegion             _surface;

        View*                        _parent;
        std::vector<Widget*>         _children;

        Form*                        _form;

        bool                         _isCapture;
        bool                         _isLayoutInvalid;

        bool                         _show;
        bool                         _enabled;

        Gfx::PointF                  _requestedPosition;
        Gfx::SizeF                   _requestedSize;

        bool                         _isMeasureInvalid;
                                    
        SizePolicy                   _sizePolicy;
        SizePolicy                   _lastPolicy;
        Gfx::SizeF                   _preferredSize;

        bool                         _hasFocus;
        FocusPolicy                  _focusPolicy;
        size_t                       _focusIndex;

        Key                          _actionKey;
        Key                          _shortcutKey;
        Pt::Char                     _mnemonic;
        Pt::Signal<Pt::Char>         _mnemonicEntered;

        Spacing                      _padding;
        Spacing                      _margin;
};

} // namespace

} // namespace

#endif // PT_HMI_WIDGET_H

