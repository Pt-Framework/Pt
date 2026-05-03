/* Copyright (C) 2015 Marc Boris Duerner

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

#ifndef Pt_Gfx_Scaling_h
#define Pt_Gfx_Scaling_h

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Gfx {

/** @brief Logical-to-physical unit conversion.
    @ingroup Drawing

    Scaling converts coordinates, sizes and rectangles between logical drawing
    units and physical device pixels. It also provides alignment helpers that
    snap geometry to pixel boundaries for crisp rendering.
*/
class Scaling
{
    friend bool operator ==(const Scaling&, const Scaling&);
    friend bool operator <(const Scaling&, const Scaling&);

    public:
        /** @brief Constructs a scaling with the given factor.
        */
        Scaling(double scaleFactor = 1.0)
        : _scaleFactor(scaleFactor)
        { }

        /** @brief Returns the scale factor.
        */
        double scaleFactor() const
        {
            return _scaleFactor;
        }
        
        /** @brief Sets the scale factor.
        */
        void setScaleFactor(double scaleFactor)
        {
            _scaleFactor = scaleFactor;
        }

    public:
        /** @brief Converts a scalar value to physical units.
        */
        double toPhysical(double n) const
        {
            return n * scaleFactor();
        }

        /** @brief Converts a point to physical units.
        */
        Gfx::PointF toPhysical(const Gfx::PointF& p) const
        {
            return p * scaleFactor();
        }

        /** @brief Converts a size to physical units.
        */
        Gfx::SizeF toPhysical(const Gfx::SizeF& s) const
        {
            return s * scaleFactor();
        }

        /** @brief Converts a rectangle to physical units.
        */
        Gfx::RectF toPhysical(const Gfx::RectF& r) const
        {
            Gfx::PointF p = toPhysical( r.topLeft() );
            Gfx::SizeF s = toPhysical( r.size() );
            return Gfx::RectF(p, s);
        }

        /** @brief Converts a scalar value to logical units.
        */
        double toLogical(double n) const
        {
            return n / scaleFactor();
        }

        /** @brief Converts a point to logical units.
        */
        Gfx::PointF toLogical(const Gfx::PointF& p) const
        {
            return p / scaleFactor();
        }

        /** @brief Converts a size to logical units.
        */
        Gfx::SizeF toLogical(const Gfx::SizeF& s) const
        {
            return s / scaleFactor();
        }

        /** @brief Converts a rectangle to logical units.
        */
        Gfx::RectF toLogical(const Gfx::RectF& r) const
        {
            Gfx::PointF p = toLogical( r.topLeft() );
            Gfx::SizeF s = toLogical( r.size() );
            return Gfx::RectF(p, s);
        }

    public:
        /** @brief Aligns a scalar value to the nearest physical pixel.
        */
        double align(double n) const
        {
            // better name: alignGrid()

            double p = toPhysical(n);
            p = lround(p);
            return toLogical(p);
        }

        /** @brief Aligns a scalar value to a pixel center.
        */
        double alignPixel(double n) const
        {
            double p = toPhysical(n);
            p = lround(p + 0.5) - 0.5;
            return toLogical(p);
        }

        /** @brief Aligns a contour width while preserving visible thickness.
        */
        double alignContour(size_t n) const
        {
            // keep contour size when downscaling
            if (_scaleFactor < 1.0)
                return toLogical( static_cast<double>(n) );

            double p = toPhysical( static_cast<double>(n) );
            size_t s = static_cast<size_t>(p);
            return toLogical( static_cast<double>(s) );
        }

        /** @brief Aligns a point to physical pixels.
        */
        Gfx::PointF align(const Gfx::PointF& p) const
        {
            Gfx::PointF pos = toPhysical(p);
            pos.setX(lround(pos.x()));
            pos.setY(lround(pos.y()));
            return toLogical(pos);
        }

        /** @brief Aligns a size to physical pixels.
        */
        Gfx::SizeF align(const Gfx::SizeF& s) const
        {
            Gfx::SizeF size = toPhysical(s);
            size.setWidth(lround(size.width()));
            size.setHeight(lround(size.height()));
            return toLogical(size);
        }

        /** @brief Aligns a rectangle to physical pixels.
        */
        Gfx::RectF align(const Gfx::RectF& rect) const
        {
            Gfx::PointF pos = toPhysical(rect.topLeft());
            pos.setX(lround(pos.x()));
            pos.setY(lround(pos.y()));

            Gfx::SizeF size = toPhysical(rect.size());
            size.setWidth(lround(size.width()));
            size.setHeight(lround(size.height()));

            return toLogical(Gfx::RectF(pos, size));
        }

    private:
        double _scaleFactor;
};


/** @brief Returns true if both scalings are equal.
    @related Scaling
*/
inline bool operator == (const Scaling& a, const Scaling& b)
{
    return a._scaleFactor == b._scaleFactor;
}


/** @brief Returns true if both scalings are different.
    @related Scaling
*/
inline bool operator != (const Scaling& a, const Scaling& b)
{
    return ! (a == b);
}


/** @brief Returns true if one scaling sorts before another.
    @related Scaling
*/
inline bool operator < (const Scaling& a, const Scaling& b)
{
    return a._scaleFactor < b._scaleFactor;
}

} // namespace

} // namespace

#endif
