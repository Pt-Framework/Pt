/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
 * Copyright (C) 2010 Aloysius Indrayanto
 * Copyright (C) 1988, 1998  The Open Group, MIT X Consortium
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
#ifndef PT_GFX_LINEFACE_H
#define PT_GFX_LINEFACE_H

namespace Pt{
namespace Gfx{ 

/**@brief The line face description. */
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
        // Endpoint of line face (rel. to (x,y))
        double _xa, _ya;

        // (dx,dy) points into line (a convention)
        int _dx, _dy;

        // Line end, i.e. center of face
        int _x, _y;

        // xa * dy - ya * dx
        double _k;
};

}}//namespace

#endif
