/*
 * Copyright (C) 2006-2007 Tobias Mueller
 * Copyright (C) 2006-2007 PTV AG
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

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
			ProgressBar(Widget& parent, const Gfx::Point& at, const Gfx::Size& size);

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
