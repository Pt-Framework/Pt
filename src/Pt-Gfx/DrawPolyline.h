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
#ifndef PT_GFX_DRAWPOLYLINE_H
#define PT_GFX_DRAWPOLYLINE_H

#include <vector>

#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Math/Point.h>

#include "Stroke.h"

namespace Pt {
namespace Gfx {


/** @brief Draw lines on an image

    This class is an interface for all function objects that can
    draw lines.
 */
class DrawPolyline
{
    public:
        /** @brief Default Constructor
        */
        DrawPolyline()
        { }

        /** @brief Destructor
        */
        virtual ~DrawPolyline()
        { }

   
        /** @brief Draw a line on an image

            The line described by a two points will be drawn on an ARgbImage.
            The attributes for the line are taken from the passed Pen object.
            Clipping has to be performed before the line is drawn.

            @param image Target image
            @param pen Pen to be used
            @param from Begin of the line
            @param end End of the line
        */
        virtual void draw( ARgbImage& image, const Pen& pen, const  Math::Point* points, size_t pointCount ) = 0;

        inline void setOutput( Stroke& d )
        { _stroke = &d; }
        

    protected:
        Stroke*                 _stroke;
        
        /** @brief Helper function called by draw dashed.
           
           Helper function, called by miWideDash() above and also by miZeroPolyArc
   (in mi_zerarc.c) and miZeroDash (in mi_zerolin.c) to perform initial
   offsetting into the dash array, before dash #0 is drawn.  In all cases,
   dashNum=0, dashIndex=0 and dashOffset=0. 
     int dist;			additional offset (assumed >= 0) 
     int *pDashNum;		dash number 
     int *pDashIndex;		current dash 
     const unsigned int *pDash;	dash list 
     int numInDashList;		dashlist length 
     int *pDashOffset;		offset into current dash   
   */
       
//        void stepDash( int dist, int* pDashNum, int* pDashIndex, const unsigned int* pDash, int numInDashList, int *pDashOffset );
        
};

} // namespace Gfx
} // namespace Pt

#endif
