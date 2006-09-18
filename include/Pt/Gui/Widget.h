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

#ifndef Pt_Gui_Widget_h
#define Pt_Gui_Widget_h

#include <Pt/Api.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>
#include <Pt/NonCopyable.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/ARgbColor.h>
#include <cstddef>


namespace Pt {

namespace Gui {

	class Event;
	class CloseEvent;
	class MouseEvent;
	class MouseMoveEvent;
	class MoveEvent;
	class PaintEvent;
	class ResizeEvent;
	class KeyEvent;
	class Painter;

	class PT_EXPORT Widget : public Connectable, public NonCopyable {
		private:
			class WidgetImpl* _impl;

		public:
			Widget(const Gfx::Point& at = Gfx::Point(0, 0), const Gfx::Size& s = Gfx::Size(100, 100));

			Widget(Widget& parent, const Gfx::Point& at, const Gfx::Size& size);

			virtual ~Widget();

			void setTitle(const char* text);

			std::string title();

			const Gfx::ARgbColor& backgroundColor() const;

			void setBackgroundColor(const Gfx::ARgbColor& color);

			const Gfx::ARgbColor& foregroundColor() const;

			void setForegroundColor(const Gfx::ARgbColor& color);

			const Gfx::Rect& rect() const;

			const Gfx::Size& size() const;

			virtual void move(size_t x, size_t y);

			virtual void resize(size_t width, size_t height);

			virtual void show();

			virtual void hide();

			void unparent();

			void reparent(Widget* parent);

			virtual void update()
			{}

			Painter& getPainter();

			void closeEvent(const CloseEvent& event);

			void mouseEvent(const MouseEvent& event);

			void mouseMoveEvent(const MouseMoveEvent& event);

			void moveEvent(const MoveEvent& event);

			void paintEvent(const PaintEvent& event);

			void resizeEvent(const ResizeEvent& event);

			void keyEvent(const KeyEvent& event);

		public:
			Signal<> closed;

		protected:
			virtual void _closeEvent(const CloseEvent& event);

			virtual void _mouseEvent(const MouseEvent& event);

			virtual void _moveEvent(const MoveEvent& event);

			virtual void _mouseMoveEvent(const MouseMoveEvent& event);

			virtual void _paintEvent(const PaintEvent& event);

			virtual void _resizeEvent(const ResizeEvent& event);

			virtual void _keyEvent(const KeyEvent& event);

		public:
			WidgetImpl& impl()
			{ return *_impl; }

			const WidgetImpl& impl() const
			{ return *_impl; }


		private:
			Gfx::ARgbColor _foregroundColor;
			Gfx::ARgbColor _backgroundColor;
	};

} // namespace Gui

} // namespace Pt

#endif
