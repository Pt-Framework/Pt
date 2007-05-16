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
#ifndef Pt_System_IOResult_h
#define Pt_System_IOResult_h

#include <Pt/Signal.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>


namespace Pt {

namespace System {

    template <typename CharT>
    class BasicIODevice;

    class IOResultImpl;

    class IOResult : protected NonCopyable
    {
        public:
            static const unsigned int WaitInfinite = static_cast<size_t>(-1);         

            IOResult()
            : _device(0)
            {}

            virtual ~IOResult()
            { this->canceled(*this); }

            BasicIODevice<char>* device() const
            { return _device; }

            virtual IOResultImpl* impl() = 0;

            void init(BasicIODevice<char>& device)
            {
                _device = &device;
            }

            virtual bool wait(unsigned int msecs = WaitInfinite) = 0;            

            Signal< IOResult& > canceled;

        private:
            BasicIODevice<char>* _device;
    };

} // namespace System

} // namespace Pt

#endif
