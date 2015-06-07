/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_UI_DRAWTHICKELLIPSE_H
#define PT_UI_DRAWTHICKELLIPSE_H

#include <Pt/Api.h>
#include <Pt/Ui/Point.h>
#include <Pt/Ui/Size.h>
#include <vector>
#include "DrawEllipse.h"

namespace Pt{
namespace Ui{

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
        virtual void draw( Image& image, const Pen& pen, const PointF& topLeft, const SizeF& size);

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

        void fillEllipse(  const PointF& topLeft, const SizeF& size );
        void outputSpan( int x, int y, unsigned len );

        std::vector<EllipseSpan> _spans;
};

} } //namespace

#endif
