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

// Based on: How do I implement a Bézier curve in C++?
//           http://stackoverflow.com/questions/785097/how-do-i-implement-a-bézier-curve-in-c
//           Answer by iforce2d, 2014 (permalink: http://stackoverflow.com/a/21642962)
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
    const Pt::int32_t nSegs = Gfx::Math::zrint(clen * abs(smoothness) / 20) + (pts.size() / 2 + 1 + 1);

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
    const Pt::int32_t nSegs = Gfx::Math::zrint(lb * abs(smoothness) / 20) + 4 + 1;

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
    //lprintf("(%5.1f, %5.1f) (%5.1f, %5.1f) (%5.1f, %5.1f)\n", x1, y1, x2, y2, x3, y3);
    //lprintf("(%5.1f, %5.1f) (%5.1f, %5.1f)\n", curX, curY, ins.p[0], ins.p[1]);

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
    const Pt::int32_t nSegs = Gfx::Math::zrint(lb * abs(smoothness) / 20) + 3 + 1;

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


// ======================================================================================
// ===== Path::PathData Implementation ==================================================
// ======================================================================================

struct Path::PathData {
    // Instruction type
    enum InsType {
        IT_Begin, IT_End,
        IT_MoveTo, IT_LineTo, IT_QuadBezierTo, IT_CubicBezierTo, IT_GenNBezierTo,
    };

    // Instruction structure
    struct Instruction {
        InsType             type;
        std::vector<double> pxy;

        inline Instruction(InsType type_)
        : type(type_)
        {}

        inline Instruction(InsType type_, double x0, double y0)
        : type(type_), pxy(2)
        { pxy[0] = x0; pxy[1] = y0; }

        inline Instruction(InsType type_, double x0, double y0, double x1, double y1)
        : type(type_), pxy(4)
        { pxy[0] = x0; pxy[1] = y0; pxy[2] = x1; pxy[3] = y1; }

        inline Instruction(InsType type_, double x0, double y0, double x1, double y1, double x2, double y2)
        : type(type_), pxy(6)
        { pxy[0] = x0; pxy[1] = y0; pxy[2] = x1; pxy[3] = y1; pxy[4] = x2; pxy[5] = y2; }

        inline Instruction(InsType type_, const std::vector<double>& pxy_)
        : type(type_), pxy(pxy_)
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

    inline void add(InsType type, double x0, double y0)
    { inss.push_back( Instruction(type, x0, y0) ); }

    inline void add(InsType type, double x0, double y0, double x1, double y1)
    { inss.push_back( Instruction(type, x0, y0, x1, y1) ); }

    inline void add(InsType type, double x0, double y0, double x1, double y1, double x2, double y2)
    { inss.push_back( Instruction(type, x0, y0, x1, y1, x2, y2 ) ); }

    inline void add(InsType type, const std::vector<double>& xy)
    { inss.push_back( Instruction(type, xy) ); }
};


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

Path::Path()
: _pathData( new PathData() )
{}

Path::Path(const Path& p)
: _pathData( 0 )
{ this->operator=(p); }

Path::~Path()
{}

const Path& Path::operator=(const Path& p)
{
    _pathData = p._pathData;

    return *this;
}

bool Path::isNull() const
{ return _pathData->empty(); }

void Path::clear()
{
    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

    // Clear the path data
    _pathData->clear();
}

// --- Path management ---

void Path::beginPath()
{
    // Check if this function call is valid in the current context
    if( !_pathData->empty() && !_pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

    // Store the instruction
    _pathData->add(PathData::IT_Begin);
}

void Path::endPath()
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_Begin) || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

    // Store the instruction
    _pathData->add(PathData::IT_End);
}

// --- Absolute coordinate ---

void Path::moveTo(double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) || _pathData->lastInstructionMatch(PathData::IT_MoveTo) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

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

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

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

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

    // Decompose and store the instruction
    decomposeAndStore_arcTo(_pathData->curX, _pathData->curY, x, y, r);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

void Path::quadraticBezierTo(double cx, double cy, double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

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

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

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

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

    // Store the instruction
    _pathData->add(PathData::IT_GenNBezierTo, points);

    // Update the current coordinate
    _pathData->curX = x;
    _pathData->curY = y;
}

// --- Relative coordinate ---

void Path::relMoveTo(double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) || _pathData->lastInstructionMatch(PathData::IT_MoveTo) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

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

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

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

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

    // Decompose and store the instruction
    decomposeAndStore_arcTo(_pathData->curX, _pathData->curY, _pathData->curX + x, _pathData->curY + y, r);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

void Path::relQuadraticBezierTo(double cx, double cy, double x, double y)
{
    // Check if this function call is valid in the current context
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw PathInvalidContext(PT_SOURCEINFO_STR);

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

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

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

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

    // COW
    if(_pathData.refs() > 1) {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }

    // Store the instruction
    _pathData->add(PathData::IT_GenNBezierTo, points);

    // Update the current coordinate
    _pathData->curX += x;
    _pathData->curY += y;
}

// --- Generate points ---

/*
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
                curX = ins.pxy[0];
                curY = ins.pxy[1];
                break;

            case PathData::IT_LineTo:
                if(dst.empty()) dst.push_back( PointF(curX, curY) );
                curX = ins.pxy[0];
                curY = ins.pxy[1];
                dst.push_back( PointF(curX, curY) );
                break;

            case PathData::IT_QuadBezierTo:
                generateQuadraticBezierPoints(dst, curX, curY, ins.pxy[0], ins.pxy[1], ins.pxy[2], ins.pxy[3], smoothness);
                curX = ins.pxy[2];
                curY = ins.pxy[3];
                break;

            case PathData::IT_CubicBezierTo:
                generateCubicBezierPoints(dst, curX, curY, ins.pxy[0], ins.pxy[1], ins.pxy[2], ins.pxy[3], ins.pxy[4], ins.pxy[5], smoothness);
                curX = ins.pxy[4];
                curY = ins.pxy[5];
                break;

            case PathData::IT_GenNBezierTo:
                generateGenericNBezierPoints(dst, curX, curY, ins.pxy, smoothness);
                curX = ins.pxy[ins.pxy.size() - 2];
                curY = ins.pxy[ins.pxy.size() - 1];
                break;

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
*/


// ======================================================================================
// ===== Static Public Member Functions =================================================
// ======================================================================================

void Path::clipPolygon(std::vector<PointF>& result, const std::vector<PointF>& subject, const std::vector<PointF>& clipRegion, ClipMode cm)
{
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
                cpath[j].X = Gfx::Math::zrint( clipRegion[startIndex + j].x() * Gfx::Math::VecResScaleUp );
                cpath[j].Y = Gfx::Math::zrint( clipRegion[startIndex + j].y() * Gfx::Math::VecResScaleUp );
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
                cpath[j].X = Gfx::Math::zrint( subject[startIndex + j].x() * Gfx::Math::VecResScaleUp );
                cpath[j].Y = Gfx::Math::zrint( subject[startIndex + j].y() * Gfx::Math::VecResScaleUp );
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
            result.push_back( PointF(
                curPath[j].X * Gfx::Math::VecResScaleDn,
                curPath[j].Y * Gfx::Math::VecResScaleDn
            ) );
        }
    }
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void Path::decomposeAndStore_arcTo(double x1, double y1, double x2, double y2, double r)
{
    // Based on How to create circle with Bézier curves?
    //          http://stackoverflow.com/questions/1734745/how-to-create-circle-with-bézier-curves
    //          Answer by Kpym, 2015 (permalink: http://stackoverflow.com/a/27863181)

    // Line equation : 0 = aX + By + c
    // Normal        : n = ai + bj
    const double a = y2 - y1;
    const double b = x1 - x2;
  //const double c = -(x1 * y2 - x2 * y1);

    // Middle point
    const double xm = (x1 + x2) * 0.5;
    const double ym = (y1 + y2) * 0.5;

    // Radius
    const double ab = Gfx::Math::fastSqrt(a * a + b * b);
    const double rx = ab * 0.5f;
    const double ry = r;

    // Normal vector
    const double iz = -1.0 / ab;
    const double nx = a * iz;
    const double ny = b * iz;

    // Circumference vectors
    const double nxrx = nx * rx;
    const double nxry = nx * ry;
    const double nyrx = ny * rx;
    const double nyry = ny * ry;

    // Optimal distance to the control points for circle approximation
    // using N segments of cubic bezier:
    //    dist = (4 / 3) * tan(pi / 2 / N)
    // If N = 4, then:
    //    dist = (4 / 3) * tan(pi / 2 / 4) = 0.0822479912358
    const double od = 0.552284749831;

    // Curve #1
    const double c1x1 = x1;
    const double c1y1 = y1;
    const double c1x4 = xm   + nxrx;
    const double c1y4 = ym   + nyry;
    const double c1x2 = c1x1 + nxrx * od;
    const double c1y2 = c1y1 + nyry * od;
    const double c1x3 = c1x4 - nyrx * od;
    const double c1y3 = c1y4 - nxry * od;
    _pathData->add(PathData::IT_CubicBezierTo, c1x2, c1y2, c1x3, c1y3, c1x4, c1y4);

    // Curve #2
    const double c2x1 = xm   + nxrx;
    const double c2y1 = ym   + nyry;
    const double c2x4 = x2;
    const double c2y4 = y2;
    const double c2x2 = c2x1 + nyrx * od;
    const double c2y2 = c2y1 - nxry * od;
    const double c2x3 = c2x4 - nxrx * od;
    const double c2y3 = c2y4 + nyry * od;
    _pathData->add(PathData::IT_CubicBezierTo, c2x2, c2y2, c2x3, c2y3, c2x4, c2y4);
}


} // namespace
} // namespace
