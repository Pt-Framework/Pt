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


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Internal Helper Functions - Generator (Drawing) Functions ======================
// ======================================================================================

// ### TODO ###


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
        float   p1, p2, p3, p4;

        inline Instruction(InsType type_)
        : type(type_)
        {}

        inline Instruction(InsType type_, float p1_)
        : type(type_), p1(p1_)
        {}

        inline Instruction(InsType type_, float p1_, float p2_)
        : type(type_), p1(p1_), p2(p2_)
        {}

        inline Instruction(InsType type_, float p1_, float p2_, float p3_)
        : type(type_), p1(p1_), p2(p2_), p3(p3_)
        {}

        inline Instruction(InsType type_, float p1_, float p2_, float p3_, float p4_)
        : type(type_), p1(p1_), p2(p2_), p3(p3_), p4(p4_)
        {}
    };

    typedef std::vector<Instruction> Instructions;

    // Data
    double       curX, curY;
    Instructions inss;

    // Member functions
    inline PathData()
    : curX(0.f), curY(0.0)
    {}

    inline void addInstruction(const Instruction& ins)
    { inss.push_back(ins); }

    inline bool empty() const
    { return inss.empty(); }

    inline bool lastInstructionMatch(InsType type) const
    { return inss.back().type == type; }
};


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

Path2D::Path2D()
: _pathData( new PathData() )
{}

Path2D::~Path2D()
{ delete _pathData; }

void Path2D::beginPath()
{
    if( !_pathData->empty() && !_pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);
}

void Path2D::endPath()
{
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_Begin) || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);
}

void Path2D::moveTo(double x, double y)
{
    if( _pathData->empty() || _pathData->lastInstructionMatch(PathData::IT_End) )
        throw Path2DInvalidContext(PT_SOURCEINFO_STR);
}

void Path2D::lineTo(double x, double y)
{
    // ### TODO ###
}

void Path2D::arcTo(double x, double y, double r)
{
    // ### TODO ###
}

void Path2D::quadraticBezierTo(double cx, double cy, double x, double y)
{
    // ### TODO ###
}

void Path2D::relMoveTo(double x, double y)
{
    // ### TODO ###
}

void Path2D::relLineTo(double x, double y)
{
    // ### TODO ###
}

void Path2D::relArcTo(double x, double y, double r)
{
    // ### TODO ###
}

void Path2D::relQuadraticBezierTo(double cx, double cy, double x, double y)
{
    // ### TODO ###
}

void  Path2D::generatePoints(std::vector<PointF> dst, Pt::uint8_t smoothness)
{
    // ### TODO ###
}


} // namespace
} // namespace
