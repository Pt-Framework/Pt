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

#ifndef Pt_Gui_Button_h
#define Pt_Gui_Button_h

#include <Pt/Api.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/Painter.h>

#include <string>
#include <memory>


namespace Pt {

namespace Gui {

	class PT_EXPORT Button : public Widget
	{
		public:
			Button(Widget& parent, const Gfx::Point& at, const Gfx::Size& size);

			void setText(const std::string& text);

			const std::string& text() const;

			virtual void update();

		public:
			Signal<> clicked;

		protected:
			virtual void _resizeEvent(const ResizeEvent& event);

			virtual void _mouseEvent(const MouseEvent& event);

			virtual void _paintEvent(const PaintEvent& event);

		private:
			std::auto_ptr<Pixmap> _backbuffer;
			std::string _text;
	};

} // namespace Gui

} // namespace Pt

#endif
