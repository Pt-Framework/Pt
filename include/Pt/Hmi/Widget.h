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
class Layouter; // TODO

class LayoutItem
{
    public:
        class Iterator
        {
            public:
                Iterator()
                : _parent(0)
                , _current(0)
                , _n(0)
                {}

                explicit Iterator(LayoutItem& item)
                : _parent(&item)
                , _current(0)
                , _n(0)
                {
                  _current = item.onGet(_n);
                }

                Iterator& operator++()
                {
                    if( ! _parent)
                      return *this;

                    _current = _parent->onGet( ++_n );
            
                    if( ! _current)
                    {
                      _parent = 0;
                      _n = 0;
                    }

                    return *this;
                }
        
                LayoutItem& operator*()
                { return *_current; }

                LayoutItem* operator->()
                { return _current; }

                bool operator!=(const Iterator& other) const
                { return _current != other._current; }

                bool operator==(const Iterator& other) const
                { return _current == other._current; }

            private:
                LayoutItem* _parent;
                LayoutItem* _current;
                std::size_t _n;
        };

    public:
        virtual ~LayoutItem()
        {}

        Iterator begin()
        { return Iterator(*this); }

        Iterator end()
        { return Iterator(); }

        virtual const Gfx::PointF& position() const = 0;

        virtual const Gfx::SizeF& size() const = 0;

        virtual const Spacing& padding() const = 0;

        virtual const Spacing& margin() const = 0;

        virtual const Docking& docking() const = 0;

        virtual void setGeometry(const Gfx::PointF& p, const Gfx::SizeF& s) = 0;

    protected:
        virtual LayoutItem* onGet(std::size_t n) = 0;
};


class PT_HMI_API Widget : public Pt::Connectable
                        , private LayoutItem
{
    friend class Window;
    friend class WidgetLayoutItem;
    friend class WidgetLayouter;

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
        virtual void onLayout(LayoutItem::Iterator begin, LayoutItem::Iterator end);

        virtual void onRender(const Gfx::PointF& pos, PaintSurface& surface); 

    protected:
        virtual void onEvent(const Pt::Event& ev);

        virtual void onPointerEvent(const MouseEvent& ev);

        virtual void onTouchEvent(const TouchEvent& ev);
        
        virtual void onScrollEvent( const ScrollEvent& ev );        
    
        virtual void onKeyEvent(const KeyEvent& ev);

        virtual void onPointerEnter();    

        virtual void onPointerLeave();

        virtual void onClicked(const Gfx::PointF& pos);

        virtual void onFocus(bool isFocused);

        virtual void onActionKey(const KeyEvent& kev);    
              
        virtual void onShortcut(const KeyEvent& kev);

        virtual void onMnemonic();   
        
        virtual Gfx::SizeF onAutoSize() const;

        String setMnemonic(const String& text);

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

        Gfx::SizeF preferredSize() const;

        typedef void (*Layout)(LayoutItem&);
        
        void setLayout(Layout layout);

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

    protected:
        LayoutItem* onGet(std::size_t n)
        { return n < _children.size() ? _children[n] : 0; }

    private:
        void setWindow(Window* window);
        void propagateUpdate();

    private:
        Pt::Signal<const Pt::Event&> _eventReady;
        Window*                      _window; 
        Widget*                      _parent;    
        std::vector<Widget*>         _children;
        std::string                  _name;    
        bool                         _isValid;
        bool                         _enabled;        
        bool                         _visible;
        Hmi::Cursor                  _cursor;
        Layout                       _layout;
        Key                          _shortcutKey;        
        Gfx::SizeF                   _size;
        Gfx::PointF                  _position;            
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
};


} // namespace

} // namespace

#endif // PT_HMI_WIDGET_H


