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
#ifndef PT_GFX_LINEFACE_H
#define PT_GFX_LINEFACE_H

namespace Pt{
namespace Gfx{ 

/**
 * @brief The line face description.
 *
 */
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

}//namespace Gfx
}//namespace Pt

#endif
