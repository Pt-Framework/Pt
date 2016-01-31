/* 
   Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
   
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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#ifndef Pt_Hmi_TouchEvent_h
#define Pt_Hmi_TouchEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Event.h>

namespace Pt {

namespace Hmi {

class TouchEvent : public Pt::BasicEvent<TouchEvent>
{
    private:    
        enum Action
        {
            Move = 0,
            Press = 1,
            Release = 2
        };

    public:
        TouchEvent()
        : _pos(0, 0)
        , _action(Move)
        , _trackingId(0)
        {
        }

        const Gfx::PointF& position() const
        {
            return _pos;
        }

        void setPosition(const Gfx::PointF& pos)
        {
            _pos = pos;
        }

        double x() const
        {
            return _pos.x();
        }

        double y() const
        {
            return _pos.y();
        }
    
        void setX(double x)
        {
            _pos.setX(x);
        }
    
        void setY(double y)
        {
            _pos.setY(y);
        }
        
        Pt::uint32_t trackingId() const
        {
            return _trackingId;
        }

        double pressure() const
        {
            return _pressure;
        }

        bool isPress() const
        {
            return _action == Press;
        }

        void setPress(Pt::uint32_t trackId, double x, double y, double p)
        {
            _action = Press;
            _trackingId = trackId;
            _pos.set(x, y);
            _pressure = p;
        }

        bool isRelease() const
        {
            return _action == Release;
        }

        void setRelease(Pt::uint32_t trackId, double x, double y, double p)
        {
            _action = Press;
            _trackingId = trackId;
            _pos.set(x, y);
            _pressure = p;
        }

        bool isMove() const
        {
            return _action == Move;
        }

        void setMove(Pt::uint32_t trackId, double x, double y, double p)
        {
            _action = Move;
            _trackingId = trackId;
            _pos.set(x, y);
            _pressure = p;
        }

    private:
        Gfx::PointF  _pos;
        Action _action;
        Pt::uint32_t _trackingId;
        double _pressure;
};

} // namespace

} // namespace

#endif
