/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Duerner                                 *
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
#ifndef Pt_System_StreamBuffer_h
#define Pt_System_StreamBuffer_h

#include <ios>
#include <streambuf>
#include <Pt/System/Api.h>

namespace Pt {

namespace System {

    template <typename CharT>
    class BasicStreamBuffer : public std::basic_streambuf<CharT>
    {
        public:
            std::streamsize speekn(CharT* buffer, std::streamsize size)
            { return this->xspeekn(buffer, size); }

        std::streamsize out_avail()
        {
            if( this->pptr() )
                return this->epptr() - this->pptr();

            return this->showmanyp();
        }

        protected:
            virtual std::streamsize xspeekn(CharT* buffer, std::streamsize size)
            {
                if(size == 0)
                    return 0;

                buffer[0] = this->sgetc();
                return 1;
            }

            virtual std::streamsize showmanyp()
            { return 0; }
    };

    typedef BasicStreamBuffer<char> StreamBuffer;

} // namespace System

} // namespace Pt

#endif
