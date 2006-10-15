/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Drner                                  *
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
#include <X11/Xft/Xft.h>

#include "X11PainterImpl.h"
#include "ApplicationImpl.h"
#include "PixmapImpl.h"

#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Gui {


X11Painter::X11Painter(Gui::Drawable& drawable)
: _drawable(&drawable)
, _font("", 15, Gfx::Font::NormalStyle, 0)
, _penGc(0)
, _brushGc(0)
, _xftDraw(0)
{
	Display* display = X11EventLoop::instance().display();
	unsigned int screen = DefaultScreen(display);
	Visual* visual = XDefaultVisual(display, screen);

	_penGc = XCreateGC( display,
	                   _drawable->x11Drawable(),
	                   0,
	                   NULL);

	_brushGc = XCreateGC( display,
	                     _drawable->x11Drawable(),
	                     0,
	                     NULL);

	_xftDraw = XftDrawCreate( display,
	                          _drawable->x11Drawable(),
	                          visual,
	                          DefaultColormap(display, screen) );

	XSync(display, false);
}


X11Painter::~X11Painter()
{
	Display* display = X11EventLoop::instance().display();

	XFreeGC(display, _penGc);
	_penGc = 0;

	XFreeGC(display, _brushGc);
	_brushGc = 0;

	XftDrawDestroy(_xftDraw);
	_xftDraw = 0;
}


void X11Painter::setPen(const Gfx::Pen& pen)
{
	if( _pen.color() != pen.color() ) {
		Display* display = X11EventLoop::instance().display();
		XSetForeground( display, _penGc, this->toXColor( pen.color() ) );
		XSetBackground( display, _penGc, this->toXColor( pen.color() ) );
		_pen.setColor( pen.color() );
	}

	if( _pen.size() != pen.size() ) {
		int lineStyle = LineSolid;
		int capStyle = CapRound;
		int joinStyle = JoinMiter;

		switch( _pen.lineStyle() ) {
			case Gfx::Pen::SolidLine: lineStyle = LineSolid; break;
			case Gfx::Pen::DashedLine: lineStyle = LineOnOffDash; break;
			case Gfx::Pen::DottedLine: lineStyle = LineDoubleDash; break;
		}

		switch( _pen.joinStyle() ) {
			case Gfx::Pen::BevelJoin: joinStyle = JoinBevel; break;
			case Gfx::Pen::RoundJoin: joinStyle = JoinRound; break;
			case Gfx::Pen::MiterJoin: joinStyle = JoinMiter; break;
		}

		switch( _pen.capStyle() ) {
			case Gfx::Pen::SquareCap: capStyle = CapButt; break;
			case Gfx::Pen::RoundCap: capStyle = CapRound; break;
			case Gfx::Pen::FlatCap: capStyle = CapProjecting; break;
		}

		Display* display = X11EventLoop::instance().display();
		XSetLineAttributes( display, _penGc, pen.size(), lineStyle, capStyle, joinStyle );

		_pen.setSize( pen.size() );
	}
}


const Gfx::Pen& X11Painter::pen() const
{
	return _pen;
}


void X11Painter::setBrush(const Gfx::Brush& brush)
{
	Display* display = X11EventLoop::instance().display();
	XSetForeground( display, _brushGc, this->toXColor( brush.color() ) );
	XSetBackground( display, _brushGc, this->toXColor( brush.color() ) );
	XSync(display, false);

	_brush = brush;
}


const Gfx::Brush& X11Painter::brush() const
{
	return _brush;
}


void X11Painter::setDrawable(Gui::Drawable& drawable)
{
	_drawable = &drawable;
	XftDrawChange( _xftDraw, _drawable->x11Drawable() );
}


Gui::Drawable& X11Painter::drawable() const
{
	return *_drawable;
}


int X11Painter::depth() const
{
	Display* display = X11EventLoop::instance().display();
	return XDefaultDepth(display, XDefaultScreen(display));
}


void X11Painter::drawPixel(const Gfx::Point& to)
{
	Display* display = X11EventLoop::instance().display();
	XDrawPoint( display, _drawable->x11Drawable(), _penGc, to.x(), to.y() );
	XSync(display, false);
}


void X11Painter::drawLine(const Gfx::Point& from, const Gfx::Point& to)
{
	Display* display = X11EventLoop::instance().display();
	XDrawLine(display, _drawable->x11Drawable(), _penGc, from.x(), from.y(), to.x(), to.y());
	XSync(display, false);
}


void X11Painter::drawText(const Gfx::Point& to, const char* text, short angle)
{
	Display* display = X11EventLoop::instance().display();
	unsigned int screen = DefaultScreen(display);
	//Visual* visual = XDefaultVisual(display, screen);

	double sina, cosa;
	angle = angle % 3600;
	switch(angle) {
		case    0: sina =  0; cosa =  1; break;
		case  900: sina =  1; cosa =  0; break;
		case 1800: sina =  0; cosa = -1; break;
		case 2700: sina = -1; cosa =  0; break;
		default:
			double a = angle * 3.14 / 1800.0;
			sina = sin(a);
			cosa = cos(a);
			break;
	}

	XftMatrix mx;
	mx.xx = cosa;
	mx.xy = -sina;
	mx.yx = sina;
	mx.yy = cosa;

	XftFont* font = XftFontOpen(display, screen,
	                            XFT_MATRIX, XftTypeMatrix, &mx,
	                            XFT_SIZE, XftTypeDouble, 12.0,
	                            NULL);
	//XftFont* font = XftFontOpenName(display, screen, "Luxi Sans");

	//FcChar8* family;
	//FcPatternGetString(font->pattern, FC_FAMILY, 0, &family);
    //printf("opened: %s\n", family);

	XftColor xftColor;
	xftColor.pixel = 0; // this would be input for XftColorAllocValue
	xftColor.color.red = 0;
	xftColor.color.green = 0;
	xftColor.color.blue = 0;
	xftColor.color.alpha = 0xffff;
	//Bool b = XftColorAllocValue (display, visual, DefaultColormap(display, screen), &rc, &xftColor);

	XftDrawString8(_xftDraw, &xftColor, font, to.x(), to.y(), (const FcChar8*)text, std::char_traits<char>::length(text));
	XSync(display, false);

	XftFontClose(display, font);
}


void X11Painter::fillRect(const Gfx::Rect& rect)
{
	Display* display = X11EventLoop::instance().display();
	XFillRectangle(display, _drawable->x11Drawable(), _brushGc, rect.x1(), rect.y1(), rect.width(), rect.height());
	XSync(display, false);
}


void X11Painter::drawPixmap(const Gfx::Point& to, Pixmap& pm, const Gfx::Rect& pmRect)
{
	Display* display = X11EventLoop::instance().display();
	::Pixmap from = pm.impl().x11Drawable();

	XCopyArea( display,
	           from,
	           _drawable->x11Drawable(),
	           _brushGc,
	           pmRect.x1(), pmRect.y1(), pmRect.width(), pmRect.height(),
	           to.x(), to.y() );

	XSync(display, false);
}


void X11Painter::drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image)
{
	this->drawImage( to.x(), to.y(), image.begin(), image.end(), image.width(), image.height() );
}


void X11Painter::drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image, const Gfx::Rect& imageRect)
{
	Gfx::ARgbSubImage subImage(const_cast<Gfx::ARgbImage&>( image ), imageRect);
	this->drawImage( to.x(), to.y(), subImage.begin(), subImage.end(), subImage.width(), subImage.height() );
}


long X11Painter::toXColor(const Gfx::ARgbColor& color)
{
	switch( this->depth() ) {
		case 32:
		case 24:
		{
			Gfx::ARgb8888Color rgb8888 = color;
			return reinterpret_cast<uint32_t&>(rgb8888);
		}

		case 16:
		{
			Gfx::Rgb565Color rgb565 = color;
			return reinterpret_cast<uint16_t&>(rgb565);
		}
	}

	return 0;
}


void X11Painter::drawImage(size_t x, size_t y, const char* data, size_t width, size_t height)
{
	Display* display = X11EventLoop::instance().display();
	unsigned int screen = DefaultScreen(display);
	Visual* visual = XDefaultVisual(display, screen);
	int depth = XDefaultDepth(display, screen);

	XImage* ximage = XCreateImage(display, visual, depth, ZPixmap, 0, NULL, width, height, 8, 0);
	ximage->data = (char*)data;
	XPutImage(display, _drawable->x11Drawable(), _brushGc, ximage, 0, 0, x, y, width, height);

	XSync(display, false);
	ximage->data = NULL;
	XDestroyImage(ximage);
}


} // namespace Gui

} // namespace Ptv

