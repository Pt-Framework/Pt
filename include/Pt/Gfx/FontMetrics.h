/***************************************************************************
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
#ifndef PT_GFX_FONTMETRICS_H
#define PT_GFX_FONTMETRICS_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>


namespace Pt {

namespace Gfx {

	class PT_GFX_API FontMetrics
	{
		public:
			FontMetrics(size_t ascent, size_t descent, size_t width, size_t height);

			size_t ascent() const;

			size_t descent() const;

			size_t width() const;

			size_t height() const;

		private:
			size_t _ascent;
			size_t _descent;
			size_t _width;
			size_t _height;
	};

} // namespace Gfx

} // namespace Pt

#endif
