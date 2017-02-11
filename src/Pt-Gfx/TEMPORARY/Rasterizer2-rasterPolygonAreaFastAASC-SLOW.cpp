        void rasterPolygonAreaFastAASC(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);

        void renderScanline(const Point* Vl, const Point* Vr, int y);





        



// Subpixel Y resolution per scanline (must be in the power of two)
#define FAASC_SUB_Y_RES 2

// Subpixel X resolution per pixel (must be in the power of two)
#define FAASC_SUB_X_RES 2

// Subpixel Y modulo
#define FAASC_SUB_Y_MOD(Y) ((Y) & (FAASC_SUB_Y_RES - 1))

// Maximum area that a pixel can cover
#define FAASC_MAX_AREA (FAASC_SUB_Y_RES * FAASC_SUB_X_RES)

#define MAX_X   0x7FFF  /* subpixel X beyond right edge */

const Point *Vleft, *VnextLeft;      /* current left edge */
const Point *Vright, *VnextRight;    /* current right edge */

struct  SubPixel  {         /* subpixel extents for scanline */
    int xLeft, xRight;
    } sp[FAASC_SUB_Y_RES];

int xLmin, xLmax;       /* subpixel x extremes for scanline */
int xRmax, xRmin;       /* (for optimization shortcut) */

inline void interpolatePoints(Point* pointOut, const Point* pointA, const Point* pointB, double factor)
{
    const double ifactor = 1.0 - factor;

    pointOut->setX(ifactor * pointA->x() + factor * pointB->x());
    pointOut->setY(ifactor * pointA->y() + factor * pointB->y());
}

inline Pt::int32_t interpolateInteger(double alpha, Pt::int32_t a, int32_t b)
{
    double ialpha = 1.0 - alpha;

    return ialpha * a + alpha * b;
}

// Compute number of subpixels covered by polygon at current pixel
Pt::int32_t Coverage(Pt::int32_t x /* left subpixel of pixel */)
{
    // Fast Anti-Aliasing Polygon Scan Conversion
    // Jack Morrison, Graphics Gems, Academic Press, 1990
    // http://www.realtimerendering.com/resources/GraphicsGems/gems/AAPolyScan.c

    Pt::int32_t area;                     // Total covered area
    Pt::int32_t partialArea;              // Covered area for current subpixel y
    Pt::int32_t xr = x+FAASC_SUB_X_RES-1; // Right subpixel of pixel
    Pt::int32_t y;

    /* shortcut for common case of fully covered pixel */
    if (x>xLmax && x<xRmin)
        return FAASC_MAX_AREA;

    for (area=y=0; y<FAASC_SUB_Y_RES; y++) {
        partialArea = std::min(sp[y].xRight, xr)
             - std::max(sp[y].xLeft, x) + 1;
        if (partialArea > 0)
            area += partialArea;
    }
    return area;
}

/*
 * Render one scanline of polygon
 */

void Rasterizer2::renderScanline(const Point* Vl, const Point* Vr, int y)
{
    Point Vpixel;  /*object info interpolated at one pixel */
    int x;          /* leftmost subpixel of current pixel */

    for (x=FAASC_SUB_X_RES*floor((double)(xLmin/FAASC_SUB_X_RES)); x<=xRmax; x+=FAASC_SUB_X_RES) {
        int cov = Coverage(x) * 255 / FAASC_MAX_AREA;
        interpolatePoints(&Vpixel, Vl, Vr, (double)(x-xLmin)/(xRmax-xLmin));

            Pixel      dstPixel(_image->view(), x/FAASC_SUB_X_RES, y);
            _image->format().setPixel(dstPixel, _brush.color(), _compositionMode, cov);
    }
}



void Rasterizer2::rasterPolygonAreaFastAASC(const Point* points_, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{

    // Scale the polygon twice as big
    std::vector<Point>  spoints(pointCount);

    Point* points  = &spoints[0];

    for(size_t i = 0; i < pointCount; ++i) {
        points[i].setX(points_[i].x() * FAASC_SUB_X_RES);
        points[i].setY(points_[i].y() * FAASC_SUB_Y_RES);
    }



    const Point *endPoly;            /* end of polygon vertex list */
    Point VscanLeft, VscanRight;   /* interpolated vertices */                                 /* at scanline */
    double aLeft, aRight;           /* interpolation ratios */
    struct SubPixel *sp_ptr;        /* current subpixel info */
    int xLeft, xNextLeft;           /* subpixel coordinates for */
    int  xRight, xNextRight;        /* active polygon edges */
    int i,y;

/* find vertex with minimum y (display coordinate) */
Vleft = points;
for  (i=1; i<pointCount; i++)
    if  (points[i].y() < Vleft->y())
        Vleft = &points[i];
endPoly = &points[pointCount-1];

/* initialize scanning edges */
Vright = VnextRight = VnextLeft = Vleft;

/* prepare bottom of initial scanline - no coverage by polygon */
for (i=0; i<FAASC_SUB_Y_RES; i++)
    sp[i].xLeft = sp[i].xRight = -1;
xLmin = xRmin = MAX_X;
xLmax = xRmax = -1;

/* scan convert for each subpixel from bottom to top */
for (y=Vleft->y(); ; y++) {

    while (y == VnextLeft->y())   {   /* reached next left vertex */
        VnextLeft = (Vleft=VnextLeft) + 1;  /* advance */
        if (VnextLeft > endPoly)            /* (wraparound) */
            VnextLeft = points;
        if (VnextLeft == Vright)    /* all y's same?  */
            return;             /* (null polygon) */
        xLeft = Vleft->x();
        xNextLeft = VnextLeft->x();
    }

    while (y == VnextRight->y())  { /*reached next right vertex */
        VnextRight = (Vright=VnextRight) -1;
        if (VnextRight < points)           /* (wraparound) */
            VnextRight = endPoly;
        xRight = Vright->x();
        xNextRight = VnextRight->x();
    }

    if (y>VnextLeft->y() || y>VnextRight->y())  {
                /* done, mark uncovered part of last scanline */
        for (; FAASC_SUB_Y_MOD(y); y++)
            sp[FAASC_SUB_Y_MOD(y)].xLeft = sp[FAASC_SUB_Y_MOD(y)].xRight = -1;
        renderScanline(Vleft, Vright, y/FAASC_SUB_Y_RES);
        return;
    }

/*
 * Interpolate sub-pixel x endpoints at this y,
 * and update extremes for pixel coherence optimization
 */

    sp_ptr = &sp[FAASC_SUB_Y_MOD(y)];
    aLeft = (double)(y - Vleft->y()) / (VnextLeft->y() - Vleft->y());
    sp_ptr->xLeft = interpolateInteger(aLeft, xLeft, xNextLeft);
    if (sp_ptr->xLeft < xLmin)
        xLmin = sp_ptr->xLeft;
    if (sp_ptr->xLeft > xLmax)
        xLmax = sp_ptr->xLeft;

    aRight = (double)(y - Vright->y()) / (VnextRight->y()
                    - Vright->y());
    sp_ptr->xRight = interpolateInteger(aRight, xRight, xNextRight);
    if (sp_ptr->xRight < xRmin)
        xRmin = sp_ptr->xRight;
    if (sp_ptr->xRight > xRmax)
        xRmax = sp_ptr->xRight;

    if (FAASC_SUB_Y_MOD(y) == FAASC_SUB_Y_RES-1) {   /* end of scanline */
            /* interpolate edges to this scanline */
        interpolatePoints(&VscanLeft, Vleft, VnextLeft, aLeft);
        interpolatePoints(&VscanRight, Vright, VnextRight, aRight);
        renderScanline(&VscanLeft, &VscanRight, y/FAASC_SUB_Y_RES);
        xLmin = xRmin = MAX_X;      /* reset extremes */
        xLmax = xRmax = -1;
    }
  }
}
