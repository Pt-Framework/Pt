/* Copyright (C) 2015-2024 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_GFX_BLEND2D_RASTER_CONTEXT_H
#define PT_GFX_BLEND2D_RASTER_CONTEXT_H

#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Polygon.h>

#include <blend2d.h>

namespace Pt {

namespace Gfx {

class RasterContext : public PaintContext
{
    typedef BasicPoint<Pt::ssize_t> Point;
    typedef BasicSize<Pt::ssize_t> Size;
    typedef BasicRect<Pt::ssize_t> Rect;

    public:
        RasterContext();

        ~RasterContext();

        void init(BLContext& rasterContext, Image& image);

    protected:
        virtual void onBeginPaint(const Gfx::Paint& paint) override;

        virtual void onResetPaint() override;

    protected:
        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode) override;

        virtual void onApplyCompositionMode(const Gfx::CompositionMode& mode);

        virtual void onSetPen(const Gfx::Pen& pen) override;

        virtual void onApplyPen(const Gfx::Pen& pen) override;

        virtual void onSetBrush(const Gfx::Brush& brush) override;

        virtual void onApplyBrush(const Gfx::Brush& brush) override;

        virtual void onSetFont(const Gfx::Font& font) override;

        virtual void onApplyFont(const Gfx::Font& font);

        virtual void onSetClip(const Gfx::RectF* clip) override;

        virtual void onApplyClip(const Gfx::RectF* clip);

    protected:
        virtual void onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to) override;

        virtual void onDrawPolyline(const Gfx::PointF* pts, const size_t n) override;

        virtual void onFillPolygon(const Gfx::PointF* ps, const size_t n) override;

        virtual void onDrawRect(const Gfx::RectF& rectangle);

        virtual void onFillRect(const Gfx::RectF& rectangle);

        virtual void onDrawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void onFillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);
    
    protected:
        virtual void onSetPath(const Gfx::Path& path) override;
        
        virtual void onDrawPath() override;

        virtual void onFillPath() override;

        virtual void onDrawPath(const Gfx::Path& path) override;

        virtual void onFillPath(const Gfx::Path& path) override;

    protected:
        virtual Gfx::TextMetrics onGetTextMetrics(const Pt::String& text) const;

        virtual void onDrawText(const Gfx::PointF& to, 
                                const Pt::String& text, 
                                const Gfx::Transform* transform);

    protected:
        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image, 
                                 const Gfx::RectF* rect = 0);

    private:
      void putImage( const Point& to, const Image& img);

      void putImage(const Point& to, const Image& image, const Rect& rect);

      void addPath(BLPath& to, const Gfx::Path& path);

      void drawSolid(const Gfx::PointF* pts, const size_t n);

      void drawDashed(const Path& path);

      void drawDashed(const Gfx::PointF* pts, const size_t n);

    private:
        BLContext*              _context;
        Image*                  _image;
        class DrawText*         _text;
        CompositionMode         _compositionMode;
        BasicRect<Pt::ssize_t>  _currentClip;
        bool                    _hasClip;
        RectF                   _clip;
        Gfx::Pen                _pen;
        std::vector<double>     _dashPattern;
        Path                    _ptPath;
        BLPath                  _blPath;
        Gfx::Polygon            _polygon;
        std::vector<BLPoint>    _points;
};

} //namespace

} //namespace

#endif
