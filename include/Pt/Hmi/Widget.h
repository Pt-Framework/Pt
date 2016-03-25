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
class PaintSurface;

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

        Window* window();

        const Window* window() const;

        Widget* parent();

        const Widget* parent() const;

        void add(Widget& w);

        void remove(Widget& w);

        std::vector<Widget*>& widgets();

        const std::vector<Widget*>& widgets() const;
       
        Widget* findWidget( const Gfx::PointF& pos );

        //
        // transformations
        //

        bool contains(const Gfx::PointF& p);

        Gfx::PointF toClient(const Gfx::PointF& globalPoint);

        Gfx::PointF fromClient(const Gfx::PointF& localPoint);

        Gfx::PointF toWindowPosition(const Gfx::PointF& p) const;
        
        Gfx::PointF toScreen(const Gfx::PointF& p) const;

        //
        // focus handling
        //

        bool hasFocus() const;

        void focus();

        bool acceptFocus() const;
        
        // TODO: focus by Tab, focus by pointer, both and none 
        void setAcceptFocus(bool a);

        size_t focusIndex() const;

        void setFocusIndex(size_t index);

        //
        // special keys
        //

        Key actionKey() const;

        void setActionKey( Key ak );

        const Key* shortcut() const;

        void setShortcut(const Key* k);

        void setMnemonicWidget(Widget* w);

        const Pt::Char* mnemonic() const;        


        void render(const Gfx::PointF& pos, 
                    PaintSurface& surface, 
                    const Gfx::RectF& updateRect);



        void processEvent(const Pt::Event& ev);

        void resize( const Gfx::SizeF& s );


        const Gfx::SizeF& size() const
        {
            return _size;
        }

        void move( const Gfx::PointF& p );

        const Gfx::PointF& position() const
        {
            return _position;
        }


        void show( bool b = true );

        void repaint();

        void enable( bool b = true );

        void repaint( const Gfx::RectF& rect );  

    protected:   
        virtual void onLayout();
        
        virtual void onPointerEnter();    

        virtual void onPointerLeave();

        virtual void onClicked(const Gfx::PointF& pos);

        virtual void onFocus(bool isFocused);

        virtual void onActionKey(const KeyEvent& kev);    
              
        virtual void onShortcut(const KeyEvent& kev);

        virtual void onMnemonic();   
        
        virtual Gfx::SizeF onAutoSize() const;

    protected:
        virtual void onPointerEvent(const MouseEvent& ev);

        virtual void onTouchEvent(const TouchEvent& ev);
        
        virtual void onScrollEvent( const ScrollEvent& ev );        
    
        virtual void onKeyEvent(const KeyEvent& ev);

    protected:
        String setMnemonic(const String& text);

    public:
        void setAutoSize(bool a);

        bool isAutoSize() const;

        Gfx::SizeF preferredSize() const;
        
        //
        // geometry properties
        //

        const Gfx::RectF geometry() const
        {
            return Gfx::RectF( position(), size() );
        }

        void setGeometry( const Gfx::PointF& pos, const Gfx::SizeF& size);

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
        bool                         _acceptFocus;
        bool                         _hasFocus;    
        Key                          _actionKey;
        size_t                       _focusIndex;     
        Gfx::PointF                  _position;
        Gfx::SizeF                   _size;   
};


} // namespace

} // namespace

#endif // PT_HMI_WIDGET_H
