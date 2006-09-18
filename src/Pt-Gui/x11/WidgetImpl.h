/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
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
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xvlib.h>

#include <list>
#include <string>


namespace Pt {

namespace Gui {

	class Widget;
	class WidgetPainter;
	class ResizeEvent;

	class PT_EXPORT WidgetImpl : public Drawable
	{
		public:
			WidgetImpl(Widget& apiWidget, Widget* parent, const Gfx::Point& at, const Gfx::Size& size);

			virtual ~WidgetImpl();

			void setTitle(const char* text);

			std::string title() const
			{ return ""; }

			const Gfx::Rect& rect() const
			{ return _rect; }

			WidgetPainter& getPainter();

			void unparent();

			void reparent(Widget* parent);

			void move(size_t x, size_t y);

			void resize(size_t width, size_t height);

			void show();

			void hide();

			void resizeEvent(const ResizeEvent& event);

		protected:
			void addChild(Widget& widget)
			{ _childWidgets.push_back(&widget); }

			void removeChild(Widget& widget)
			{ _childWidgets.remove(&widget); }

		private:
			//! a reference to the API object
			Widget& _apiWidget;

			//! The X11 root window.
			Window _root;

			//! the X11 parent window id
			Widget* _parent;

			//! X geometry of the widget relative the pparent
			Gfx::Rect _rect;

			std::list<Widget*> _childWidgets;

			WidgetPainter* _painter;
	};

} // namespace Gui

} // namespace Pt

#endif
