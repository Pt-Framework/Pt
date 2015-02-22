/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
 * Copyright (C) 2010 Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PT_GFX_CLIPLINE_H
#define PT_GFX_CLIPLINE_H

#include <Pt/Gfx/Point.h>

namespace Pt {

namespace Gfx {


/** @brief Line clipper

    This class is a function object that can perform clipping
    of a line against x and y limits.
 */
class ClipLine
{
    public:
        /** @brief Perform line clipping

            The line described by a two points is clipped against x ynd y
            limits. The points may be modified, thus the clipping can results
            in a new line.

            @return true if clipping was perfomed
            @param from Begin of the line
            @param End of the line
            @param xmin Minimum x to clip against
            @param xmax Maximum x to clip against
            @param ymin Minimum y to clip against
            @param ymax Maximum y to clip against
        */
        bool clip( Gfx::PointF& from, Gfx::PointF& to,
                   Pt::ssize_t xmin, Pt::ssize_t xmax,
                   Pt::ssize_t ymin, Pt::ssize_t ymax )
        { return this->operator()(from, to, xmin, xmax, ymin, ymax); }

        /** @brief Perform clipping

            @see ClipLine::clip
        */
        bool operator()( Gfx::PointF& from, Gfx::PointF& to,
                         Pt::ssize_t xmin, Pt::ssize_t xmax,
                         Pt::ssize_t ymin, Pt::ssize_t ymax )
        {
            int outCode0 = outCode( from.x(), from.y(), xmin, xmax, ymin, ymax );
            int outCode1 = outCode( to.x(), to.y(), xmin, xmax, ymin, ymax );
            int outCodeOut;

            while( true )
            {
                if( !(outCode0 | outCode1) )
                {
                    return true;
                }
                else if( outCode0 & outCode1 )
                {
                    return false;
                }
                else
                {
                    Pt::ssize_t x, y;
                    outCodeOut = outCode0 ? outCode0 : outCode1;

                    if( outCodeOut & Top )
                    {
                        x = from.x() + ( to.x() - from.x() ) * ( ymax - from.y() ) / ( to.y() -from.y() );
                        y = ymax;
                    }
                    else if( outCodeOut & Bottom )
                    {
                        x = from.x() + ( to.x() - from.x() ) * ( ymin - from.y() ) / (to.y() - from.y() );
                        y = ymin;
                    }
                    else if( outCodeOut & Right )
                    {
                        y = from.y() + ( to.y() - from.y() ) * ( xmax - from.x() ) / ( to.x() - from.x() );
                        x = xmax;
                    }
                    else
                    {
                        y = from.y() + ( to.y() - from.y() ) * ( xmin - from.x()) / ( to.x() - from.x() );
                        x = xmin;
                    }

                    if( outCodeOut == outCode0 )
                    {
                        from.setX( x );
                        from.setY( y );
                        outCode0 = outCode( from.x(), from.y(), xmin, xmax, ymin, ymax );
                    }
                    else
                    {
                        to.setX( x );
                        to.setY( y );
                        outCode1 = outCode( to.x(), to.y(), xmin, xmax, ymin, ymax );
                    }

                }
            }
            return true;
        }

    private:
        enum{ Top=0x1, Bottom=0x2, Right=0x4, Left=0x8 };

        int outCode( Pt::ssize_t x, Pt::ssize_t y, Pt::ssize_t xmin,
                     Pt::ssize_t xmax, Pt::ssize_t ymin, Pt::ssize_t ymax )
        {
            int code = 0;

            if( y > ymax)
            {
              code |= Top;
            }
            else if( y < ymin )
            {
              code |= Bottom;
            }

            if( x > xmax )
            {
              code |= Right;
            }
            else if( x < xmin )
            {
              code |= Left;
            }

            return code;
        }
};

}

}

#endif
