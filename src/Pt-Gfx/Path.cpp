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

static inline void generateGenericNBezierPoints(std::vector<PointF>& dst, double x1, double y1, const std::vector<double>& points, float smoothness)
{
    // ### TODO: Make the curve smoother, if possible. ###

    // Add the start coordinate to the point
    std::vector<double> pts;
    pts.reserve(points.size() + 2);

    pts.push_back(x1);
    pts.push_back(y1);

    pts.insert(pts.end(), points.begin(), points.end());

    /*
    // ### TODO: Complete the support for N-th degree spline curve !!! ###

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
    Pt::int32_t nSegs = round(lb * smoothness / 8);
    if(nSegs < (pts.size() + 1)) nSegs = (pts.size() + 1);
    */
    Pt::int32_t nSegs = 25;

    const double nSegs1i = 1.0 / (nSegs - 1);

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

static inline void generateCubicBezierPoints(std::vector<PointF>& dst, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, float smoothness)
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
    Pt::int32_t nSegs = round(lb * smoothness / 16) + 4;
    if(nSegs < 9) nSegs = 9;

    const double nSegs1i = 1.0 / (nSegs - 1);

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

static inline void generateQuadraticBezierPoints(std::vector<PointF>& dst, double x1, double y1, double x2, double y2, double x3, double y3, float smoothness)
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
    Pt::int32_t nSegs = round(lb * smoothness / 16) + 2;
    if(nSegs < 5) nSegs = 5;

    const double nSegs1i = 1.0 / (nSegs - 1);

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

static inline void generateArcPoints(std::vector<PointF>& dst, double x1, double y1, double x2, double y2, double r, float smoothness)
{
    // Line equation : 0 = aX + By + c
    // Normal        : n = ai + bj
    const double a = y2 - y1;
    const double b = x1 - x2;
  //const double c = -(x1 * y2 - x2 * y1);

    // Negated inverse line length
    const float il = -Gfx::Math::fastInvSqrt(a * a + b * b);

    // Circumference vector
    const double cx =  a * il * r;
    const double cy =  b * il * r;

    // Middle point
    const double xm = (x1 + x2) * 0.5;
    const double ym = (y1 + y2) * 0.5;

    // Use quadratic bezier curve to generate the arc
    generateQuadraticBezierPoints(dst, x1, y1, x1 + cx, y1 + cy, xm + cx, ym + cy, smoothness);
    generateQuadraticBezierPoints(dst, xm + cx, ym + cy, x2 + cx, y2 + cy, x2, y2, smoothness);
}


// ======================================================================================
// ===== Path::PathData Implementation ================================================
// ======================================================================================

struct Path::PathData {
    // Instruction type
    enum InsType {
        IT_Begin, IT_End,
        IT_MoveTo, IT_LineTo, IT_ArcTo, IT_QuadBezierTo, IT_CubicBezierTo, IT_GenNBezierTo
    };

    // Instruction structure
    struct Instruction {
        InsType             type;
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
};


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

Path::Path()
: _pathData( new PathData() )
{}

Path::~Path()
{ delete _pathData; }

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

void Path::genericNBezierTo(Pt::int32_t controlPointCount, double* cxy, double x, double y)
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

void Path::relGenericNBezierTo(Pt::int32_t controlPointCount, double* cxy, double x, double y)
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
                if(!dst.empty()) dst.push_back( Painter::PolygonSeparatorPointF );
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

            default:
                throw PathError("Invalid Path instruction type");
                break;
        }
    }

    //for(size_t i = 0; i < dst.size(); ++i)
    //    printf("GenPts: %5.1f, %5.1f\n", dst[i].x(), dst[i].y());
    //printf("\n");
}


} // namespace
} // namespace
