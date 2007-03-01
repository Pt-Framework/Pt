/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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
#ifndef PT_GFX_DRAWWIDEPOLYLINE_H
#define PT_GFX_DRAWWIDEPOLYLINE_H

#include <vector>
#include <Pt/Gfx/Brush.h>

#include "Span.h"
#include "FillPolygon.h"
#include "DrawPolyline.h"
#include "RasterBuffer.h"
#include "LineEdge.h"
#include "LineFace.h"

namespace Pt {
namespace Gfx {

typedef struct
{
  int dx, dy;			/* dy/dx is (rational) slope */
  double k;			/* x0 * dy - y0 * dx */
} PolySlope;

/** @brief Draw thick lines on an image
*
*  This class implements DrawLine and is specialised for the  drawing
*  of thick lines.
*/
class DrawWidePolyline : public DrawPolyline
{
    public:
        /** @brief Default constructor
        */
        DrawWidePolyline();

        /** @brief Draw a line on an image

            @see DrawLine::draw
        */
        void draw( ARgbImage& image, const Pen& pen, const  Math::Point* points, size_t pointCount );

        void drawSegment(ARgbImage& image, const Pen& pen,
                         Pt::Math::Point from, Pt::Math::Point to,
                         bool projectLeft, bool projectRight,
                         LineFace* leftFace, LineFace* rightFace);

        void drawDashSegment(ARgbImage& image, const Pen& pen,
                         Pt::Math::Point from, Pt::Math::Point to,
                         bool projectLeft, bool projectRight,
                         LineFace* leftFace, LineFace* rightFace );

    private:
    
        void drawLine( ARgbImage& image, const Pen& pen, const  Math::Point* points, size_t pointCount );
        void drawDashLine( ARgbImage& image, const Pen& pen, const  Math::Point* points, size_t pointCount );
        
        void lineArc( ARgbImage& image, const Pen& pen, LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt);
        int lineArcI( const Pen& pen, int xorg, int yorg, std::vector<Pt::Math::Point>& points, std::vector<size_t>& widths );
        int lineArcD( const Pen & pen, double xorg, double yorg, std::vector<Pt::Math::Point>& points, std::vector<size_t>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2 );
        
        void fillLine( ARgbImage& image, const Pen& pen, int y, unsigned int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count);
        void fillRect(ARgbImage& image,const Pen& pen, int x, int y, unsigned int w, unsigned int h);
        
        
        void lineJoin( ARgbImage& image, const Pen& pen, LineFace *pLeft, LineFace *pRight );        
        void roundJoinClip( LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2 );        
        int roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge );
        
        int roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge );                

        int buildLineEdge (double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge);
        int polyBuildPoly( const Pt::Math::PointF *vertices, const PolySlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, unsigned int *h);

                
        FillPolygon _fillPolygon;
        ClipPolygon _clipPolygon;
        RasterBuffer _rasterBuffer;
};

} // namespace Gfx
} // namespace Pt

#endif
