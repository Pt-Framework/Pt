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
#ifndef PT_GFX_SPAN_H
#define PT_GFX_SPAN_H

#include <limits>
#include <Pt/Types.h>

namespace Pt{
namespace Gfx{

class Span
{
    public:
        Span()
        : _begin( std::numeric_limits<Pt::ssize_t>::max() )
        , _end( 0 )
        { }

        inline Pt::ssize_t begin() const
        { return _begin; }

        inline void setBegin( Pt::ssize_t begin)
        { _begin = begin; }

        inline Pt::ssize_t end() const
        { return _end; }

        inline void setEnd( Pt::ssize_t end)
        { _end = end; }

    private:
        Pt::ssize_t _begin;
        Pt::ssize_t _end;
};

} //namespace Gfx
} //namespace Pt

#endif
