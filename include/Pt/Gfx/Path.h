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

#ifndef PT_GFX_PATH_H
#define PT_GFX_PATH_H

#include <stdexcept>
#include <string>
#include <vector>

#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Point.h>


namespace Pt{
namespace Gfx{


class DrawText2;


/** @brief Indicates invalid/erroneous usage of the Path API.
  * @ingroup Utilities
  */
class PT_GFX_API PathError : public std::runtime_error {
    public:
        //! @brief Construct with error message.
        explicit PathError(const std::string& msg)
        : std::runtime_error(msg)
        {}

        //! @brief Construct with error message.
        explicit PathError(const char* msg)
        : std::runtime_error(msg)
        {}

        //! @brief Destructor.
        inline ~PathError() throw()
        {}
};


/** @brief Indicates that the Path API has been used in an invalid context.
  * @ingroup Utilities
  */
class PT_GFX_API PathInvalidContext : public PathError {
    public:
        //! @brief Construct with error message prefix.
        explicit PathInvalidContext(const std::string& msg)
        : PathError(msg + ": cannot call this function in the current context")
        {}

        //! @brief Construct with error message prefix.
        explicit PathInvalidContext(const char* msg)
        : PathError(std::string(msg) + ": cannot call this function in the current context")
        {}

        //! @brief Destructor.
        inline ~PathInvalidContext() throw()
        {}
};


/** @brief 2D path builder.
  */
class PT_GFX_API Path {

    public:
        Path();

        Path(const Path& p);

        ~Path();

        const Path& operator=(const Path& p);

        bool isNull() const;

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

        void cubicBezierTo(double cx1, double cy1, double cx2, double cy2, double x, double y);

        void genericNBezierTo(Pt::int32_t controlPointCount, const double* cxy, double x, double y);

        //
        // Relative coordinate
        //

        void relMoveTo(double x, double y);

        void relLineTo(double x, double y);

        void relArcTo(double x, double y, double r);

        void relQuadraticBezierTo(double cx, double cy, double x, double y);

        void relCubicBezierTo(double cx1, double cy1, double cx2, double cy2, double x, double y);

        void relGenericNBezierTo(Pt::int32_t controlPointCount, const double* cxy, double x, double y);

        //
        // Text related
        //
        void setFont(const Font& font);

        const Font& font() const;

        void getCharSpacing(Pt::int32_t& x, Pt::int32_t& y, const Char& from, const Char& to);

        void putChar(const Char& chr, const Char& autoAddSpaceFor = 0);

        void putText(const String& str);

        //
        // Generators
        //
        // NOTE: * If you enlarge (scale-up) the shape, you may need to increase the "smoothness" factor as needed
        //       * If the "smoothness" factor is too large, the anti-aliasing will become less effective
        void generatePoints(std::vector<PointF>& dst, float smoothness = 1.0f) const;

        //
        // Polygon clipper
        //

        enum ClipMode {
            Intersection, Union, Difference, Xor
        };

        static void clipPolygon(std::vector<PointF>& result, const std::vector<PointF>& subject, const std::vector<PointF>& clipRegion, ClipMode cm);

    private:
        struct PathData;

    private:
        PathData* _pathData;

        DrawText2* _text;
        Font       _font;
};


} // namespace
} // namespace

#endif
