/* Copyright (C) 2013 Marc Boris Duerner 
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_Hmi_Controller_Window_H
#define Pt_Hmi_Controller_Window_H

#include <Pt/Hmi/Widget.h>

namespace Pt{
namespace Hmi{

class WindowImpl;
class ResizeEvent;
class PositionEvent;

namespace WindowStartPositionType
{
	enum Type
	{
		Manual,
		CenterScreen,
		CenterParent,
	};
}

namespace WindowBorderType
{
	enum Type
	{
        NoBorder,
		Fixed,
		Sizeable,
		Tool,
		ToolSizeable,
		Dialog,
		DialogSizeable
	};
}


namespace WindowStateType
{
	enum Type
	{
		Normal,
		Minimized,
		Maximazed,
	};
}


class PT_HMI_API Window  : public Widget
{
public:	
	Window();
	
	virtual ~Window();

	Property<Pt::Gfx::SizeF>							MinimumSize;
	Property<Pt::Gfx::SizeF>								MaximumSize;
	Property<WindowStartPositionType::Type>	WindowStartPostion;
	Property<WindowStateType::Type>				WindowState;	
	Property<bool>								ShowInTaskbar;
	Property<bool>								ShowTitle;
	Property<bool>								ShowMinimizeButton;
	Property<bool>								ShowMaximizeButton;
	Property<bool>								ShowSysMenu;
	Property<std::string>						Caption;
	Property<WindowBorderType::Type>			Border;
	Property<Pt::Gfx::ARgbImage>				Icon;
	Property<bool>								Closed;
	Property<bool>								CanClose;
	Property<bool>								TopMost;
	Property<std::string>				  FocuseMoveKey;

public:
	Widget* mainWidget();
	const Widget* mainWidget() const;	
	bool close();	

protected:
	virtual void onPointerInput(const PointingEvent& ev);
	virtual void onKeyInput(const KeyEvent& ev);
	virtual void onSizeChanged(const Property<Pt::Gfx::SizeF>& prop);
	virtual void onInvalidate();
	
protected:
	bool focusNextChild(int index);
	
	int getFocusedChild() const
	{
		return 0;
	}

  bool moveFocusNext();
	
	bool moveFocusPrev()
	{
		return true;
	}	

	void resizeEvent(const ResizeEvent& ev);
	void positionEvent(const PositionEvent& ev);

private:
	void onClosed(const Property<bool> & closed);

private:
	WindowImpl*		_impl;	
};

}}
#endif
