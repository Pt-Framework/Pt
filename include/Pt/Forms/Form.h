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

#ifndef PT_FORMS_FORM_H
#define PT_FORMS_FORM_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Forms/Control.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////
// TODO: move base functionality to Widget API
//
//       where to align
//       when is invalidate, relayout, repaint called
//       some onXYZRequested handlers in Widget
//       
///////////////////////////////////////////////////////////////////////

//
// TODO:
//  - alignment for move/resize of window
//  - WindowImpl for native and framework windows
//  - set Decorator on Window to translate positions
//

class PT_FORMS_API Form : public View
{
    friend class Control;

    typedef View Base;

    protected:
        Form();

    public:
        virtual ~Form();

    public:
        Control* content();

        const Control* content()  const;

        void setContent(Control* control);       


        //Gfx::PaintSurface& surface();

        //const Gfx::PaintSurface& surface() const;

        //void setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos);

    //
    // layouting
    //
    public:
        Gfx::SizeF measure(const SizePolicy& policy);

        void relayout();

    protected:
        virtual void onRequestRelayout();

        virtual void onProcessLayoutEvent(const LayoutEvent& ev);

        virtual Gfx::SizeF onProcessMeasure();

        virtual void onProcessLayout(const Gfx::RectF& rect);
        
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

    //
    // focus handling
    // 
    public:
        Control* focusControl();

        void focusNext();

        void focusPrev();

    protected:
        virtual void onAddElement(Control& control);

        virtual void onRemoveElement(Control& control);

        virtual void onSetFocusPolicy(Control& control, FocusPolicy policy);

        virtual void onSetFocusIndex(Control& control, unsigned index);

        virtual void onSetFocus(Control& control);

        virtual void onSetShortcut(Control& control, const std::vector<Key>& keys);

        virtual void onSetMnemonic(Control& control, const std::vector<Char>& chs);
    
    //
    // View
    //
    protected:
        virtual void onSetSurface(PaintSurface* surface, const Gfx::PointF& pos);

        virtual void onAttach(Control& control);

        virtual void onDetach(Control& control);

        virtual void onInit(Control& control);

        virtual void onRelease(Control& control);

        virtual Gfx::PointF onToControl(const Control& control, 
                                        const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromControl(const Control& control, 
                                          const Gfx::PointF& pos) const;

    protected:
        virtual void onRepaintRequest(Control& control, const Gfx::RectF& rect);

        virtual void onRelayoutRequest(Control& control);

        virtual void onEnableRequest(Control& control, bool isEnable);

        virtual void onActivateRequest(Control& control, bool active);

        virtual void onShowRequest(Control& control, bool isShown);

        virtual void onMoveRequest(Control& control, const Gfx::PointF& pos);

        virtual void onResizeRequest(Control& control, const Gfx::SizeF& size);

        virtual void onRaiseRequest(Control& control);

    //
    // Widget
    //
    protected:
        virtual void onConnect(Screen& screen);

        virtual void onDisconnect();

        
        virtual Widget* onHitTest(const Gfx::PointF& pos);

        virtual void onRequestCapture(bool capture);

        virtual void onRequestResize(const Gfx::SizeF& s);


    protected:
        virtual void onProcessEvent(const Pt::Event& ev);

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

    //
    // scaling
    //
    protected:
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onRescale(double scaling);

    //
    // enabling
    //
    protected:
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        virtual void onEnableEvent(const EnableEvent& ev);

        virtual void onEnable(bool e);

    //
    // visibility
    //
    protected:
        virtual void onProcessShowEvent(const ShowEvent& ev);

        virtual void onShowEvent(const ShowEvent& ev);

        virtual void onShow(bool visible);

    //
    // geometry
    //
    protected:
        virtual void onProcessMoveEvent(const MoveEvent& ev);

        virtual void onMoveEvent(const MoveEvent& ev);

        virtual void onProcessResizeEvent(const ResizeEvent& ev);
        
        virtual void onResizeEvent(const ResizeEvent& ev);
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

    //
    // Responder
    //
    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);
        
        virtual bool onTouchEvent(const TouchEvent& ev);
        
        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onEnterEvent(const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);
    
    private:
        template <typename Iter>
        void moveFocus(Iter begin, Iter end);
        
    protected:
        const std::map<Key, Control*>& shortcuts() const
        {
            return _shortcuts;
        }

        const std::map<Pt::Char, Control*>& mnemonics() const
        {
            return _mnemonics;
        }

    private:
        Control*                      _mainControl;
                                     
        int                           _layouts;

        Control*                      _active;

        std::vector<Control*>         _focusList;
        Control*                      _focusControl;        
        std::map<Key, Control*>       _shortcuts;
        std::map<Pt::Char, Control*>  _mnemonics;

};

} // namespace

} // namespace

#endif // include guard
