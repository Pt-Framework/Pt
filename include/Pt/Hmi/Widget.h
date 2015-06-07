/* Copyright (C) 2015 Marc Boris Duerner 
 * Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef Pt_Hmi_Controller_Widget_H
#define Pt_Hmi_Controller_Widget_H

#include <Pt/Hmi/PointerEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/SizeEvent.h>
#include <Pt/Hmi/PositionEvent.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/Margin.h>
#include <Pt/Connectable.h>
#include <Pt/Ui/Font.h>
#include <Pt/Ui/Point.h>
#include <Pt/Ui/Size.h>
#include <Pt/Ui/Color.h>

namespace Pt{
namespace Hmi{
  
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

		void bindMnemonicToWidget(Widget& widget);	

		PaintSurface& paintSurface()
		{
			return _paintSurface;
		}

		bool contains(const Ui::PointF& p);
 	
		bool focusNext();
	
		bool focusPrev();		

		void invalidate();

		void render();

		void mnemonic();

		bool hasPointer() const
		{
			return _containPointer;
		}

		Ui::PointF toClient(const Ui::PointF& globalPoint);

		Ui::PointF fromClient(const Ui::PointF& localPoint, bool toRoot);

		Pt::Signal<const Pt::Event&>& eventReceived()
		{
			return _eventReceived;
		}

	public:
		static size_t getMnemonicIndex(const std::string& text);
		static std::string removeMnemonic(const std::string& text);

	public:
		ValueProperty<bool>											Enabled;		
		ValueProperty<bool>											Visible;
		ValueProperty<Ui::Font>						Font;
		Property<Widget,Ui::PointF>				Position;
		Property<Widget,Ui::SizeF>					Size;
		ValueProperty<Ui::Color>				BackColor;
		ValueProperty<Ui::Color>				HighlightColor;
		ValueProperty<Ui::Color>				ForeColor;
		ValueProperty<Ui::Color>				DisabledColor;
		ValueProperty<Ui::Image>				BackgroundImage;
		ValueProperty<ImageLayout::Type>				BackgroundImageLayout;
		ValueProperty<int>											Opacity;
		ValueProperty<Hmi::Cursor>							Cursor;
		ValueProperty<Align::Type>							TextAlign;		
		ValueProperty<bool>											AcceptFocus;
		ValueProperty<bool>											HighLight;
		ValueProperty<std::string>							FocusedActionKey;
		Property<Widget,std::string>						Caption;		
		ValueProperty<bool>											UseMnemonic;	
		ValueProperty<std::string>							Name;		
		ValueProperty<Pt::Hmi::Margin>					Margin;
		ValueProperty<Docking::Type>						Dock;
		ValueProperty<FlowLayout::Type>					FlowLayout;			
		ValueProperty<std::string>							ShortcutKey;
		ValueProperty<FlowLayoutDirection::Type> FlowDirection;				

		Signal<bool> Focused;

		bool isFocused() const
		{
			return _isFocused;
		}

		void focus()
		{
			setFocus(true);			
		}


	protected:
		Widget();	

	public:
		virtual void onPointerLeaved();

	protected:
		virtual void onInvalidate();
		virtual void onRender();
		virtual void onLayout();

		virtual void onPointerInput(const PointerEvent& ev);		
		virtual void onPointerEnter();		
		
		virtual void onKeyInput(const KeyEvent& ev);
		virtual void onMnemonic();
		virtual void onActionKey(KeyEvent::KeyState state);
		virtual void onShortcutKey(KeyEvent::KeyState state);	  
		

		virtual void setPosition(const Ui::PointF& pos)
		{
			_position = pos;			
		}	

		virtual void setSize(const Ui::SizeF& size);
			
		virtual void setFocus(bool isFocused);
		virtual void setCaption( const std::string& c);

	protected:
		const Ui::SizeF& size() const
		{
			return _size;
		}

		const Ui::PointF& position() const
		{
			return _position;
		}
		
		const std::string& caption() const
		{
			return _caption;
		}


	private:			  
		bool focusNextChild(int index);
		bool focusPrevChild(int index);
		int getFocusedChild() const;	

	private:
		static void updatePosAndSize(Widget& w, const Ui::SizeF& s, const Ui::PointF& p);

	private:	  
		Widget*										 _parent;	
		std::vector<Widget*>			 _children;	
		Widget*										 _mnemonicWidget;	
		PaintSurface							 _paintSurface;	
		std::string								 _mnemonicKey;
		bool											 _isValid;		
		bool											 _containPointer;	
		Pt::Signal<const Pt::Event&> _eventReceived;
		Ui::SizeF							_size;
		Ui::PointF             _position;
		bool											  _isFocused;
		std::string									_caption;		
};

}}

#endif
