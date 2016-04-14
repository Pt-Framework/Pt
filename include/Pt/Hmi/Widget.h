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

class PT_HMI_API Widget : public Visual
{
    friend class Window;

    public: 
        enum ImageLayout
        {
            NoLayout,        
            Tile,
            Center,
            Strech,
            Zoom
        };

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
        
        bool hasFocus() const;

        void focus();

        bool acceptsFocus() const;
        
        // TODO: focus policy:
        //       focus by Tab, focus by pointer, both and none 
        void setAcceptsFocus(bool a);

        size_t focusIndex() const;

        void setFocusIndex(size_t index);

        //
        // keyboard input
        //

        Key actionKey() const;

        void setActionKey( Key ak );

        const Key* shortcut() const;

        void setShortcut(const Key* k);

        const Pt::Char* mnemonic() const; 

        void setMnemonicWidget(Widget* w);

        //
        // widget properties
        //
        
        void update();

        void update(const Gfx::RectF& rect);

        bool isVisible() const;

        void show( bool b = true );

        bool isEnabled() const;

        void enable( bool b = true );

        const Gfx::PointF& position() const;

        void move( const Gfx::PointF& p );

        const Gfx::SizeF& size() const;

        void resize( const Gfx::SizeF& s );

        const Gfx::RectF geometry() const;

        void setGeometry( const Gfx::PointF& pos, const Gfx::SizeF& size);

        // TODO: rethink
        bool isAutoSize() const;

        // TODO: rethink
        void setAutoSize(bool a);

        // TODO: rethink
        Gfx::SizeF preferredSize() const;

    protected:   
        void onPaint(const Gfx::RectF& updateRect);

        // TODO: move this to a Layout base class
        virtual void onLayout();

        virtual void onClicked(const Gfx::PointF& pos);

        virtual Gfx::SizeF onAutoSize() const;

        virtual void onFocus(bool isFocused);

        virtual void onActionKey(const KeyEvent& kev);    
              
        virtual void onShortcut(const KeyEvent& kev);

        virtual void onMnemonic();   

    protected:
        virtual void onEvent( const Event& ev );

        virtual void onPaintEvent( const PaintEvent& ev );

        virtual void onMoveEvent(const MoveEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

        virtual void onPointerEvent(const MouseEvent& ev);

        virtual void onTouchEvent(const TouchEvent& ev);
        
        virtual void onScrollEvent( const ScrollEvent& ev );        
    
        virtual void onKeyEvent(const KeyEvent& ev);

        virtual void onEnterEvent( const EnterEvent& ev );    

        virtual void onLeaveEvent(const LeaveEvent& ev );

    protected:
        String setMnemonic(const String& text);

    public:
        //
        // layout properties
        //

        Docking& docking()
        {
            return _docking;
        }

        const Docking& docking() const
        {
            return _docking;
        }

        void setDocking( const Docking& d )
        {
            _docking = d;
        }

        // outer spacing
        Spacing& margin()
        {
            return _margin;                  
        }

        const Spacing& margin() const
        {
            return _margin;                  
        }

        void setMargin( const Spacing& s )
        {
            _margin = s;
        }

        // inner spacing
        const Spacing& padding() const
        {
            return _padding;
        }

        Spacing& padding()
        {
            return _padding;
        }

        void setPadding( const Spacing& p )
        {
            _padding = p;
        }

        //
        // apperance properties
        //
        const Hmi::Cursor& cursor() const
        {
            return  _cursor;
        }

        void setCursor( const Hmi::Cursor& c ) 
        {
            _cursor = c;
        }

    private:
        void setWindow(Window* window);
        
    private:
        Pt::Signal<const Pt::Event&> _eventReady;
        Window*                      _window; 
        Widget*                      _parent;    
        std::vector<Widget*>         _children;        
        Hmi::Cursor                  _cursor;
        Key                          _shortcutKey;        
        Pt::Char                     _mnemonic;    
        Pt::Delegate<void>           _mnemonicEntered;
        Docking                      _docking;
        Spacing                      _margin;
        Spacing                      _padding;              
        bool                         _autoSize;
        bool                         _acceptsFocus;
        bool                         _hasFocus;   
        bool                         _enabled;
        bool                         _visible; 
        Key                          _actionKey;
        size_t                       _focusIndex;     
        Gfx::PointF                  _position;
        Gfx::SizeF                   _size;   
};


} // namespace

} // namespace

#endif // PT_HMI_WIDGET_H
