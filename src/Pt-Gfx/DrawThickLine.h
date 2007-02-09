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
#ifndef PT_GFX_DRAWTHICKLINE_H
#define PT_GFX_DRAWTHICKLINE_H

#include <vector>
#include <Pt/Gfx/Brush.h>

#include "DrawLine.h"
#include "Span.h"
#include "FillPolygon.h"
#include "RasterBuffer.h"


namespace Pt {

namespace Gfx {

class LineEdge
{
    public:
        LineEdge()
        : _height(0), _x(0)
        , _stepx(0), _signdx(0)
        , _e(0), _dx(0), _dy(0)
        {}

        unsigned int height() const
        { return _height; }

        void setHeight(int height_)
        { _height = height_; }

        int x() const
        { return _x; }

        void setX(int x_)
        { _x = x_; }

        int stepx() const
        { return _stepx; }

        void setStepX(int sdx)
        { _stepx = sdx; }

        int signdx() const
        { return _signdx; }

        void setSignDX(int sdx)
        { _signdx = sdx; }

        int e() const
        { return _e; }

        void setE(int e_)
        { _e = e_; }

        int dx() const
        { return _dx; }

        void setDX(int dx_)
        { _dx = dx_; }

        int dy() const
        { return _dy; }

        void setDY(int dy_)
        { _dy = dy_; }

    private:
        // number of scanlines in edge
        unsigned int _height;

        // starting x coordinate of edge
        int _x;

        // fixed integer dx (usually 0)
        int _stepx;

        // additional (optional) integer dx
        int _signdx;

        // initial value for decision variable
        int _e;

        // dy/dx is (rational) slope of edge
        int _dx;
        int _dy;
};


class LineFace
{
    public:
        LineFace()
        : _xa(0.0), _ya(0.0)
        , _dx(0), _dy(0)
        , _x(0), _y(0)
        , _k(0.0)
        {}

        double xa() const
        { return _xa; }

        void setXA(double xa_)
        { _xa = xa_; }

        double ya() const
        { return _ya; }

        void setYA(double ya_)
        { _ya = ya_; }

        int dx() const
        { return _dx; }

        void setDX(int dx)
        { _dx = dx; }

        int dy() const
        { return _dy; }

        void setDY(int dy)
        { _dy = dy; }

        int x() const
        { return _x; }

        void setX(int x)
        { _x = x; }

        int y() const
        { return _y; }

        void setY(int y)
        { _y = y; }

        double k() const
        { return _k; }

        void setK(double k_)
        { _k = k_; }

    private:
        // endpoint of line face (rel. to (x,y))
        double _xa, _ya;

        // (dx,dy) points into line (a convention)
        int _dx, _dy;

        // line end, i.e. center of face
        int _x, _y;

        // xa * dy - ya * dx
        double _k;
};


/** @brief Draw thick lines on an image

    This class implements DrawLine and is specialised for the  drawing
    of thick lines.
 */
class DrawThickLine : public DrawLine
{
    public:
        /** @brief Default constructor
        */
        DrawThickLine();

        /** @brief Draw a line on an image

            @see DrawLine::draw
        */
        void draw( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to );

        void drawSegment(ARgbImage& image, const Pen& pen,
                         Pt::Math::Point from, Pt::Math::Point to,
                         bool projectLeft, bool projectRight,
                         LineFace* leftFace, LineFace* rightFace);
                         
        
        void drawDashSegment(ARgbImage& image, const Pen& pen,
                         Pt::Math::Point from, Pt::Math::Point to,
                         bool projectLeft, bool projectRight,
                         LineFace* leftFace, LineFace* rightFace );                          
        
    private:
        FillPolygon _fillPolygon;
        ClipPolygon _clipPolygon;
        RasterBuffer _rasterBuffer;
};

} // namespace Gfx

} // namespace Pt

#endif
