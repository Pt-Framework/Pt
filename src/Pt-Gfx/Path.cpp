/* Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include <Pt/SourceInfo.h>

#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Math.h>

#include "DrawText2.h"

#include "clipper_aj/clipper.hpp"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Internal Helper Functions - Generator (Drawing) Functions ======================
// ======================================================================================

// Based on: http://stackoverflow.com/a/21642962
//           Answer by iforce2d, 2014
static inline void getGenericNBezierPoint(double& x, double& y, const std::vector<double>& points, double t)
{
    std::vector<double> tmp = points;

    size_t i = points.size() / 2 - 1;

    while(i > 0) {
        for(size_t k = 0; k < i; ++k) {
            const size_t cidx =  k      * 2;
            const size_t nidx = (k + 1) * 2;
            tmp[cidx + 0] = tmp[cidx + 0] + t * ( tmp[nidx + 0] - tmp[cidx + 0] ); // X
            tmp[cidx + 1] = tmp[cidx + 1] + t * ( tmp[nidx + 1] - tmp[cidx + 1] ); // Y
        }
        --i;
    }

    x = tmp[0];
    y = tmp[1];
}

static inline void generateGenericNBezierPoints(std::vector<PointF>& dst, double x1, double y1, const std::vector<double>& points, double smoothness)
{
    // ### TODO: Make the curve smoother, if possible. ###

    // Add the start coordinate to the point
    std::vector<double> pts;
    pts.reserve(points.size() + 2);

    pts.push_back(x1);
    pts.push_back(y1);

    pts.insert(pts.end(), points.begin(), points.end());

    // Calculate the approximate length of the curve
    double clen = 0.0;
    for(size_t i = 0; i < (points.size() / 2 - 1); ++i) {
        const size_t cidx =  i      * 2;
        const size_t nidx = (i + 1) * 2;
        const double x1   = pts[cidx + 0];
        const double y1   = pts[cidx + 1];
        const double x2   = pts[nidx + 0];
        const double y2   = pts[nidx + 1];
        const double dx   = x2 - x1;
        const double dy   = y2 - y1;
        clen += ::sqrt(dx * dx + dy * dy);
    }

    // Determine the number of segments
    const Pt::int32_t minNS = pts.size() / 2 + 1;
          Pt::int32_t nSegs = Gfx::Math::zrint(clen * smoothness / 24);
    if(nSegs < minNS) nSegs = minNS;

    // Calculate the inverse multiplication factor
    const double nSegs1i = 1.0 / (nSegs - 1);

    // Generate the points
    for(Pt::int32_t i = 0; i < nSegs; ++i) {
        // Calculate the coordinates
        const double t  = i * nSegs1i;
              double x;
              double y;
        getGenericNBezierPoint(x, y, pts, t);
        // Store the coordinate as needed
        if(i || dst.empty()) dst.push_back( PointF(x, y) );
    }
}

static inline void generateCubicBezierPoints(std::vector<PointF>& dst, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double smoothness)
{
    // ### TODO: Make the curve smoother, if possible. ###

    // Calculate the approximate length of the curve
    const double dx43 = x4 - x3;
    const double dy43 = y4 - y3;
    const double dx32 = x3 - x2;
    const double dy32 = y3 - y2;
    const double dx12 = x1 - x2;
    const double dy12 = y1 - y2;
    const double l43  = ::sqrt(dx43 * dx43 + dy43 * dy43);
    const double l32  = ::sqrt(dx32 * dx32 + dy32 * dy32);
    const double l12  = ::sqrt(dx12 * dx12 + dy12 * dy12);
    const double lb   = l43 + l32 + l12;

    // Determine the number of segments
    Pt::int32_t nSegs = Gfx::Math::zrint(lb * smoothness / 16) + 4;
    if(nSegs < 9) nSegs = 9;

    // Calculate the inverse multiplication factor
    const double nSegs1i = 1.0 / (nSegs - 1);

    // Generate the points
    // PB = (1 - t) * (1 - t) * (1 - t) * P1 + 3 * t * (1 - t) * (1 - t) * P2 + 3 * t * t * (1 - t) * P3 + t * t * t * P4
    //      ---------------------------        -------------------------        -------------------        ---------
    //      a                                  b                                c                          d
    for(Pt::int32_t i = 0; i < nSegs; ++i) {
        // Calculate the coordinates
        const double t  = i * nSegs1i;
        const double it = 1.0 - t;
        const double a  = it * it * it;
        const double b  = 3.0 * t * it * it;
        const double c  = 3.0 * t * t * it;
        const double d  = t * t * t;
        const double x  = a * x1 + b * x2 + c * x3 + d * x4;
        const double y  = a * y1 + b * y2 + c * y3 + d * y4;
        // Store the coordinate as needed
        if(i || dst.empty()) dst.push_back( PointF(x, y) );
    }
}

static inline void generateQuadraticBezierPoints(std::vector<PointF>& dst, double x1, double y1, double x2, double y2, double x3, double y3, double smoothness)
{
    // ### TODO: Make the curve smoother, if possible. ###

    // Check if the points actually specify a straight line
    const double dx32 = x3 - x2;
    const double dy32 = y3 - y2;
    const double dx12 = x1 - x2;
    const double dy12 = y1 - y2;

    if( !(dx12 * dy32 - dy12 * dx32) ) { // Curvature
        if(dst.empty()) dst.push_back( PointF(x1, y1) );
        dst.push_back( PointF(x3, y3) );
        return;
    }

    // Calculate the approximate length of the curve
    const double l32 = ::sqrt(dx32 * dx32 + dy32 * dy32);
    const double l12 = ::sqrt(dx12 * dx12 + dy12 * dy12);
    const double lb  = l32 + l12;

    // Determine the number of segments
    Pt::int32_t nSegs = Gfx::Math::zrint(lb * smoothness / 16) + 2;
    if(nSegs < 5) nSegs = 5;

    // Calculate the inverse multiplication factor
    const double nSegs1i = 1.0 / (nSegs - 1);

    // Generate the points
    // PB = (1 - t) * (1 - t) * P1 + 2 * t * (1 - t) * P2 + t * t * P3
    //      -----------------        ---------------        -----
    //      a                        b                      c
    for(Pt::int32_t i = 0; i < nSegs; ++i) {
        // Calculate the coordinates
        const double t  = i * nSegs1i;
        const double it = 1.0 - t;
        const double a  = it * it;
        const double b  = 2.0 * t * it;
        const double c  = t * t;
        const double x  = a * x1 + b * x2 + c * x3;
        const double y  = a * y1 + b * y2 + c * y3;
        // Store the coordinate as needed
        if(i || dst.empty()) dst.push_back( PointF(x, y) );
    }
}

static inline void generateArcPoints(std::vector<PointF>& dst, double x1, double y1, double x2, double y2, double r, double smoothness)
{
    // Line equation : 0 = aX + By + c
    // Normal        : n = ai + bj
    const double a = y2 - y1;
    const double b = x1 - x2;
  //const double c = -(x1 * y2 - x2 * y1);

    // Negated inverse line length
    const double il = -Gfx::Math::fastInvSqrt(a * a + b * b);

#if 1
    // Circumference vector
    const double cx = (1.0 + 1.0 / 3.0) * a * il * r;
    const double cy = (1.0 + 1.0 / 3.0) * b * il * r;

    // Use cubic bezier curve to generate the arc
    generateCubicBezierPoints(dst, x1, y1, x1 + cx, y1 + cy, x2 + cx, y2 + cy, x2, y2, smoothness);
#else
    // Circumference vector
    const double cx = a * il * r;
    const double cy = b * il * r;

    // Middle point
    const double xm = (x1 + x2) * 0.5;
    const double ym = (y1 + y2) * 0.5;

    // Use quadratic bezier curve to generate the arc
    generateQuadraticBezierPoints(dst, x1, y1, x1 + cx, y1 + cy, xm + cx, ym + cy, smoothness);
    generateQuadraticBezierPoints(dst, xm + cx, ym + cy, x2 + cx, y2 + cy, x2, y2, smoothness);
#endif
}

static inline void generateChrPoints(std::vector<PointF>& dst, double x, double y, const std::vector<PointF>& pointsF_, const std::vector<Pt::uint8_t>& tags, const std::vector<Pt::int32_t>& contours, double smoothness)
{
    //std::clog << "points/tags = " << points.size() << " ; contours = " << contours.size() << std::endl;

    // Translate the points
    std::vector<PointF> pointsF(pointsF_.size());

    for(size_t i = 0; i < pointsF_.size(); ++i) {
        pointsF[i].set(
            x + pointsF_[i].x(),
            y + pointsF_[i].y()
        );
    }

    // Helper macros
    //    The point tags:
    //        Bit #0 -> 0 = control point          ; 1 = non-control point
    //        Bit #1 -> 0 = quadratic bezier (TTF) ; 1 = cubic bezier (OTF)
    //        Bit #2 -> 0 = bit #5-#7 is unused    ; 1 = bit #5-#7 contain the OTF drop-out mode (currently ignored)
    #define CURVE_TAG_C_POINT(T) ( ( (T) & 0x03 ) == 0x01 )
    #define CURVE_TAG_Q_B_CTL(T) ( ( (T) & 0x03 ) == 0x00 )
    #define CURVE_TAG_C_B_CTL(T) ( ( (T) & 0x03 ) == 0x02 )

    // Index of the first point in the contour
    Pt::int32_t begIdx = 0;

    // Walk through the contours
    for(size_t i = 0; i < contours.size(); ++i) {
       // Index of the last point in the contour
        Pt::int32_t endIdx = contours[i];
        // Prepare the iterators
        const PointF*      pMax = &pointsF[endIdx];
        const PointF*      pItr = &pointsF[begIdx];
        const Pt::uint8_t* tItr = &tags   [begIdx];
        // Get the initial begin, end, and control points as well as the tag
        PointF      pBeg = pointsF[begIdx];
        PointF      pEnd = pointsF[endIdx];
        PointF      pCtl = pBeg;
        Pt::uint8_t pTag = tItr[0];
        // A contour cannot begin with a cubic bezier control point
        if(CURVE_TAG_C_B_CTL(pTag)) {
            dst.clear();
            return;
        }
        // Check the tag of the begin point to determine the origin
        if(CURVE_TAG_Q_B_CTL(pTag)) {
            // Start from the end point if it is on the curve
            if(CURVE_TAG_C_POINT(tags[endIdx])) {
                pBeg = pEnd;
                --pMax;
            }
            // Both begin and points are quadratic bezier control points, hence,
            // start at the middle
            else {
              pBeg.set(
                  ( pBeg.x() + pEnd.x() ) * 0.5,
                  ( pBeg.y() + pEnd.y() ) * 0.5
              );
              pEnd = pBeg;
            }
            // Adjust the iterators
            --pItr;
            --tItr;
        }
        // Start a new subpath
        if(!dst.empty()) dst.push_back(Painter::PolygonSeparatorPointF);
        dst.push_back(pBeg);
        // Walk through the points
        while(pItr < pMax) {
            // Adjust the iterators
            ++pItr;
            ++tItr;
            // Get the new tag
            pTag = tItr[0];
            // If the point is on the curve, generate a line
            if(CURVE_TAG_C_POINT(pTag)) {
                dst.push_back(PointF(pItr->x(), pItr->y()));
                continue;
            }
            // If the point is a quadratic bezier control point, generate quadratic bezier curve(s)
            else if(CURVE_TAG_Q_B_CTL(pTag)) {
                // Update the control point
                pCtl = *pItr;
                // There can be multiple quadratic bezier curves, process them all
                bool done = false;
                while(!done && pItr < pMax) {
                    // Adjust the iterators
                    ++pItr;
                    ++tItr;
                    // Get the new tag
                    pTag = tItr[0];
                    // A non-control point marks the end of the curves
                    if(CURVE_TAG_C_POINT(pTag)) {
                        generateQuadraticBezierPoints(
                            dst,
                            dst.back().x(), dst.back().y(),
                            pCtl      .x(), pCtl      .y(),
                            pItr     ->x(), pItr     ->y(),
                            smoothness
                        );
                        done = true;
                        break;
                    }
                    if(done) break;
                    // If the point turns out to be not a quadratic bezier control point,
                    // abort the whole process
                    if(!CURVE_TAG_Q_B_CTL(pTag)) {
                        dst.clear();
                        return;
                    }
                    // Generate one quadratic bezier curve to the middle point
                    generateQuadraticBezierPoints(
                        dst,
                        dst.back().x(),                 dst.back().y(),
                        pCtl      .x(),                 pCtl      .y(),
                      ( pCtl.x() + pItr->x() ) * 0.5, ( pCtl.y() + pItr->y() ) * 0.5,
                        smoothness
                    );
                    // Update the control point
                    pCtl = *pItr;
                }
                if(done) continue;
                // Generate one quadratic bezier curve to the begin point
                generateQuadraticBezierPoints(
                    dst,
                    dst.back().x(), dst.back().y(),
                    pCtl      .x(), pCtl      .y(),
                    pBeg      .x(), pBeg      .y(),
                    smoothness
                );
                // Update the control point
                pItr = pMax;
            }
            // Generate a cubic bezier curve
            else {
                // If the next point turns out to be not a cubic bezier control point,
                // abort the whole process
                if( pItr + 1 > pMax || !CURVE_TAG_C_B_CTL(tItr[1]) ) {
                    dst.clear();
                    return;
                }
                // Capture the control points
                const PointF& pCtl1 = pItr[0];
                const PointF& pCtl2 = pItr[1];
                // Adjust the iterators
                pItr += 2;
                tItr += 2;
                // Generate one cubic bezier curve to the current point
                if(pItr < pMax) {
                    generateCubicBezierPoints(
                        dst,
                        dst.back().x(), dst.back().y(),
                        pCtl1     .x(), pCtl1     .y(),
                        pCtl2     .x(), pCtl2     .y(),
                        pItr     ->x(), pItr     ->y(),
                        smoothness
                    );
                    continue;
                }
                // Generate one cubic bezier curve to the begin point
                generateCubicBezierPoints(
                    dst,
                    dst.back().x(), dst.back().y(),
                    pCtl1     .x(), pCtl1     .y(),
                    pCtl2     .x(), pCtl2     .y(),
                    pBeg      .x(), pBeg      .y(),
                    smoothness
                );
                // Set the point iterator to maximum to end the iteration
                pItr = pMax;
            }
        }
        // Update the start index of the contour
        begIdx = endIdx + 1;
    }

    // Add a separator point
    dst.push_back(Painter::PolygonSeparatorPointF);

    // Undefine the helper macro
    #undef CURVE_TAG_C_POINT
    #undef CURVE_TAG_Q_B_CTL
    #undef CURVE_TAG_C_B_CTL
}


// ======================================================================================
// ===== Path::PathData Implementation ==================================================
// ======================================================================================

struct Path::PathData {
    // Instruction type
    enum InsType {
        IT_Begin, IT_End,
        IT_MoveTo, IT_LineTo, IT_ArcTo, IT_QuadBezierTo, IT_CubicBezierTo, IT_GenNBezierTo,
        IT_Char
    };

    // Instruction structure
    struct Instruction {
        InsType             type;
        Char                chr;
        std::vector<double> p;

        inline Instruction(InsType type_)
        : type(type_)
        {}

        inline Instruction(InsType type_, double p0)
        : type(type_), p(1)
        { p[0] = p0; }

        inline Instruction(InsType type_, double p0, double p1)
        : type(type_), p(2)
        { p[0] = p0; p[1] = p1; }

        inline Instruction(InsType type_, double p0, double p1, double p2)
        : type(type_), p(3)
        { p[0] = p0; p[1] = p1; p[2] = p2; }

        inline Instruction(InsType type_, double p0, double p1, double p2, double p3)
        : type(type_), p(4)
        { p[0] = p0; p[1] = p1; p[2] = p2; p[3] = p3; }

        inline Instruction(InsType type_, double p0, double p1, double p2, double p3, double p4)
        : type(type_), p(5)
        { p[0] = p0; p[1] = p1; p[2] = p2; p[3] = p3; p[4] = p4; }

        inline Instruction(InsType type_, double p0, double p1, double p2, double p3, double p4, double p5)
        : type(type_), p(6)
        { p[0] = p0; p[1] = p1; p[2] = p2; p[3] = p3; p[4] = p4; p[5] = p5; }

        inline Instruction(InsType type_, const std::vector<double>& p_)
        : type(type_), p(p_)
        {}

        inline Instruction(const Char& chr_)
        : type(IT_Char), chr(chr_)
        {}
    };

    typedef std::vector<Instruction> Instructions;

    // Data
    double       curX, curY;
    Instructions inss;

    // Member functions
    inline PathData()
    : curX(0.0), curY(0.0)
    {}

    inline void clear()
    {
        curX = 0.0;
        curY = 0.0;
        inss.clear();
    }

    inline bool empty() const
    { return inss.empty(); }

    inline bool lastInstructionMatch(InsType type) const
    { return inss.back().type == type; }

    inline void add(InsType type)
    { inss.push_back( Instruction(type) ); }

    inline void add(InsType type, double p0)
    { inss.push_back( Instruction(type, p0) ); }

    inline void add(InsType type, double p0, double p1)
    { inss.push_back( Instruction(type, p0, p1) ); }

    inline void add(InsType type, double p0, double p1, double p2)
    { inss.push_back( Instruction(type, p0, p1, p2) ); }

    inline void add(InsType type, double p0, double p1, double p2, double p3)
    { inss.push_back( Instruction(type, p0, p1, p2, p3) ); }

    inline void add(InsType type, double p0, double p1, double p2, double p3, double p4)
    { inss.push_back( Instruction(type, p0, p1, p2, p3, p4) ); }

    inline void add(InsType type, double p0, double p1, double p2, double p3, double p4, double p5)
    { inss.push_back( Instruction(type, p0, p1, p2, p3, p4, p5) ); }

    inline void add(InsType type, const std::vector<double>& p)
    { inss.push_back( Instruction(type, p) ); }

    inline void add(const Char& chr)
    { inss.push_back( Instruction(chr) ); }
};


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

Path::Path()
: _pathData( new PathData() )
, _text    ( new DrawText2() )
{}

Path::~Path()
{
    delete _text;
    delete _pathData;
}

void Path::clear()
{ _pathData->clear(); }

void Path::beginPath()
{
    // Check if this function call is valid in the current context
    if( !_pathData->empty() && !_pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_Begin);
}

void Path::endPath()
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_Begin) || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_End);
}

void Path::moveTo(double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) || _pathData->lastInstructionMatch(PathData::IT_MoveTo) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_MoveTo, x, y);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path::lineTo(double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_LineTo, x, y);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path::arcTo(double x, double y, double r)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_ArcTo, x, y, r);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path::quadraticBezierTo(double cx, double cy, double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_QuadBezierTo, cx, cy, x, y);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path::cubicBezierTo(double cx1, double cy1, double cx2, double cy2, double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_CubicBezierTo, cx1, cy1, cx2, cy2, x, y);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path::genericNBezierTo(Pt::int32_t controlPointCount, const double* cxy, double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Points buffer
    std::vector<double> points;

    // Extract and store the control coordinates
    if(controlPointCount < 3) throw PathError("A generic N-bezier must have at least 3 control points");

    for(Pt::int32_t i = 0; i < controlPointCount * 2; ++i) {
        points.push_back(cxy[i]);
    }

    // Store the end coordinate
    points.push_back(x);
    points.push_back(y);

    // Store the instruction
    _pathData->add(PathData::IT_GenNBezierTo, points);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path::relMoveTo(double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) || _pathData->lastInstructionMatch(PathData::IT_MoveTo) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_MoveTo, _pathData->curX + x, _pathData->curY + y);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

void Path::relLineTo(double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_LineTo, _pathData->curX + x, _pathData->curY + y);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

void Path::relArcTo(double x, double y, double r)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_ArcTo, _pathData->curX + x, _pathData->curY + y, r);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

void Path::relQuadraticBezierTo(double cx, double cy, double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_QuadBezierTo, _pathData->curX + cx, _pathData->curY + cy, _pathData->curX + x, _pathData->curY + y);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

void Path::relCubicBezierTo(double cx1, double cy1, double cx2, double cy2, double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_CubicBezierTo, _pathData->curX + cx1, _pathData->curY + cy1, _pathData->curX + cx2, _pathData->curY + cy2, _pathData->curX + x, _pathData->curY + y);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

void Path::relGenericNBezierTo(Pt::int32_t controlPointCount, const double* cxy, double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Points buffer
    std::vector<double> points;

    // Extract and store the control coordinates
    if(controlPointCount < 3) throw PathError("A generic N-bezier must have at least 3 control points");

    for(Pt::int32_t i = 0; i < controlPointCount * 2; ++i) {
        if(i % 2) points.push_back(cxy[i] + _pathData->curY); // Odd  -> Y
        else      points.push_back(cxy[i] + _pathData->curX); // Even -> X
    }

    // Store the end coordinate
    points.push_back(_pathData->curX + x);
    points.push_back(_pathData->curY + y);

    // Store the instruction
    _pathData->add(PathData::IT_GenNBezierTo, points);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

void Path::setFont(const Font& font)
{
    _font = font;
    _text->setFont(_font);
}

const Font& Path::font() const
{ return _font; }

void Path::putChar(const Char& chr)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(chr);
}

void Path::getCharSpacing(Pt::int32_t& x, Pt::int32_t& y, const Char& from, const Char& to)
{ _text->getCharSpacing(x, y, from, to); }

void Path::generatePoints(std::vector<PointF>& dst, float smoothness) const
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || !_pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // For convenience
    typedef std::vector<PathData::Instruction>::const_iterator PDIIterator;

    // State variables
    double curX = 0.0;
    double curY = 0.0;

    // Walk through the instructions
    for(PDIIterator it = _pathData->inss.begin(); it != _pathData->inss.end(); ++it) {
        // Get the instruction
        const PathData::Instruction& ins = *it;
        // Act based on the type of the instruction
        switch(ins.type) {
            case PathData::IT_Begin:
                if(!dst.empty()) dst.push_back(Painter::PolygonSeparatorPointF);
                break;

            case PathData::IT_End:
                // Nothing to do here!
                break;

            case PathData::IT_MoveTo:
                curX = ins.p[0];
                curY = ins.p[1];
                break;

            case PathData::IT_LineTo:
                if(dst.empty()) dst.push_back( PointF(curX, curY) );
                curX = ins.p[0];
                curY = ins.p[1];
                dst.push_back( PointF(curX, curY) );
                break;

            case PathData::IT_ArcTo:
                generateArcPoints(dst, curX, curY, ins.p[0], ins.p[1], ins.p[2], smoothness);
                curX = ins.p[0];
                curY = ins.p[1];
                break;

            case PathData::IT_QuadBezierTo:
                generateQuadraticBezierPoints(dst, curX, curY, ins.p[0], ins.p[1], ins.p[2], ins.p[3], smoothness);
                curX = ins.p[2];
                curY = ins.p[3];
                break;

            case PathData::IT_CubicBezierTo:
                generateCubicBezierPoints(dst, curX, curY, ins.p[0], ins.p[1], ins.p[2], ins.p[3], ins.p[4], ins.p[5], smoothness);
                curX = ins.p[4];
                curY = ins.p[5];
                break;

            case PathData::IT_GenNBezierTo:
                generateGenericNBezierPoints(dst, curX, curY, ins.p, smoothness);
                curX = ins.p[ins.p.size() - 2];
                curY = ins.p[ins.p.size() - 1];
                break;

            case PathData::IT_Char: {
                std::vector<PointF     > pointsF;
                std::vector<Pt::uint8_t> tags;
                std::vector<Pt::int32_t> contours;
                _text->pathFromChar(pointsF, tags, contours, ins.chr);
                generateChrPoints(dst, curX, curY, pointsF, tags, contours, smoothness);
                break;
            }

            default:
                throw PathError("Invalid Path instruction type");
                break;
        }
    }

    // Remove dangling separator point as needed
    if(!dst.empty() && dst.back().x() > Painter::MaximumCoordinateF && dst.back().y() > Painter::MaximumCoordinateF)
        dst.pop_back();

    //for(size_t i = 0; i < dst.size(); ++i)
    //    printf("GenPts: %5.1f, %5.1f\n", dst[i].x(), dst[i].y());
    //printf("\n");
}

void Path::clipPolygon(std::vector<PointF>& result, const std::vector<PointF>& subject, const std::vector<PointF>& clipRegion, ClipMode cm)
{
    // Scaling factors
    const double mmFac = 64.0;
    const double imFac =  0.015625; // (1.0 / 64.0)

    // Working variables
    ClipperLib::Clipper clipper;
    ClipperLib::Path    cpath;
    ClipperLib::Paths   cpresult;
    size_t              startIndex;

    // Separate and append the clipper polygons
    startIndex = 0;
    for(size_t i = 0; i <= clipRegion.size(); ++i) {
        // Search for the end and/or separator points
        if( i == clipRegion.size() || (clipRegion[i].x() > Painter::MaximumCoordinateF && clipRegion[i].y() > Painter::MaximumCoordinateF) ) {
            // Calculate the number of points for this polygon
            const size_t curPC = i - startIndex;
            // Append the polygon to the clipper
            cpath.resize(curPC);
            for(size_t j = 0; j < curPC; ++j) {
                cpath[j].X = Gfx::Math::zrint( clipRegion[startIndex + j].x() * mmFac );
                cpath[j].Y = Gfx::Math::zrint( clipRegion[startIndex + j].y() * mmFac );
            }
            clipper.AddPath(cpath, ClipperLib::ptClip, true);
            // Increment the start index
            startIndex += curPC + 1;
        }
    }

    // Separate and append the subject polygons
    startIndex = 0;
    for(size_t i = 0; i <= subject.size(); ++i) {
        // Search for the end and/or separator points
        if( i == subject.size() || (subject[i].x() > Painter::MaximumCoordinateF && subject[i].y() > Painter::MaximumCoordinateF) ) {
            // Calculate the number of points for this polygon
            const size_t curPC = i - startIndex;
            // Append the polygon to the clipper
            cpath.resize(curPC);
            for(size_t j = 0; j < curPC; ++j) {
                cpath[j].X = Gfx::Math::zrint( subject[startIndex + j].x() * mmFac );
                cpath[j].Y = Gfx::Math::zrint( subject[startIndex + j].y() * mmFac );
            }
            clipper.AddPath(cpath, ClipperLib::ptSubject, cpath[0] == cpath.back());
            // Increment the start index
            startIndex += curPC + 1;
        }
    }

    // Perform clipping
    result.clear();

    switch(cm) {
        case Intersection:
            clipper.Execute(ClipperLib::ctIntersection, cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
            break;

        case Union:
            clipper.Execute(ClipperLib::ctUnion,        cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
            break;

        case Difference:
            clipper.Execute(ClipperLib::ctDifference,   cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
            break;

        case Xor:
            clipper.Execute(ClipperLib::ctXor,          cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
            break;

        default:
            return;
    }

    // Combine back the result polygons
    for(size_t i = 0; i < cpresult.size(); ++i) {
        const ClipperLib::Path& curPath = cpresult[i];
        if(!result.empty()) result.push_back(Painter::PolygonSeparatorPointF);
        for(size_t j = 0; j < curPath.size(); ++j) {
            result.push_back( PointF( curPath[j].X * imFac, curPath[j].Y * imFac ) );
        }
    }
}


} // namespace
} // namespace
