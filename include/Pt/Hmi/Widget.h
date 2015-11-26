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
	02110-1301  USA
*/

#ifndef Pt_Hmi_Controller_Widget_H
#define Pt_Hmi_Controller_Widget_H

#include <Pt/Hmi/PointerEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/Margin.h>
#include <Pt/Connectable.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Signal.h>
#include <map>

namespace Pt {

namespace Hmi {
  
namespace BorderStyle
{
  enum Type
  {
    NoBorder,
    Single,
    Border3D,
    Widget
  };
}


namespace ImageLayout
{
	enum Type
	{
		NoLayout,		
		Tile,
		Center,
		Strech,
		Zoom
	};
}


namespace Align
{
	enum Type
	{
		TopLeft,
		TopCenter,
		TopRight,
		MidleLeft,
		MidleCenter,
		MidleRight,
		BottomLeft,
		BottomCenter,
		BottomRight
	};
}


namespace Docking
{
	enum Type
	{
    None,
		Left,
		Top,
		Right,
		Bottom,
		Fill
	};
}

namespace FlowLayout
{
  enum  Type
  {
    None,
    Vertical,
    Horizontal
  };
}


namespace FlowLayoutDirection
{
  enum  Type
  {
    LeftToRightTopToBottom,
    RightToLeftBottomToTop
  };
}


class PT_HMI_API Widget : public Pt::Connectable
{
	friend class Window;

	public:	
		virtual ~Widget();

    void addChild(Widget* child);

		void removeChild(Widget* child);

		const std::vector<Widget*>& children() const
		{
			return _children;
		}

		const Widget* parent() const
		{
			return _parent;
		}

		Widget* parent()
		{		 
			return _parent;
		}

		void setParent(Widget* parent)
		{
			_parent = parent;
		}

		Widget* findWidget( const std::string& name );
		
		Widget* findWidget( const Gfx::PointF& pos );

		const PaintSurface& surface() const
		{
			return _surface;
		}

		PaintSurface& surface()
		{
			return _surface;
		}
		
		void processEvent(const Pt::Event& ev);

		void invalidate();

		void render();

		Gfx::PointF toClient(const Gfx::PointF& globalPoint);

		Gfx::PointF fromClient(const Gfx::PointF& localPoint);

	protected:
		Widget();	

		Pt::Signal<const Pt::Event&>& eventReceived()
		{
			return _eventReceived;
		}

		virtual void onEvent(const Pt::Event& ev);

		virtual void onPointerEvent(const PointerEvent& ev);		
    
		virtual void onKeyEvent(const KeyEvent& ev);

		virtual void onPointerEnter();	

		virtual void onPointerLeave();

		virtual void onMove(const Gfx::PointF& pos);

		virtual void onResize(const Gfx::SizeF& size);
			
		virtual void onFocus(bool isFocused);

		virtual void onInvalidate();

		virtual void onRender(PaintSurface& paintSurface);

	//
	// TODO: review
	//
	public:
		void bindMnemonicToWidget(Widget& widget);	

		bool contains(const Gfx::PointF& p);
 	
		bool focusNext();
	
		bool focusPrev();		

		void mnemonic();

	public:
		static size_t getMnemonicIndex(const std::string& text);
		static std::string removeMnemonic(const std::string& text);

	public:
		  bool hasFocus() const
		  {
			  return _hasFocus;
		  }

		  void setFocus()
		  {
			  onFocus(true);			
		  }

		 bool isEnabled() const
		 {
				return _enabled;
		 }

		 void setEnabled( bool e )
		 {
				_enabled = e;
		 }
		 
		 const Gfx::Font& font() const
		 {
				return _font;
		 }
		 
		 void setFont( const Gfx::Font& f )
		 {
				_font = f;
		 }
		 
		const Gfx::SizeF& size() const
		{
			return _size;
		}

		void setSize( const Gfx::SizeF& s )
		{
				onResize( s );
		}

		const Gfx::PointF& position() const
		{
			return _position;
		}

		void setPosition( const Gfx::PointF&  p)
		{
				onMove( p );
		}

		const Gfx::Color& backgroundColor() const
		{
			return _backgroundColor;
		}

		void setBackgroundColor( const Gfx::Color& c )
		{
			_backgroundColor = c;
		}

		const Gfx::Color& highlightColor() const
		{
			return _highlightColor;
		}

		void setHighlightColor( const Gfx::Color& c )
		{
			_highlightColor = c;
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

    ImageLayout::Type backgroundImageLayout() const
    {
        return _backgroundImageLayout;
    }

    void setBckgroundImageLayout( ImageLayout::Type l )
    {
        _backgroundImageLayout = l;
    }

    const Hmi::Cursor& cursor() const
    {
        return  _cursor;
    }

    void setCursor( const Hmi::Cursor& c ) 
    {
        _cursor = c;
    }

    Align::Type textAlign() const
    {
        return _textAlign;
    }

    void setTextAlign(Align::Type a) 
    {
        _textAlign = a;
    }

    bool acceptFocus() const
    {
        return _acceptFocus;
    }

    void setAcceptFocus(bool a) 
    {
        _acceptFocus = a;
    }

    const std::string& focusedActionKey() const
    {
        return _focusedActionKey;
    }

    void setFocusedActionKey( const std::string& ak )
    {
      _focusedActionKey = ak;
    }

    const std::string& caption() const
    {
        return _caption;
    }

    void setCcaption( const std::string& c )
    {
      _caption = c;
    }

    const std::string& name() const
    {
        return _name;
    }

		void setName(const std::string& name)
    {
      _name = name;
    }

    bool useMnemonic() const
    {
      return   _useMnemonic;
    }

    void  setUseMnemonic( bool u )
    {
      _useMnemonic = u;
    }

    const Pt::Hmi::Margin& margin() const
    {
        return _margin;
    }

    void setMargin(const Pt::Hmi::Margin& m )
    {
        _margin = m;
    }

    Docking::Type	dock() const
    {
        return _dock;
    }

    void setDock( Docking::Type d )
    {
      _dock = d;
    }

    FlowLayout::Type flowLayout() const
    {
        return _flowLayout;
    }

    void setFlowLayout( FlowLayout::Type t )
    {
        _flowLayout = t;
    }

    FlowLayoutDirection::Type flowDirection() const
    {
        return _flowDirection;	
    }

    void  setFlowDirection( FlowLayoutDirection::Type d )
    {
      _flowDirection = d;
    }

    const std::string& shortcutKey() const
    {
       return _shortcutKey;
    }

    void setShortcutKey( const std::string&  k )
    {
       _shortcutKey = k;
    }

    void setVisible( bool b )
    {
      onVisible(b);
    }

    bool visible() const
    {
      return _visible;
    }

    void setHighlight( bool b )
    {
      _highlight = b;
    }

    bool highlight() const
    {
      return _highlight;
    }

    
    const Gfx::Color& disabledColor() const
    {
      return _disabledColor;
    }
    		virtual void setCaption( const std::string& c);

      
	private:
		bool				_enabled;		
		bool				_visible;
    bool _highlight;
		Gfx::Font		_font;
		Gfx::Color  _backgroundColor;
		Gfx::Color	_highlightColor;
		Gfx::Color  _foregroundColor;
		Gfx::Color	_disabledColor;
		Gfx::Image	_backgroundImage;
		ImageLayout::Type _backgroundImageLayout;
		Hmi::Cursor				 _cursor;
		Align::Type				_textAlign;		
		bool							_acceptFocus;
		std::string			 _focusedActionKey;
		std::string			 _caption;		
    std::string							 _name;					
		Pt::Hmi::Margin					 _margin;
		Docking::Type						 _dock;
		FlowLayout::Type				_flowLayout;			
		FlowLayoutDirection::Type _flowDirection;	
    std::string							_shortcutKey;
		bool											  _hasFocus;			
    bool				 _useMnemonic;		
		std::string								 _mnemonicKey;
		Gfx::SizeF									_size;
		Gfx::PointF                _position;
    

		
	protected:
		virtual void onLayout( PaintSurface& surface );
		virtual void onMnemonic();
		virtual void onActionKey(KeyEvent::KeyState state);
		virtual void onShortcutKey(KeyEvent::KeyState state);	  
    virtual void onVisible( bool b )
    {
          _visible = b;
    }
	protected:		

    const Gfx::SizeF clientSize() const
    {
        return Gfx::SizeF( _size.width() - margin().left() -  margin().right(),
                          _size.height() - margin().top() -  margin().bottom() );
                          
    }

    const Gfx::PointF clientPos() const
    {
        return Gfx::PointF( margin().left(),  margin().top() );
    }

	private:			  
		bool focusNextChild(int index);
		bool focusPrevChild(int index);
		int getFocusedChild() const;	

	private:
		void updatePosAndSize(Widget& w, const Gfx::SizeF& s, const Gfx::PointF& p);

	private:	  
	  Pt::Signal<const Pt::Event&> _eventReceived;
		Widget*										 _parent;	
		std::vector<Widget*>			 _children;	
		Widget*										 _mnemonicWidget;	
    bool                        _isValid;
		PaintSurface 							_surface;
};

} // namespace

} // namespace

#endif
