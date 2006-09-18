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

#ifndef Pt_Gui_gdi_PixmapImpl_h
#define Pt_Gui_gdi_PixmapImpl_h


#include <Pt/Api.h>


namespace Pt {

namespace Gui {

	class PT_EXPORT PixmapImpl {
		public:
			PixmapImpl(size_t width, size_t height)
			{}

			virtual ~PixmapImpl()
			{}

			size_t width() const
			{ return 0; }

			size_t height() const
			{ return 0; }

			const Gfx::Size& size() const
			{ return _size; }
		private:
			Gfx::Size _size;
	};

} // namespace Gui

} // namespace Pt

#endif
