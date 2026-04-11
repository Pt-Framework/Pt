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

#ifndef PT_GFX_RASTER_BITMAP_CANVAS_H
#define PT_GFX_RASTER_BITMAP_CANVAS_H

#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Rgb32.h>
#include "BitmapSurface.h"
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>
#include "../freetype/FreeTypeRenderer.h"

namespace Pt {

namespace Gfx {

class LineSlope;
class LineEdge;
class LineFace;
class ActiveEdgeTable;

class BitmapCanvas : public Canvas
{
    public:
        typedef Rgb32Image::pos_t   pos_t;

    public:
        BitmapCanvas();

        ~BitmapCanvas();

        void init(BitmapSurface& surface);

    protected:
        virtual void onBeginPaint(const Gfx::Paint& paint) override;

        virtual void onFinishPaint() override;

    protected:
        virtual void onSetTransform(const Gfx::Transform& tx) override;

        virtual void onApplyTransform() override;

        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode) override;

        virtual void onApplyCompositionMode() override;

        virtual void onSetPen(const Gfx::Pen& pen) override;

        virtual void onApplyPen() override;

        virtual void onSetBrush(const Gfx::Brush& brush) override;

        virtual void onApplyBrush() override;

        virtual void onSetFont(const Gfx::Font& font) override;

        virtual void onApplyFont() override;

        virtual void onSetClip(const Gfx::RectF* clip) override;

        virtual void onApplyClip() override;

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
        virtual const Gfx::FontMetrics& onGetFontMetrics() const;

        virtual Gfx::TextMetrics onGetTextMetrics(const Pt::String& text) const;

        virtual void onDrawText(const Gfx::PointF& to, 
                                const Pt::String& text, 
                                const Gfx::Transform* transform);
    
    protected:
        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image, 
                                 const Gfx::RectF* rect = 0);

    private:
        PointI toLocal(const Gfx::PointF& from) const
        {
            Gfx::PointF p = transform() * from;

            PointI pp( lround(p.x() - 0.4999),
                      lround(p.y() - 0.4999) );
            return pp;
        }

        RectI round(const RectF& r) const
        {
          PointI pos( lround(r.x()),
                     lround(r.y()) );
      
          SizeI size( lround(r.width()),
                     lround(r.height()) );
      
          return RectI(pos, size);
        }
    
        PointI round(const PointF& p) const
        {
          PointI pos( lround(p.x()),
                     lround(p.y()) );
           
          return pos;
        }

        SizeI round(const SizeF& s) const
        {
          SizeI size( lround(s.width()),
                     lround(s.height()) );
      
          return size;
        }

        void updateGradientBrush(int width, int height);
  
  private:
    void stroke(const PointI* points, size_t pointCount, const RectI& currentClip);

    void stroke(const PointI& pixel, const RectI& currentClip);

    void stroke(int x, int y, const RectI& currentClip);

    void stroke(int xpos, int ypos, int length, const RectI& currentClip);

    void clipSpan( int& x, int& y, int& length, const RectI& currentClip);

  private:
    void fill(const PointI* points, size_t pointCount, const RectI& currentClip);

    void fill( const PointI& origin, const PointI& pos, int length );

    void fillRect(const RectI& r, const RectI& currentClip);

    void fillSolid( const PointI& pos,  int length );
    
    void fillVerticalGradient( const PointI& origin, const PointI& pos,  int length );
    
    void fillHorizontalGradient( const PointI& origin, const PointI& pos,  int length );
    
    void fillTexture( const PointI& origin, const PointI& pos,  int length );

    void outputEdges(const ActiveEdgeTable& edges, const PointI& origin, int scalLine);

    void outputSpan( const PointI& topLeft, int x, int y, int width );

  private:
    void strokePolygons(const std::vector<Polygon>& polygons, const RectI& currentClip);

    void fillPolygons(const std::vector<Polygon>& polygons, const RectI& currentClip);

    void strokeEllipse( const PointI& topLeft, const SizeI& size, const RectI& currentClip);

    void fillEllipse( const PointI& topLeft, const SizeI& size, const RectI& currentClip);

  // Thin polyline
  private:
    enum { xAxis, yAxis };
    
    void drawThinSolidPolyline( const PointI* points, int pointCount, const RectI& currentClip);
    
    void drawThinDashPolyline( const PointI* points, int pointCount,
                               int dashOn, int dashOff, const RectI& currentClip);
    
    void stepDash( int dist, int* pDashNum, int* pDashIndex, const int* pDash, int numInDashList, int *pDashOffset );

    void bresenhamDasheLineSegment(int *pdashNum, int *pdashIndex, const  int *pDash, int numInDashList, int *pdashOffset, 
                                   bool isDoubleDash, int signdx, int signdy, int axis, int x1, int y1, 
                                   int e, int e1, int e2, int len, const RectI& currentClip);

    void bresenhamLineSegment( int signdx, int signdy, int axis, int x1, int y1, 
                                int e, int e1, int e2, int len, const RectI& currentClip);

  // Wide polyline base
  protected:
    int polyBuildPoly( const PointI *vertices, const LineSlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, int *h );
    
    int buildLineEdge( double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge);
    
    void fillSpans(int x, int y,  int w,  int h, const RectI& currentClip);
    
    void fillLine(int y,  int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count, const RectI& currentClip);

    void roundJoinClip( LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2 );
    
    int roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge );
    
    void lineArc( LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt, const RectI& currentClip);
    
    int lineArcI( int xorg, int yorg, std::vector<PointI>& points, std::vector<int>& widths);
    
    int lineArcD( double xorg, double yorg, std::vector<PointI>& points, std::vector<int>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2);
    
    int roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge );
    
    void lineJoin(LineFace *pLeft, LineFace *pRight, const RectI& currentClip);
    
    void lineProjectingCap(const LineFace *face, bool isLeft, bool isInt, const RectI& currentClip);
    
    void clipStepEdge( int ybase, int& xcl, int& xcr, int& edgey,  LineEdge* edge, bool edgeleft );


  // Wide solid polyline
  private:
    void drawWideSolidPolyline( const PointI* points, int pointCount, 
                                const RectI& currentClip);
    
    void drawSegment( PointI from, PointI to, bool projectLeft, bool projectRight, LineFace* leftFace, LineFace* rightFace, const RectI& currentClip);

  // Wide dashed polyline
  private:
    enum { V_TOP =  0, V_RIGHT = 1, V_BOTTOM = 2, V_LEFT = 3 };

    void drawWideDashPolyline( const PointI* points, int pointCount,
                               int dashOn, int dashOff, const RectI& currentClip );

    void dashSegment( int *pDashNum, int *pDashIndex, int *pDashOffset, int x1, int y1, int x2, int y2, 
                      bool projectLeft, bool projectRight, LineFace *leftFace, LineFace *rightFace,  int* dash, const RectI& currentClip);

    private:
        BitmapSurface* _surface;
        Rgb32Image*    _image;
        double         _lastScaleFactor;

        CompositionMode _compositionMode;
        
        Pen                     _logicalPen;
        Pen                     _pen;
        Rgb32Color              _penColor;
        Rgb32Image              _penBuffer;

        Brush                   _brush;
        Rgb32Image              _brushBuffer;
        const Rgb32Image*       _brushSource;
        bool                    _isGradient;

        Font                    _font;
        FontMetrics             _fontMetrics;
        RectF                   _clip;
        RectI                   _currentClip;
        bool                    _hasClip;

        std::vector<Polygon>    _flatPath;

        FreeTypeRenderer        _fontRenderer;
};

} //namespace

} //namespace

#endif
