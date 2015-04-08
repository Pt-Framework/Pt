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

#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/Margin.h>
#include <Pt/Connectable.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbColor.h>

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

		bool contains(const Pt::Gfx::PointF& p);
 	
		bool focusNext();
	
		bool focusPrev();		

		void invalidate();

		void render();

		void mnemonic();

		Pt::Gfx::PointF toClient(const Pt::Gfx::PointF& globalPoint);

		Pt::Gfx::PointF fromClient(const Pt::Gfx::PointF& localPoint, bool toRoot);

		Pt::Signal<const Pt::Event&>& eventReceived()
		{
			return _eventReceived;
		}

	public:
		static size_t getMnemonicIndex(const std::string& text);
		static std::string removeMnemonic(const std::string& text);

	public:
		Property<bool>											Focused; 
		Property<bool>											Enabled;		
		Property<bool>											Visible;
		Property<Pt::Gfx::Font>							Font;
		Property<Pt::Gfx::PointF>						Position;
		Property<Pt::Gfx::SizeF>						Size;
		Property<Pt::Gfx::ARgbColor>				BackColor;
		Property<Pt::Gfx::ARgbColor>				HighlightColor;
		Property<Pt::Gfx::ARgbColor>				ForeColor;
		Property<Pt::Gfx::ARgbColor>				DisabledColor;
		Property<Pt::Gfx::ARgbImage>				BackgroundImage;
		Property<ImageLayout::Type>					BackgroundImageLayout;
		Property<int>												Opacity;
		Property<Hmi::Cursor>								Cursor;
		Property<Align::Type>								TextAlign;		
		Property<bool>											AcceptFocus;
		Property<bool>											HighLight;
		Property<std::string>								FocusedActionKey;
		Property<std::string>								Caption;		
		Property<bool>											UseMnemonic;	
		Property<std::string>								Name;		
		Property<Pt::Hmi::Margin>						Margin;
		Property<Docking::Type>							Dock;
		Property<FlowLayout::Type>					FlowLayout;	
		Property<FlowLayoutDirection::Type> FlowDirection;
		Property<std::string>								ShortcutKey;				

	protected:
		Widget();	

	protected:
		virtual void onInvalidate();
		virtual void onRender();
		virtual void onLayout();
		virtual void onPointerInput(const PointingEvent& ev);
		virtual void onKeyInput(const KeyEvent& ev);
		virtual void onMnemonic();
		virtual void onActionKey(KeyEvent::KeyState state);
		virtual void onShortcutKey(KeyEvent::KeyState state);

	private:
		void onSizeChanged(const Property<Pt::Gfx::SizeF>& prop);
		void onFocusChanged(const Property<bool>& prop);
		void onCaptionChanged(const Property<std::string>& prop);

	private:			  
		bool focusNextChild(int index);
		bool focusPrevChild(int index);
		int getFocusedChild() const;	

	private:
		static void updatePosAndSize(Widget& w, const Pt::Gfx::SizeF& s, const Pt::Gfx::PointF& p);

	private:	  
		Widget*										 _parent;	
		std::vector<Widget*>			 _children;	
		Widget*										 _mnemonicWidget;	
		PaintSurface							 _paintSurface;	
		std::string								 _mnemonicKey;
		bool											 _isValid;
		Pt::Signal<const Pt::Event&> _eventReceived;
};

}}

#endif
