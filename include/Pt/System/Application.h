/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
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

#ifndef PT_SYSTEM_APPLICATION_H
#define PT_SYSTEM_APPLICATION_H

#include <Pt/System/Api.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Arg.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>

namespace Pt {

namespace System {

class ApplicationImpl;

/**
 * \brief The %Application class provides an event loop for console applications
 * without a GUI.
 *
 * This class is used by non-GUI applications to provide the applications's event
 * loop. There should be only exactly one instance of Application (or one of its
 * subclasses) per application. This is not ensured, though.
 *
 * Application contains the main event loop, where event sources can be registered
 * and events from those sources are dispatched to listeners, that were registered
 * to the event loop. Events may for example be operating system events (timer, file
 * system changes).
 *
 * The application and therefore the event loop is started with a call to run() and
 * can be exited with a call to exit(). After calling exit() the application should
 * terminate.
 *
 * The event loop can be access by calling eventLoop(). Events can be committed by
 * calling EventLoop::commitEvent(). Long running operations can call
 * EventLoop::processEvents() to keep the application responsive.
 *
 * There are convenience methods available for easier access to functionality of
 * the underlying event loop. commitEvent() delegates to EventLoop::commitEvent(),
 * queueEvent() delegates to EventLoop::delegateEvent() and processEvents() delegates
 * to EventLoop::processEvents() without making it necessary to first obtain the
 * event loop manually.
 */
class PT_SYSTEM_API Application : public Pt::Connectable
{
    public:
        explicit Application(int argc = 0, char** argv = 0);

        explicit Application(EventLoop* loop, int argc = 0, char** argv = 0);

        ~Application();

        static Application& instance();

        EventLoop& loop()
        { return *_loop; }

        void run()
        { _loop->run(); }

        void exit()
        { _loop->exit(); }

        bool ignoreSystemSignal(int sig);

        bool catchSystemSignal(int sig);

        bool raiseSystemSignal(int sig);

        Signal<int>& systemSignal()
        { return _systemSignal; }

        int argc() const
        { return _argc; }

        char** argv() const
        { return _argv; }

        template <typename T>
        Arg<T> getArg(const char* name)
        {
            return Arg<T>(_argc, _argv, name);
        }

        template <typename T>
        Arg<T> getArg(const char* name, const T& def)
        {
            return Arg<T>(_argc, _argv, name, def);
        }

        template <typename T>
        Arg<T> getArg(const char name)
        {
            return Arg<T>(_argc, _argv, name);
        }

        template <typename T>
        Arg<T> getArg(const char name, const T& def)
        {
            return Arg<T>(_argc, _argv, name, def);
        }

    public:
        //! @brief Changes the current directory
        static void chdir(const std::string& path);

        //! @brief Returns the current directory
        static std::string cwd();

        /** @brief Returns the system root path

            Returns "/" (root) on Linux, "c:\" on Windows
        */
        static std::string rootdir();

        /** @brief Returns the systems tmp directory.

            The environment variables TEMP and TMP are checked first. If not set,
            "/tmp" is returned if it exists. If none of the environment variables
            are set and the default system tmp directory does not exist, the 
            current directory is returned.
        */
        static std::string tmpdir();

        //! Set environment variable
        /**
            @throw SystemError
        */
        static void setEnvVar(const std::string& name, const std::string& value);

        //! Unset environment variable
        /**
            @throw SystemError
        */
        static void unsetEnvVar(const std::string& name);

        //! Get environment variable
        /**
            @throw SystemError
        */
        static std::string getEnvVar(const std::string& name);

        static unsigned long usedMemory();

        //! @internal
        ApplicationImpl& impl()
        { return *_impl; }

    protected:
        void init(EventLoop& loop);

    private:
        int     _argc;
        char**  _argv;
        ApplicationImpl* _impl;
        EventLoop* _loop;
        MainLoop* _owner;
        Signal<int> _systemSignal;
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_APPLICATION_H
