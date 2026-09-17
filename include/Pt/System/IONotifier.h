/*
 * Copyright (C) 2006-2018 Marc Boris Duerner
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

#ifndef Pt_System_IONotifier_h
#define Pt_System_IONotifier_h

#include <Pt/System/Api.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Selectable.h>
#include <Pt/Types.h>
#include <Pt/Signal.h>

namespace Pt {

namespace System {

/** @brief Monitors a native handle or file descriptor.

    %IONotifier is a %Selectable that reports activity on a native
    endpoint the rest of the I/O API does not own. setHandle() names a
    Windows handle. setFd() names a POSIX file descriptor. The
    constructor can pass either.

    Attach the notifier to an %EventLoop with setActive(). beginWait()
    starts monitoring for a combination of WaitFlags. When the loop
    sees activity, eventReady is emitted. endWait() returns the flags
    that are ready. reset() clears the handle or descriptor.

    Use this type when the endpoint is not an %IODevice: a socket
    created outside Pt, or a handle from another library.

    @ingroup Pt-System-IO
*/
class PT_SYSTEM_API IONotifier : public Selectable
{
    public:
        enum WaitFlags
        {
            Read = 1,   //!< Readable
            Write = 2,  //!< Writable
            Except = 4  //!< Exceptional condition
        };

    public:
        /** @brief Default constructor.
        */
        IONotifier();

        /** @brief Construct with a native handle.
        */
        explicit IONotifier(void* handle);

        /** @brief Construct with a file descriptor.
        */
        explicit IONotifier(int fd);

        /** @brief Destructor.
        */
        ~IONotifier();

        /** @brief Clears the handle or file descriptor.
        */
        void reset();

        /** @brief Sets the file descriptor to monitor.
        */
        void setFd(int fd);

        /** @brief Sets the native handle to monitor.
        */
        void setHandle(void* handle);

        /** @brief Begins monitoring for @a flags.
        */
        void beginWait(int flags);

        /** @brief Ends monitoring and returns the ready flags.
        */
        int endWait();

        /** @brief Notifies when the handle or descriptor is ready.
        */
        Pt::Signal<>& eventReady()
        { return _eventReady; } 

        /** @brief Returns the used event loop.
        */
        EventLoop* loop() const
          { return _loop; }

    protected:
        virtual void onAttach(EventLoop& loop);

        virtual void onDetach(EventLoop& loop);

        virtual void onCancel();

        virtual bool onRun();

    private:
        class IONotifierImpl* _impl;
        EventLoop*            _loop;
        bool                  _isWaiting;
        Pt::Signal<>          _eventReady;
};

} // namespace System

} // namespace Pt

#endif // Pt_System_IODevice_h
