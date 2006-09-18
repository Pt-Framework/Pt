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

#ifndef Pt_Gui_Application_h
#define Pt_Gui_Application_h

#include <Pt/Api.h>
#include <Pt/Application.h>

#include <typeinfo>

namespace Pt {

namespace Gui {

	class PT_EXPORT Application : public Pt::Application
	{
		private:
			class ApplicationImpl* _impl;

		public:
			static const std::type_info& TYPE_CLOSE_EVENT;
			static const std::type_info& TYPE_MOUSE_EVENT;
			static const std::type_info& TYPE_KEY_EVENT;
			static const std::type_info& TYPE_MOVE_EVENT;
			static const std::type_info& TYPE_RESIZE_EVENT;
			static const std::type_info& TYPE_PAINT_EVENT;
			static const std::type_info& TYPE_MOUSEMOVE_EVENT;

			Application();

			~Application();

			ApplicationImpl& impl();

			void commitEvent(const Pt::Event& event);

			void queueEvent(const Pt::Event& event);

			void processEvents();

			int run();

			void exit();

			void dispatchEvent(const Pt::Event& ev) const;
	};

} // namespace Gui

} // namespace Ptv

#endif
