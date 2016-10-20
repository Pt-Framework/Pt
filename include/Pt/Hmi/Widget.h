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
#include <Pt/Hmi/ShowEvent.h>
#include <Pt/Hmi/FocusEvent.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/Docking.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Connectable.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Signal.h>
#include <Pt/Delegate.h>

namespace Pt {

namespace Hmi {

class Window;
class Layout;

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

        void setAcceptInput( bool a );

        bool acceptsInput() const;

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

        bool acceptsFocus() const;

        // TODO: focus policy:
        //       focus by Tab, focus by pointer, both and none 
        void setAcceptsFocus(bool a);

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

        void move( const Gfx::PointF& p );

        const Gfx::SizeF& size() const;

        void resize( const Gfx::SizeF& s );

        const Gfx::RectF geometry() const;

        void setGeometry( const Gfx::PointF& pos, const Gfx::SizeF& size);

        const Cursor& cursor() const;

        void setCursor( const Cursor& c );

        //
        // layouting
        //
 
        bool isAutoSize() const;

        void setAutoSize(bool a);

        Gfx::SizeF preferredSize() const;

        // outer spacing
        const Spacing& margin() const;

        // outer spacing
        void setMargin(const Spacing& s);

        // outer spacing
        void setMargin(double n);

        // inner spacing
        const Spacing& padding() const;

        // inner spacing
        void setPadding(const Spacing& p);

        // inner spacing
        void setPadding(double n);

        const Docking& docking() const;

        void setDocking(const Docking& d);

    public:
        virtual Gfx::PointF toScreen(const Gfx::PointF& pos) const;
    
        virtual Gfx::PointF fromScreen(const Gfx::PointF& pos) const;       

    protected:
        virtual void onAddWidget(Widget& w);

        virtual void onRemoveWidget(Widget& w);

        virtual void onParentChanged(Widget* w);

        virtual void onRaise(Widget& w);

        virtual void onLayout();

        virtual Gfx::SizeF onAutoSize() const;

        virtual void onSetActionKey(const Key& ak);

        virtual void onActionKey(const KeyEvent& kev);

        virtual void onSetShortcut(const Key* k);

        virtual void onShortcut(const KeyEvent& kev);

        virtual void onMnemonic();

    protected:
        virtual void onEvent( const Event& ev );

        virtual void onEnableEvent(const EnableEvent& ev);

        virtual void onShowEvent(const ShowEvent& ev );

        virtual void onPaintEvent( const PaintEvent& ev );

        virtual void onMoveEvent(const MoveEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

        virtual void onMouseEvent(const MouseEvent& ev);

        virtual void onTouchEvent(const TouchEvent& ev);

        virtual void onScrollEvent( const ScrollEvent& ev );

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

        std::vector<Widget*>         _children;
        Widget*                      _parent; 
        Window*                      _window; 
        Widget*                      _content;

        bool                         _visible;
        bool                         _enabled;
        bool                         _enabledState;
        Gfx::PointF                  _position;
        Gfx::SizeF                   _size;

        bool                         _hasFocus;
        bool                         _acceptsFocus;
        size_t                       _focusIndex;
        bool                         _acceptsInput; 

        Hmi::Cursor                  _cursor;
        Key                          _actionKey;
        Key                          _shortcutKey;
        Pt::Char                     _mnemonic;
        Pt::Delegate<void>           _mnemonicEntered;

        bool                         _autoSize;
        Spacing                      _padding;
        Spacing                      _margin;
        Docking                      _docking;

};

} // namespace

} // namespace

#endif // PT_HMI_WIDGET_H
