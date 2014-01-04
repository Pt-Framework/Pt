/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *   Copyright (C) 2010 Aloysius Indrayanto                                *
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

#include "DrawPolyline.h"
#include "LineEdge.h"
#include "LineFace.h"
#include "LineSlope.h"

#include "Pt/Gfx/Point.h"

namespace Pt{
namespace Gfx{

class DrawWidePolyline : public DrawPolyline
{
    protected:
        DrawWidePolyline();
        virtual ~DrawWidePolyline();

        //miPolyBuildPoly
        int polyBuildPoly( const Pt::Gfx::PointF *vertices, const LineSlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, unsigned int *h );

        //miPolyBuildEdge
        int buildLineEdge( double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge);

        //miFillRectPolyHelper
        void fillRect( ARgbImage& image,const Pen& pen, int x, int y, unsigned int w, unsigned int h );

        //miFillPolyHelper
        void fillLine( ARgbImage& image, const Pen& pen, int y, unsigned int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count );

        //miLineArc
        void lineArc( ARgbImage& image, const Pen& pen, LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt );

        //miRoundJoinClip
        void roundJoinClip( LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2 );

        //miRoundCapClip
        int roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge );

        //miLineArcI
        int lineArcI( const Pen& pen, int xorg, int yorg, std::vector<Pt::Gfx::Point>& points, std::vector<size_t>& widths);

        //miLineArcD
        int lineArcD( const Pen & pen, double xorg, double yorg, std::vector<Pt::Gfx::Point>& points, std::vector<size_t>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2);

        //miRoundJoinFace
        int roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge );

        //miLineJoin
        void lineJoin( ARgbImage& image, const Pen& pen, LineFace *pLeft, LineFace *pRight );

        //miLineProjectingCap
        void lineProjectingCap( ARgbImage& image,const Pen& pen, const LineFace *face, bool isLeft, bool isInt );

    private:

        inline int stepAround( int v, int incr, int max )
        {
            return (((v) + (incr) < 0) ? (max - 1) : ((v) + (incr) == max) ? 0 : ((v) + (incr)));
        }

        inline void clipStepEdge( int ybase, int& xcl, int& xcr, int& edgey,  LineEdge* edge, bool edgeleft )
        {
            if (ybase != edgey)
                return;

            if (edgeleft)
            {
                if (edge->x() > xcl)
                    xcl = edge->x();
            }
            else
            {
                if (edge->x() < xcr)
                    xcr = edge->x();
            }

            edgey++;
            edge->setX( edge->x() + edge->stepx() );
            edge->setE( edge->e() + edge->dx());

            if (edge->e() > 0)
            {
                edge->setX( edge->x() + edge->signdx() );
                edge->setE( edge->e() - edge->dy() );
            }
        }
};

} //namespace Gfx
} //namespaec Pt

#endif
