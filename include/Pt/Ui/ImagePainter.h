/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_UI_IMAGEPAINTER_H
#define PT_UI_IMAGEPAINTER_H

#include <Pt/String.h>
#include <Pt/Ui/Api.h>
#include <Pt/Ui/Painter.h>
#include <Pt/Ui/Pen.h>
#include <Pt/Ui/Brush.h>
#include <Pt/Ui/Region.h>
#include <Pt/Ui/Font.h>
#include <Pt/Ui/Rect.h>
#include <Pt/Ui/Region.h>
#include <Pt/Ui/ClipLine.h>
#include <Pt/Ui/ClipPolygon.h>
#include <memory>


namespace Pt {
namespace Ui {

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
class PT_UI_API ImagePainter : public Painter
{
    public:

        /** @brief Construct from target image

          All drawing operation will be performed on the given image.
          Ownership is not taken by the ImagePainter.

          @param image The target image
        */
        ImagePainter( Image& image );

        //! @brief Destructor.
        ~ImagePainter();

				virtual void setRenderMode( RenderMode::Type mode);

         //!@see Pt::Ui::Painter
        virtual void setPen(const Pen& pen);

        //!@see Pt::Ui::Painter
        virtual const Pen& pen() const;

        //!@see Pt::Ui::Painter
        virtual void setBrush(const Brush& brush);

        //!@see Pt::Ui::Painter
        virtual const Brush& brush() const;

        //!@see Pt::Ui::Painter
        virtual void setFont(const Font& font);

        //!@see Pt::Ui::Painter
        virtual const Font& font() const;

        //!@see Pt::Ui::Painter
        virtual FontMetrics fontMetrics() const;

        //!@see Pt::Ui::Painter
        virtual FontMetrics fontMetrics( Pt::String text) const;

        //!@see Pt::Ui::Painter
        virtual const std::list<std::string>& fontFamilyNames();

        //!@see Pt::Ui::Painter
        virtual void drawPixel(const  PointF& to);

        //!@see Pt::Ui::Painter
        virtual void drawLine(const  PointF& from, const  PointF& to);

        //!@see Pt::Ui::Painter
        virtual void drawText(const  PointF& to, const Pt::String& text,
                              const Color* outline = 0 );

        //!@see Pt::Ui::Painter
        virtual void drawRect(const  RectF& rect);

        //!@see Pt::Ui::Painter
        virtual void fillRect(const  RectF& rect);

        //!@see Pt::Ui::Painter
        virtual void drawEllipse(const  PointF& topLeft, const  SizeF& size);

        //!@see Pt::Ui::Painter
        virtual void fillEllipse(const  PointF& topLeft, const  SizeF& size);

        //!@see Pt::Ui::Painter
        virtual void drawPolyline(const  PointF* points, const size_t pointCount);

        //!@see Pt::Ui::Painter
        virtual void fillPolygon(const  PointF* points, const size_t pointCount);

        //!@see Pt::Ui::Painter
        virtual void drawImage(const  PointF& to, const Image& image);

        //!@see Pt::Ui::Painter
        virtual void drawImage(const  PointF& to, const Image& image,
                               const  Region& imageRegion);

				void setClip( const RectF& rect)
        {
          _clipRect = rect;
        }

				const RectF& clip() const
				{
					return _clipRect;
				}

        void setOrigin( const Ui::PointF& org )
        {
          _origin = org;
        }

        const Ui::PointF& origin() const
        {
            return _origin;
        }

    private:
      Ui::PointF fromOrigin( const Ui::PointF& p )
      {
          return _origin + p;
      }

      Ui::RectF fromOrigin( const Ui::RectF& r )
      {
        return Ui::RectF( _origin + r.origin(), r.size() );
      }


    private:
        Image&               _image;
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
				RectF                 _clipRect;
        Ui::PointF           _origin;
        ClipPolygon          _clipPolygon;
        ClipLine             _clipLine;
};

} //namespace Gfx

} //namespace Pt

#endif
