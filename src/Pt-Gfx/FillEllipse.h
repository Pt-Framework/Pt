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
#ifndef PT_GFX_FILLELLIPSE_H
#define PT_GFX_FILLELLIPSE_H

#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Brush.h>

namespace Pt{
namespace Gfx{

class FillEllipse
{
    public:
        FillEllipse();
        ~FillEllipse();

        void draw( ARgbImage& image, const Brush& brush, const Pt::Math::Point& topLeft, const Pt::Math::Size& size );
        
    private:
        void outputSpan( ARgbImage& image, const Brush& brush, int x, int y, unsigned int width );
};

}
}

 #endif
 