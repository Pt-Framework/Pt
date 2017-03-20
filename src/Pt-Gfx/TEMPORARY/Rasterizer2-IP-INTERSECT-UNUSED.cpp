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

