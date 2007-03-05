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
#ifndef PT_GFX_DRAWWIDEDASHPOLYLINE_H
#define PT_GFX_DRAWWIDEDASHPOLYLINE_H

#include "DrawWidePolyline.h"

namespace Pt{
namespace Gfx{

class DrawWideDashPolyline : public DrawWidePolyline
{
    public:
        DrawWideDashPolyline();
        ~DrawWideDashPolyline();
        
        virtual void draw( ARgbImage& image, const Pen& pen, const  Math::Point* points, size_t pointCount );
    
    private:    
        void dashSegment( ARgbImage& image, const Pen& pen, int *pDashNum, int *pDashIndex, int *pDashOffset, int x1, int y1, int x2, int y2, bool projectLeft, bool projectRight, LineFace *leftFace, LineFace *rightFace, unsigned int* dash ); 
        enum { V_TOP =  0, V_RIGHT = 1, V_BOTTOM = 2, V_LEFT = 3 };
};

} //namespaec Gfx
} //namespace Pt

#endif
