/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Drner                                  *
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
#ifndef Pt_Gfx_Painter_h
#define Pt_Gfx_Painter_h

#include <Pt/Api.h>

#include <cstddef>


namespace Pt {

namespace Gfx {
	class Pen;
	class Brush;
	class Font;
	class Rect;
	class RgbColor;
	class RgbImage;

	class PT_EXPORT Painter {
		public:
			virtual ~Painter()
			{}

			virtual void setPen(const Pen& pen) = 0;

			virtual const Pen& pen() const = 0;

			virtual void setBrush(const Brush& brush) = 0;

			virtual const Brush& brush() const = 0;

			void setFont(const Font& font);

			const Font& font() const;

			virtual void drawPixel(const Point& to) = 0;

			virtual void drawLine(const Point& from, const Point& to) = 0;

			virtual void drawText(const Point& from, const char* text, short angle = 0) = 0;

			virtual void fillRect(const Rect& imageRect) = 0;

			virtual void drawImage(const Point& to, const ARgbImage& image) = 0;

			virtual void drawImage(const Point& to, const ARgbImage& image, const Rect& imageRect) = 0;
	};

} // namespace Gfx

} // namespace Pt

#endif
