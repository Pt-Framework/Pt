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
#include <Pt/Hmi/Layout.h>
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

class PT_HMI_API Widget : public Pt::Connectable
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

        enum Alignment
        {
            TopLeft,
            TopCenter,
            TopRight,
            MiddleLeft,
            MiddleCenter,
            MiddleRight,
            BottomLeft,
            BottomCenter,
            BottomRight
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

        const std::vector<Widget*>& widgets() const;

        Widget* findWidget( const std::string& name );
        
        Widget* findWidget( const Gfx::PointF& pos );

        //
        // transformations
        //

        bool contains(const Gfx::PointF& p);

        Gfx::PointF toClient(const Gfx::PointF& globalPoint);

        Gfx::PointF fromClient(const Gfx::PointF& localPoint);
        
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

        void processEvent(const Pt::Event& ev);

        void update();

        bool isValid() const
        { return _isValid; }

        void render(const Gfx::PointF& pos, PaintSurface& parentSurface);

    protected:    
        virtual void onEvent(const Pt::Event& ev);

        virtual void onPointerEvent(const MouseEvent& ev);

        virtual void onTouchEvent(const TouchEvent& ev);
        
        virtual void onScrollEvent( const ScrollEvent& ev );        
    
        virtual void onKeyEvent(const KeyEvent& ev);

    protected:
        virtual void onUpdate();

        virtual void onLayout();

        virtual void onRender(const Gfx::PointF& pos, PaintSurface& surface); 

        virtual void onPointerEnter();    

        virtual void onPointerLeave();

        virtual void onClicked(const Gfx::PointF& pos);

        virtual void onFocus(bool isFocused);

        virtual void onActionKey(const KeyEvent& kev);    
              
        virtual void onShortcut(const KeyEvent& kev);

        virtual void onMnemonic();   
        
        virtual Gfx::SizeF onAutoSize() const;    

    public:
        // TODO
        const std::string& name() const
        {
            return _name;
        }

        void setName(const std::string& name)
        {
          _name = name;
        }

        void setAutoSize(bool a);

        bool isAutoSize() const;

        void setCaption(const std::string& c);

        const std::string& caption() const
        {
            return _caption;
        }

        bool isEnabled() const
        {
            return _enabled;
        }

        void setEnabled( bool e );

        void setVisible( bool b );

        bool visible() const
        {
          return _visible;
        }

        //
        // geometry properties
        //

        const Gfx::SizeF& size() const
        {
            return _size;
        }

        void setSize( const Gfx::SizeF& s );

        const Gfx::PointF& position() const
        {
            return _position;
        }

        void setPosition( const Gfx::PointF&  p);

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

        void setLayout(const Layout& l )
        {
            _layout = l;
        }

        const Layout& layout() const 
        {
            return _layout;
        }

        Layout& layout()
        {
            return _layout;
        }

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

        Alignment contentAlignment()
        {
            return _contentAlignment;
        }

        void setContentAlignment( Alignment a )
        {
            _contentAlignment = a;
        }

        //
        // apperance properties
        //

        const Gfx::Color& backgroundColor() const
        {
            return _backgroundColor;
        }

        void setBackgroundColor( const Gfx::Color& c )
        {
            _backgroundColor = c;
        }

        const Gfx::Color& foregroundColor() const
        {
            return _foregroundColor;
        }

        void setForegroundColor(const Gfx::Color& c )
        {
            _foregroundColor = c;
        }

        const Gfx::Image& backgroundImage() const
        {
            return _backgroundImage;
        }

        void setBackgroundImage( const Gfx::Image& im )
        {
            _backgroundImage = im;
        }

        ImageLayout backgroundImageLayout() const
        {
            return _backgroundImageLayout;
        }

        void setBackgroundImageLayout( ImageLayout l )
        {
            _backgroundImageLayout = l;
        }

        void setFont( const Gfx::Font& f );

        const Gfx::Font& font() const
        {
            return _font;
        }

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

        void updateGeometry(Widget& w, const Gfx::PointF& p, const Gfx::SizeF& s);

    private:
        Pt::Signal<const Pt::Event&> _eventReady;
        Window*                      _window; 
        Widget*                      _parent;    
        std::vector<Widget*>         _children;    
        bool                         _isValid;
          
        bool                         _enabled;        
        bool                         _visible;
        Gfx::Color                   _backgroundColor;
        Gfx::Color                   _foregroundColor;
        Gfx::Image                   _backgroundImage;
        ImageLayout                  _backgroundImageLayout;
        Hmi::Cursor                  _cursor;
        std::string                  _name;                    
        Layout                       _layout;
        Key                          _shortcutKey;        
        Gfx::SizeF                   _size;
        Gfx::PointF                  _position;            
        Alignment                    _contentAlignment;
        std::string                  _caption;
        //Gfx::FontMetrics             _captionMetrics;
        Gfx::Font                    _font;
        Pt::Char                     _mnemonic;    
        Pt::Delegate<void>           _mnemonicEntered;
        Docking                      _docking;
        Spacing                      _margin;                  
        bool                         _autoSize;
        bool                         _acceptFocus;
        bool                         _hasFocus;    
        Key                          _actionKey;
        size_t                       _focusIndex;   
};

} // namespace

} // namespace

#endif // PT_HMI_WIDGET_H


