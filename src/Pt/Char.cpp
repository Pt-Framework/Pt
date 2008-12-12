/*
 * Copyright (C) 2005-2007 Marc Boris Duerner
 * Copyright (C) 2007 PTV AG
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
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
