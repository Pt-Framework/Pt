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
#ifndef PT_UI_RASTERIZER_H
#define PT_UI_RASTERIZER_H

#include <Pt/Ui/Painter.h>

namespace Pt{
namespace Ui{

class LineSlope;
class LineEdge;
class LineFace;

class Rasterizer
{    
  public:
    Rasterizer( Image& image );    

    void setPen( const Pen& pen )
    {
      _pen = pen;
    }

    void setBrush( const Brush& brush )
    {
      _brush = brush;
    }

    void setClip( const Rect& clip )
    {
      _clip = clip;
    }

    void setRenderMode( RenderMode::Type m )
    {
      _mode = m;
    }

    void setFont( const Font& font )
    {
        _font = font;
    }

    void stroke( const PointF* points, size_t pointCount );
    
    void fill( const PointF* points, size_t pointCount );
    
    void text( const PointF& to, const Pt::String& text );
    
    void ellipseStroke( const PointF& leftTop, const SizeF& size );
    
    void ellipseFill( const PointF& leftTop, const SizeF& size );

  //Output algo.
  protected: 
    void stroke( int x, int y );
    void stroke( int x, int y, size_t length );
    void fill( const Point& origin, const Point& pos, size_t length );
    void fillSolid( const Point& origin, const Point& pos,  size_t length );
    void fillTexture( const Point& origin, const Point& pos,  size_t length );
 
  //Thin polyline algo.
  protected:
    enum { xAxis, yAxis };
    void drawThinSolidPolyline( const PointF* points, size_t pointCount );
    void drawThinDashPolyline( const PointF* points, size_t pointCount);
    void addPoint(int xx, int yy, PointF** ppt, unsigned int** pwidth, int& numSpans, int& ycurr, bool& firstspan, int signdy);
    void absDeltaAndSign( int p2, int p1, int& absdelta, int& sign);
    void stepDash( int dist, int* pDashNum, int* pDashIndex, const unsigned int* pDash, int numInDashList, int *pDashOffset );
    void bresenhamDasheLineSegment(int *pdashNum, int *pdashIndex, const unsigned int *pDash, int numInDashList, int *pdashOffset, bool isDoubleDash, int signdx, int signdy, int axis, int x1, int y1, int e, int e1, int e2, int len);    
    void bresenhamLineSegment( int signdx, int signdy, int axis, int x1, int y1, int e, int e1, int e2, int len );

  //Wide polyline base algo.  
  protected:      
    int polyBuildPoly( const PointF *vertices, const LineSlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, unsigned int *h );        
    int buildLineEdge( double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge);
    void fillRect(int x, int y, unsigned int w, unsigned int h );
    void fillLine(int y, unsigned int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count );
    void lineArc( LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt );
    void roundJoinClip( LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2 );
    int roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge );
    int lineArcI( int xorg, int yorg, std::vector<PointF>& points, std::vector<size_t>& widths);
    int lineArcD( double xorg, double yorg, std::vector<PointF>& points, std::vector<size_t>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2);
    int roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge );
    void lineJoin(LineFace *pLeft, LineFace *pRight );
    void lineProjectingCap(const LineFace *face, bool isLeft, bool isInt );
    int stepAround( int v, int incr, int max );
    void clipStepEdge( int ybase, int& xcl, int& xcr, int& edgey,  LineEdge* edge, bool edgeleft );

  //Wide solid polyline algo.
  protected:

  //Wide dashed polyline algo.
  protected:

  //Thin ellipse algo.
  protected:

  //Thick ellipse algo.
  protected:

  //Fill ellipse algo.
  protected:

  //Fill polygon algo.
  protected:

  //Text algo.
  protected:


  private:
    Image& _image;
    Brush   _brush;
    Pen     _pen;
    Rect    _clip;
    Font    _font;
    RenderMode::Type _mode;         
};


}}//namespace

#endif
