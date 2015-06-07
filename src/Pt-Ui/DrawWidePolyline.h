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
#ifndef PT_UI_DRAWWIDEPOLYLINE_H
#define PT_UI_DRAWWIDEPOLYLINE_H

#include "DrawPolyline.h"
#include "LineEdge.h"
#include "LineFace.h"
#include "LineSlope.h"

#include <Pt/Ui/Point.h>

namespace Pt{
namespace Ui{

class DrawWidePolyline : public DrawPolyline
{
    protected:
        DrawWidePolyline();
        virtual ~DrawWidePolyline();

        //miPolyBuildPoly
        int polyBuildPoly( const PointF *vertices, const LineSlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, unsigned int *h );

        //miPolyBuildEdge
        int buildLineEdge( double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge);

        //miFillRectPolyHelper
        void fillRect( Image& image,const Pen& pen, int x, int y, unsigned int w, unsigned int h );

        //miFillPolyHelper
        void fillLine( Image& image, const Pen& pen, int y, unsigned int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count );

        //miLineArc
        void lineArc( Image& image, const Pen& pen, LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt );

        //miRoundJoinClip
        void roundJoinClip( LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2 );

        //miRoundCapClip
        int roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge );

        //miLineArcI
        int lineArcI( const Pen& pen, int xorg, int yorg, std::vector<PointF>& points, std::vector<size_t>& widths);

        //miLineArcD
        int lineArcD( const Pen & pen, double xorg, double yorg, std::vector<PointF>& points, std::vector<size_t>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2);

        //miRoundJoinFace
        int roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge );

        //miLineJoin
        void lineJoin( Image& image, const Pen& pen, LineFace *pLeft, LineFace *pRight );

        //miLineProjectingCap
        void lineProjectingCap( Image& image,const Pen& pen, const LineFace *face, bool isLeft, bool isInt );

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

}} //namespace 

#endif
