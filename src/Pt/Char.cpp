/***************************************************************************
 *   Copyright (C) 2005-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2007 PTV AG                                             *
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
#include "Pt/Char.h"

//#include "Pt/String.h" // for CharDummyStream

namespace Pt {
/*
    CharDummyStream::CharDummyStream()
    : std::basic_iostream<Pt::Char>(0)
    {}

    CharDummyStreamBuf::CharDummyStreamBuf()
    : std::basic_streambuf<Pt::Char>()
    {}
*/
}

// TODO: Move this into STLport?
#if PT_STLPORT

_STLP_BEGIN_NAMESPACE
_STLP_MOVE_TO_PRIV_NAMESPACE

    bool __get_fdigit(Pt::Char& c, const Pt::Char* digits) {
      const Pt::Char* p = find(digits, digits + 10, c);
      if (p != digits + 10) {
        c = (char)('0' + (p - digits));
        return true;
      }
      else
        return false;
    }

    bool __get_fdigit_or_sep(Pt::Char& c, Pt::Char sep,
                                        const Pt::Char* digits) {
      if (c == sep) {
        c = (char)',';
        return true;
      }
      else
        return __get_fdigit(c, digits);
    }

_STLP_MOVE_TO_STD_NAMESPACE
_STLP_END_NAMESPACE    
    
#endif
