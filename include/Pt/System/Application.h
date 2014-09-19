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

/** @brief Console applications without a GUI.
 
    This class is used by non-GUI applications to provide the central event
    loop, handle C signals and process command line arguments. There can be
    only one instance per application.
    The application and therefore the event loop is started with a call to
    run() and can be exited with a call to exit(). The event loop can be
    obtained by calling loop(). Command line arguments can be parsed as Arg
    and static methods exist to set environment variables.

 
*/
class PT_SYSTEM_API Application : public Pt::Connectable
{
    public:
        /** @brief Construct with command line arguments.
        */
        explicit Application(int argc = 0, char** argv = 0);

        /** @brief Construct with custom event loop.
        */
        explicit Application(EventLoop* loop, int argc = 0, char** argv = 0);

        /** @brief Destructor.
        */
        ~Application();

        /** @brief Returns an instance to the application.
        */
        static Application& instance();

        /** @brief Returns the event loop.
        */
        EventLoop& loop()
        { return *_loop; }

        /** @brief Starts the contained event loop.
        */
        void run()
        { _loop->run(); }

        /** @brief Exits from the contained event loop.
        */
        void exit()
        { _loop->exit(); }

        /** @brief Ignores a system signal.
        */
        bool ignoreSystemSignal(int sig);

        /** @brief Catch a system signal.
        */
        bool catchSystemSignal(int sig);

        /** @brief Raise a system signal.
        */
        bool raiseSystemSignal(int sig);

        /** @brief Notifies when a system signal was caught.
        */
        Signal<int>& systemSignal()
        { return _systemSignal; }

        /** @brief Number of command line arguments.
        */
        int argc() const
        { return _argc; }

        /** @brief Command line arguments.
        */
        char** argv() const
        { return _argv; }

        /** @brief Returns the value of a long option.
        */
        template <typename T>
        Arg<T> getArg(const char* name)
        {
            return Arg<T>(_argc, _argv, name);
        }

        /** @brief Returns the value of a long option.
        */
        template <typename T>
        Arg<T> getArg(const char* name, const T& def)
        {
            return Arg<T>(_argc, _argv, name, def);
        }

        /** @brief Returns the value of a short option.
        */
        template <typename T>
        Arg<T> getArg(const char name)
        {
            return Arg<T>(_argc, _argv, name);
        }

        /** @brief Returns the value of a short option.
        */
        template <typename T>
        Arg<T> getArg(const char name, const T& def)
        {
            return Arg<T>(_argc, _argv, name, def);
        }

    public:
        //! @brief Changes the current directory
        static void chdir(const Path& path);

        //! @brief Returns the current directory
        static Path cwd();

        /** @brief Returns the system root path

            Returns "/" (root) on Linux, "c:\" on Windows
        */
        static Path rootdir();

        /** @brief Returns the systems tmp directory.

            Returns the value of the environment variable named TEMP or TMP.
            If neither one is set, "/tmp" is returned on POSIX systems or a
            path to the current directory.
        */
        static Path tmpdir();

        /** @brief Set environment variable.

            @throw SystemError
        */
        static void setEnvVar(const std::string& name, const std::string& value);

        /** @brief Unset environment variable.

            @throw SystemError
        */
        static void unsetEnvVar(const std::string& name);

        /** @brief Get environment variable.

            @throw SystemError
        */
        static std::string getEnvVar(const std::string& name);

        static unsigned long usedMemory();

        //! @internal
        ApplicationImpl& impl()
        { return *_impl; }

    protected:
        //! @internal
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
