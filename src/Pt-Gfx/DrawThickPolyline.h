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
#ifndef PT_GFX_DRAWTHICKPOLYLINE_H
#define PT_GFX_DRAWTHICKPOLYLINE_H

#include "DrawPolyline.h"
#include "DrawThickLine.h"
#include "RasterBuffer.h"

namespace Pt{
namespace Gfx{

class DrawThickPolyline : public DrawPolyline
{
    public:
       DrawThickPolyline();
       ~DrawThickPolyline();

        void output( ARgbImage& image,  const Pen& pen, std::vector<RasterBuffer>& lineRasterBuffer);
        void draw( ARgbImage& image,  const Pen& pen, const std::vector<Math::Point>& points );

    private:
        DrawThickLine               _drawThickLine;
        std::vector<RasterBuffer>   _lineRasterBuffer;
        std::vector<ARgbColor>      _colorBuffer;
};

} //namespace gfx
} //namespace ptv

#endif
