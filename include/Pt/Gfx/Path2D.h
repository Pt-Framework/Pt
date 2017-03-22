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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_GFX_PATH2D_H
#define PT_GFX_PATH2D_H

#include <vector>

#include <Pt/Gfx/AffineMatrix2D.h>


namespace Pt{
namespace Gfx{


class Path2D {

    public:
        Path2D();

        ~Path2D();

        //
        // Path management - call them multiple times to create multi-path (e.g. path with holes)
        //

        void beginPath();

        void endPath();


        //
        // Absolute addressing
        //

        void moveTo(double x, double y);

        void lineTo(double x, double y);

        void arcTo(double x, double y);

        void quadraticBezierTo(double cx, double cy, double x, double y);

        //
        // Relative addressing
        //

        void relMoveTo(double x, double y);

        void relLineTo(double x, double y);

        void relArcTo(double x, double y);

        void relQuadraticBezierTo(double cx, double cy, double x, double y);

        //
        // Generators
        //
        void generatePoints(std::vector<PointF> dst, Pt::uint8_t smoothness = 0);

    private:
        struct PathData;

    private:
        PathData* _pathData;
};


} // namespace
} // namespace

#endif
