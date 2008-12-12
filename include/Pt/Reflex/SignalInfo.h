/*
 * Copyright (C) 2005 by Marc Boris Duerner
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

#ifndef Pt_Reflex_SignalInfo_h
#define Pt_Reflex_SignalInfo_h

#include <Pt/Reflex/Api.h>
#include <Pt/Reflex/CallableInfo.h>
#include <Pt/Signal.h>


namespace Pt {

class Slot;

namespace Reflex {

    /** @brief Reflection support for signals
        @ingroup Reflection
    */
    class SignalInfo
    {
        public:
            SignalInfo(Pt::Signal<>& signal)
            : _signal(&signal)
            {}

            void send(const Pt::Any* args, size_t argCount)
            {
                _signal->send();
            }

            Pt::Connection connect(CallableInfo& ci)
            {
                if(ci.argSize() != 0)
                {
                    throw std::invalid_argument("Incompatible slot" + PT_SOURCEINFO);
                }

                Pt::Slot* slot = ci.createSlot();
                return Pt::Connection(*_signal, slot);
            }

        private:
            Pt::Signal<>* _signal;
    };

} // namespace Reflex

} // namespace Pt

#endif
