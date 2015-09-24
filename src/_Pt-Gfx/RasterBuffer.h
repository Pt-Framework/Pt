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
#ifndef PT_GFX_RASTERBUFFER_H
#define PT_GFX_RASTERBUFFER_H

#include <limits>
#include <vector>
#include <Pt/Types.h>
#include "Span.h"

namespace Pt{
namespace Gfx{

class RasterBuffer
{
    public:
        RasterBuffer()
        : _miny( std::numeric_limits<Pt::ssize_t>::max() )
        , _maxy( 0 )
        { }

        ~RasterBuffer()
        { }

        void clear()
        {
            _spans.clear();
            _miny = std::numeric_limits<Pt::ssize_t>::max();
            _maxy = 0;
        }

        bool empty() const
        {  return (_miny == std::numeric_limits<Pt::ssize_t>::max()) && (_maxy == 0); }

        std::vector<Span>& spans()
        { return _spans; }

        const std::vector<Span>& spans() const
        { return _spans; }

        Pt::ssize_t miny() const
        {  return _miny; }

        void setMiny( Pt::ssize_t miny )
        {  _miny = miny; }

        Pt::ssize_t maxy() const
        {  return _maxy; }

        void setMaxy( Pt::ssize_t maxy )
        {  _maxy = maxy; }

     private:
        std::vector<Span>   _spans;
        Pt::ssize_t        _miny;
        Pt::ssize_t        _maxy;

};

} //namespace Gfx
} //namespace Pt

#endif
