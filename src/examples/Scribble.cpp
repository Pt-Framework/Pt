/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris D�rner                      *
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
#include <Pt/Math/Point.h>
#include <Pt/Math/Rect.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/ARgbImage.h>

#include <Pt/Gui/Widget.h>
#include <Pt/Gui/Button.h>
#include <Pt/Gui/Pixmap.h>
#include <Pt/Gui/Application.h>
#include <Pt/Gui/MouseEvent.h>
#include <Pt/Gui/MouseMoveEvent.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Gui/KeyEvent.h>

#include <string>
#include <iostream>
#include <memory>

using namespace std;
using namespace Pt;
using namespace Pt::Gfx;
using namespace Pt::Gui;


class ScribbleWidget : public Gui::Widget
{
	public:
		ScribbleWidget(const Math::Point& at, const Math::Size& size)
		: Widget( at, size ),
		  _redButton(   *this, Math::Point(0,0),  Math::Size(50, 30) ),
		  _greenButton( *this, Math::Point(0,30), Math::Size(50, 30) ),
		  _blueButton(  *this, Math::Point(0,60), Math::Size(50, 30) ),
		  _clearButton( *this, Math::Point(0,90), Math::Size(50, 30) ),
		  _pixmap( new Pixmap( size.width(), size.height() ) ),
		  _lastX(0), _lastY(0),
		  _brush( Gfx::Brush::SolidFill, Gfx::ARgbColor(0xffff,0xffff,0xffff) )
		{
			Widget::setTitle("Scribble");

			this->getPainter().setBrush(_brush);
			_pixmap->getPainter().setBrush(_brush);

			_redButton.setBackgroundColor( ARgbColor(0xffff, 0, 0) );
			connect(_redButton.clicked, *this, &ScribbleWidget::onRedButton);

			_greenButton.setBackgroundColor( ARgbColor(0, 0xffff, 0) );
			connect(_greenButton.clicked, *this, &ScribbleWidget::onGreenButton);

			_blueButton.setBackgroundColor( ARgbColor(0, 0, 0xffff) );
			connect(_blueButton.clicked, *this, &ScribbleWidget::onBlueButton);

			_clearButton.setBackgroundColor( ARgbColor(0xffff, 0xffff, 0xffff) );
			_clearButton.setText("Clear");
			connect(_clearButton.clicked, *this, &ScribbleWidget::onClear);

			this->onClear();
		}

		void onRedButton()
		{ this->setPenColor( ARgbColor(0xffff, 0, 0) ); }

		void onGreenButton()
		{ this->setPenColor(  ARgbColor(0, 0xffff, 0) ); }

		void onBlueButton()
		{ this->setPenColor( ARgbColor(0, 0, 0xffff) ); }

		void onClear()
		{
			_pixmap->getPainter().fillRect( Math::Rect( Math::Point(0, 0), _pixmap->size() ) );
			this->getPainter().fillRect( Math::Rect( Math::Point(0, 0), _pixmap->size() ) );
		}

		void setPenColor( const ARgbColor& color)
		{
			Pen pen = this->getPainter().pen();
			pen.setColor(color);

			this->getPainter().setPen( pen );
			_pixmap->getPainter().setPen( pen );
		}

	protected:
		virtual void _mouseEvent(const MouseEvent& event)
		{
			if( event.button() & MouseEvent::WheelUp && event.action() & MouseEvent::Release )
			{
				size_t size = this->getPainter().pen().size();
				if( size == std::numeric_limits<size_t>::max() ) return;

				Pen pen = this->getPainter().pen();
				pen.setSize(size + 1);

				this->getPainter().setPen( pen );
				_pixmap->getPainter().setPen( pen );
			}
			else if( event.button() & MouseEvent::WheelDown && event.action() & MouseEvent::Release)
			{
				size_t size = this->getPainter().pen().size();
				if(size == 1) return;

				Pen pen = this->getPainter().pen();
				pen.setSize(size -1);

				this->getPainter().setPen( pen );
				_pixmap->getPainter().setPen( pen );
			}
		}

		virtual void _mouseMoveEvent(const MouseMoveEvent& event)
		{
			if( event.modifiers() & MouseMoveEvent::LeftButtonDown )
			{
				this->getPainter().drawLine( Math::Point(_lastX, _lastY), Math::Point(event.x(), event.y()) );
				_pixmap->getPainter().drawLine( Math::Point(_lastX, _lastY), Math::Point(event.x(), event.y()) );
			}

			_lastX = event.x();
			_lastY = event.y();
		}

		virtual void _resizeEvent(const ResizeEvent& event)
		{
			auto_ptr<Pixmap> pm( new Pixmap( event.width(), event.height() ) );
			pm->getPainter().setBrush(_brush);
			pm->getPainter().setPen(_pen);
			pm->getPainter().fillRect( Math::Rect( Math::Point(0, 0), pm->size() ) );
			pm->getPainter().drawPixmap(Math::Point(0, 0), *_pixmap, Math::Rect( Math::Point(0, 0), _pixmap->size()) );
			_pixmap = pm;
		}

		virtual void _paintEvent(const PaintEvent& event)
		{
			this->getPainter().drawPixmap( event.origin(), *_pixmap, event.rect() );
		}

		virtual void _keyEvent(const KeyEvent& event)
		{
			if(event.text() == 'r') {
				this->setPenColor( ARgbColor(0xffff, 0, 0) );
			}
			else if(event.text() == 'g') {
				this->setPenColor( ARgbColor(0, 0xffff, 0) );
			}
			else if(event.text() == 'b') {
				this->setPenColor( ARgbColor(0, 0, 0xffff) );
			}
		}

	private:
		Button _redButton;
		Button _greenButton;
		Button _blueButton;
		Button _clearButton;
		auto_ptr<Pixmap> _pixmap;
		size_t _lastX;
		size_t _lastY;
		Pen _pen;
		Brush _brush;
};


int main()
{
	try
	{
		Gui::Application app;

		ScribbleWidget widget( Math::Point(100, 100), Math::Size(500, 500) );
		connect( widget.closed, app, &Gui::Application::exit );
		widget.show();

		return app.run();
	}
	catch(const std::exception& e)
	{
		cerr << "Exception: " << e.what() << endl;
		return 1;
	}

	return 0;
}
