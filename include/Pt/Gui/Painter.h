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

#ifndef PT_GUI_PAINTER_H
#define PT_GUI_PAINTER_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/Gui/IPainter.h>
#include <Pt/Gfx/Gfx.h>


namespace Pt {

namespace Gui {
	class PainterImpl;

	class PT_API Painter : public IPainter
	{
		public:
			/**
			 * @brief Painter constructor which takes a pointer to the PainterImpl-object.
			 *
			 * The painter stores the given pointer of the PainterImpl-object. Also the painter
			 * is set to non-active, as it is still inactive after it was created. It is set
			 * active after begin() was called. This is usually done automatically when the
			 * Painter is used for the first time.
			 *
			 * @see begin()
			 */
			Painter(PainterImpl* painterImpl);
		
			/**
			 * @brief Ends the painting (in PainterImpl) and destructs this painter.
			 */
			virtual ~Painter();

			// inerhit doc
			virtual void setPen(const Gfx::Pen& pen);

			// inerhit doc
			virtual const Gfx::Pen& pen() const;

			// inerhit doc
			virtual void setBrush(const Gfx::Brush& brush);

			// inerhit doc
			virtual const Gfx::Brush& brush() const;

			// inerhit doc
			virtual void setFont(const Gfx::Font& font);

			// inerhit doc
			virtual const Gfx::Font& font() const;

			// inerhit doc
			virtual Gfx::FontMetrics fontMetrics() const;

			// inerhit doc
			virtual Gfx::FontMetrics fontMetrics(std::string text) const;

			// inerhit doc
			virtual const std::list<std::string>& fontFamilyNames();
			
			// inerhit doc
			virtual void drawPixel(const Gfx::Point& to);

			// inerhit doc
			virtual void drawLine(const Gfx::Point& from, const Gfx::Point& to);

			// inerhit doc
			virtual void drawText(const Gfx::Point& to, const std::string& text);

			// inerhit doc
			virtual void drawRect(const Gfx::Rect& Rect);

			// inerhit doc
			virtual void fillRect(const Gfx::Rect& rect);

			// inerhit doc
			virtual void drawEllipse(const Gfx::Point& topLeft, const Gfx::Size& size);

			// inerhit doc
			virtual void fillEllipse(const Gfx::Point& topLeft, const Gfx::Size& size);

			// inerhit doc
			virtual void drawPolyline(const Gfx::Point* points, const size_t pointCount);

			// inerhit doc
			virtual void fillPolygon(const Gfx::Point* points, const size_t pointCount);

			// inerhit doc
			virtual void drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image);

			// inerhit doc
			virtual void drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image, const Gfx::Rect& imageRect);

			// inerhit doc
			virtual void drawPixmap(const Gfx::Point& to, Pixmap& from, const Gfx::Rect& fromRect);

			// inerhit doc
			virtual void drawPixmap(const Gfx::Point& to, Pixmap& pm);

		protected:
			/**
			 * @brief Initializes the painting by calling begin() of the associated PainterImpl-object.
			 *
			 * Only active painters, which where initialized by calling begin() are ready to be drawn.
			 * If begin() was not called, the painter may throw an exception.
			 */
			void begin() const;
			
		private:
			// mutable because some getters require non-const access
			mutable PainterImpl* _painterImpl;
			mutable bool _active;

	};

} // namespace Gui

} // namespace Pt

#endif
