/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 As a special exception, you may use this file as part of a free
 software library without restriction. Specifically, if other files
 instantiate templates or use macros or inline functions from this
 file, or you compile this file and link it with other files to
 produce an executable, this file does not by itself cause the
 resulting executable to be covered by the GNU General Public
 License. This exception does not however invalidate any other
 reasons why the executable file might be covered by the GNU Library
 General Public License.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
 02110-1301  USA
*/

#ifndef Pt_Hmi_EnableEvent_h
#define Pt_Hmi_EnableEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Types.h>
#include <Pt/Event.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API EnableEvent : public Pt::BasicEvent<EnableEvent>
{
    public:
        EnableEvent(Visual& v, bool enabled)
        : _vid( v.vid() )
        , _visual(&v)
        , _enabled(enabled)
        {
        }

        EnableEvent()
        : _vid(0)
        , _visual(0)
        , _enabled(false)
        {
        }

        virtual ~EnableEvent()
        {
        }

        Pt::uint64_t vid() const
        {
            return _vid;
        }

        Visual* visual() const
        {
            return _visual;
        }

        bool  enabled( ) const
        {
            return _enabled;
        }

    private:
        Pt::uint64_t _vid;
        Visual*      _visual;
        bool         _enabled;
};

} // namespace

} // namespace

#endif
