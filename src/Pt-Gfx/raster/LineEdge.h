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
#ifndef PT_GFX_LINEEDGE_H
#define PT_GFX_LINEEDGE_H
 
namespace Pt{
namespace Gfx{ 
 
/**
* @brief The line edge description.
*
*/

class LineEdge
{
    public:
        LineEdge()
        : _height(0)
        , _x(0)
        , _stepx(0)
        , _signdx(0)
        , _e(0)
        , _dx(0)
        , _dy(0)
        { }

        unsigned int height() const
        { return _height; }

        void setHeight( int height_ )
        { _height = height_; }

        int x() const
        { return _x; }

        void setX( int x_ )
        { _x = x_; }

        int stepx() const
        { return _stepx; }

        void setStepX( int sdx )
        { _stepx = sdx; }

        int signdx() const
        { return _signdx; }

        void setSignDX( int sdx )
        { _signdx = sdx; }

        int e() const
        { return _e; }

        void setE( int e_ )
        { _e = e_; }

        int dx() const
        { return _dx; }

        void setDX( int dx_ )
        { _dx = dx_; }

        int dy() const
        { return _dy; }

        void setDY( int dy_ )
        { _dy = dy_; }

    private:
        // Number of scanlines in edge
        unsigned int _height;

        // Starting x coordinate of edge
        int _x;

        // Fixed integer dx (usually 0)
        int _stepx;

        // Additional (optional) integer dx
        int _signdx;

        // Initial value for decision variable
        int _e;

        // dy/dx is (rational) slope of edge
        int _dx;
        int _dy;
};

}} //namespace

#endif
