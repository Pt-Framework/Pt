/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
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

#include <Pt/Text/String.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Region.h>

#include <memory>

namespace Pt {

namespace Gfx {

class DrawLine;
class DrawThinLine;
class DrawThickLine;
class DrawPolyline;
class DrawThinPolyline;
class DrawEllipse;
class DrawThinEllipse;
class DrawThickEllipse;
class FillPolygon;
class FillEllipse;
class DrawText;
class FillSolid;
class FillTexture;


/**
    \brief A painter to draw on an ARgbImage.
*/
class PT_GFX_API ImagePainter : public Painter
{
    public:

    	/** @brief Construct from target image

          All drawing operation will be performed on the given image.
          Ownership is not taken by the ImagePainter.

		  @param image The target image
		*/
        ImagePainter( ARgbImage& image );

        //! @brief Destructor.
        ~ImagePainter();

         //!@see Pt::Gfx::Painter
        virtual void setPen(const Pen& pen);

        //!@see Pt::Gfx::Painter
	    virtual const Pen& pen() const;

	    //!@see Pt::Gfx::Painter
	    virtual void setBrush(const Brush& brush);

	    //!@see Pt::Gfx::Painter
	    virtual const Brush& brush() const;

	    //!@see Pt::Gfx::Painter
	    virtual void setFont(const Font& font);

	    //!@see Pt::Gfx::Painter
	    virtual const Font& font() const;

	    //!@see Pt::Gfx::Painter
	    virtual FontMetrics fontMetrics() const;

	    //!@see Pt::Gfx::Painter
	    virtual FontMetrics fontMetrics( Text::String text) const;

	    //!@see Pt::Gfx::Painter
	    virtual const std::list<std::string>& fontFamilyNames();

	    //!@see Pt::Gfx::Painter
        virtual void drawPixel(const  Math::Point& to);

        //!@see Pt::Gfx::Painter
	    virtual void drawLine(const  Math::Point& from, const  Math::Point& to);

	    //!@see Pt::Gfx::Painter
		virtual void drawText(const  Math::Point& to, const Text::String& text,
		                      const Pt::Gfx::ARgbColor* outline = 0 );

		//!@see Pt::Gfx::Painter
	    virtual void drawRect(const  Math::Rect& rect);

	    //!@see Pt::Gfx::Painter
        virtual void fillRect(const  Math::Rect& rect);

        //!@see Pt::Gfx::Painter
	    virtual void drawEllipse(const  Math::Point& topLeft, const  Math::Size& size);

	    //!@see Pt::Gfx::Painter
	    virtual void fillEllipse(const  Math::Point& topLeft, const  Math::Size& size);

	    //!@see Pt::Gfx::Painter
        virtual void drawPolyline(const  Math::Point* points, const size_t pointCount);

        //!@see Pt::Gfx::Painter
	    virtual void fillPolygon(const  Math::Point* points, const size_t pointCount);

	    //!@see Pt::Gfx::Painter
	    virtual void drawImage(const  Math::Point& to, const ARgbImage& image);

	    //!@see Pt::Gfx::Painter
	    virtual void drawImage(const  Math::Point& to, const ARgbImage& image,
	                           const  Region& imageRegion);

    private:
        ARgbImage&           _image;
        Pen                  _pen;
        Brush                _brush;
        Font                 _font;
        DrawLine*            _drawLine;
        DrawThinLine*        _drawThinLine;
        DrawThickLine*       _drawThickLine;
        DrawPolyline*        _drawPolyline;
        DrawThinPolyline*    _drawThinPolyline;
        DrawEllipse*         _drawEllipse;
        DrawThinEllipse*     _drawThinEllipse;
        DrawThickEllipse*    _drawThickEllipse;
        FillEllipse*         _fillEllipse;
        FillPolygon*         _fillPolygon;
        DrawText*            _drawText;
        FillSolid*           _fillSolid;
        FillTexture*         _fillTexture;
};

} //namespace Gfx

} //namespace Pt

#endif
