/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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
#ifndef PT_GFX_IMAGE_PAINTER_H
#define PT_GFX_IMAGE_PAINTER_H

#include <memory>
#include <Pt/Api.h>
#include <Pt/Text/String.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Region.h>


namespace Pt{
namespace Gfx{

class DrawLine;
class DrawThinLine;
class DrawThickLine;

class DrawPolyline;
class DrawThinPolyline;
class DrawThickPolyline;

class FillConvexPolygon;
class DrawText;

class PT_API ImagePainter : public Painter
{
    public:
        ImagePainter( ARgbImage& image );
        ~ImagePainter();

        virtual void setPen(const Pen& pen);
	    virtual const Pen& pen() const;
	    virtual void setBrush(const Brush& brush);
	    virtual const Brush& brush() const;
	    virtual void setFont(const Font& font);
	    virtual const Font& font() const;
	    virtual FontMetrics fontMetrics() const;
	    virtual FontMetrics fontMetrics( Text::String text) const;
	    virtual const std::list<std::string>& fontFamilyNames();
        virtual void drawPixel(const  Math::Point& to);
	    virtual void drawLine(const  Math::Point& from, const  Math::Point& to);
		virtual void drawText(const  Math::Point& to, const Text::String& text);
	    virtual void drawRect(const  Math::Rect& rect);
        virtual void fillRect(const  Math::Rect& rect);

	    inline void drawCircle(const  Math::Point& topLeft, size_t diameter)
	    {
    	    drawEllipse(topLeft, Math::Size(diameter, diameter));
	    }

	    inline void fillCircle(const Math::Point& topLeft, size_t diameter)
	    {
		    fillEllipse( topLeft, Math::Size(diameter, diameter));
	    }

	    virtual void drawEllipse(const  Math::Point& topLeft, const  Math::Size& size);
	    virtual void fillEllipse(const  Math::Point& topLeft, const  Math::Size& size);
        virtual void drawPolyline(const  Math::Point* points, const size_t pointCount);
	    virtual void fillPolygon(const  Math::Point* points, const size_t pointCount);
	    virtual void drawImage(const  Math::Point& to, const ARgbImage& image);
	    virtual void drawImage(const  Math::Point& to, const ARgbImage& image, const  Region& imageRegion);


    private:
        void drawGlyph( int penX, int penY, const Pt::uint8_t* bitmap, Pt::uint32_t bmWidth, Pt::uint32_t bmHeight, Pt::uint32_t bmPitch );

        void mixColor( ARgbColor& dst, ARgbColor src, float factor)
        {
            dst *= ( 1.0f - factor );
            dst += ( src *= factor );
        }

        ARgbImage&                    		_image;
        Pen                                 _pen;
        Brush                               _brush;

        DrawLine*                           _drawLine;
        std::auto_ptr<DrawThinLine>         _drawThinLine;
        std::auto_ptr<DrawThickLine>        _drawThickLine;

        DrawPolyline*                       _drawPolyline;
        std::auto_ptr<DrawThinPolyline>     _drawThinPolyline;
        std::auto_ptr<DrawThickPolyline>    _drawThickPolyline;
        std::auto_ptr<FillConvexPolygon>    _fillConvexPolygon;
        std::auto_ptr<DrawText>             _drawText;
};

}
}

#endif
