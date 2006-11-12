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

#ifndef PTV_GUI_GDI_PAINTERIMPL_H
#define PTV_GUI_GDI_PAINTERIMPL_H

#include <windows.h>

#include <ptv/Api.h>
#include <ptv/gfx/Font.h>
#include <ptv/gfx/gfx.h>
#include <ptv/gui/Painter.h>
#include <ptv/gfx/Pen.h>
#include <ptv/gfx/Brush.h>
#include <ptv/gfx/Algorithm.h>
#include <ptv/gfx/ARgbImage.h>
#include <ptv/gfx/XRgb8888Image.h>
#include <ptv/gfx/Rgb565Image.h>
#include <ptv/gfx/XRgb1555Color.h>

#include "Drawable.h"


namespace ptv {

namespace gui {

	class WidgetImpl;
	class Pixmap;

	class PTV_EXPORT PainterImpl {
		public:
			PainterImpl(Drawable& drawable);

			virtual ~PainterImpl();

			virtual void begin() = 0;
			
			virtual void end() = 0;

			void setPen(const gfx::Pen& pen);

			const gfx::Pen& pen() const;

			void setBrush(const gfx::Brush& brush);

			const gfx::Brush& brush() const;

			void setFont(const gfx::Font& font);

			const gfx::Font& font() const;

			gfx::FontMetrics fontMetrics() const;

			gfx::FontMetrics fontMetrics(std::string text) const;

			const std::list<std::string>& fontFamilyNames();
			
			int depth() const;

			void drawPixel(const gfx::Point& to);

			void drawLine(const gfx::Point& from, const gfx::Point& to);

			void drawText(const gfx::Point& to, const std::string& text);

			void drawRect(const gfx::Rect& rectangle);

			void fillRect(const gfx::Rect& rectangle);

			void drawEllipse(const gfx::Point& topLeft, const gfx::Size& size);

			void fillEllipse(const gfx::Point& topLeft, const gfx::Size& size);

			void drawPolyline(const gfx::Point* points, const size_t pointCount) const;

			void fillPolygon(const gfx::Point* points, const size_t pointCount) const;

			void drawPixmap(const gfx::Point& to, Pixmap& pm, const gfx::Rect& pmRect);

			void drawPixmap(const gfx::Point& to, Pixmap& pm);

			void drawImage(const gfx::Point& to, const gfx::ARgbImage& image);

			void drawImage(const gfx::Point& to, const gfx::ARgbImage& image, const gfx::Rect& imageRect);

			template <typename Iterator>
			void drawImage(size_t x, size_t y, Iterator begin, Iterator end, size_t width, size_t height)
			{
				// Try to Convert our generic image format (ARgbImage) to an image format that is compatible
				// with the current device settings. If this is not possible, convert it to a 32-bit
				// device-independent image that windows has to convert to the current device settings
				// when we bit-blit it.
				switch (depth()) {
					case 32:
					case 24: {
						gfx::XRgb8888Image rgb32Image(width, height);
						assign(begin, end, rgb32Image.begin());
						drawCompatibleImage(x, y, depth(), (char*)rgb32Image.data(), rgb32Image.width(), rgb32Image.height());
						break;
					}

					case 16:
					case 15: {
						gfx::XRgb1555Image rgb16Image(width, height);
						assign(begin, end, rgb16Image.begin());
						drawCompatibleImage(x, y, 16, (char*)rgb16Image.data(), rgb16Image.width(), rgb16Image.height());
						break;
					}

					default: { // Below 16 bit or anything inbetween
						gfx::XRgb8888Image rgb32Image(width, height);
						assign(begin, end, rgb32Image.begin());
						drawIndependentImage(x, y, (char*)rgb32Image.data(), rgb32Image.width(), rgb32Image.height());
						break;
					}
				}
			}

			void addFontName(const std::string& fontName);

		protected:
			void drawCompatibleImage(size_t x, size_t y, size_t depth, const char* data, size_t width, size_t height);
			void drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height);

			std::string determinePlatformDefaultFontName();

			void updatePen();
			void updateFont();
			void updateBrush();

			void ensureActivePainter() const
			{
				if (!_drawable.isPainting()) {
					throw LogicError("Painter is not currently active. Use painter() to activate painter.", PTV_SOURCEINFO);
				}
			}

/*			#ifdef _WIN32_WCE
				static int CALLBACK PainterImpl::EnumFontsProc(LOGFONT *logFont, TEXTMETRIC *physFont, DWORD type, LPARAM param);
			#else
				static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *logFont, NEWTEXTMETRICEX *physFont, DWORD type, LPARAM param);
			#endif*/

		protected:
			Drawable&  _drawable;

			gfx::Pen   _pen;
			gfx::Brush _brush;
			gfx::Font  _font;

			std::list<std::string> _fontNamesList;
	};

} // namespace gui

} // namespace ptv

#endif
