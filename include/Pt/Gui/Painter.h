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

#ifndef Pt_Gui_Painter_h
#define Pt_Gui_Painter_h

#include <Pt/Api.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Painter.h>
#include <cstddef>


namespace Pt {

namespace Gui {
	class Pixmap;

	class PT_EXPORT Painter : public Gfx::Painter {
		public:
			virtual void drawPixmap(const Gfx::Point& to, Pixmap& from, const Gfx::Rect& fromRect) = 0;
	};

} // namespace Gui

} // namespace Pt

#endif
