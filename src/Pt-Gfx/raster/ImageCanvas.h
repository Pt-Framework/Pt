/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_IMAGE_CANVAS_H
#define PT_GFX_IMAGE_CANVAS_H

#include "RasterContext.h"

#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Image.h>
#include <Pt/System/Path.h>
#include <Pt/String.h>
#include <Pt/Math.h>

namespace Pt {

namespace Gfx {

class LineSlope;
class LineEdge;
class LineFace;
class DrawText;
class ActiveEdgeTable;

/* TODO: Reuse paint attributes in RasterContext
*/
class ImageCanvas : public Canvas
{
  public:
    typedef ImageView::Point Point;
    typedef ImageView::Size Size;
    typedef ImageView::Rect Rect;

  public:
    ImageCanvas(PaintSurface& surface);

    ~ImageCanvas();

    const Image& image() const;

    void reset(const Gfx::Image& image);

    void reset(Pt::ssize_t width, Pt::ssize_t height, 
               std::size_t stride);

    void setScaleFactor(double scaleFactor);

    const Size& physicalSize() const;

    const SizeF& logicalSize() const;

  protected:
    virtual const Gfx::ImageFormat& onGetFormat() const;

    virtual const Gfx::SizeF& onGetSize() const;

    virtual const Scaling& onGetScaling() const;

  protected:
    virtual bool onSetPaint(Gfx::PaintContext* context) override;

    virtual Gfx::PaintContext* onCreatePaint() override;

    virtual void onReleasePaint() override;

  protected:
    virtual void onCompositionModeChanged() override;

    virtual void onPenChanged() override;

    virtual void onBrushChanged() override;

    virtual void onFontChanged() override;

    virtual void onClipChanged() override;

  protected:
    virtual void onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to) override;

    virtual void onDrawPolyline(const Gfx::Polyline& line) override;

    virtual void onFillPolygon(const Gfx::Polyline& line) override;

    virtual void onDrawRect(const Gfx::RectF& rectangle) override;

    virtual void onFillRect(const Gfx::RectF& rectangle) override;

    virtual void onDrawEllipse(const Gfx::PointF& topLeft, 
                               const Gfx::SizeF& size) override;

    virtual void onFillEllipse(const Gfx::PointF& topLeft, 
                               const Gfx::SizeF& size) override;

    virtual void onDrawArc(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                           float degBegin, float degEnd) override
    {}

    virtual void onFillChord(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                             float degBegin, float degEnd) override
    {}

    virtual void onFillPie(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                           float degBegin, float degEnd) override
    {}

    virtual void onDrawPath(const Gfx::Path& path, float smoothness) override
    {}

    virtual void onFillPath(const Gfx::Path& path, float smoothness) override
    {}

  protected:
    virtual Gfx::FontMetrics onGetFontMetrics(const Pt::String& text) const override;

    virtual void onDrawText(const Gfx::PointF& to, 
                            const Pt::String& text, 
                            const Gfx::Transform* trans) override;

  protected:
    virtual void onDrawImage(const Gfx::PointF& to, 
                             const Gfx::Image& image, 
                             const Gfx::RectF* imgRect) override;

    virtual bool onDrawLayer(const Gfx::PointF& to,
                             const Gfx::PaintLayer& layer,
                             const Gfx::RectF* rect) override;

  public:
      static void setFontDir(const System::Path& path);

      static const std::string& defaultFont();

      static void setDefaultFont(const std::string& name);

      static std::vector<std::string> fontNames();

  private:
    void putImage( const Point& to, const Image& image);

    void putImage(const Point& to, const Image& image, const Rect& imageRect);

    void fillRect(const Rect& r, const Rect& currentClip);

    void stroke(const Point* points, size_t pointCount, const Rect& currentClip);

    void stroke(const Point& pixel, const Rect& currentClip);

    void fill(const Point* points, size_t pointCount, const Rect& currentClip);

    void strokeEllipse( const Point& topLeft, const Size& size, const Rect& currentClip);

    void fillEllipse( const Point& topLeft, const Size& size, const Rect& currentClip);

  protected:
    void outputSpan( const Point& topLeft, int x, int y, int width );
    
    void fill( const Point& origin, const Point& pos, int length );
    
    void fillSolid( const Point& pos,  int length );
    
    void fillVerticalGradient( const Point& origin, const Point& pos,  int length );
    
    void fillHorizontalGradient( const Point& origin, const Point& pos,  int length );
    
    void fillTexture( const Point& origin, const Point& pos,  int length );
    
    //void createGradientTexture(Image& img, int width, int height,
    //                           Pt::Gfx::Color gradientStart,
    //                           Pt::Gfx::Color gradientStop, 
    //                           Pt::Gfx::Brush::GradientDirection style);
    
    void clipSpan( int& x, int& y, int& length, const Rect& currentClip);

    Rect updateClip() const
    {
        Rect imageRect;
        imageRect.setWidth( _image.width() );
        imageRect.setHeight( _image.height() );
        return _clip.intersect(imageRect);
    }
    
    void outputEdges(const ActiveEdgeTable& edges, const Point&  origin, int scalLine);

  // Thin polyline
  protected:
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
    
    void lineArc( LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt, const Rect& currentClip);
    
    void roundJoinClip( LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2 );
    
    int roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge );
    
    int lineArcI( int xorg, int yorg, std::vector<Point>& points, std::vector<int>& widths);
    
    int lineArcD( double xorg, double yorg, std::vector<Point>& points, std::vector<int>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2);
    
    int roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge );
    
    void lineJoin(LineFace *pLeft, LineFace *pRight, const Rect& currentClip);
    
    void lineProjectingCap(const LineFace *face, bool isLeft, bool isInt, const Rect& currentClip);
    
    void clipStepEdge( int ybase, int& xcl, int& xcr, int& edgey,  LineEdge* edge, bool edgeleft );

  // Wide solid polyline
  protected:
    void drawWideSolidPolyline( const Point* points, int pointCount, const Rect& currentClip);
    
    void drawSegment( Point from, Point to, bool projectLeft, bool projectRight, LineFace* leftFace, LineFace* rightFace, const Rect& currentClip);

  // Wide dashed polyline
  protected:
    enum { V_TOP =  0, V_RIGHT = 1, V_BOTTOM = 2, V_LEFT = 3 };

    void drawWideDashPolyline( const Point* points, int pointCount,
                               int dashOn, int dashOff, const Rect& currentClip );

    void dashSegment( int *pDashNum, int *pDashIndex, int *pDashOffset, int x1, int y1, int x2, int y2, 
                      bool projectLeft, bool projectRight, LineFace *leftFace, LineFace *rightFace,  int* dash, const Rect& currentClip);

  private:
    void stroke(int x, int y, const Rect& currentClip);

    void stroke(int xpos, int ypos, int length, const Rect& currentClip);

    void updateGradientBrush(int width, int height);

    Rect round(const RectF& r)
    {
      Point pos( lround(r.x()),
                 lround(r.y()) );
      
      Size size( lround(r.width()),
                 lround(r.height()) );
      
      return Rect(pos, size);
    }
    
    Point round(const PointF& p)
    {
      Point pos( lround(p.x()),
                 lround(p.y()) );
           
      return pos;
    }

    Size round(const SizeF& s)
    {
      Size size( lround(s.width()),
                 lround(s.height()) );
      
      return size;
    }

  private:
    Image          _image;
    RasterContext* _paint;

    Gfx::Size      _physicalSize;
    Gfx::SizeF     _logicalSize;
    Gfx::Scaling   _scaling;

    DrawText*       _text;
    Rect            _clip;
    Font            _font;

    Brush           _brush;
    Image           _brushBuffer;
    const Image*    _brushImage;
    ConstPixel      _brushPixel;
    bool            _isGradient;

    Pen             _pen;
    Image           _penBuffer;
    ConstPixel      _penPixel;

    CompositionMode _compositionMode;
};

} //namespace

} //namespace

#endif
