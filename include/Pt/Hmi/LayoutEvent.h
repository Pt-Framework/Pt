/* Copyright (C) 2017 Marc Boris Duerner 
  
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

#ifndef Pt_Hmi_LayoutEvent_h
#define Pt_Hmi_LayoutEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/SizePolicy.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Types.h>
#include <Pt/Event.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API RescaleEvent : public Pt::BasicEvent<RescaleEvent>
{
    public:
        explicit RescaleEvent(Visual& visual,
                              double scaleFactor = 1.0)
        : _vid( visual.vid() )
        , _visual(&visual)
        , _scaleFactor(scaleFactor)
        {
        }

        virtual ~RescaleEvent()
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

        double scaleFactor() const
        {
            return _scaleFactor;
        }

      private:
          Pt::uint64_t _vid;
          Visual*      _visual;
          double       _scaleFactor;
};


class PT_HMI_API MeasureEvent : public Pt::BasicEvent<MeasureEvent>
{
    public:
        MeasureEvent(Pt::uint64_t vid)
        : _vid(vid)
        {
        }

        MeasureEvent(Pt::uint64_t vid, const SizePolicy& policy)
        : _vid(vid)
        , _sizePolicy(policy)
        {
        }

        virtual ~MeasureEvent()
        {
        }

        Pt::uint64_t vid() const
        {
            return _vid;
        }

        const SizePolicy& sizePolicy() const
        {
            return _sizePolicy;
        }

        void setSizePolicy(const SizePolicy& policy)
        {
            _sizePolicy = policy;
        }

    private:
        Pt::uint64_t _vid;
        SizePolicy   _sizePolicy;
};


class PT_HMI_API LayoutEvent : public Pt::BasicEvent<LayoutEvent>
{
    public:
        LayoutEvent(Visual& v)
        : _vid( v.vid() )
        , _visual(&v)
        {
        }

        LayoutEvent(Visual& v, const Gfx::RectF& rect)
        : _vid( v.vid() )
        , _visual(&v)
        , _rect(rect)
        {
        }

        virtual ~LayoutEvent()
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

        const Gfx::RectF& rect() const
        {
            return _rect;
        }

        void setRect(const Gfx::RectF& r)
        {
            _rect = r;
        }

    private:
        Pt::uint64_t _vid;
        Visual*      _visual;
        Gfx::RectF   _rect;
};

} // namespace

} // namespace

#endif
