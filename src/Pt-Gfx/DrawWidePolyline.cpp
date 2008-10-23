/***************************************************************************
 *   Copyright (C) 2006-2008 Laurentiu-Gheorghe Crisan                     *
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
#include <math.h>

#include "DrawWidePolyline.h"

namespace Pt{
namespace Gfx{

DrawWidePolyline::DrawWidePolyline()
{ }

DrawWidePolyline::~DrawWidePolyline()
{ }

int DrawWidePolyline::polyBuildPoly( const Pt::Math::PointF *vertices, const LineSlope *slopes, int count, int xi, int yi, LineEdge *left, LineEdge *right, int *pnleft, int *pnright, unsigned int *h)
{
    int	    top, bottom;
    double  miny, maxy;
    int     i;
    int     j;
    int	    clockwise;
    int	    slopeoff;
    int     s;
    int     nright, nleft;
    int     y, lasty = 0, bottomy, topy = 0;

    // Compute min, max y values for polygon (floating-point); also location
    // of corresponding vertices in vertex array
    maxy = miny = vertices[0].y();
    bottom = top = 0;

    for (i = 1; i < count; i++)
    {
        if (vertices[i].y() < miny)
        {
            top = i;
            miny = vertices[i].y();
        }

        if (vertices[i].y() >= maxy)
        {
            bottom = i;
            maxy = vertices[i].y();
        }
    }

    // Compute integer y-value for bottom of polygon (round up).
    bottomy = static_cast<ssize_t>( ceil( maxy ) + yi );

    // Determine whether should go `clockwise' or `counterclockwise'
    // to move down the right side of the polygon
    i = top;
    j = stepAround (top, -1, count);

    clockwise = 1;
    slopeoff = 0;

    if( ( slopes[j].dy() *  slopes[i].dx() )  > (  slopes[i].dy() *  slopes[j].dx()  ) )
    {
        clockwise = -1;
        slopeoff = -1;
    }

    // step around right side of polygon from top to bottom, building array
    // of `right' edges (horizontal edges are ignored)
    i = top;
    s = stepAround( top, slopeoff, count );
    nright = 0;

    while (i != bottom)
    {
        if (slopes[s].dy() != 0)
        {
            y = buildLineEdge( vertices[i].x(), vertices[i].y(), slopes[s].k(), slopes[s].dx(), slopes[s].dy(), xi, yi, false, &right[nright]);

            if( nright != 0 )
                right[nright-1].setHeight( y - lasty );
            else			// y is top of first edge.
                topy = y;

            nright++;
            lasty = y;
        }

        i = stepAround (i, clockwise, count);
        s = stepAround (s, clockwise, count);
    }

    if( nright != 0 )
        right[nright-1].setHeight( bottomy - lasty );

    // step around left side of polygon from top to bottom, building array of
    //`left' edges (horizontal edges are ignored)
    if (slopeoff == 0)
        slopeoff = -1;
    else
        slopeoff = 0;

    i = top;
    s = stepAround (top, slopeoff, count);
    nleft = 0;

    while (i != bottom)
    {
        if( slopes[s].dy() != 0 )
        {
            y = buildLineEdge( vertices[i].x(), vertices[i].y(), slopes[s].k(), slopes[s].dx(),  slopes[s].dy(), xi, yi, true, &left[nleft]);

            if( nleft != 0 )
                left[nleft-1].setHeight( y - lasty );

            nleft++;
            lasty = y;
        }

        i = stepAround( i, -clockwise, count );
        s = stepAround( s, -clockwise, count );
    }

    if (nleft != 0)
        left[nleft-1].setHeight( bottomy - lasty );

    // return number of left-side and right-side edges; also height (vertical
    // range, an unsigned int) and the vertical location of the top vertex
    // (an integer)
    *pnleft = nleft;
    *pnright = nright;
    *h = bottomy - topy;

    return topy;
}

int DrawWidePolyline::buildLineEdge( double x0, double y0, double k, int dx, int dy, int xi, int yi, bool left, LineEdge *edge )
{
    int x, y, e;
    int xady;

    // make dy positive, since edge will be traversed downward
    if (dy < 0)
    {
        dy = -dy;
        dx = -dx;
        k = -k;
    }

    // integer starting value for y: round up the floating-point value
    y = (int) ceil(y0);

    // work out integer starting value for x
    xady = ( (int) ceil(k) ) + y * dx;

    if (xady <= 0)
        x = - (-xady / dy) - 1;
    else
        x = (xady - 1) / dy;

    // start working out initial value of decision variable
    e = xady - x * dy;

    // work out optional and non-optional x increment for algorithm
    if (dx >= 0)
    {
        // optional step
        edge->setSignDX(1);

        // non-optional step, 0 if dx<dy in mag.
        edge->setStepX( dx / dy );

        edge->setDX( dx % dy );
    }
    else
    {
        // optional step
        edge->setSignDX(-1);

        // non-optional step, 0 if dx<dy in mag.
        edge->setStepX( -(-dx/dy) );

        edge->setDX(-dx % dy);
        e = dy - e + 1;
    }

    edge->setDY(dy);

    // starting value for x
    edge->setX( x + (left == true ? 1 : 0) + xi );

    // bias: initial value for e
    edge->setE( e - dy );

    // return integer starting value for y, i.e. top of edge
    return y + yi;
}

void DrawWidePolyline::fillRect(ARgbImage& image,const Pen& pen, int x, int y, unsigned int w, unsigned int h)
{    
    Pt::ssize_t ypos = std::max( 0, y );

    Pt::ssize_t yend = 0;

    if( (y + (int) h) > 0 )
        yend = std::min<int>( image.height(), y + h ) ;

    for( ; ypos < yend; ypos++ )
        _stroke->stroke( image, pen, x, ypos, w );
}

void DrawWidePolyline::fillLine(ARgbImage& image, const Pen& pen, int y, unsigned int overall_height, LineEdge *left, LineEdge *right, int left_count, int right_count)
{
    int left_x		= 0;
    int left_e		= 0;
    int left_stepx	= 0;
    int left_signdx = 0;
    int left_dy		= 0;
    int left_dx		= 0;

    int right_x      = 0;
    int right_e		 = 0;
    int right_stepx  = 0;
    int right_signdx = 0;
    int right_dy     = 0;
    int right_dx     = 0;

    unsigned int left_height = 0;
    unsigned int right_height = 0;

    while( (left_count || left_height) && (right_count || right_height) )
    {
        unsigned int height;
        
        if (!left_height && left_count)
        { // Load fields from next left edge, right edge
            
            left_height = left->height();
            left_x		= left->x();
            left_stepx	= left->stepx();
            left_signdx = left->signdx();
            left_e		= left->e();
            left_dy		= left->dy();
            left_dx		= left->dx();
            
            --left_count;
            ++left;
        }

        // load fields from next right edge
        if (!right_height && right_count)
        {
            right_height = right->height();
            right_x		 = right->x();
            right_stepx  = right->stepx();
            right_signdx = right->signdx();
            right_e		 = right->e();
            right_dy	 = right->dy();
            right_dx     = right->dx();

            --right_count;
            ++right;
        }

        height = std::min(left_height, right_height);
        left_height -= height;
        right_height -= height;

        // walk down to end of left or right edge, whichever comes first
        while (height--)
        {
            // generate a span (omitting point on right end, see above)
            if (right_x >= left_x)
            {
                int xpos = std::max( left_x, 0 );
                const int endx = std::min<int>( right_x, image.width() -1);
                
                _stroke->stroke( image, pen, xpos, y, endx - xpos + 1 );
            }

            y++;

            // Update left_x, right_x by stepping along left and right edges,
            // using midpoint line algorithm.
            left_x += left_stepx;
            left_e += left_dx;

            if (left_e > 0)
            {
                left_x += left_signdx;
                left_e -= left_dy;
            }

            right_x += right_stepx;
            right_e += right_dx;

            if (right_e > 0)
            {
                right_x += right_signdx;
                right_e -= right_dy;
            }
        }
    }
}

/* From two line faces, construct clipping edges that will be used by
   miLineArcD when drawing a pie wedge.  The line faces may be modified. */
void DrawWidePolyline::roundJoinClip (LineFace *pLeft, LineFace *pRight, LineEdge *edge1, LineEdge *edge2, int *y1, int *y2, bool *left1, bool *left2)
{
    int	denom;

    denom = - pLeft->dx() * pRight->dy() + pRight->dx() * pLeft->dy();

    if (denom >= 0)
    {
        pLeft->setXA( -pLeft->xa() );
        pLeft->setYA( -pLeft->ya() );
    }
    else
    {
        pRight->setXA( -pRight->xa() );
        pRight->setYA( -pRight->ya() );
    }

    *y1 = roundJoinFace( pLeft, edge1, left1 );
    *y2 = roundJoinFace( pRight, edge2, left2 );
}


void DrawWidePolyline::lineArc( ARgbImage& image, const Pen& pen, LineFace *leftFace, LineFace *rightFace, double xorg, double yorg, bool isInt)
{
    std::vector<Pt::Math::Point>    points;
    std::vector<size_t>             widths;

    int      xorgi = 0;
    int		 yorgi = 0;
    int 	 n;
    LineEdge edge1;
    LineEdge edge2;
    int		 edgey1; 
    int		 edgey2;
    bool	 edgeleft1;
    bool	 edgeleft2;

    if (isInt)
    { //In integer case, take (xorgi,yorgi) from face; otherwise (0,0).
        xorgi = leftFace ? leftFace->x() : rightFace->x();
        yorgi = leftFace ? leftFace->y() : rightFace->y();
    }

    edgey1 = std::numeric_limits<int>::max();
    edgey2 = std::numeric_limits<int>::max();
    edge1.setX( 0 );			/* not used, keep memory checkers happy */
    edge1.setDY( -1 );
    edge2.setX( 0 );			/* not used, keep memory checkers happy */
    edge2.setDY( -1 );

    edgeleft1 = false;
    edgeleft2 = false;

    if( (pen.style() != Pen::SolidStyle || pen.size() > 2) &&
        ((pen.capStyle() == Pen::RoundCap && pen.joinStyle() != Pen::RoundJoin)  ||
        ( pen.joinStyle() == Pen::RoundJoin && pen.capStyle() == Pen::ButtCap)))
    { // Construct clipping edges from the passed line faces (otherwise,
      // ignore them; will just draw a disk).

        if (isInt)
        {
            xorg = (double) xorgi;
            yorg = (double) yorgi;
        }

        if (leftFace && rightFace) // Have two faces, so construct clipping edges for pie wedge.
            roundJoinClip (leftFace, rightFace, &edge1, &edge2, &edgey1, &edgey2, &edgeleft1, &edgeleft2);

        else if (leftFace)// Will draw half-disk on left face, so construct clipping edge.
            edgey1 = roundCapClip( leftFace, isInt, &edge1, &edgeleft1 );

        else if (rightFace)
            /* will draw half-disk on right face, so construct clipping edge */
            edgey2 = roundCapClip (rightFace, isInt, &edge2, &edgeleft2);

        /* due to clipping, switch to using floating-point coordinates */
        isInt = false;
    }

    points.resize( pen.size() );
    widths.resize( pen.size() );

    // Construct a Spans by calling integer or floating point routine.
    if (isInt) // Integer routine, no clipping: just draw a disk.
        n = lineArcI( pen, xorgi, yorgi, points, widths );
    else //Call floating point routine, supporting clipping by edge(s).
        n = lineArcD( pen, xorg, yorg, points, widths, &edge1, edgey1, edgeleft1, &edge2, edgey2, edgeleft2);

    //Stroke the span.
    for( ssize_t i = 0; i < n; i++)
        _stroke->stroke( image, pen, points[i].x(), points[i].y(), widths[i] );
}

int DrawWidePolyline::lineArcI (const Pen& pen, int xorg, int yorg, std::vector<Pt::Math::Point>& points, std::vector<size_t>& widths)
{
    Pt::Math::Point *tpts, *bpts;
    size_t* twids, *bwids;
    int x, y, e, ex;
    int slw;

    tpts = &points[0];
    twids = &widths[0];
    slw = (int)(pen.size());

    if (slw == 1) // 'disk' is a single pixel.
    {
        tpts->setX( xorg );
        tpts->setY( yorg );
        *twids = 1;
        return 1;
    }

    // Otherwise, draw the disk scanline by scanline.
    bpts = tpts + slw;
    bwids = twids + slw;
    y = (slw >> 1) + 1;

    if (slw & 1)
        e = - ((y << 2) + 3);
    else
        e = - (y << 3);

    ex = -4;
    x = 0;

    while (y)
    {
        e += (y << 3) - 4;

        while (e >= 0)
        {
            x++;
            e += (ex = -((x << 3) + 4));
        }

        y--;
        slw = (x << 1) + 1;

        if ((e == ex) && (slw > 1))
            slw--;

        tpts->setX( xorg - x );
        tpts->setY( yorg - y );
        tpts++;
        *twids++ = slw;

        if ((y != 0) && ((slw > 1) || (e != ex)))
        {
            bpts--;
            bpts->setX( xorg - x );
            bpts->setY( yorg + y );
            *--bwids = slw;
        }
    }

    /* return linewidth (no. of spans in the Spans) */
    return (int)( pen.size() );
}

/* Draw as a Spans a filled disk of diameter equal to the linewidth, paying
   attention to one or two clipping edges.  This is used for round caps and
   round joins, respectively (it respectively yields a half-disk or a pie
   wedge).  Floating point coordinates are used.  Returns number of spans
   in the Spans.  The clipping edges may be modified. */
int DrawWidePolyline::lineArcD( const Pen & pen, double xorg, double yorg, std::vector<Pt::Math::Point>& points, std::vector<size_t>& widths, LineEdge *edge1, int edgey1, bool edgeleft1, LineEdge *edge2, int edgey2, bool edgeleft2)
{
    Pt::Math::Point *pts;
    size_t *wids;
    double radius, x0, y0, el, er, yk, xlk, xrk, k;
    int xbase, ybase, y, boty, xl, xr, xcl, xcr;
    int ymin, ymax;
    bool edge1IsMin, edge2IsMin;
    int ymin1, ymin2;

    pts = &points[0];
    wids = &widths[0];
    xbase = (int)(floor(xorg));
    x0 = xorg - xbase;
    ybase = static_cast<ssize_t>( ceil(yorg) );
    y0 = yorg - ybase;
    xlk = x0 + x0 + 1.0;
    xrk = x0 + x0 - 1.0;
    yk = y0 + y0 - 1.0;
    radius = 0.5 * ((double)pen.size());
    y = (int)(floor(radius - y0 + 1.0));
    ybase -= y;
    ymin = ybase;
    ymax = std::numeric_limits<int>::max();
    edge1IsMin = false;
    ymin1 = edgey1;

    if( edge1->dy() >= 0 )
    {
        if( !edge1->dy() )
        {
            if (edgeleft1)
                edge1IsMin = true;
            else
                ymax = edgey1;

            edgey1 = std::numeric_limits<int>::max();
        }
        else
        {
            if( (edge1->signdx() < 0 ) == edgeleft1)
                edge1IsMin = true;
        }
    }

    edge2IsMin = false;
    ymin2 = edgey2;

    if( edge2->dy() >= 0 )
    {
        if( !edge2->dy() )
        {
            if( edgeleft2 )
                edge2IsMin = true;
            else
                ymax = edgey2;

            edgey2 = std::numeric_limits<int>::max();
        }
        else
        {
            if( (edge2->signdx() < 0) == edgeleft2 )
                edge2IsMin = true;
        }
    }

    if( edge1IsMin )
    {
        ymin = ymin1;

        if( edge2IsMin && ymin1 > ymin2 )
            ymin = ymin2;
    }
    else if (edge2IsMin)
    {
        ymin = ymin2;
    }

    el = radius * radius - ((y + y0) * (y + y0)) - (x0 * x0);
    er = el + xrk;
    xl = 1;
    xr = 0;

    if (x0 < 0.5)
    {
        xl = 0;
        el -= xlk;
    }

    boty = (y0 < -0.5) ? 1 : 0;

    if (ybase + y - boty > ymax)
        boty = ymax - ybase - y;

    while (y > boty)
    {
        k = (y << 1) + yk;
        er += k;

        while (er > 0.0)
        {
            xr++;
            er += xrk - (xr << 1);
        }
        el += k;

        while (el >= 0.0)
        {
            xl--;
            el += (xl << 1) - xlk;
        }

        y--;
        ybase++;

        if (ybase < ymin)
            continue;

        xcl = xl + xbase;
        xcr = xr + xbase;

        clipStepEdge(ybase,xcl,xcr, edgey1, edge1,edgeleft1);
        clipStepEdge(ybase,xcl,xcr, edgey2, edge2,edgeleft2);

        if (xcr >= xcl)
        {
            pts->setX( xcl );
            pts->setY( ybase );
            pts++;
            *wids++ = (unsigned int)(xcr - xcl + 1);
        }
    }

    er = xrk - (xr << 1) - er;
    el = (xl << 1) - xlk - el;
    boty = (int)(floor(-y0 - radius + 1.0));

    if (ybase + y - boty > ymax)
        boty = ymax - ybase - y;

    while (y > boty)
    {
        k = (y << 1) + yk;
        er -= k;

        while ((er >= 0.0) && (xr >= 0))
        {
            xr--;
            er += xrk - (xr << 1);
        }

        el -= k;

        while ((el > 0.0) && (xl <= 0))
        {
            xl++;
            el += (xl << 1) - xlk;
        }

        y--;
        ybase++;

        if( ybase < ymin )
            continue;

        xcl = xl + xbase;
        xcr = xr + xbase;

        clipStepEdge(ybase, xcl, xcr, edgey1, edge1, edgeleft1);
        clipStepEdge(ybase, xcl, xcr, edgey2, edge2, edgeleft2);

        if (xcr >= xcl)
        {
            pts->setX( xcl );
            pts->setY( ybase );
            pts++;
            *wids++ = (unsigned int)(xcr - xcl + 1);
        }
    }

    /* return number of spans in the Spans */
    return (pts - &points[0]);
}

/* From a line face, construct a clipping edge that will be used by
   miLineArcD when drawing a half-disk.  */
int DrawWidePolyline::roundCapClip( const LineFace *face, bool isInt, LineEdge *edge, bool *leftEdge )
{
    int	    y;
    int 	dx, dy;
    double  xa, ya, k;
    bool	left;

    dx = -face->dy();
    dy = face->dx();
    xa = face->xa();
    ya = face->ya();
    k = 0.0;

    if( !isInt )
        k = face->k();

    left = true;

    if (dy < 0 || (dy == 0 && dx > 0))
    {
        dx = -dx;
        dy = -dy;
        xa = -xa;
        ya = -ya;
        left = false;
    }

    if( dx == 0 && dy == 0 )
        dy = 1;

    if( dy == 0 )
    {
        y = static_cast<ssize_t>( ceil( face->ya() ) + face->y() );
        edge->setX( std::numeric_limits<int>::min() );
        edge->setStepX( 0 );
        edge->setSignDX( 0 );
        edge->setE( -1 );
        edge->setDY( 0 );
        edge->setDX( 0 );
        edge->setHeight( 0 );
    }
    else
    {
        y = buildLineEdge( xa, ya, k, dx, dy, face->x(), face->y(), (left ? false : true), edge);
        edge->setHeight( std::numeric_limits<unsigned int>::max() );	/* number of scanlines to process */
    }

    *leftEdge = (left ? false : true);

    return y;
}

/* helper function called by the preceding */
int DrawWidePolyline::roundJoinFace( const LineFace *face, LineEdge *edge, bool *leftEdge )
{
    int	    y;
    int	    dx, dy;
    double  xa, ya;
    bool    left;

    dx = -face->dy();
    dy = face->dx();
    xa = face->xa();
    ya = face->ya();
    left = true;

    if (ya > 0)
    {
        ya = 0.0;
        xa = 0.0;
    }

    if (dy < 0 || (dy == 0 && dx > 0))
    {
        dx = -dx;
        dy = -dy;
        left = (left ? false : true);
    }

    if (dx == 0 && dy == 0)
        dy = 1;

    if (dy == 0)
    {
        y = static_cast<ssize_t>( ceil( face->ya() ) + face->y() );
        edge->setX( std::numeric_limits<int>::min() );
        edge->setStepX( 0 );
        edge->setSignDX( 0 );
        edge->setE( -1 );
        edge->setDY( 0 );
        edge->setDX( 0 );
        edge->setHeight( 0 );
    }
    else
    {
        y = buildLineEdge( xa, ya,  0.0, dx, dy, face->x(), face->y(), (left ? false : true), edge );

        edge->setHeight( std::numeric_limits<unsigned int>::max() );	/* number of scanlines to process */
    }

    *leftEdge = (left ? false : true);

    return y;
}

/* Paint all types of line join: round/miter/bevel/triangular.  Called by
   both miWideLine() and miWideDash().  Left and right line faces are
   supplied, each with its own value of k.  They may be modified. */
void DrawWidePolyline::lineJoin( ARgbImage& image, const Pen& pen, LineFace *pLeft, LineFace *pRight )
{
    double	            mx = 0.0, my = 0.0;
    int		            denom = 0;
    Pt::Math::PointF    vertices[4];
    LineSlope           slopes[4];
    int		            edgecount;
    LineEdge            left[4], right[4];
    int                 nleft, nright;
    int                 y;
    unsigned int        height;
    bool		        swapslopes;
    int		            lw = pen.size();

    if( pen.joinStyle() == Pen::RoundJoin )
    { // invoke miLineArc to fill the round join, isInt = true-
        lineArc( image, pen,pLeft, pRight, (double)0.0, (double)0.0, true );
        return;
    }

    denom = - pLeft->dx() * pRight->dy() + pRight->dx() * pLeft->dy();

    if( denom == 0 )
        return; // No join to draw.

    // Now must handle cases where line join is a small polygon to be filled;
    // specify its vertices clockwise.

    // Swap slopes if cross product of line faces has wrong sign.
    if (denom > 0)
    {
        swapslopes = false;
        pLeft->setXA( -pLeft->xa() );
        pLeft->setYA( -pLeft->ya() );
        pLeft->setDX( -pLeft->dx() );
        pLeft->setDY( -pLeft->dy() );
    }
    else
    {
        swapslopes = true;
        pRight->setXA( -pRight->xa() );
        pRight->setYA( -pRight->ya() );
        pRight->setDX( -pRight->dx() );
        pRight->setDY( -pRight->dy() );
    }

    // vertex #0 is at the right end of the right face.
    vertices[0].setX( pRight->xa() );
    vertices[0].setY( pRight->ya() );
    slopes[0].setDX( - pRight->dy() );
    slopes[0].setDY( pRight->dx() );
    slopes[0].setK( 0 );

    // vertex #1 is the nominal join point (i.e. halfway across both the.
    // right face and the left face).
    vertices[1].setX( 0 );
    vertices[1].setY( 0 );
    slopes[1].setDX( pLeft->dy() );
    slopes[1].setDY( -pLeft->dx() );
    slopes[1].setK( 0 );

    // vertex #2 is at the left end of the left face.
    vertices[2].setX( pLeft->xa() );
    vertices[2].setY( pLeft->ya() );
    Pen::JoinStyle joinStyle = pen.joinStyle();

    if( pen.joinStyle() == Pen::MiterJoin )
    {
        double miterlimit = 10.43; // default = 10.43, as in X11.

        // compute vertex (mx,my) of miter quadrilateral
        my = (pLeft->dy()  * (pRight->xa() * pRight->dy() - pRight->ya() * pRight->dx()) -
             pRight->dy() * (pLeft->xa()  * pLeft->dy()  - pLeft->ya()  * pLeft->dx() )) / (double) denom;
        
        if (pLeft->dy() != 0)
            mx = pLeft->xa() + (my - pLeft->ya()) * (double) pLeft->dx() / (double) pLeft->dy();
        else
            mx = pRight->xa() + (my - pRight->ya()) *(double) pRight->dx() / (double) pRight->dy();

        // if miter limit violated, switch to bevelled join
        if ((mx * mx + my * my) * 4 > miterlimit * miterlimit * lw * lw)
            joinStyle = Pen::BevelJoin;
    }


    double scale, dx, dy, adx, ady;

    switch( joinStyle )
    {
        case Pen::MiterJoin :
        default: // join by adding a quadrilateral
            
            edgecount = 4;
            slopes[2].setDX( pLeft->dx() );
            slopes[2].setDY( pLeft->dy() );
            slopes[2].setK( pLeft->k() );

            if( swapslopes )
            {
                slopes[2].setDX( -slopes[2].dx() );
                slopes[2].setDY( -slopes[2].dy() );
                slopes[2].setK( -slopes[2].k() );
            }

            /* vertex #3 is miter vertex (mx,my) */
            vertices[3].setX( mx );
            vertices[3].setY( my );
            slopes[3].setDX( pRight->dx() );
            slopes[3].setDY( pRight->dy() );
            slopes[3].setK( pRight->k() );

            if( swapslopes )
            {
                slopes[3].setDX( -slopes[3].dx() );
                slopes[3].setDY( -slopes[3].dy() );
                slopes[3].setK( -slopes[3].k() );
            }
        break;

        case Pen::BevelJoin: //join by adding a triangle
        {
            Pt::Math::PointF midpoint;
            edgecount = 3;

            // Third edge of triangle will pass through midpoint.
            midpoint.setX( 0.5 * (pLeft->xa() + pRight->xa() ) );
            midpoint.setY( 0.5 * (pLeft->ya() + pRight->ya() ) );

            // Vector along third edge of triangle.
            dx = pRight->xa() - pLeft->xa();
            dy = pRight->ya() - pLeft->ya();

            // Compute scale = max(|dx|,|dy|).
            adx = dx;
            ady = dy;

            if (adx < 0)
                adx = -adx;

            if (ady < 0)
                ady = -ady;

            scale = ady;

            if (adx > ady)
                scale = adx;

            // Use integer dx, dy in range -65536..65536.
            slopes[2].setDX(  (int)((dx * 65536) / scale) );
            slopes[2].setDY( (int)((dy * 65536) / scale) );
            slopes[2].setK( midpoint.x() * slopes[2].dy() - midpoint.y() * slopes[2].dx() );
        }
        break;

        case Pen::TriangularJoin: // join by adding a stubby quadrilateral
        {
            Pt::Math::PointF midpoint, newpoint;
            double mid2, mid, dx2, dy2, dx3, dy3;

            edgecount = 4;

            // compute additional vertex, offset by linewidth/2
            midpoint.setX( 0.5 * (pLeft->xa() + pRight->xa()) );
            midpoint.setY( 0.5 * (pLeft->ya() + pRight->ya()) );
            mid2 = midpoint.x() * midpoint.x() + midpoint.y() * midpoint.y();
            mid = sqrt (mid2);
            newpoint.setX( 0.5 * lw * midpoint.x() / mid );
            newpoint.setY( 0.5 * lw * midpoint.y() / mid );
            vertices[3] = newpoint;

            // offset from vertices[2] to vertices[3]
            dx2 = vertices[3].x() - vertices[2].x();
            dy2 = vertices[3].y() - vertices[2].y();

            // offset from vertices[3] back to vertices[0]
            dx3 = vertices[0].x() - vertices[3].x();
            dy3 = vertices[0].y() - vertices[3].y();

            // compute scale = max(|dx|,|dy|), where (dx,dy) is offset between
            // the two corners, i.e. vertices[0] and vertices[2]
            dx = pRight->xa() - pLeft->xa();
            dy = pRight->ya() - pLeft->ya();
            adx = dx;
            ady = dy;

            if (adx < 0)
                adx = -adx;

            if (ady < 0)
                ady = -ady;

            scale = ady;

            if (adx > ady)
                scale = adx;

            // use integer dx, dy in range -65536..65536
            slopes[2].setDX( (int)((dx2 * 65536) / scale) );
            slopes[2].setDY( (int)((dy2 * 65536) / scale) );
            slopes[2].setK( newpoint.x() * slopes[2].dy() - newpoint.y() * slopes[2].dx() );

            // use integer dx, dy in range -65536..65536
            slopes[3].setDX( (int)((dx3 * 65536) / scale) );
            slopes[3].setDY( (int)((dy3 * 65536) / scale) );
            slopes[3].setK(  newpoint.x() * slopes[3].dy() - newpoint.y() * slopes[3].dx() );
        }
        break;
    }

    // Compute lists of left and right edges for the small polygon, using the
    // just-computed slopes array.
    y = polyBuildPoly( vertices, slopes, edgecount, pLeft->x(), pLeft->y(), left, right, &nleft, &nright, &height);

    // Fill the small polygon.
    fillLine( image, pen, y, height, left, right, nleft, nright);
}

/* Paint a projecting rectangular cap on a line face.  Called only by
   miWideDash (with isInt = true); not by miWideLine. */
void DrawWidePolyline::lineProjectingCap( ARgbImage& image,const Pen& pen, const LineFace *face, bool isLeft, bool isInt )
{
    int		    xorgi = 0, yorgi = 0;
    int	       	lw;
    LineEdge	lefts[2], rights[2];
    int		    lefty, righty, topy, bottomy;
    LineEdge    *left, *right;
    LineEdge    *top, *bottom;
    double	    xa,ya;
    double	    k;
    double	    xap, yap;
    int		    dx, dy;
    double	    projectXoff, projectYoff;
    double	    maxy;
    int		    finaly;

    // in integer case, take (xorgi,yorgi) from face; otherwise (0,0)
    if (isInt)
    {
        xorgi = face->x();
        yorgi = face->y();
    }

    lw = (int)(pen.size() );
    dx = face->dx();
    dy = face->dy();
    k = face->k();

    // special case: line face is horizontal
    if( dy == 0 )
    {
        lefts[0].setHeight( (unsigned int)lw );
        lefts[0].setX( xorgi );

        if( isLeft )
            lefts[0].setX( lefts[0].x()  - (lw >> 1) );

        lefts[0].setStepX( 0 );
        lefts[0].setSignDX( 1 );
        lefts[0].setE( -lw );
        lefts[0].setDX( 0 );
        lefts[0].setDY( lw );

        rights[0].setHeight( (unsigned int)lw );
        rights[0].setX( xorgi );

        if( !isLeft )
            rights[0].setX( rights[0].x() + ((lw + 1) >> 1) );

        rights[0].setStepX( 0 );
        rights[0].setSignDX( 1 );
        rights[0].setE( -lw );
        rights[0].setDX( 0 );
        rights[0].setDY( lw );

        // fill the rectangle (1 left edge, 1 right edge)
        fillLine(image, pen, yorgi - (lw >> 1), (unsigned int)lw,  lefts, rights, 1, 1);
    }
    else if( dx == 0 ) // special case: line face is vertical
    {
        topy = yorgi;
        bottomy = yorgi + dy;

        if (isLeft)
            topy -= (lw >> 1);
        else
            bottomy += (lw >> 1);

        lefts[0].setHeight( (unsigned int)(bottomy - topy) );
        lefts[0].setX( xorgi - (lw >> 1) );
        lefts[0].setStepX( 0 );
        lefts[0].setSignDX( 1 );
        lefts[0].setE( -dy );
        lefts[0].setDX( dx );
        lefts[0].setDY( dy );

        rights[0].setHeight( (unsigned int)(bottomy - topy) );
        rights[0].setX( lefts[0].x() + (lw - 1) );
        rights[0].setStepX( 0 );
        rights[0].setSignDX( 1 );
        rights[0].setE( -dy );
        rights[0].setDX(  dx );
        rights[0].setDY( dy );

        // fill the rectangle (1 left edge, 1 right edge)
        fillLine(image, pen, topy, (unsigned int)(bottomy - topy), lefts, rights, 1, 1);
    }
    else // general case: line face is neither horizontal nor vertical
    {
        xa = face->xa();
        ya = face->ya();
        projectXoff = -ya;
        projectYoff = xa;

        if( dx < 0 )
        {
            right   = &rights[1];
            left    = &lefts[0];
            top     = &rights[0];
            bottom  = &lefts[1];
        }
        else
        {
            right   = &rights[0];
            left    = &lefts[1];
            top     = &lefts[0];
            bottom  = &rights[1];
        }

        if( isLeft ) // Cap goes left; build four edges.
        {
            righty = buildLineEdge( xa, ya, k, dx, dy,  xorgi, yorgi, false, right );
            xa = -xa;
            ya = -ya;
            k = -k;

            lefty = buildLineEdge (xa - projectXoff, ya - projectYoff, k, dx, dy, xorgi, yorgi, true, left);

            if( dx > 0 )
            {
                ya = -ya;
                xa = -xa;
            }

            xap = xa - projectXoff;
            yap = ya - projectYoff;
            topy = buildLineEdge( xap, yap,  xap * dx + yap * dy, -dy, dx, xorgi, yorgi, (dx > 0 ? true : false), top );
            bottomy = buildLineEdge( xa, ya, 0.0, -dy, dx,  xorgi, yorgi, (dx < 0 ? true : false), bottom );
            maxy = -ya;
        }
        else  // cap goes right; build four edges
        {
            righty = buildLineEdge( xa - projectXoff, ya - projectYoff, k, dx, dy,  xorgi, yorgi, false, right );

            xa = -xa;
            ya = -ya;
            k = -k;

            lefty = buildLineEdge( xa, ya, k, dx, dy,  xorgi, yorgi, true, left );

            if( dx > 0 )
            {
                ya = -ya;
                xa = -xa;
            }

            xap = xa - projectXoff;
            yap = ya - projectYoff;

              topy = buildLineEdge( xa, ya, 0.0, -dy, dx, xorgi, xorgi, (dx > 0 ? true : false), top);
              bottomy = buildLineEdge( xap, yap,  xap * dx + yap * dy, -dy, dx, xorgi, xorgi, (dx < 0 ? true : false), bottom );
              maxy = -ya + projectYoff;
          }

        finaly = static_cast<ssize_t>( ceil(maxy) + yorgi );

        if (dx < 0)
        {
            left->setHeight( (unsigned int)(bottomy - lefty) );
            right->setHeight( (unsigned int)(finaly - righty) );
            top->setHeight( (unsigned int)(righty - topy) );
        }
        else
        {
            right->setHeight( (unsigned int)(bottomy - righty) );
            left->setHeight( (unsigned int)(finaly - lefty) );
            top->setHeight( (unsigned int)(lefty - topy) );
        }
            bottom->setHeight( (unsigned int)(finaly - bottomy) );

        // fill the rectangle (2 left edges, 2 right edges)
        fillLine( image, pen, topy, (unsigned int)(bottom->height() + bottomy - topy), lefts, rights, 2, 2 );
    }
}

} //namespace Gfx
} //namespace Pt
