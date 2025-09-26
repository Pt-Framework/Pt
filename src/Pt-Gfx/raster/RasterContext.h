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

#ifndef PT_GFX_RASTER_CONTEXT_H
#define PT_GFX_RASTER_CONTEXT_H

#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Gfx {

class LineSlope;
class LineEdge;
class LineFace;
class ActiveEdgeTable;
class DrawText;

class RasterContext : public PaintContext
{
    public:
        typedef Image::pos_t       pos_t;
        typedef BasicPoint<pos_t>  Point;
        typedef BasicSize<pos_t>   Size;
        typedef BasicRect<pos_t>   Rect;

    public:
        RasterContext();

        ~RasterContext();

        void setImage(Image& image);

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

        virtual bool onDrawLayer(const Gfx::PointF& to,
                                 const Gfx::PaintLayer& layer,
                                 const Gfx::RectF* rect = 0);

    private:
        Point toLocal(const Gfx::PointF& from) const
        {
            Gfx::PointF p = transform() * from;

            Point pp( lround(p.x() - 0.4999),
                      lround(p.y() - 0.4999) );
            return pp;
        }

        Rect round(const RectF& r) const
        {
          Point pos( lround(r.x()),
                     lround(r.y()) );
      
          Size size( lround(r.width()),
                     lround(r.height()) );
      
          return Rect(pos, size);
        }
    
        Point round(const PointF& p) const
        {
          Point pos( lround(p.x()),
                     lround(p.y()) );
           
          return pos;
        }

        Size round(const SizeF& s) const
        {
          Size size( lround(s.width()),
                     lround(s.height()) );
      
          return size;
        }

        void updateGradientBrush(int width, int height);
  
  private:
    void stroke(const Point* points, size_t pointCount, const Rect& currentClip);

    void stroke(const Point& pixel, const Rect& currentClip);

    void stroke(int x, int y, const Rect& currentClip);

    void stroke(int xpos, int ypos, int length, const Rect& currentClip);

    void clipSpan( int& x, int& y, int& length, const Rect& currentClip);

  private:
    void fill(const Point* points, size_t pointCount, const Rect& currentClip);

    void fill( const Point& origin, const Point& pos, int length );

    void fillRect(const Rect& r, const Rect& currentClip);

    void fillSolid( const Point& pos,  int length );
    
    void fillVerticalGradient( const Point& origin, const Point& pos,  int length );
    
    void fillHorizontalGradient( const Point& origin, const Point& pos,  int length );
    
    void fillTexture( const Point& origin, const Point& pos,  int length );

    void outputEdges(const ActiveEdgeTable& edges, const Point& origin, int scalLine);

    void outputSpan( const Point& topLeft, int x, int y, int width );

  private:
    void strokePolygons(const std::vector<Polygon>& polygons, const Rect& currentClip);

    void fillPolygons(const std::vector<Polygon>& polygons, const Rect& currentClip);

    void strokeEllipse( const Point& topLeft, const Size& size, const Rect& currentClip);

    void fillEllipse( const Point& topLeft, const Size& size, const Rect& currentClip);

  // Thin polyline
  private:
    enum { xAxis, yAxis };
    
    void drawThinSolidPolyline( const Point* points, int pointCount, const Rect& currentClip);
    
    void drawThinDashPolyline( const Point* points, int pointCount,
                               int dashOn, int dashOff, const Rect& currentClip);
    
    void stepDash( int dist, int* pDashNum, int* pDashIndex, const int* pDash, int numInDashList, int *pDashOffset );

    void bresenhamDasheLineSegment(int *pdashNum, int *pdashIndex, const  int *pDash, int numInDashList, int *pdashOffset, 
                                   bool isDoubleDash, int signdx, int signdy, int axis, int x1, int y1, 
                                   int e, int e1, int e2, int len, const Rect& currentClip);

    void bresenhamLineSegment( int signdx, int signdy, int axis, int x1, int y1, 
                                int e, int e1, int e2, int len, const Rect& currentClip);

  // Wide polyline base
  protected:
    int polyBuildPoly( const Point *vertices, const LineSlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, int *h );
    
    int buildLineEdge( double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge);
    
    void fillSpans(int x, int y,  int w,  int h, const Rect& currentClip);
    
    void fillLine(int y,  int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count, const Rect& currentClip);

    void roundJoinClip( LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2 );
    
    int roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge );
    
    void lineArc( LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt, const Rect& currentClip);
    
    int lineArcI( int xorg, int yorg, std::vector<Point>& points, std::vector<int>& widths);
    
    int lineArcD( double xorg, double yorg, std::vector<Point>& points, std::vector<int>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2);
    
    int roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge );
    
    void lineJoin(LineFace *pLeft, LineFace *pRight, const Rect& currentClip);
    
    void lineProjectingCap(const LineFace *face, bool isLeft, bool isInt, const Rect& currentClip);
    
    void clipStepEdge( int ybase, int& xcl, int& xcr, int& edgey,  LineEdge* edge, bool edgeleft );


  // Wide solid polyline
  private:
    void drawWideSolidPolyline( const Point* points, int pointCount, 
                                const Rect& currentClip);
    
    void drawSegment( Point from, Point to, bool projectLeft, bool projectRight, LineFace* leftFace, LineFace* rightFace, const Rect& currentClip);

  // Wide dashed polyline
  private:
    enum { V_TOP =  0, V_RIGHT = 1, V_BOTTOM = 2, V_LEFT = 3 };

    void drawWideDashPolyline( const Point* points, int pointCount,
                               int dashOn, int dashOff, const Rect& currentClip );

    void dashSegment( int *pDashNum, int *pDashIndex, int *pDashOffset, int x1, int y1, int x2, int y2, 
                      bool projectLeft, bool projectRight, LineFace *leftFace, LineFace *rightFace,  int* dash, const Rect& currentClip);

    void putImage( const Point& to, const Image& image);

    void putImage(const Point& to, const Image& image, const Rect& imageRect);

    private:
        DrawText*            _text;
        Image*               _image;
        double               _lastScaleFactor;

        CompositionMode      _compositionMode;
        
        Pen                  _pen;
        Image                _penBuffer;
        ConstPixel           _penPixel;

        Brush                _brush;
        Image                _brushBuffer;
        const Image*         _brushImage;
        ConstPixel           _brushPixel;
        bool                 _isGradient;

        Font                 _font;
        RectF                _clip;
        Rect                 _currentClip;
        bool                 _hasClip;

        std::vector<Polygon> _flatPath;      
};

} //namespace

} //namespace

#endif
