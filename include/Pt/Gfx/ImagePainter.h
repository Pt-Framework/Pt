/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
 * Copyright (C) 2010 Aloysius Indrayanto
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
#ifndef PT_GFX_IMAGEPAINTER_H
#define PT_GFX_IMAGEPAINTER_H

#include <Pt/Gfx/Api.h>
#include <Pt/String.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Region.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Rect.h>
#include <memory>


namespace Pt {

namespace Gfx {

class DrawPolyline;
class DrawThinPolyline;
class DrawWideSolidPolyline;
class DrawWideDashPolyline;

class DrawEllipse;
class DrawThinEllipse;
class DrawThickEllipse;

class FillPolygon;
class FillEllipse;

class DrawText;
class FillSolid;
class FillTexture;
class Stroke;

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

				virtual void setRenderMode( Gfx::RenderMode::Type mode);

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
        virtual FontMetrics fontMetrics( Pt::String text) const;

        //!@see Pt::Gfx::Painter
        virtual const std::list<std::string>& fontFamilyNames();

        //!@see Pt::Gfx::Painter
        virtual void drawPixel(const  Gfx::PointF& to);

        //!@see Pt::Gfx::Painter
        virtual void drawLine(const  Gfx::PointF& from, const  Gfx::PointF& to);

        //!@see Pt::Gfx::Painter
        virtual void drawText(const  Gfx::PointF& to, const Pt::String& text,
                              const Pt::Gfx::ARgbColor* outline = 0 );

        //!@see Pt::Gfx::Painter
        virtual void drawRect(const  Gfx::RectF& rect);

        //!@see Pt::Gfx::Painter
        virtual void fillRect(const  Gfx::RectF& rect);

        //!@see Pt::Gfx::Painter
        virtual void drawEllipse(const  Gfx::PointF& topLeft, const  Gfx::SizeF& size);

        //!@see Pt::Gfx::Painter
        virtual void fillEllipse(const  Gfx::PointF& topLeft, const  Gfx::SizeF& size);

        //!@see Pt::Gfx::Painter
        virtual void drawPolyline(const  Gfx::PointF* points, const size_t pointCount);

        //!@see Pt::Gfx::Painter
        virtual void fillPolygon(const  Gfx::PointF* points, const size_t pointCount);

        //!@see Pt::Gfx::Painter
        virtual void drawImage(const  Gfx::PointF& to, const ARgbImage& image);

        //!@see Pt::Gfx::Painter
        virtual void drawImage(const  Gfx::PointF& to, const ARgbImage& image,
                               const  Region& imageRegion);

    private:
        ARgbImage&           _image;
        Pen                  _pen;
        Brush                _brush;
        Font                 _font;

        DrawPolyline*           _drawPolyline;
        DrawThinPolyline*       _drawThinPolyline;
        DrawWideSolidPolyline*  _drawWideSolidPolyline;
        DrawWideDashPolyline*   _drawWideDashPolyline;

        DrawEllipse*         _drawEllipse;
        DrawThinEllipse*     _drawThinEllipse;
        DrawThickEllipse*    _drawThickEllipse;
        FillEllipse*         _fillEllipse;
        FillPolygon*         _fillPolygon;
        DrawText*            _drawText;
        Stroke*              _stroke;
        FillSolid*           _fillSolid;
        FillTexture*         _fillTexture;
				RenderMode::Type     _renderMode;
};

} //namespace Gfx

} //namespace Pt

#endif
