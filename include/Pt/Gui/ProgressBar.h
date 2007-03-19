/***************************************************************************
 *   Copyright (C) 2006-2007 Tobias Mueller                                *
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

#ifndef PT_GUI_PROGRESSBAR_H
#define PT_GUI_PROGRESSBAR_H

#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/Widget.h>


namespace Pt {

namespace Gui {

	class PT_GUI_API ProgressBar : public Widget
	{
		public:
			ProgressBar(Widget& parent, const Math::Point& at, const Math::Size& size);

			//! @brief Empty destructor.
			~ProgressBar();

			virtual void update();
			
			void setMinimum(ssize_t minimum);
			
			ssize_t minimum() const;
			
			void setMaximum(ssize_t maximum);
			
			ssize_t maximum() const;
			
			void setValue(ssize_t value);

			ssize_t value() const;
			
			float percentage() const;
			
			void setBlockWidth(size_t width);
			
			size_t blockWidth() const;

		protected:
			//! @brief Does a repaint of the widget.
			virtual void _resizeEvent(const ResizeEvent& event);

			//! @brief Does a repaint of the widget.
			virtual void _paintEvent(const PaintEvent& event);
			
        private:
            ssize_t _minimum;
            ssize_t _maximum;
            ssize_t _value;
            size_t  _blockWidth;
	};

} // namespace Gui

} // namespace Pt

#endif
