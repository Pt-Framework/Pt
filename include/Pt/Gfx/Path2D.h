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

#include <stdexcept>
#include <string>
#include <vector>

#include <Pt/Gfx/AffineMatrix2D.h>


namespace Pt{
namespace Gfx{


/** @brief Indicates invalid/erroneous usage of the Path2D API.
  * @ingroup Utilities
  */
class PT_GFX_API Path2DError : public std::runtime_error {
    public:
        //! @brief Construct with error message.
        explicit Path2DError(const std::string& msg)
        : std::runtime_error(msg)
        {}

        //! @brief Construct with error message.
        explicit Path2DError(const char* msg)
        : std::runtime_error(msg)
        {}

        //! @brief Destructor.
        inline ~Path2DError() throw()
        {}
};


/** @brief Indicates that the Path2D API has been used in an invalid context.
  * @ingroup Utilities
  */
class PT_GFX_API Path2DInvalidContext : public Path2DError {
    public:
        //! @brief Construct with error message prefix.
        explicit Path2DInvalidContext(const std::string& msg)
        : Path2DError(msg + ": cannot call this function in the current context")
        {}

        //! @brief Construct with error message prefix.
        explicit Path2DInvalidContext(const char* msg)
        : Path2DError(std::string(msg) + ": cannot call this function in the current context")
        {}

        //! @brief Destructor.
        inline ~Path2DInvalidContext() throw()
        {}
};



/** @brief 2D path builder.
  */
class PT_GFX_API Path2D {

    public:
        Path2D();

        ~Path2D();

        //
        // Path management - call them multiple times to create multi-path (e.g. path with holes)
        //

        void clear();

        void beginPath();

        void endPath();

        //
        // Absolute coordinate
        //

        void moveTo(double x, double y);

        void lineTo(double x, double y);

        void arcTo(double x, double y, double r);

        void quadraticBezierTo(double cx, double cy, double x, double y);

        //
        // Relative coordinate
        //

        void relMoveTo(double x, double y);

        void relLineTo(double x, double y);

        void relArcTo(double x, double y, double r);

        void relQuadraticBezierTo(double cx, double cy, double x, double y);

        //
        // Generators
        //
        void generatePoints(std::vector<PointF> dst, Pt::uint8_t smoothness = 0) const;

    private:
        struct PathData;

    private:
        PathData* _pathData;
};


} // namespace
} // namespace

#endif
