/***************************************************************************
 *   Copyright (C) 2006 Marc Boris D�rner                                  *
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

#ifndef PT_GUI_GDI_PAINTERIMPL_H
#define PT_GUI_GDI_PAINTERIMPL_H

#include <windows.h>

#include <Pt/Api.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/XRgb8888Image.h>
#include <Pt/Gfx/Rgb565Image.h>
#include <Pt/Gfx/XRgb1555Color.h>

#include "Drawable.h"


namespace Pt {

namespace Gui {

	class WidgetImpl;
	class Pixmap;

	class PT_EXPORT PainterImpl {
		public:
			PainterImpl(Drawable& drawable);

			virtual ~PainterImpl();

			virtual void begin() = 0;

			virtual void end() = 0;

			void setPen(const Gfx::Pen& pen);

			const Gfx::Pen& pen() const;

			void setBrush(const Gfx::Brush& brush);

			const Gfx::Brush& brush() const;

			void setFont(const Gfx::Font& font);

			const Gfx::Font& font() const;

			Gfx::FontMetrics fontMetrics() const;

			Gfx::FontMetrics fontMetrics(std::string text) const;

			const std::list<std::string>& fontFamilyNames();

			int depth() const;

			void drawPixel(const Math::Point& to);

			void drawLine(const Math::Point& from, const Math::Point& to);

			void drawText(const Math::Point& to, const std::string& text);

			void drawRect(const Math::Rect& rectangle);

			void fillRect(const Math::Rect& rectangle);

			void drawEllipse(const Math::Point& topLeft, const Math::Size& size);

			void fillEllipse(const Math::Point& topLeft, const Math::Size& size);

			void drawPolyline(const Math::Point* points, const size_t pointCount) const;

			void fillPolygon(const Math::Point* points, const size_t pointCount) const;

			void drawPixmap(const Math::Point& to, Pixmap& pm, const Math::Rect& pmRect);

			void drawPixmap(const Math::Point& to, Pixmap& pm);

			void drawImage(const Math::Point& to, const Gfx::ARgbImage& image);

			void drawImage(const Math::Point& to, const Gfx::ARgbImage& image, const Math::Rect& imageRect);

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
						Gfx::XRgb8888Image rgb32Image(width, height);
						assign(begin, end, rgb32Image.begin());
						drawCompatibleImage(x, y, depth(), (char*)rgb32Image.data(), rgb32Image.width(), rgb32Image.height());
						break;
					}

					case 16:
					case 15: {
						Gfx::XRgb1555Image rgb16Image(width, height);
						assign(begin, end, rgb16Image.begin());
						drawCompatibleImage(x, y, 16, (char*)rgb16Image.data(), rgb16Image.width(), rgb16Image.height());
						break;
					}

					default: { // Below 16 bit or anything inbetween
						Gfx::XRgb8888Image rgb32Image(width, height);
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
					throw LogicError("Painter is not currently active. Use painter() to activate painter.", PT_SOURCEINFO);
				}
			}

/*			#ifdef _WIN32_WCE
				static int CALLBACK PainterImpl::EnumFontsProc(LOGFONT *logFont, TEXTMETRIC *physFont, DWORD type, LPARAM param);
			#else
				static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *logFont, NEWTEXTMETRICEX *physFont, DWORD type, LPARAM param);
			#endif*/

		protected:
			Drawable&  _drawable;

			Gfx::Pen   _pen;
			Gfx::Brush _brush;
			Gfx::Font  _font;

			std::list<std::string> _fontNamesList;
	};

} // namespace Gui

} // namespace Pt

#endif
