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

#ifndef PT_GUI_PAINTER_H
#define PT_GUI_PAINTER_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/Gui/IPainter.h>
#include <Pt/Gfx/Gfx.h>
#include "Pt/Gfx/Region.h"


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
			virtual Gfx::FontMetrics fontMetrics( Text::String text) const;

			// inerhit doc
			virtual const std::list<std::string>& fontFamilyNames();
			
			// inerhit doc
			virtual void drawPixel(const Math::Point& to);

			// inerhit doc
			virtual void drawLine(const Math::Point& from, const Math::Point& to);

			// inerhit doc
            virtual void drawText( const Pt::Math::Point& to, const Pt::Text::String& Text, const Pt::Gfx::ARgbColor* outline = 0 );

			// inerhit doc
			virtual void drawRect(const Math::Rect& rect);

			// inerhit doc
			virtual void fillRect(const Math::Rect& rect);

			// inerhit doc
			virtual void drawEllipse(const Math::Point& topLeft, const Math::Size& size);

			// inerhit doc
			virtual void fillEllipse(const Math::Point& topLeft, const Math::Size& size);

			// inerhit doc
			virtual void drawPolyline(const Math::Point* points, const size_t pointCount);

			// inerhit doc
			virtual void fillPolygon(const Math::Point* points, const size_t pointCount);

			// inerhit doc
			virtual void drawImage(const Math::Point& to, const Gfx::ARgbImage& image);

			// inerhit doc
			virtual void drawImage(const Math::Point& to, const Gfx::ARgbImage& image, const Gfx::Region& imageRect);

			// inerhit doc
			virtual void drawPixmap(const Math::Point& to, Pixmap& from, const Gfx::Region& fromRect);

			// inerhit doc
			virtual void drawPixmap(const Math::Point& to, Pixmap& pm);

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
