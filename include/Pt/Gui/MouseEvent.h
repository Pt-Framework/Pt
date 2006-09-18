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

#ifndef Pt_Gui_MouseEvent_h
#define Pt_Gui_MouseEvent_h

#include <Pt/Api.h>
#include <Pt/Gui/Event.h>

#include <cstddef>


namespace Pt {

namespace Gui {

	class Widget;

	class PT_EXPORT MouseEvent : public Event {
		public:
			enum Button {
				LeftButton,
				MiddleButton,
				RightButton,
				WheelUp,
				WheelDown
			};

			enum Action {
				Press,
				Release,
				DoubleClick
			};

			enum Modifier {
				ShiftDown        = 1 << 0,
				AltDown          = 1 << 1,
				CtrlDown         = 1 << 2,
				LeftButtonDown   = 1 << 3,
				RightButtonDown  = 1 << 4,
				MiddleButtonDown = 1 << 5
			};

		public:
			MouseEvent(Widget& widget, size_t x, size_t y, const Button& button, const Action& action, unsigned int modifiers);

			virtual ~MouseEvent();

			virtual Event* clone() const
			{ return new MouseEvent(*this); }

			size_t x() const;

			size_t y() const;

			Button button() const;

			Action action() const;

			unsigned int modifiers() const;

		private:
			size_t _x;
			size_t _y;
			Button _button;
			Action _action;
			int    _modifiers;
	};

} // namespace Gui

} // namespace Ptv

#endif
