/*
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
#ifndef PT_SELECTABLE_H
#define PT_SELECTABLE_H

#include <Pt/Types.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/EventLoop.h>

namespace Pt {

namespace System {

class SelectableImpl;

class PT_SYSTEM_API Selectable : protected NonCopyable
{
    friend class EventLoopImpl;

    public:
        static const std::size_t WaitInfinite = EventLoop::WaitInfinite;

        // TODO: Idle, Active, Avail
        enum State
        {
            Disabled = 0,
            Active = 1,
            Avail = 2
        };

    public:
        //! @brief Destructor
        virtual ~Selectable();

        void setParent(EventLoop* parent);

        EventLoop* parent();

        const EventLoop* parent() const;

        /** @brief Closes the I/O device

           Frees any resources associated with this object, like I/O handles.
        */
        void close();

        // TODO: remove single wait, use blocking calls with timeout instead
        bool wait(std::size_t msecs = WaitInfinite);

        /** @brief Test if the I/O device object is enabled

            Test if the I/O device object is enabled i.e. open and ready
            to perform I/O operations

            \return true if the I/O device is usable, false otherwise.
        */
        bool enabled() const;

        // TODO: active
        //bool idle() const;

        bool avail() const;

    protected:
        //! @brief Default Constructor
        Selectable();

        //! @brief Sets or unsets the device enabled
        void setEnabled(bool isEnabled);

        // TODO: setUnavail would be enough
        //       alternatively, the EventLoop could unset the avail flag
        //void setIdle();

        // TODO: setUnavail would be enough
        //       alternatively, the EventLoop could unset the avail flag
        //void setActive();

        //void setAvail();

        void setAvail(bool isAvail);

        // TODO: move close to IODevice, Selectable only knows cancel
        //! @brief Closes the Selectable
        virtual void onClose() = 0;

        virtual bool onWait(std::size_t msecs) = 0;

        virtual void onAttach(EventLoop&) = 0;

        virtual void onDetach(EventLoop&) = 0;

        virtual void onEnable(EventLoop&) = 0;

        virtual void onDisable(EventLoop&) = 0;

        virtual bool onAvail() = 0;

    private:
        EventLoop* _parent;
        State _state;
};

} // namespace System

} // namespace Pt

#endif
