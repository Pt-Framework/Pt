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

#ifndef Pt_Gui_KeyEvent_h
#define Pt_Gui_KeyEvent_h

#include <Pt/Api.h>
#include <Pt/Gui/Event.h>

#include <cstddef>


namespace Pt {

namespace Gui {
	class Widget;

	class PT_EXPORT KeyEvent : public Event {
		public:
			enum Type {
				Press,
				Release
			};

			enum KeyCode {
				Void = 0, ControlL, ControlR, AltL, AltR, Ctrl, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
			};

		public:
			KeyEvent(Widget& widget, const Type& type, KeyCode code, wchar_t text);

			virtual ~KeyEvent();

			virtual Event* clone() const
			{ return new KeyEvent(*this); }

			Type type() const
			{ return _type; }

			wchar_t code() const
			{ return _code; }

			wchar_t text() const
			{ return _text; }

		private:
			Type _type;
			KeyCode _code;
			wchar_t _text;
	};

} // namespace Gui

} // namespace Pt

#endif
