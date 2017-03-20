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

class Window;

class SizePolicy
{
    public:
        enum Mode
        {
            Fixed,
            Preferred
            // Fill
            // Maximum
            // Minimum
        };

    public:
        SizePolicy()
        : _h(Preferred)
        , _v(Preferred)
        { }

        SizePolicy(Mode horizontal, Mode vertical)
        : _h(horizontal)
        , _v(vertical)
        { }

        Mode horizontal() const
        {
            return _h;
        }

        void setHorizontal(Mode m)
        {
            _h = m;
        }
        
        Mode vertical() const
        {
            return _v;
        }
        
        void setVertical(Mode m)
        {
            _v = m;
        }
        
        const Gfx::SizeF& size() const
        {
            return _sizeHint;
        }

        void setSize(const Gfx::SizeF& hint)
        {
            _sizeHint = hint;
        }

        void setSize(double w, double h)
        {
            _sizeHint.set(w, h);
        }

        double width() const
        { 
            return _sizeHint.width(); 
        }

        void setWidth(double w)
        {
            _sizeHint.setWidth(w);
        }

        double height() const
        { 
            return _sizeHint.height(); 
        }

        void setHeight(double h)
        {
            _sizeHint.setHeight(h);
        }

        bool operator== (const SizePolicy& s) const
        {
            return _h == s._h && _v == s._v && _sizeHint == s._sizeHint;
        }

    private:
        Mode       _h;
        Mode       _v;
        Gfx::SizeF _sizeHint;
};

class PT_HMI_API Widget : public Visual
{
    friend class Window;

    public:
        Widget();
        
        virtual ~Widget();

        //
        // widget hierachy
        //

        Window* window();

        const Window* window() const;

        Widget* parent();

        const Widget* parent() const;

        void add(Widget& w);

        void remove(Widget& w);
        
        const std::vector<Widget*>& widgets() const;

        Widget* findWidget( const Gfx::PointF& pos );

        void setContent(Widget& widget);

        bool acceptsInput() const;

        void setAcceptInput( bool a );

        void setTextInput(bool b);

        bool isTextInput() const;

        //
        // coordinate transformations
        //

        Gfx::PointF toParent(const Gfx::PointF& pos) const;

        Gfx::PointF fromParent(const Gfx::PointF& pos) const;

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

        void invalidate();

        void relayout();

        void update();

        void update(const Gfx::RectF& rect);

        void repaint(const Gfx::RectF& updateRect);

        bool isVisible() const;

        void show( bool b = true );

        bool isEnabled() const;

        void enable( bool b = true );

        void raise();

        void grabPointer();

        void releasePointer();

        const Gfx::PointF& position() const;

        void move(const Gfx::PointF& p);

        void move(double x, double y);

        const Gfx::SizeF& size() const;

        void resize(const Gfx::SizeF& s);

        void resize(double width, double height);

        const Gfx::RectF geometry() const;

        void setGeometry(const Gfx::PointF& pos, 
                         const Gfx::SizeF& size);

        const Cursor& cursor() const;

        void setCursor(const Cursor& c);

        //
        // layouting
        //
 
        const SizePolicy& sizePolicy() const;

        void setSizePolicy(const SizePolicy& policy);

        bool isAutoSize() const;

        void setAutoSize(bool a);

        Gfx::SizeF preferredSize() const;

        Gfx::SizeF preferredSize(const SizePolicy& policy) const;


        const Gfx::SizeF& measuredSize() const;

        void measure(const SizePolicy& policy);


        void layout(const Gfx::RectF& rect);

        void layout(const Pt::Gfx::PointF& p, const Pt::Gfx::SizeF& s);

        void layout(double x, double y, double width, double height);


        

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

        /** @brief Position, size or preferred size has changed.
        */
        Pt::Signal<>& layoutChanged();

        //
        // event processing
        //

        Pt::Signal<const Pt::Event&>& eventReady();

    public:
        virtual Gfx::PointF toScreen(const Gfx::PointF& pos) const;

        virtual Gfx::PointF fromScreen(const Gfx::PointF& pos) const;

    protected:
         virtual void onAddWidget(Widget& w);

        virtual void onRemoveWidget(Widget& w);

        virtual void onParentChanged(Widget* w);

        virtual void onRaise(Widget& w);

        
        virtual void onInvalidate();
        
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout();
        

        virtual Pt::Gfx::SizeF onAutoSize(const SizePolicy& policy) const;

        virtual void onSetActionKey(const Key& ak);

        virtual void onActionKey(const KeyEvent& kev);

        virtual void onSetShortcut(const Key* k);

        virtual void onShortcut(const KeyEvent& kev);

        virtual void onMnemonic();

    protected:
        virtual void onEvent( const Event& ev );

        virtual void onInvalidateEvent(const InvalidateEvent& ev);

        virtual void onEnableEvent(const EnableEvent& ev);

        virtual void onShowEvent(const ShowEvent& ev );

        virtual void onPaintEvent( const PaintEvent& ev );

        virtual void onMoveEvent(const MoveEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

        virtual bool onMouseEvent(const MouseEvent& ev);

        void mouseEvent(const MouseEvent& ev);

        virtual void onTouchEvent(const TouchEvent& ev);

        void scrollEvent( const ScrollEvent& ev );

        virtual bool onScrollEvent( const ScrollEvent& ev );

        virtual void onKeyEvent(const KeyEvent& ev);

        virtual void onEnterEvent( const EnterEvent& ev );

        virtual void onFocusEvent(const FocusEvent& ev);

        virtual void onLeaveEvent(const LeaveEvent& ev );

    private:
        void setParent(Widget* parent);

        void setWindow(Window* window);

        Widget* findWidget( const Gfx::PointF& pos, bool input );

        


    private:
        Pt::Signal<const Pt::Event&> _eventReady;
        Pt::Signal<>                 _layoutChanged;

        std::vector<Widget*>         _children;
        Widget*                      _parent; 
        Window*                      _window; 
        Widget*                      _content;

        int                          _invalidates;

        bool                         _visible;
        bool                         _enabled;
        bool                         _enabledState;
        Gfx::PointF                  _position;
        Gfx::SizeF                   _size;
        SizePolicy                   _sizePolicy;
        Gfx::SizeF                   _preferredSize;
        bool                         _autoSize;

        Gfx::SizeF                   _measuredSize;

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

} // namespace

} // namespace

#endif // PT_HMI_WIDGET_H

