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

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////
// TODO: move base functionality to Visual API
//
//       where to align
//       when is invalidate, relayout, repaint called
//       some onXYZRequested handlers in Visual
//       
///////////////////////////////////////////////////////////////////////

//
// TODO:
//  - alignment for move/resize of window
//  - WindowImpl for native and framework windows
//  - set Decorator on Window to translate positions
//

class PT_HMI_API Form : public View
{
    friend class Widget;

    typedef View Base;

    public:
        virtual ~Form();

    public:
        Widget* content();

        const Widget* content()  const;

        void setContent(Widget* widget);       


        Gfx::PaintSurface& surface();

        const Gfx::PaintSurface& surface() const;

        void setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos);

    public:
        Widget* focusWidget();

        void focusNext();

        void focusPrev();

    protected:
        Form();

        void relayout();

    protected:
        virtual Gfx::SizeF onMeasure();

        virtual void onProcessLayoutEvent(const LayoutEvent& ev);
        
        virtual void onLayoutEvent(const LayoutEvent& ev);

    protected:
        virtual void onAddElement(Widget& widget);

        virtual void onRemoveElement(Widget& widget);

        virtual void onSetFocusPolicy(Widget& w, FocusPolicy policy);

        virtual void onSetFocusIndex(Widget& w, unsigned index);

        virtual void onSetFocus(Widget& w);

        virtual void onSetShortcut(Widget& w, const std::vector<Key>& keys);

        virtual void onSetMnemonic(Widget& w, const std::vector<Char>& chs);
    
    //
    // View
    //
    protected:
        virtual void onAttach(Widget& widget);

        virtual void onDetach(Widget& widget);

        virtual void onInit(Widget& widget);

        virtual void onRelease(Widget& widget);

        virtual Gfx::PointF onToWidget(const Widget& widget, 
                                        const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromWidget(const Widget& widget, 
                                          const Gfx::PointF& pos) const;

    protected:
        virtual void onRepaintRequest(Widget& widget, const Gfx::RectF& rect);

        virtual void onRelayoutRequest(Widget& widget);

        virtual void onEnableRequest(Widget& widget, bool isEnable);

        virtual void onActivateRequest(Widget& w, bool active);

        virtual void onShowRequest(Widget& widget, bool isShown);

        virtual void onMoveRequest(Widget& widget, const Gfx::PointF& pos);

        virtual void onResizeRequest(Widget& widget, const Gfx::SizeF& size);

        virtual void onRaiseRequest(Widget& widget);

    //
    // Visual
    //
    protected:
        virtual Visual* onHitTest(const Gfx::PointF& pos);

        virtual void onRequestCapture(bool capture);

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

        const std::map<Key, Widget*>& shortcuts() const
        {
            return _shortcuts;
        }

        const std::map<Pt::Char, Widget*>& mnemonics() const
        {
            return _mnemonics;
        }


    private:
        Gfx::PaintRegion             _surface;
        Widget*                      _mainWidget;
                                     
        int                          _layouts;

        Widget*                      _active;

        std::vector<Widget*>         _focusList;
        Widget*                      _focusWidget;        
        std::map<Key, Widget*>       _shortcuts;
        std::map<Pt::Char, Widget*>  _mnemonics;

};

} // namespace

} // namespace

#endif
