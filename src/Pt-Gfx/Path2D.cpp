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

#include <Pt/Gfx/Path2D.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Math.h>


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Internal Helper Functions - Generator (Drawing) Functions ======================
// ======================================================================================

static inline void generateQuadraticBezierPoints(std::vector<PointF>& dst, double x1, double y1, double x2, double y2, double x3, double y3, Pt::uint8_t smoothness)
{
    // Check if the points actually specify a straight line
    const double sx = x3 - x2;
    const double sy = y3 - y2;
    const double xx = x1 - x2;
    const double yy = y1 - y2;

    if( !(xx * sy - yy * sx) ) { // Curvature
        if(dst.empty()) dst.push_back( PointF(x1, y1) );
        dst.push_back( PointF(x3, y3) );
        return;
    }

    // Calculate the length
    const double l1 = ::sqrt(sx * sx + sy * sy);
    const double l2 = ::sqrt(xx * xx + yy * yy);
    const double l3 = l1 + l2;

    // Determine the number of segments
    const Pt::int32_t mf = (Pt::int32_t) smoothness + 1;

    Pt::int32_t nSegs = round(l3 * mf / 16) + 2;
    if(nSegs < 5) nSegs = 5;

    const double nSegs1i = 1.0 / (nSegs - 1);

    for(Pt::int32_t i = 0; i < nSegs; ++i) {
        // Calculate the coordinates
        const double t  = i * nSegs1i;
        const double it = 1.0 - t;
        const double a  = it * it;
        const double b  = 2.0 * t  * it;
        const double c  = t * t;
        const double x  = a * x1 + b * x2 + c * x3;
        const double y  = a * y1 + b * y2 + c * y3;
        // Store the coordinate as needed
        if(i || dst.empty()) dst.push_back( PointF(x, y) );
    }
}

static inline void generateArcPoints(std::vector<PointF>& dst, double x1, double y1, double x2, double y2, double r, Pt::uint8_t smoothness)
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
// ===== Path2D::PathData Implementation ================================================
// ======================================================================================

struct Path2D::PathData {
    // Instruction type
    enum InsType {
        IT_Begin, IT_End,
        IT_MoveTo, IT_LineTo, IT_ArcTo, IT_QuadBezierTo
    };

    // Instruction structure
    struct Instruction {
        InsType type;
        double   p1, p2, p3, p4;

        inline Instruction(InsType type_)
        : type(type_)
        {}

        inline Instruction(InsType type_, double p1_)
        : type(type_), p1(p1_)
        {}

        inline Instruction(InsType type_, double p1_, double p2_)
        : type(type_), p1(p1_), p2(p2_)
        {}

        inline Instruction(InsType type_, double p1_, double p2_, double p3_)
        : type(type_), p1(p1_), p2(p2_), p3(p3_)
        {}

        inline Instruction(InsType type_, double p1_, double p2_, double p3_, double p4_)
        : type(type_), p1(p1_), p2(p2_), p3(p3_), p4(p4_)
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

    inline void add(InsType type, double p1)
    { inss.push_back( Instruction(type, p1) ); }

    inline void add(InsType type, double p1, double p2)
    { inss.push_back( Instruction(type, p1, p2) ); }

    inline void add(InsType type, double p1, double p2, double p3)
    { inss.push_back( Instruction(type, p1, p2, p3) ); }

    inline void add(InsType type, double p1, double p2, double p3, double p4)
    { inss.push_back( Instruction(type, p1, p2, p3, p4) ); }
};


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

Path2D::Path2D()
: _pathData( new PathData() )
{}

Path2D::~Path2D()
{ delete _pathData; }

void Path2D::clear()
{ _pathData->clear(); }

void Path2D::beginPath()
{
    // Check if this function call is valid in the current context
    if( !_pathData->empty() && !_pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_Begin);
}

void Path2D::endPath()
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_Begin) || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_End);
}

void Path2D::moveTo(double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) || _pathData->lastInstructionMatch(PathData::IT_MoveTo) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_MoveTo, x, y);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path2D::lineTo(double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_LineTo, x, y);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path2D::arcTo(double x, double y, double r)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_ArcTo, x, y, r);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path2D::quadraticBezierTo(double cx, double cy, double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_QuadBezierTo, cx, cy, x, y);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path2D::relMoveTo(double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) || _pathData->lastInstructionMatch(PathData::IT_MoveTo) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_MoveTo, _pathData->curX + x, _pathData->curY + y);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

void Path2D::relLineTo(double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_LineTo, _pathData->curX + x, _pathData->curY + y);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

void Path2D::relArcTo(double x, double y, double r)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_ArcTo, _pathData->curX + x, _pathData->curY + y, r);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

void Path2D::relQuadraticBezierTo(double cx, double cy, double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

    // Store the instruction
    _pathData->add(PathData::IT_QuadBezierTo, _pathData->curX + cx, _pathData->curY + cy, _pathData->curX + x, _pathData->curY + y);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path2D::generatePoints(std::vector<PointF>& dst, Pt::uint8_t smoothness) const
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || !_pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);

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
                curX = ins.p1;
                curY = ins.p2;
                break;

            case PathData::IT_LineTo:
                if(dst.empty()) dst.push_back( PointF(curX, curY) );
                curX = ins.p1;
                curY = ins.p2;
                dst.push_back( PointF(curX, curY) );
                break;

            case PathData::IT_ArcTo:
                generateArcPoints(dst, curX, curY, ins.p1, ins.p2, ins.p3, smoothness);
                curX = ins.p1;
                curY = ins.p2;
                break;

            case PathData::IT_QuadBezierTo:
                generateQuadraticBezierPoints(dst, curX, curY, ins.p1, ins.p2, ins.p3, ins.p4, smoothness);
                curX = ins.p3;
                curY = ins.p4;
                break;

            default:
                throw Path2DError("Invalid Path2D instruction type");
                break;
        }
    }

    //for(size_t i = 0; i < dst.size(); ++i)
    //    printf("GenPts: %5.1f, %5.1f\n", dst[i].x(), dst[i].y());
    //printf("\n");
}


} // namespace
} // namespace
