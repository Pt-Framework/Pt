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

#include <Pt/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>

#include <list>
using namespace std;


namespace Pt {

namespace Gui {

	class Widget;
	class ResizeEvent;

	class PT_EXPORT WidgetImpl
	{
		public:
			WidgetImpl(Widget& apiWidget, Widget* parent, const Gfx::Point& at, const Gfx::Size& size);

			virtual ~WidgetImpl();

			void unparent();

			void setMainWidget(bool isMain);

			bool isMainWidget();

			void setTitle(const char* text);

			const Gfx::Rect& rect() const;

			void move(size_t x, size_t y);

			void resize(size_t width, size_t height);

			void show();

			void hide();

			void resizeEvent(const ResizeEvent& event);

			void addChild(Widget& widget)
			{ _childWidgets.push_back(&widget); }

			void removeChild(Widget& widget)
			{ _childWidgets.remove(&widget); }

		protected:
			void destroy();

			bool valid();

		private:
			//! a reference to the API object
			Widget& _apiWidget;

			//! the X11 parent window id
			Widget* _parent;

			bool _isMain;

			Gfx::Rect _rect;

			std::list<Widget*> _childWidgets;
	};

} // namespace Gui

} // namespace Pt

#endif
