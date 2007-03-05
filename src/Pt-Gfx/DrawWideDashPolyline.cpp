#include "DrawWideDashPolyline.h"
#include "LineFace.h"
#include "LineSlope.h"
#include "LineEdge.h"
#include "Dash.h"

namespace Pt{ 
namespace Gfx{

DrawWideDashPolyline::DrawWideDashPolyline()
{ }


DrawWideDashPolyline::~DrawWideDashPolyline()
{ }


        
void DrawWideDashPolyline::draw( ARgbImage& image, const Pen& pen, const  Math::Point* pPts, size_t npt )
{    
    int	      x1, y1, x2, y2;
    int	      dashNum;		    // absolute number of dash, starts with 0 
    int       dashIndex;		    // index into array (i.e. dashNum % length)
    int       dashOffset;		    // offset into selected dash */
    int       startPaintType, endPaintType = 0, prevEndPaintType = 0;
    int       firstPaintType = 0;	// used only for closed polylines; will be 1
    int       numPixels;
    bool	  selfJoin;		    // polyline is closed?
    bool	  first;		        // first line segment of polyline
    bool	  somethingDrawn = false;
    bool	  projectLeft, projectRight;
    LineFace  leftFace, rightFace, prevRightFace;
    LineFace  firstFace;

    //Define the dash patter.
    unsigned  int dashes[2];
    dashes[0] = pen.size() * 3; // Length of `on' dashes.
    dashes[1] = pen.size(); // Length of `off' dashes.
    
    // Ensure we have >=1 points
    if( npt <= 0 )
        return;

    x2 = pPts->x;
    y2 = pPts->y;
    first = true;	// first line segment of polyline

    /* determine whether polyline is closed */
    selfJoin = false;
    
    if( x2 == pPts[npt-1].x() && y2 == pPts[npt-1].y() )
        selfJoin = true;

    // Dash segments (except for the last) will not project right; and
    // (except for the first) will not project left 
    projectLeft  = false; 
    projectRight = false;

    // perform initial offsetting into the dash sequence 
    dashNum     = 0; // absolute number of dash 
    dashIndex   = 0; // index into dash array 
    dashOffset  = 0; // index into selected dash
    
    Dash::stepDash( 0, &dashNum, &dashIndex, dashes, 2, &dashOffset );
    //miStepDash (pGC->dashOffset, &dashNum, &dashIndex, pGC->dash, pGC->numInDashList, &dashOffset);

    // How many paint types?  (Will cycle through 0..numPixels-1, beginning
    // with 1, with `off' dashes defined as those with paint type #0.)
    numPixels = 2; //pGC->numPixels;

    // Iterate through points, drawing a dashed segment for each line segment
    // of nonzero length
    while( --npt )
    {
        x1 = x2;
        y1 = y2;
        
        ++pPts;
        
        x2 = pPts->x;
        y2 = pPts->y;

        // have a line segment of nonzero length.
        if( x1 != x2 || y1 != y2 )
        {
            int prevDashNum, lastPaintedDashNum;

            // Final point; and need a projecting cap here.
            /*
            if( npt == 1 && pGC->capStyle == (int)MI_CAP_PROJECTING  && (!selfJoin || (firstPaintType == 0)))                
                projectRight = true;
            */
                
            prevDashNum = dashNum;
            
            /* draw dashed segment, updating dashNum, dashIndex and
            dashOffset, returning faces */
            dashSegment(paintedSet, pGC,  &dashNum, &dashIndex, &dashOffset, x1, y1, x2, y2, projectLeft, projectRight, &leftFace, &rightFace);

            /* determine paint types used at start and end of just-drawn
            segment */
            startPaintType = ((dashNum & 1) ? 
                0 : 1 + ((dashNum / 2) % (numPixels - 1)));
            lastPaintedDashNum = (dashOffset != 0 ? dashNum : dashNum - 1);
            endPaintType = ((lastPaintedDashNum & 1) ? 
                0 : 1 + ((dashNum / 2) % (numPixels - 1)));

            /* add round cap or line join at left end of just-drawn segment;
            if OnOffDash, do so only if segment began with an `on' dash */
            if (pGC->lineStyle == (int)MI_LINE_DOUBLE_DASH || (startPaintType != 0))
            {
                pixel = pGC->pixels[startPaintType];
                if (first || (pGC->lineStyle == (int)MI_LINE_ON_OFF_DASH 
                    && prevEndPaintType == 0))
                    /* draw cap at left end, unless this is first segment of a
                    closed polyline */
                {
                    if (first && selfJoin)
                    {
                        firstFace = leftFace;
                        firstPaintType = startPaintType;
                    }
                    else if (pGC->capStyle == (int)MI_CAP_ROUND
                        || pGC->capStyle == (int)MI_CAP_TRIANGULAR)
                        /* invoke miLineArc to draw round cap, isInt = true */
                        miLineArc (paintedSet, pixel, pGC,
                        &leftFace, (LineFace *)NULL,
                        (double)0.0, (double)0.0, true);
                }
                else
                    /* draw join at left end */
                    miLineJoin (paintedSet, pixel, pGC,
                    &leftFace, &prevRightFace);
            }

            somethingDrawn = true;
            first = false;
            prevRightFace = rightFace;
            prevEndPaintType = endPaintType;
            projectLeft = false;
        }

        if (npt == 1 && somethingDrawn)
            /* last point of a nonempty polyline, so add line join or round cap
            if appropriate, i.e. if we're doing OnOffDash and ended on an
            `on' dash, or if we're doing DoubleDash */
        {
            if (pGC->lineStyle == (int)MI_LINE_DOUBLE_DASH || (endPaintType != 0))
            {
                pixel = pGC->pixels[endPaintType];
                if (selfJoin && (pGC->lineStyle == (int)MI_LINE_DOUBLE_DASH 
                    || (firstPaintType != 0)))
                    /* closed, so draw a join */
                    miLineJoin (paintedSet, pixel, pGC,
                    &firstFace, &rightFace);
                else 
                {
                    if (pGC->capStyle == (int)MI_CAP_ROUND
                        || pGC->capStyle == (int)MI_CAP_TRIANGULAR)
                        /* invoke miLineArc, isInt = true, to draw a round cap */
                        miLineArc (paintedSet, pixel, pGC,
                        (LineFace *)NULL, &rightFace,
                        (double)0.0, (double)0.0, true);
                }
            }
            else
                /* we're doing OnOffDash, and final segment of polyline ended
                with an (undrawn) `off' dash */
            {
                if (selfJoin && (firstPaintType != 0))
                    /* closed; if projecting or round caps are being used, draw
                    one on the first face */
                {
                    pixel = pGC->pixels[firstPaintType];
                    if (pGC->capStyle == (int)MI_CAP_PROJECTING)
                        miLineProjectingCap (paintedSet, pixel, pGC,
                        &firstFace, true, true);
                    else if (pGC->capStyle == (int)MI_CAP_ROUND
                        || pGC->capStyle == (int)MI_CAP_TRIANGULAR)
                        /* invoke miLineArc, isInt = true, to draw a round cap */
                        miLineArc (paintedSet, pixel, pGC,
                        &firstFace, (LineFace *)NULL,
                        (double)0.0, (double)0.0, true);
                }
            }
        }
    }

    /* handle `all points coincident' crock, nothing yet drawn */
    if (!somethingDrawn 
        && (pGC->lineStyle == (int)MI_LINE_DOUBLE_DASH || !(dashNum & 1)))
    {
        unsigned int w1;

        pixel = (dashNum & 1) ? pGC->pixels[0] : pGC->pixels[1];
        switch ((int)pGC->capStyle) 
        {
        case (int)MI_CAP_ROUND:
        case (int)MI_CAP_TRIANGULAR:
            /* invoke miLineArc, isInt = false, to draw a round disk */
            miLineArc (paintedSet, pixel, pGC,
                (LineFace *)NULL, (LineFace *)NULL,
                (double)x2, (double)y2,
                false);
            break;
        case (int)MI_CAP_PROJECTING:
            /* draw a square box with edge size equal to line width */
            w1 = pGC->lineWidth;
            miFillRectPolyHelper (paintedSet, pixel,
                (int)(x2 - (w1 >> 1)), (int)(y2 - (w1 >> 1)),
                w1, w1);
            break;
        case (int)MI_CAP_BUTT:
        default:
            break;
        }
    }
        
}
}

/* Helper function, called by miWideDash().  Draw a single dashed line
   segment, i.e. a sequence of dashes including a possible final incomplete
   dash, and step DashNum, DashIndex and DashOffset appropriately.  Also
   pass back left and right faces for the line segment, for possible use in
   adding caps or joins.  If the LineOnOffDash line style is used, each
   dash will be given a round cap if lines are drawn in the rounded cap
   style, and a projecting cap if lines are drawn in the projecting cap
   style. 
   
     const miGC *pGC;
     int *pDashNum;		// absolute number of dash
     int *pDashIndex;   // index into array (i.e. dashNum % length)
     int *pDashOffset;	// offset into selected dash 
*/

void DrawWideDashPolyline::dashSegment( ARgbImage& image, const Pen& pen, int *pDashNum, int *pDashIndex, int *pDashOffset, int x1, int y1, int x2, int y2, bool projectLeft, bool projectRight, LineFace *leftFace, LineFace *rightFace, unsigned int* dash)
{
    int		            dashNum, dashIndex, dashRemain;
    double	            L, l;
    double	            k;
    Pt::Math::PointF	vertices[4];
    Pt::Math::PointF    saveRight, saveBottom;
    LineSlope	        slopes[4];
    LineEdge	        left[2], right[2];
    LineFace	        lcapFace, rcapFace;
    int		            nleft, nright;
    unsigned int	    h;
    int		            y;
    int		            dy, dx;
    double	            LRemain;
    double	            r;
    double	            rdx, rdy;
    double	            dashDx, dashDy;
    double	            saveK = 0.0;
    bool	    	    first = true;
    double	            lcenterx, lcentery, rcenterx = 0.0, rcentery = 0.0;
    int    	            numPixels, paintType;
    
    dx          = x2 - x1;
    dy          = y2 - y1;
    dashNum     = *pDashNum;
    dashIndex   = *pDashIndex;

    // Determine portion of current dash remaining (i.e. the portion after
    // the current offset.
    dashRemain = (int)(dash[dashIndex]) - *pDashOffset;	

    // compute color of current dash
    numPixels = 2;
    paintType = (dashNum & 1) ? 0 : 1 + ((dashNum / 2) % (numPixels - 1));
    //pixel = pGC->pixels[paintType];

    // Compute e.g. L, the distance to go (for dashing).
    l = 0.5 * ((double) pen.size() );
    
    // Vertical segment.
    if( dx == 0 )
    {
        L   = dy;
        rdx = 0;
        rdy = l;
        
        if( dy < 0  )
	    {
	        L = -dy;
	        rdy = -l;
	    }
    }
    else if( dy == 0 ) //Horizontal segment.
    {
        L = dx;
        rdx = l;
        rdy = 0;

        if (dx < 0)
        {
            L = -dx;
            rdx = -l;
        }
    }
    else // Neither horizontal nor vertical.
    {
        L = hypot ((double) dx, (double) dy);
        r = l / L;		/* this is ell / L, not 1 / L */
        rdx = r * dx;
        rdy = r * dy;
    }
    
    k = l * L; // this is ell * L, not 1 * L.

    // All position comments are relative to a line with dx and dy > 0,
    // but the code does not depend on this. 
    
    // top
    slopes[V_TOP].setDX( dx );
    slopes[V_TOP].setDY( dy );
    slopes[V_TOP].setK( k );
    // right
    slopes[V_RIGHT].setDX( -dy );
    slopes[V_RIGHT].setDY( dx );
    slopes[V_RIGHT].setK( 0 );
    // bottom
    slopes[V_BOTTOM].setDX( -dx );
    slopes[V_BOTTOM].setDY( -dy );
    slopes[V_BOTTOM].setK( k );
    // left
    slopes[V_LEFT].setDX( dy );
    slopes[V_LEFT].setDY( -dx );
    slopes[V_LEFT].setK( 0 );

    // preload the start coordinates
    vertices[V_RIGHT]setX( rdy) ;
    vertices[V_TOP].setX( rdy );
    vertices[V_RIGHT].setY( -rdx );
    vertices[V_TOP].setY( -rdx );

    vertices[V_BOTTOM].setX( -rdy );
    vertices[V_LEFT].setX( -rdy );
    vertices[V_BOTTOM].setY( rdx );
    vertices[V_LEFT].setY( rdx );
    
    // offset the vertices appropriately
    if (projectLeft)    
    {
        vertices[V_TOP].setX( vertices[V_TOP].x() - rdx );
        vertices[V_TOP].setY( vertices[V_TOP].y() - rdy );

        vertices[V_LEFT].setX( vertices[V_LEFT].x() - rdx );
        vertices[V_LEFT].setY( vertices[V_LEFT].y() - rdy );

        slopes[V_LEFT].setK( rdx * dx + rdy * dy );
    }

    // starting point for first dash (floating point)
    lcenterx = x1;
    lcentery = y1;

    // Keep track of starting face (need only in OnOff case)
    if( pen.capStyle() == Pen::RoundCap /* || pen.capStyle() == Pen::TriangularCap*/ )    
    {
        lcapFace.setDX( dx );
        lcapFace.setDY( dy );
        lcapFace.setX( x1 );
        lcapFace.setY( y1 );

        rcapFace.setDX( -dx );
        rcapFace.setDY( -dy );
        rcapFace.setX( x1 );
        rcapFace.setY( y1 );
    }

    // draw dashes until end of line segment is reached, and no additional
    // (complete) dash can be drawn.
    LRemain = L;
    
    while( LRemain > dashRemain )
    {
        dashDx = (dashRemain * dx) / L;
        dashDy = (dashRemain * dy) / L;

        // Ending point for dash
        rcenterx = lcenterx + dashDx;
        rcentery = lcentery + dashDy;

        vertices[V_RIGHT].setX( vertices[V_RIGHT].x() + dashDx );
        vertices[V_RIGHT].setY( vertices[V_RIGHT].y() + dashDy );

        vertices[V_BOTTOM].setX( vertices[V_BOTTOM].x() + dashDx );
        vertices[V_BOTTOM].setY( vertices[V_BOTTOM].y() + dashDy );

        slopes[V_RIGHT].setK( vertices[V_RIGHT].x() * dx + vertices[V_RIGHT].y() * dy );

        // Draw dash (if OnOffDash, don't draw `off' dashes)
        if( /*pGC->lineStyle == (int)MI_LINE_DOUBLE_DASH ||*/ !(paintType == 0))
        {
            // Will draw projecting caps, so save vertices for later use
            /*
            if( pGC->lineStyle == (int)MI_LINE_ON_OFF_DASH && pGC->capStyle == (int)MI_CAP_PROJECTING)        
            {
                saveRight = vertices[V_RIGHT];
                saveBottom = vertices[V_BOTTOM];
                saveK = slopes[V_RIGHT].k;

                if( !first )
                {
                    vertices[V_TOP].x -= rdx;
                    vertices[V_TOP].y -= rdy;

                    vertices[V_LEFT].x -= rdx;
                    vertices[V_LEFT].y -= rdy;

                    slopes[V_LEFT].k = vertices[V_LEFT].x * slopes[V_LEFT].dy - vertices[V_LEFT].y * slopes[V_LEFT].dx;
                }

                vertices[V_RIGHT].x += rdx;
                vertices[V_RIGHT].y += rdy;

                vertices[V_BOTTOM].x += rdx;
                vertices[V_BOTTOM].y += rdy;

                slopes[V_RIGHT].k = vertices[V_RIGHT].x * slopes[V_RIGHT].dy - vertices[V_RIGHT].y * slopes[V_RIGHT].dx;
            }
            */

	     /* build lists of left and right edges for the dash, using the
	         just-computed array of slopes */
	        y = miPolyBuildPoly (vertices, slopes, 4, x1, y1,
			       left, right, &nleft, &nright, &h);

	  /* fill the dash, with either fg or bg color (alternates) */
	  miFillPolyHelper (paintedSet, pixel, 
			    y, h, left, right, nleft, nright);

	  if (pGC->lineStyle == (int)MI_LINE_ON_OFF_DASH)
	    /* if doing OnOffDash, add caps if any */
	    {
	      switch ((int)pGC->capStyle)
		{
		case (int)MI_CAP_BUTT:
		default:
		  break;
		case (int)MI_CAP_PROJECTING:
		  /* use saved vertices */
		  vertices[V_BOTTOM] = saveBottom;
		  vertices[V_RIGHT] = saveRight;
		  slopes[V_RIGHT].k = saveK;
		  break;
		case (int)MI_CAP_ROUND:
		case (int)MI_CAP_TRIANGULAR:
		  if (!first)
		    {
		      if (dx < 0)
		    	{
			  lcapFace.xa = -vertices[V_LEFT].x;
			  lcapFace.ya = -vertices[V_LEFT].y;
			  lcapFace.k = slopes[V_LEFT].k;
		    	}
		      else
		    	{
			  lcapFace.xa = vertices[V_TOP].x;
			  lcapFace.ya = vertices[V_TOP].y;
			  lcapFace.k = -slopes[V_LEFT].k;
		    	}
		      /* invoke miLineArc, isInt = false, to draw half-disk
			 on left end of dash (only if dash is not first) */
		      miLineArc (paintedSet, pixel, pGC,
				 &lcapFace, (LineFace *) NULL,
				 lcenterx, lcentery, false);
		    }
		  if (dx < 0)
		    {
		      rcapFace.xa = vertices[V_BOTTOM].x;
		      rcapFace.ya = vertices[V_BOTTOM].y;
		      rcapFace.k = slopes[V_RIGHT].k;
		    }
		  else
		    {
		      rcapFace.xa = -vertices[V_RIGHT].x;
		      rcapFace.ya = -vertices[V_RIGHT].y;
		      rcapFace.k = -slopes[V_RIGHT].k;
		    }
		  /* invoke miLineArc, isInt = false, to draw half-disk on
		     right end of dash */
		  miLineArc (paintedSet, pixel, pGC,
			     (LineFace *)NULL, &rcapFace,
			     rcenterx, rcentery, false);
		  break;
	    	}
	    }
	}

      /* we just drew a dash, or (in the OnOff case) we either drew a dash
	 or we didn't */

      LRemain -= dashRemain;	/* decrement float by int (distance over
				   which we just drew, i.e. the remainder
				   of current dash) */

      /* bump absolute dash number, and index of dash in array (cyclically) */
      ++dashNum;
      ++dashIndex;
      if (dashIndex == pGC->numInDashList)
	dashIndex = 0;
      dashRemain = (int)(pDash[dashIndex]); /* whole new dash now `remains' */

      /* compute color of next dash */
      paintType = (dashNum & 1) ? 0 : 1 + ((dashNum / 2) % (numPixels - 1));
      pixel = pGC->pixels[paintType];

      /* next dash will start where previous one ended */
      lcenterx = rcenterx;
      lcentery = rcentery;

      vertices[V_TOP] = vertices[V_RIGHT];
      vertices[V_LEFT] = vertices[V_BOTTOM];
      slopes[V_LEFT].k = -slopes[V_RIGHT].k;
      first = false;		/* no longer first dash of line segment */
    }

  /* final portion of segment is dashed specially, with an incomplete dash */
  if (pGC->lineStyle == (int)MI_LINE_DOUBLE_DASH || !(paintType == 0))
    {
      vertices[V_TOP].x -= dx;
      vertices[V_TOP].y -= dy;

      vertices[V_LEFT].x -= dx;
      vertices[V_LEFT].y -= dy;

      vertices[V_RIGHT].x = rdy;
      vertices[V_RIGHT].y = -rdx;

      vertices[V_BOTTOM].x = -rdy;
      vertices[V_BOTTOM].y = rdx;
	
      if (projectRight)
	/* offset appropriately */
	{
	  vertices[V_RIGHT].x += rdx;
	  vertices[V_RIGHT].y += rdy;
    
	  vertices[V_BOTTOM].x += rdx;
	  vertices[V_BOTTOM].y += rdy;
	  slopes[V_RIGHT].k = vertices[V_RIGHT].x *
	    slopes[V_RIGHT].dy -
	      vertices[V_RIGHT].y *
		slopes[V_RIGHT].dx;
	}
      else
	slopes[V_RIGHT].k = 0;

      /* if OnOffDash line style and cap mode is projecting, offset the
	 face, so as to draw a projecting cap */
      if (!first && pGC->lineStyle == (int)MI_LINE_ON_OFF_DASH 
	  && pGC->capStyle == (int)MI_CAP_PROJECTING)
	{
	  vertices[V_TOP].x -= rdx;
	  vertices[V_TOP].y -= rdy;
	  
	  vertices[V_LEFT].x -= rdx;
	  vertices[V_LEFT].y -= rdy;
	  slopes[V_LEFT].k = vertices[V_LEFT].x *
	    slopes[V_LEFT].dy -
	      vertices[V_LEFT].y *
		slopes[V_LEFT].dx;
	}
      else
	slopes[V_LEFT].k += dx * dx + dy * dy;
      
      /* build lists of left and right edges for the final incomplete dash,
	 using the just-computed vertices and slopes */
      y = miPolyBuildPoly (vertices, slopes, 4, x2, y2,
			   left, right, &nleft, &nright, &h);

      /* fill the final dash */
      miFillPolyHelper (paintedSet, pixel,
			y, h, left, right, nleft, nright);

      /* if OnOffDash line style and cap mode is round, draw a round cap */
      if (!first && pGC->lineStyle == (int)MI_LINE_ON_OFF_DASH
	  && (pGC->capStyle == (int)MI_CAP_ROUND
	      || pGC->capStyle == (int)MI_CAP_TRIANGULAR))
	{
	  lcapFace.x = x2;
	  lcapFace.y = y2;
	  if (dx < 0)
	    {
	      lcapFace.xa = -vertices[V_LEFT].x;
	      lcapFace.ya = -vertices[V_LEFT].y;
	      lcapFace.k = slopes[V_LEFT].k;
	    }
	  else
	    {
	      lcapFace.xa = vertices[V_TOP].x;
	      lcapFace.ya = vertices[V_TOP].y;
	      lcapFace.k = -slopes[V_LEFT].k;
	    }
	  /* invoke miLineArc, isInt = false, to draw disk on end */
	  miLineArc (paintedSet, pixel, pGC,
		     &lcapFace, (LineFace *) NULL,
		     rcenterx, rcentery, false);
	}
    }

  /* work out left and right faces of the dashed segment, to pass back */
  leftFace->x = x1;
  leftFace->y = y1;
  leftFace->dx = dx;
  leftFace->dy = dy;
  leftFace->xa = rdy;
  leftFace->ya = -rdx;
  leftFace->k = k;

  rightFace->x = x2;
  rightFace->y = y2;
  rightFace->dx = -dx;
  rightFace->dy = -dy;
  rightFace->xa = -rdy;
  rightFace->ya = rdx;
  rightFace->k = k;

  /* update absolute dash number, dash index, dash offset */
  dashRemain = (int)(((double) dashRemain) - LRemain);
  if (dashRemain == 0)		/* on to next dash in array */
    {
      dashNum++;		/* bump absolute dash number */
      dashIndex++;
      if (dashIndex == pGC->numInDashList) /* wrap */
	dashIndex = 0;
      dashRemain = (int)(pDash[dashIndex]);
    }

  *pDashNum = dashNum;
  *pDashIndex = dashIndex;
  *pDashOffset = (int)(pDash[dashIndex]) - dashRemain;
}

}// namespace Gfx
}// namespace Pt
