/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *   Copyright (C) 2010 Aloysius Indrayanto                                *
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
#ifndef PT_GFX_DRAWTHICKELLIPSE_H
#define PT_GFX_DRAWTHICKELLIPSE_H

#include <Pt/Api.h>
#include <vector>
#include "DrawEllipse.h"

namespace Pt{
namespace Gfx{

/** @brief Draw thick ellipses on an image

    This class implements DrawEllipse and is specialised for the  drawing
    of thick ellipses.
 */
class DrawThickEllipse : public DrawEllipse
{
    public:
        /** @brief Default constructor
        */
        DrawThickEllipse();

        /** @brief Destructor
        */
        virtual ~DrawThickEllipse();

        /** @brief Draw an ellipse on an image

            @see DrawEllipse::draw
        */
        virtual void draw( ARgbImage& image, const Pen& pen, const Pt::Gfx::Point& topLeft, const Pt::Gfx::Size& size);

    private:
        class EllipseSpan
        {
            public:
                EllipseSpan()
                : x1( 0 )
                , len1( 0 )
                , x2( 0 )
                , len2( 0 )
                {}

                ~EllipseSpan()
                { }

                int x1;
                int len1;
                int x2;
                int len2;

        };

        void fillEllipse(  const Pt::Gfx::Point& topLeft, const Pt::Gfx::Size& size );
        void outputSpan( int x, int y, unsigned len );

        std::vector<EllipseSpan> _spans;
};

}//namespace Gfx
}//namespace Pt

#endif
