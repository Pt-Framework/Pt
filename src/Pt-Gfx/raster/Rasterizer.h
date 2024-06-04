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

#ifndef PT_GFX_RASTERIZER_H
#define PT_GFX_RASTERIZER_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/String.h>
#include <Pt/System/Path.h>

namespace Pt {

namespace Gfx {

class LineSlope;
class LineEdge;
class LineFace;
class DrawText;
class Image;
class ActiveEdgeTable;

class Rasterizer
{
  protected:
    class ClipRect : public Rect
    {
      public:
        ClipRect( const Rect& r = Rect() )
        : Rect(r)
        , _bottom(0)
        , _right(0)
        {
            _right = x() + width();
            _bottom = y() + height();
        }

        ClipRect& operator=(const ClipRect& r)
        {
            Rect::operator=(r);
            _right = x() + width();
            _bottom = y() + height();
            return *this;
        }
        
        int bottom() const
        {
            return _bottom;
        }

        int right() const
        {
            return _right;
        }

      private:
        int _bottom;
        int _right;
    };

  public:
    Rasterizer();

    ~Rasterizer();

    const Image& image() const
    {
        return _image;
    }

    Gfx::PaintData* begin(Gfx::PaintData*)
    { return 0; }

    void finish()
    {}

    const ImageFormat& format() const;

    void reset(const Gfx::Image& image)
    {
        if( image.format() == _image.format() )
        {
          _image = image;
          return;
        }

        _image.reset( format(), image.size() );
        Pt::Gfx::copy( image.begin(), image.end(), _image.begin() );
    }

    void reset(const Gfx::Size& size, std::size_t stride)
    {
        _image.reset( _image.format(), size, stride );
    }

    void setClip(const RectF& clip);

    void resetClip();

    void setCompositionMode(const CompositionMode& mode)
    {
        _compositionMode = mode;
    }

    void setPen( const Pen& pen );

    void setBrush( const Brush& brush );

    void setFont(const Font& font);

    FontMetrics fontMetrics(const String& text) const;

    void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

    void drawText(const Gfx::PointF& to, const Pt::String& Text);

    void drawText(const Gfx::PointF& to, const Pt::String& Text, const Gfx::Transform& trans);

    void drawRect(const Gfx::RectF& rectangle);

    void fillRect(const Gfx::RectF& rectangle);

    void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

    void drawPolyline(const Gfx::PointF* points, size_t pointCount);

    void fillPolygon(const Gfx::PointF* points, size_t pointCount);

    void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

    void drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect);

    void drawPath(const Gfx::Path& path, float smoothness);

    void fillPath(const Path& path, float smoothness);

    void drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    void fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    void drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    void fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    void drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd);

    Image toImage() const;


  public:
      static void setFontDir(const System::Path& path);

      static const std::string& defaultFont();

      static void setDefaultFont(const std::string& name);

      static std::vector<std::string> fontNames();

      static FontMetrics fontMetrics(const Font& font, const Pt::String& text);

private:

    void image( const Point& to, const Image& image);

    void image(const Point& toIn,
               const Image& image,
               const Rect& imageRect);

    void fillRect(const Rect& r, const ClipRect& currentClip);

    void stroke( const Point* points, size_t pointCount, const ClipRect& currentClip);

    void stroke( const Point& pixel, const ClipRect& currentClip);

    void fill( const Point* points, size_t pointCount, const ClipRect& currentClip);

    void strokeText( const Point& to, const Pt::String& text, const ClipRect& currentClip);

    void strokeText(const Point& to, const Pt::String& text, const Transform& trans, const ClipRect& currentClip);

    void strokeEllipse( const Point& topLeft, const Size& size, const ClipRect& currentClip);

    void fillEllipse( const Point& topLeft, const Size& size, const ClipRect& currentClip);

  //Output algo.
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
    void clipSpan( int& x, int& y, int& length, const ClipRect& currentClip);

    ClipRect updateClip() const
    {
        const Rect imageRect( _image.size() );
        return _clip.intersect(imageRect);
    }
    
    void outputEdges(const ActiveEdgeTable& edges, const Point&  origin, int scalLine);

  //Thin polyline algo.
  protected:
    enum { xAxis, yAxis };
    void drawThinSolidPolyline( const Point* points, int pointCount, const ClipRect& currentClip);
    void drawThinDashPolyline( const Point* points, int pointCount,
                               int dashOn, int dashOff, const ClipRect& currentClip);
    void stepDash( int dist, int* pDashNum, int* pDashIndex, const int* pDash, int numInDashList, int *pDashOffset );

    void bresenhamDasheLineSegment(int *pdashNum, int *pdashIndex, const  int *pDash, int numInDashList, int *pdashOffset, 
                                   bool isDoubleDash, int signdx, int signdy, int axis, int x1, int y1, 
                                   int e, int e1, int e2, int len, const ClipRect& currentClip);

    void bresenhamLineSegment( int signdx, int signdy, int axis, int x1, int y1, 
                                int e, int e1, int e2, int len, const ClipRect& currentClip);

  //Wide polyline base algo.
  protected:
    int polyBuildPoly( const Point *vertices, const LineSlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, int *h );
    int buildLineEdge( double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge);
    void fillSpans(int x, int y,  int w,  int h, const ClipRect& currentClip);
    void fillLine(int y,  int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count, const ClipRect& currentClip);
    void lineArc( LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt, const ClipRect& currentClip);
    void roundJoinClip( LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2 );
    int roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge );
    int lineArcI( int xorg, int yorg, std::vector<Point>& points, std::vector<int>& widths);
    int lineArcD( double xorg, double yorg, std::vector<Point>& points, std::vector<int>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2);
    int roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge );
    void lineJoin(LineFace *pLeft, LineFace *pRight, const ClipRect& currentClip);
    void lineProjectingCap(const LineFace *face, bool isLeft, bool isInt, const ClipRect& currentClip);
    void clipStepEdge( int ybase, int& xcl, int& xcr, int& edgey,  LineEdge* edge, bool edgeleft );

  //Wide solid polyline algo.
  protected:
    void drawWideSolidPolyline( const Point* points, int pointCount, const ClipRect& currentClip);
    void drawSegment( Point from, Point to, bool projectLeft, bool projectRight, LineFace* leftFace, LineFace* rightFace, const ClipRect& currentClip);

  //Wide dashed polyline algo.
  protected:
    enum { V_TOP =  0, V_RIGHT = 1, V_BOTTOM = 2, V_LEFT = 3 };

    void drawWideDashPolyline( const Point* points, int pointCount,
                               int dashOn, int dashOff, const ClipRect& currentClip );

    void dashSegment( int *pDashNum, int *pDashIndex, int *pDashOffset, int x1, int y1, int x2, int y2, 
                      bool projectLeft, bool projectRight, LineFace *leftFace, LineFace *rightFace,  int* dash, const ClipRect& currentClip);

  private:
    void stroke(int x, int y, const ClipRect& currentClip);

    void stroke(int xpos, int ypos, int length, const ClipRect& currentClip);

    void updateGradientBrush(int width, int height);

  private:
    Image           _image;
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
