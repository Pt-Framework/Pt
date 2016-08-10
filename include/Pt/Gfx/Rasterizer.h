/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner 
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
#ifndef PT_GFX_RASTERIZER_H
#define PT_GFX_RASTERIZER_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/String.h>
#include <Pt/Gfx/Painter.h>

namespace Pt{
namespace Gfx{

class LineSlope;
class LineEdge;
class LineFace;
class DrawText;
class Image;

class PT_GFX_API Rasterizer
{    
  public:
    Rasterizer( Image& image );    
    
    ~Rasterizer();

    void setImage(Image& image);

    void setPen( const Pen& pen )
    {
      _pen = pen;
    }

    const Pen& pen() const
    {
      return _pen;
    }

    void setBrush( const Brush& brush )
    {
      _brush = brush;
    }

    const Brush& brush() const
    {
      return _brush;
    }

    void setClip( const RectF& clip );

    const RectF& clip() const
    {
      return _clip;
    }


    void setFont( const Font& font );

    const Font& font() const
    {
      return _font;
    }

    void setRenderFlags( RenderFlags::Type f)
    {
      _renderFlags = f;
    }

    RenderFlags::Type renderFlags() const
    {
      return _renderFlags;
    }

    void image( const PointF& to, const Image& image);

    void image(const PointF& toIn, 
               const Image& image, 
               const RectF& imageRect);

    void fillRect(const RectF& r);

    void stroke( const PointF* points, size_t pointCount );

    void stroke( const PointF& pixel);

    void fill( const PointF* points, size_t pointCount );

    void strokeText( const PointF& to, const Pt::String& text );
    
    void strokeEllipse( const PointF& topLeft, const SizeF& size );
    
    void fillEllipse( const PointF& topLeft, const SizeF& size );

    FontMetrics fontMetrics( const String& text ) const;

    void clear( const Color& color = Color(1,1,1) );

    

    static FontMetrics fontMetrics( const Font& font, const Pt::String& text );

  //Output algo.
  protected: 
    void stroke( int x, int y );
    void stroke( int x, int y, int length );
    void outputSpan( const Point& topLeft, int x, int y, int width );
    void fill( const Point& origin, const Point& pos, int length );
    void fillSolid( const Point& pos,  int length );
    void fillTexture( const Point& origin, const Point& pos,  int length );
    void clipSpan( int& x, int& y, int& length );
 
  //Thin polyline algo.
  protected:
    enum { xAxis, yAxis };
    void drawThinSolidPolyline( const PointF* points, int pointCount );
    void drawThinDashPolyline( const PointF* points, int pointCount);
    void stepDash( int dist, int* pDashNum, int* pDashIndex, const int* pDash, int numInDashList, int *pDashOffset );
    void bresenhamDasheLineSegment(int *pdashNum, int *pdashIndex, const  int *pDash, int numInDashList, int *pdashOffset, bool isDoubleDash, int signdx, int signdy, int axis, int x1, int y1, int e, int e1, int e2, int len);    
    void bresenhamLineSegment( int signdx, int signdy, int axis, int x1, int y1, int e, int e1, int e2, int len );

  //Wide polyline base algo.  
  protected:      
    int polyBuildPoly( const PointF *vertices, const LineSlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, int *h );        
    int buildLineEdge( double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge);
    void fillRect(int x, int y,  int w,  int h );
    void fillLine(int y,  int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count );
    void lineArc( LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt );
    void roundJoinClip( LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2 );
    int roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge );
    int lineArcI( int xorg, int yorg, std::vector<PointF>& points, std::vector<int>& widths);
    int lineArcD( double xorg, double yorg, std::vector<PointF>& points, std::vector<int>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2);
    int roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge );
    void lineJoin(LineFace *pLeft, LineFace *pRight );
    void lineProjectingCap(const LineFace *face, bool isLeft, bool isInt );
    void clipStepEdge( int ybase, int& xcl, int& xcr, int& edgey,  LineEdge* edge, bool edgeleft );

  //Wide solid polyline algo.
  protected:
    void drawWideSolidPolyline( const PointF* points, int pointCount );
    void drawSegment( PointF from, PointF to, bool projectLeft, bool projectRight, LineFace* leftFace, LineFace* rightFace );

  //Wide dashed polyline algo.
  protected:
    enum { V_TOP =  0, V_RIGHT = 1, V_BOTTOM = 2, V_LEFT = 3 };
    void drawWideDashPolyline( const PointF* points, int pointCount );
    void dashSegment( int *pDashNum, int *pDashIndex, int *pDashOffset, int x1, int y1, int x2, int y2, bool projectLeft, bool projectRight, LineFace *leftFace, LineFace *rightFace,  int* dash );
        
  private:
    Image* 		_image;
    DrawText*  _text;   
    RectF			_clip;    
    Font			_font;
    Brush			_brush;
    Pen				_pen;   
    RenderFlags::Type _renderFlags;   
};


}}//namespace

#endif
