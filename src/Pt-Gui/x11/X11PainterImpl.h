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

#ifndef Pt_X11PainterImpl_h
#define Pt_X11PainterImpl_h

#include <Pt/Api.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/XRgb8888Image.h>
#include <Pt/Gfx/XRgb1555Image.h>
#include <Pt/Gfx/Rgb565Image.h>
#include <Pt/Gui/Painter.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

struct _XftDraw;


namespace Pt {

namespace Gui {

	class Drawable;
	class Pixmap;

	class X11Painter : public Gui::Painter {
		public:
			X11Painter(Gui::Drawable& drawable);

			~X11Painter();

			void setDrawable(Gui::Drawable& drawable);

			Gui::Drawable& drawable() const;

			void setPen(const Gfx::Pen& pen);

			const Gfx::Pen& pen() const;

			void setBrush(const Gfx::Brush& brush);

			const Gfx::Brush& brush() const;

			void setFont(const Gfx::Font& font)
			{}

			const Gfx::Font& font() const
			{ return _font; }

			int depth() const;

			void drawPixel(const Gfx::Point& to);

			void drawLine(const Gfx::Point& from, const Gfx::Point& to);

			void drawText(const Gfx::Point& to, const char* text, short angle);

			void fillRect(const Gfx::Rect& rect);

			void drawPixmap(const Gfx::Point& to, Pixmap& pm, const Gfx::Rect& pmRect);

			void drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image);

			void drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image, const Gfx::Rect& imageRect);

			template <typename Iterator>
			void drawImage(size_t x, size_t y, Iterator begin, Iterator end, size_t width, size_t height)
			{
				switch( this->depth() ) {
					case 32:
					case 24: {
						Gfx::XRgb8888Image rgb24Image( width, height );
						assign( begin, end, rgb24Image.begin() );
						this->drawImage( x, y, (char*)rgb24Image.data(), rgb24Image.width(), rgb24Image.height() );
						break;
					}

					case 16: {
						Gfx::Rgb565Image rgb16Image( width, height );
						assign( begin, end, rgb16Image.begin() );
						this->drawImage( x, y, (char*)rgb16Image.data(), rgb16Image.width(), rgb16Image.height() );
						break;
					}
					case 15: {
						Gfx::XRgb1555Image rgb15Image( width, height );
						assign( begin, end, rgb15Image.begin() );
						this->drawImage( x, y, (char*)rgb15Image.data(), rgb15Image.width(), rgb15Image.height() );
						break;
					}
				}
			}

		protected:
			long toXColor(const Gfx::ARgbColor& color);

			void drawImage(size_t toX, size_t toY, const char* data, size_t width, size_t height);

		private:
			Gui::Drawable* _drawable;
			Gfx::Pen _pen;
			Gfx::Brush _brush;
			Gfx::Font  _font;
			GC _penGc;
			GC _brushGc;
			_XftDraw* _xftDraw;
	};

} // namespace Gui

} // namespace Pt

#endif
