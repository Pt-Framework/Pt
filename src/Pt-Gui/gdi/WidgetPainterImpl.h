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

#ifndef Pt_Gui_WidgetPainterImpl_h
#define Pt_Gui_WidgetPainterImpl_h

#include <Pt/Api.h>


namespace Pt {

namespace Gui {
	class WidgetImpl;

	class PT_EXPORT WidgetPainterImpl {
		public:
			WidgetPainterImpl(WidgetImpl& wimpl)
			{}

			void setWidget(WidgetImpl& pixmap)
			{}

			void setForegroundColor(const Gfx::ARgbColor& color)
			{}

			void setBackgroundColor(const Gfx::ARgbColor& color)
			{}

			int depth() const
			{ return 0; }

			void drawPixel(const Gfx::Point& to)
			{}

			void drawLine(const Gfx::Point& from, const Gfx::Point& to)
			{}

			void fillRect(const Gfx::Rect& rect)
			{}

			void drawPixmap(const Gfx::Point& to, Pixmap& pm, const Gfx::Rect& pmRect)
			{}

			void drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image)
			{}

			void drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image, const Gfx::Rect& imageRect)
			{}

			void drawText(const Gfx::Point& from, const char* text, short angle)
			{}
	};

} // namespace Gui

} // namespace Ptv

#endif
