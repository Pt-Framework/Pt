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
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/SizePolicy.h>
#include <Pt/Hmi/Spacing.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Connectable.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Signal.h>
#include <Pt/Delegate.h>

namespace Pt {

namespace Hmi {

class Widget;
class Window;
class Screen;
class Layouter;

class PT_HMI_API Widget : public Visual
                        , public LayoutManager
{
    friend class Window;
    friend class Layouter;
    friend class LayoutManager;

    public:
        Widget();

        virtual ~Widget();

        //
        // widget hierachy
        //

        Window* window();

        const Window* window() const;

        Screen* screen();

        const Screen* screen() const;

        const std::vector<Widget*>& widgets() const;

        Widget* findWidget(const Gfx::PointF& pos);

        Widget* findWidget(const std::string& name);

        Widget* findWidget(Pt::uint64_t vid);

        bool acceptsInput() const;

        void setAcceptInput( bool a );

        void setTextInput(bool b);

        bool isTextInput() const;

        //
        // coordinate transformations
        //
        Gfx::PointF toWindow(const Gfx::PointF& p) const;

        Gfx::PointF fromWindow(const Gfx::PointF& pos) const;

        //
        // focus handling
        // 

        enum FocusPolicy
        {
            NoFocus,
            KeepFocus,
            NormalFocus
        };

        void setFocusPolicy(FocusPolicy f);

        FocusPolicy focusPolicy() const;

        bool hasFocus() const;

        void focus();

        size_t focusIndex() const;

        void setFocusIndex(size_t index);

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

        //
        // widget operations
        //

        // deprecated
        void update()
        { repaint(); }

        // deprecated
        void update(const Gfx::RectF& rect)
        { repaint(rect); }


        void invalidate();


        void repaint()
        {
            Gfx::RectF rect( Gfx::PointF(0, 0), size() );
            onRepaint(rect);
        }

        void repaint(const Gfx::RectF& rect)
        {
            onRepaint(rect);
        }

      public:
        bool isVisible() const;

        void show( bool b = true );

        bool isEnabled() const;

        void enable( bool b = true );

        void raise();

        void grabPointer();

        void releasePointer();


        const Cursor& cursor() const;

        void setCursor(const Cursor& c);

    //
    // layouting
    //
    public:
        const Gfx::PointF& position() const;

        const Gfx::RectF geometry() const;

        const SizePolicy& sizePolicy() const;

        void setSizePolicy(const SizePolicy& policy);

        Gfx::SizeF preferredSize() const;

    protected:
        // onMeasure
        virtual Gfx::SizeF measure(const SizePolicy& policy);

        // onMeasureContent (widget specific)
        virtual Gfx::SizeF onMeasure(Layouter& layouter, const SizePolicy& policy);

        // onLayout
        virtual void layout(const Gfx::RectF& rect);

        // onLayoutContent (widget specific)
        virtual void onLayout(Layouter& layouter, const Gfx::RectF& rect);

    //
    // painting
    //
    protected:
        // onPaint
        virtual void onPaintContent(const Gfx::RectF& r);

        // onPaintContent (widget specific)
        virtual void onPaintEvent(const PaintEvent& ev);

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

        const Gfx::SizeF& minimumSize() const;

        void setMinimumSize(const Gfx::SizeF&);

        void setMinimumSize(double w, double h);

        double minimumWidth() const;

        void setMinimumWidth(double w);

        double minimumHeight() const;

        void setMinimumHeight(double h);

        //
        // event processing
        //

        Pt::Signal<const Pt::Event&>& eventReady();

    protected:
        //void add(Widget& w);

        //void remove(Widget& w);

    protected:
        virtual Visual* onParent() const;

        Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual const Gfx::SizeF& onSize() const;

        virtual double onScaleFactor() const;

        virtual void onRelayout();

        virtual void onRepaint(const Gfx::RectF& rect);

    protected:
        virtual void onSetWindow(Window* w);

        virtual void onSetScreen(Screen* s);


        virtual void onAddWidget(Widget& w);

        virtual void onRemoveWidget(Widget& w);

        virtual void onParentChanged(Widget* w);

        virtual void onRaise(Widget& w);


        virtual void onInvalidate();


        virtual void onSetActionKey(const Key& ak);

        virtual void onActionKey(const KeyEvent& kev);

        virtual void onSetShortcut(const Key* k);

        virtual void onShortcut(const KeyEvent& kev);

        virtual void onMnemonic();

    protected:
        virtual void onEvent(const Event& ev);

        virtual void onInvalidateEvent(const InvalidateEvent& ev);

        virtual void onEnableEvent(const EnableEvent& ev);

        virtual void onShowEvent(const ShowEvent& ev);

        virtual void onFocusEvent(const FocusEvent& ev);

        virtual void onMoveEvent(const MoveEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    public:
        void mouseEvent(const MouseEvent& ev);

        void touchEvent(const TouchEvent& ev);

        void scrollEvent( const ScrollEvent& ev);

    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent( const ScrollEvent& ev);

        virtual void onKeyEvent(const KeyEvent& ev);

        virtual void onEnterEvent( const EnterEvent& ev);

        virtual void onLeaveEvent(const LeaveEvent& ev);

    private:
        virtual Visual* onGetVisual();

        virtual Window* onGetWindow();

        virtual Screen* onGetScreen();

        virtual void onAttach(Widget& widget);

        virtual void onDetach(Widget& widget);

    private:
        void setParent(LayoutManager* parent);

        void setWindow(Window* window);

        void setScreen(Screen* screen);

        Widget* findWidget( const Gfx::PointF& pos, bool input );

    private:
        Pt::Signal<const Pt::Event&> _eventReady;

        std::vector<Widget*>         _children;
        
        Screen*                      _screen; 
        Window*                      _window; 
        Visual*                      _parent;
        LayoutManager*               _layouter;

        int                          _invalidates;
        bool                         _isLayoutInvalid;

        bool                         _visible;
        bool                         _enabled;
        bool                         _enabledState;
        Gfx::PointF                  _position;
        Gfx::SizeF                   _size;
        
        Gfx::SizeF                   _minimumSize;
        SizePolicy                   _sizePolicy;
        SizePolicy                   _lastPolicy;
        Gfx::SizeF                   _preferredSize;

        bool                         _hasFocus;
        FocusPolicy                  _focusPolicy;
        size_t                       _focusIndex;
        bool                         _acceptsInput;
        bool                         _textInput;

        Hmi::Cursor                  _cursor;
        Key                          _actionKey;
        Key                          _shortcutKey;
        Pt::Char                     _mnemonic;
        Pt::Delegate<void>           _mnemonicEntered;
        
        Spacing                      _padding;
        Spacing                      _margin;
};


class Layouter
{
    friend class Widget;
    friend class Layout;

    protected:
        Layouter()
        {}

    public:
        virtual ~Layouter()
        {}

        Gfx::SizeF measure(Widget& w, const SizePolicy& policy)
        {
            return w.measure(policy);
        }

        void layout(Widget& w, const Gfx::RectF& r)
        {
            w.layout(r);
        }

        void layout(Widget& w, const Gfx::PointF& p, const Gfx::SizeF& s)
        {
            w.layout( Gfx::RectF(p, s) );
        }
};

} // namespace

} // namespace

#endif // PT_HMI_WIDGET_H

