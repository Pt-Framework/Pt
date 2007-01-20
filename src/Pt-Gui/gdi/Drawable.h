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

#ifndef PT_GUI_GDI_DRAWABLE_H
#define PT_GUI_GDI_DRAWABLE_H

#include <Pt/Api.h>

#include <windows.h>

namespace Pt {

namespace Gui {

	class Drawable
	{
		public:
			virtual ~Drawable()
			{ }

			virtual HDC beginPaint() = 0;

			virtual void endPaint() = 0;

			/**
			 * @brief Returns the current device context.
			 *
			 * If there is no currently active device context, 0 is returned. The device context is
			 * not activated in this method.
			 */
			virtual HDC deviceContext() const = 0;

			/**
			 * @brief Returns the current painting state.
			 *
			 * Return the current painting state. Returns true if the drawable is currently activated
			 * for painting. It's the case when beginPaint() was called at least one time more often
			 * than endPaint(). False is returned when the drawable is not currently activated for
			 * painting.
			 *
			 * @return The current painting state (true/false).
			 */
			virtual bool isPainting() const = 0;
	};

} // namespace Gui

} // namespace Pt

#endif
