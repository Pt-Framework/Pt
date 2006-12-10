/***************************************************************************
 *   Copyright (C) 2006 Marc Boris D�rner                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_Gui_WidgetImpl_h
#define Pt_Gui_WidgetImpl_h

#include "Drawable.h"

#include <Pt/Api.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Rect.h>
#include <Pt/Gui/Painter.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
//#include <X11/extensions/Xvlib.h>

#include <list>
#include <string>


namespace Pt {

namespace Gui {

	class Widget;
	class WidgetPainterImpl;
	class ResizeEvent;

	class PT_API WidgetImpl : public Drawable
	{
		public:
			WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at = Math::Point(0, 0),
			                                              const Math::Size& size = Math::Size(400, 300));

			virtual ~WidgetImpl();

			void setTitle(const std::string& text);

			std::string title() const
			{ return _title; }

			Painter painter();

			void setParent(Widget* parent);

			void move(size_t x, size_t y);

			void resize(size_t width, size_t height);

			void show();

			void hide();

		private:
			//! The X11 root window.
			Window _root;

			//! the X11 parent window id
			Widget* _parent;

			WidgetPainterImpl* _painter;

			std::string _title;
	};

} // namespace Gui

} // namespace Pt

#endif
