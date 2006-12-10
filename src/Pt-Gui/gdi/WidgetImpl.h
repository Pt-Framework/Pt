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

#ifndef Pt_Gui_WidgetImpl_h
#define Pt_Gui_WidgetImpl_h

#include <Pt/Api.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Rect.h>
#include <Pt/Gui/Painter.h>
#include "WidgetPainter.h"
#include "Drawable.h"

#include <windows.h>

#include <string>
#include <list>
using namespace std;


namespace Pt {

namespace Gui {

	class Widget;
	class ResizeEvent;
	class GDIPainter;

	class PT_API WidgetImpl : public Drawable
	{
		public:
			WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at, const Math::Size& size);

			WidgetImpl(Widget& widget, Widget* parent);

			virtual ~WidgetImpl();

			void setTitle(const std::string& text);

			std::string title();

			void move(size_t x, size_t y);

			void resize(size_t width, size_t height);

			void show();

			void hide();

			void setParent(Widget* parent);

			HWND hwnd();

			Painter painter();

			virtual HDC beginPaint();

			virtual void endPaint();

			virtual HDC deviceContext() const;

			virtual bool isPainting() const;

		private:
			void init(Widget& widget, Widget* parent, const Math::Point& at, const Math::Size& size);

		private:
			HWND           _hwnd;
			HDC            _deviceContext;
			WidgetPainter* _painter;
			size_t         _deviceContextUsageCount;

			Widget&     _widget;

			DWORD       _windowStyle;

			HPEN   _oldPen;
			HBRUSH _oldBrush;
			HFONT  _oldFont;
	};

} // namespace Gui

} // namespace Pt

#endif
