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
#ifndef PT_UI_DRAWWIDEDASHPOLYLINE_H
#define PT_UI_DRAWWIDEDASHPOLYLINE_H

#include "DrawWidePolyline.h"

namespace Pt{
namespace Ui{

class DrawWideDashPolyline : public DrawWidePolyline
{
    public:
        DrawWideDashPolyline();
        ~DrawWideDashPolyline();

        virtual void draw( Image& image, const Pen& pen, const PointF* points, size_t pointCount );

    private:
        void dashSegment( Image& image, const Pen& pen, int *pDashNum, int *pDashIndex, int *pDashOffset, int x1, int y1, int x2, int y2, bool projectLeft, bool projectRight, LineFace *leftFace, LineFace *rightFace, unsigned int* dash );
        enum { V_TOP =  0, V_RIGHT = 1, V_BOTTOM = 2, V_LEFT = 3 };
};

}} //namespace

#endif
