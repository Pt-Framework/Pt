#include <Pt/Main.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Rect.h>
#include "Pt/Gui/Application.h"
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/Button.h>
#include <Pt/Gui/Pixmap.h>
#include <Pt/Gui/MouseEvent.h>
#include <Pt/Gui/MouseMoveEvent.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Gui/KeyEvent.h>
#include <Pt/Gui/Label.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>

#include <string>
#include <iostream>

using namespace Pt;
using namespace Pt::Gfx;
using namespace Pt::Gui;


class ScribbleWidget : public Pt::Gui::Widget
{
	public:
		ScribbleWidget()
		: Widget( )
		, _lastX(0)
		, _lastY(0)
		, _penSize(2)
		, _penColor(0, 0, 0)
		{
			Widget::setTitle(L"Scribble");

			_redButton.reset  ( new Button( *this, Math::Point(10, 10),  Math::Size(70, 30), L"RED")   );
			_greenButton.reset( new Button( *this, Math::Point(10, 45),  Math::Size(70, 30), L"GREEN")  );
			_blueButton.reset ( new Button( *this, Math::Point(10, 80),  Math::Size(70, 30), L"BLUE")  );
			_clearButton.reset( new Button( *this, Math::Point(10, 115), Math::Size(70, 30), L"CLEAR") );
			_pixmap.reset( new Pixmap( 0, 0 ) );

			updatePen();

			Gui::Painter widgetPainter = painter();
			Gui::Painter pixmapPainter = _pixmap->painter();

			// Clear the backbuffer (pixmap) with white color.
			pixmapPainter.setBrush(Brush(ARgbColor(65535, 65535, 65535)));
			pixmapPainter.fillRect( Math::Rect(Math::Point(0, 0), this->size() ) );

			_redButton->setBackgroundColor( ARgbColor(65535, 0, 0) );
			connect(_redButton->clicked, *this, &ScribbleWidget::onRedButton);

			_greenButton->setBackgroundColor( ARgbColor(0, 65535, 0) );
			connect(_greenButton->clicked, *this, &ScribbleWidget::onGreenButton);

			_blueButton->setBackgroundColor( ARgbColor(20000, 20000, 65535) );
			connect(_blueButton->clicked, *this, &ScribbleWidget::onBlueButton);

			connect(_clearButton->clicked, *this, &ScribbleWidget::onClearButton);
		}

		void onRedButton()
		{
			this->setPenColor( ARgbColor(65535, 0, 0) );
		}

		void onGreenButton()
		{
			this->setPenColor( ARgbColor(0, 65535, 0) );
		}

		void onBlueButton()
		{
			this->setPenColor( ARgbColor(0, 0, 65535) );
		}

		void onClearButton()
		{
			Brush brush(ARgbColor(65535, 65535, 65535));

			Gui::Painter widgetPainter = painter();
			Gui::Painter pixmapPainter = _pixmap->painter();

			widgetPainter.setBrush(brush);
			widgetPainter.fillRect( Math::Rect(Math::Point(0, 0), this->size() ) );

			pixmapPainter.setBrush(brush);
			pixmapPainter.fillRect( Math::Rect(Math::Point(0, 0), this->size() ) );
		}

		void setPenColor(const ARgbColor& color)
		{
			_penColor = color;
			updatePen();
		}

		void updatePen()
		{
			Pen newPen(_penSize, _penColor);

			painter().setPen(newPen);
			_pixmap->painter().setPen(newPen);
		}

	protected:
		virtual void _mouseMoveEvent(const MouseMoveEvent& event)
		{
			if (event.action() != MouseMoveEvent::Moved) {
				return;
			}

			Gui::Painter widgetPainter = painter();
			Gui::Painter pixmapPainter = _pixmap->painter();

			if( event.modifiers() & MouseMoveEvent::LeftButtonDown )
			{
				widgetPainter.drawLine( Math::Point(_lastX, _lastY), Math::Point(event.x(), event.y()) );
				pixmapPainter.drawLine( Math::Point(_lastX, _lastY), Math::Point(event.x(), event.y()) );

				_lastX = event.x();
				_lastY = event.y();
			}
		}

		virtual void _mouseEvent(const MouseEvent& event)
		{
			Gui::Painter widgetPainter = painter();
			Gui::Painter pixmapPainter = _pixmap->painter();

			if (event.action() == MouseEvent::Press && event.button() == MouseEvent::LeftButton)
			{
				_lastX = event.x();
				_lastY = event.y();
			}
		}

		virtual void _resizeEvent(const ResizeEvent& event)
		{
			if (event.resizeType() == ResizeEvent::Minimized) {
				return;
			}

			Pixmap* newPixmap = new Pixmap( event.width(), event.height() );
			Gui::Painter pixmapPainter = newPixmap->painter();

			pixmapPainter.setBrush( Brush(ARgbColor(65535, 65535, 65535)) );
			pixmapPainter.fillRect( Math::Rect( Math::Point(0, 0), newPixmap->size() ) );

			pixmapPainter.drawPixmap( Math::Point(0, 0), *_pixmap );
			_pixmap.reset(newPixmap);

			setPenColor(_penColor);
		}

		virtual void _paintEvent(const PaintEvent& event)
		{
			Gui::Painter widgetPainter = painter();
			widgetPainter.drawPixmap(event.origin(), *_pixmap, event.region());
		}

		virtual void _keyEvent(const KeyEvent& event)
		{
			if(event.text() == 'r') {
				this->setPenColor( ARgbColor(65535, 0, 0) );
			}
			else if(event.text() == 'g') {
				this->setPenColor( ARgbColor(0, 58000, 0) );
			}
			else if(event.text() == 'b') {
				this->setPenColor( ARgbColor(0, 0, 65535) );
			}
		}

	private:
		std::auto_ptr<Button> _redButton;
		std::auto_ptr<Button> _greenButton;
		std::auto_ptr<Button> _blueButton;
		std::auto_ptr<Button> _clearButton;
		std::auto_ptr<Pixmap> _pixmap;
		size_t _lastX;
		size_t _lastY;
		size_t _penSize;
		ARgbColor _penColor;
};


int main(int argc, char* argv[])
{
	try
	{
		Pt::Gui::Application app;

		ScribbleWidget widget;
		connect(widget.closed, app, &Gui::Application::exit);

		widget.show();

		return app.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
