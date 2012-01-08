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
#include <Pt/Atomicity.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/EventLoop.h>

namespace Pt {

namespace System {

//! @brief Dispatches operations through an event loop
class PT_SYSTEM_API Selectable : protected NonCopyable
{
    public:
        static const std::size_t WaitInfinite = EventLoop::WaitInfinite;

    public:
        //! @brief Destructor
        virtual ~Selectable();

        //! @brief Sets the parent loop, so that operations can be run
        void setActive(EventLoop& parent);

        //! @brief Returns true if operations can be run
        bool isActive() const;

        //! @brief Remove from event loop and cancels outstanding operations
        void detach();

        //! @brief Returns the parent event loop in which operations are running
        EventLoop* parent() const;

        //! @brief Cancels all operations
        void cancel();

        //! @brief Run operation if it is ready
        bool run();

        void setAvail();

    protected:
        //! @brief Default Constructor
        Selectable();

        //! @brief Attached to loop
        virtual void onAttach(EventLoop& loop) = 0;

        //! @brief Detached from loop
        virtual void onDetach(EventLoop& loop) = 0;

        //! @brief Blocks until operation has cancelled
        virtual void onCancel() = 0;

        //! @brief Check if ready and run
        virtual bool onRun() = 0;

    private:
        EventLoop* _parent;
};


class Active : public Selectable
{
    public:
        virtual ~Active()
        {}

        void begin()
        {
            if( ! isActive() )
                return;

            this->onBegin( *parent() );
        }

    protected:
        Active()
        { }

        virtual void onBegin(EventLoop& loop)
        {
            loop.setAvail(*this);
        }

        virtual void onCancel()
        {
        }

        virtual bool onRun() = 0;

        virtual void onAttach(EventLoop& loop)
        {
        }

        virtual void onDetach(EventLoop& loop)
        {
        }
};

} // namespace System

} // namespace Pt

#endif
