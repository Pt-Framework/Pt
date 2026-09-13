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

#ifndef Pt_Forms_ScrollEvent_h
#define Pt_Forms_ScrollEvent_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Types.h>
#include <Pt/Event.h>

namespace Pt {

namespace Forms {

/** @brief Scroll or wheel movement.

    The event identifies the scroll axis. The widget pointer and widget ID
    identify the target.

    @ingroup Pt-Forms-Input
*/
class ScrollEvent : public Pt::BasicEvent<ScrollEvent>
{
    public:
        /** @brief Defines a scroll axis.
        */
        enum Wheel
        {
            Vertical = 0,
            Horizontal = 1,
            Depth = 2
        };

        /** @brief Creates an event without a target and with zero delta.
        */
        ScrollEvent()
        : _widgetId_(0)
        , _widget(0)
        , _wheel(Vertical)
        , _delta(0)
        { }

        /** @brief Creates an event targeted at @a widget.
        */
        explicit ScrollEvent(Widget& widget)
        : _widgetId_( widget.id() )
        , _widget(&widget)
        , _wheel(Vertical)
        , _delta(0)
        { }

        /** @brief Returns the target widget ID, or 0 when no target is set.
        */
        Pt::uint64_t widgetId() const
        {
            return _widgetId_;
        }

        /** @brief Returns the target widget, or 0 when no target is set.
        */
        Widget* widget() const
        {
            return _widget;
        }

        /** @brief Sets the target widget.
        */
        void setWidget(Widget* widget)
        {
            _widget = widget;
        }

        /** @brief Returns the scroll axis.
        */
        Pt::uint32_t wheel() const
        {
          return _wheel;
        }

        /** @brief Returns the signed scroll delta.
        */
        double delta() const
        {
            return _delta;
        }

        /** @brief Sets the scroll @a wheel and delta @a d.
        */
        void set(Pt::uint32_t wheel, double d)
        {
            _wheel = wheel;
            _delta = d;
        }

    private:
        Pt::uint64_t  _widgetId_;
        Widget*       _widget;
        Pt::uint32_t  _wheel;
        double        _delta;
};

} // namespace

} // namespace

#endif
