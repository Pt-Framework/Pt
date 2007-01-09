/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_GFX_CLIPLINE_H
#define PT_GFX_CLIPLINE_H

#include <Pt/Math/Point.h>

namespace Pt{
namespace Gfx{

class ClipLine
{

public:

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

    bool operator()( Math::Point& from, Math::Point& to,
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
};

}
}

#endif
