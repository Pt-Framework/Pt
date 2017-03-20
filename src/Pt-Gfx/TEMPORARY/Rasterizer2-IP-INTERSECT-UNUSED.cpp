static inline bool bboxDetectPolygonIntersection(const PointF* poly1, size_t poly1Count, const PointF* poly2, size_t poly2Count)
{
    float minX1 = poly1[0].x();
    float minY1 = poly1[0].y();
    float maxX1 = minX1;
    float maxY1 = minY1;
    for(size_t i = 1; i < poly1Count; ++i) {
        if(poly1[i].x() < minX1) minX1 = poly1[i].x();
        if(poly1[i].y() < minY1) minY1 = poly1[i].y();
        if(poly1[i].x() > maxX1) maxX1 = poly1[i].x();
        if(poly1[i].y() > maxY1) maxY1 = poly1[i].y();
    }

    float minX2 = poly2[0].x();
    float minY2 = poly2[0].y();
    float maxX2 = minX2;
    float maxY2 = minY2;
    for(size_t i = 1; i < poly2Count; ++i) {
        if(poly2[i].x() < minX2) minX2 = poly2[i].x();
        if(poly2[i].y() < minY2) minY2 = poly2[i].y();
        if(poly2[i].x() > maxX2) maxX2 = poly2[i].x();
        if(poly2[i].y() > maxY2) maxY2 = poly2[i].y();
    }

#define CHECK_IF_INSIDE_BOUNDING_BOX(X, Y, X1, Y1, X2, Y2) if( (X) >= (X1) && (X) <= (X2) && (Y) >= (Y1) && Y <= (Y2) ) return true

    CHECK_IF_INSIDE_BOUNDING_BOX(minX2, minY2, minX1, minY1, maxX1, maxY1);
    CHECK_IF_INSIDE_BOUNDING_BOX(minX2, maxY2, minX1, minY1, maxX1, maxY1);
    CHECK_IF_INSIDE_BOUNDING_BOX(maxX2, minY2, minX1, minY1, maxX1, maxY1);
    CHECK_IF_INSIDE_BOUNDING_BOX(maxX2, maxY2, minX1, minY1, maxX1, maxY1);

#undef CHECK_INSIDE_BOUNDING_BOX

    return false;
}






// Based on: Naive Algorithm in Detecting intersection of convex polygons in 2D
//           http://wm.ite.pl/articles/convex-polygon-intersection/article.html
//           http://wm.ite.pl/articles/convex-polygon-intersection/demo/demo.xhtml
//           http://wm.ite.pl/articles/convex-polygon-intersection/demo/line.js
//           Public domain code by Wojciech Muła, 2013-2017
static inline Pt::int32_t naiveDPILineSide(float la, float lb, float lc, const PointF& p)
{
    const Pt::int32_t val = la * p.x() + lb * p.y() + lc;

    if(val < 0.0f) return -1;
    if(val > 0.0f) return  1;

    return 0;
}

// Based on: Naive Algorithm in Detecting intersection of convex polygons in 2D
//           http://wm.ite.pl/articles/convex-polygon-intersection/article.html
//           http://wm.ite.pl/articles/convex-polygon-intersection/demo/demo.xhtml
//           http://wm.ite.pl/articles/convex-polygon-intersection/demo/naive.js
//           Public domain code by Wojciech Muła, 2013-2017
static inline Pt::int32_t naiveDPIGetSide(float la, float lb, float lc, const PointF& p1, const PointF& p2)
{
    const Pt::int32_t s1 = naiveDPILineSide(la, lb, lc, p1);
    const Pt::int32_t s2 = naiveDPILineSide(la, lb, lc, p2);

    const Pt::int32_t s  = s1 * s2;

         if(s < 0) return 0xFF;
    else if(s > 0) return s1;


    if(!s1) return s2;
    if(!s2) return s1;

    return 0;
}

// Based on: Naive Algorithm in Detecting intersection of convex polygons in 2D
//           http://wm.ite.pl/articles/convex-polygon-intersection/article.html
//           http://wm.ite.pl/articles/convex-polygon-intersection/demo/demo.xhtml
//           http://wm.ite.pl/articles/convex-polygon-intersection/demo/naive.js
//           Public domain code by Wojciech Muła, 2013-2017
static inline bool naiveDetectPolygonIntersection(const PointF* poly1, size_t poly1Count, const PointF* poly2, size_t poly2Count)
{
    #define FIX_INDEX(I, M)  ( ( (I) < 0 ) ? ( (I) + (M) ) : ( ( (I) >= (M) ) ? ( (I) - (M) ) : (I) ) )

    for(size_t i = 0; i < poly1Count; ++i) {
        // Get the points
        const PointF& a1 = poly1[FIX_INDEX(i - 1, poly1Count)];
        const PointF& a2 = poly1[FIX_INDEX(i    , poly1Count)];
        const PointF& a3 = poly1[FIX_INDEX(i + 1, poly1Count)];
        for(size_t j = 0; j < poly2Count; ++j) {
            // Get the points
            const PointF& b1 = poly2[FIX_INDEX(j - 1, poly2Count)];
            const PointF& b2 = poly2[FIX_INDEX(j    , poly2Count)];
            const PointF& b3 = poly2[FIX_INDEX(j + 1, poly2Count)];
            // Calculate the line's parameter
            const float la =  ( b2.y() - a2.y() );
            const float lb = -( b2.x() - a2.x() );
            const float lc = -( la * a2.x() + lb * a2.y() );
            // Get the sides
            const Pt::int32_t sideA = naiveDPIGetSide(la, lb, lc, a1, a3);
            const Pt::int32_t sideB = naiveDPIGetSide(la, lb, lc, b1, b3);
            if(sideA == 0xFF || sideB == 0xFF) continue;
            if(sideA * sideB < 0) return true;
        }
    }

    #undef FIX_INDEX

    return false;
}
