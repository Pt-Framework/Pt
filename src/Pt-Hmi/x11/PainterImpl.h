/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
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

#ifndef PT_HMI_X11_PAINTERIMPL_H
#define PT_HMI_X11_PAINTERIMPL_H

#include <Pt/Api.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Gfx/Rgb565Image.h>
#include <Pt/Gfx/Rgb555Color.h>
#include <Pt/Gfx/Region.h>
#include <Pt/Hmi/Application.h>

#include <X11/X.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

struct _XftFont;
struct _XftDraw;


namespace Pt {
namespace Hmi {

class PaintSurfaceImpl;
class NativePaintSurface;
class PaintSurface;

class PainterImpl
{
    public:
        PainterImpl(PaintSurfaceImpl* surface);

        virtual ~PainterImpl();

        void setPen(const Gfx::Pen& pen);

        const Gfx::Pen& pen() const;

        void setBrush(const Gfx::Brush& brush);

        const Gfx::Brush& brush() const;

        void setFont(const Gfx::Font& font);

        const Gfx::Font& font() const;

        Gfx::FontMetrics fontMetrics() const;

        Gfx::FontMetrics fontMetrics(Pt::String Text) const;

        const std::list<std::string>& fontFamilyNames();

        int depth() const;

        void drawPixel(const Pt::Gfx::PointF& to);

        void drawLine(const Pt::Gfx::PointF& from, const Pt::Gfx::PointF& to);

		void drawText( const Pt::Gfx::PointF& to, const Pt::String& text, const Gfx::ARgbColor* outline );

        void drawText(const Pt::Gfx::PointF& to, const Pt::String& Text);

        void drawRect(const Pt::Gfx::RectF& rectangle);

        void fillRect(const Pt::Gfx::RectF& rectangle);

        void drawEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size);

        void fillEllipse(const Pt::Gfx::PointF& topLeft, const Pt::Gfx::SizeF& size);

        void drawPolyline(const Pt::Gfx::PointF* points, const size_t pointCount);

        void fillPolygon(const Pt::Gfx::PointF* points, const size_t pointCount);

        void drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm, const Pt::Gfx::Region& pmRegion);

        void drawSurface(const Pt::Gfx::PointF& to, PaintSurface& pm);
		
        void drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image);

        void drawImage(const Pt::Gfx::PointF& to, const Gfx::ARgbImage& image, const Pt::Gfx::Region& imageRegion);

        template <typename Iterator>
        void drawImage(double xx, double yy, Iterator begin, Iterator end, double widthF, double heightF)
        {
            Pt::Gfx::Point to = Application::instance().fromUnit(Pt::Gfx::PointF(xx,yy));
            int x = to.x();
            int y = to.y();

            Pt::Gfx::Size size =  Application::instance().fromUnit(Pt::Gfx::SizeF(widthF,heightF));

            int width = size.width();
            int height = size.height();


            switch( this->depth() ) {
                case 32:
                case 24:
                {
                    Gfx::Rgb888Image rgb24Image( width, height );
                    assign( begin, end, rgb24Image.begin() );
                    this->drawImage( x, y, (char*)rgb24Image.data(), rgb24Image.width(), rgb24Image.height() );
                    break;
                }

                case 16:
                {
                    Gfx::Rgb565Image rgb16Image( width, height );
                    assign( begin, end, rgb16Image.begin() );
                    this->drawImage( x, y, (char*)rgb16Image.data(), rgb16Image.width(), rgb16Image.height() );
                    break;
                }
                case 15:
                {
                    Gfx::Rgb555Image rgb15Image( width, height );
                    assign( begin, end, rgb15Image.begin() );
                    this->drawImage( x, y, (char*)rgb15Image.data(), rgb15Image.width(), rgb15Image.height() );
                    break;
                }

                default:
                    break;
            }
        }

        void addFontName(const std::string& fontName);        
        void setSurface(NativePaintSurface& surface);

    protected:
        std::string determinePlatformDefaultFontName();
        void drawImage(size_t toX, size_t toY, const char* data, size_t width, size_t height);
        long toXColor(const Gfx::ARgbColor& color);
        ::Drawable drawable();
        void create();
        void destroy();


    protected:
        PaintSurfaceImpl*  _surface;
        Gfx::Pen _pen;
        Gfx::Brush _brush;
        Gfx::Font  _font;
        GC _penGc;
        GC _brushGc;
        _XftDraw* _xftDraw;
        _XftFont* _xftFont;
        std::list<std::string> _fontList;

    private:
};

}}

#endif
