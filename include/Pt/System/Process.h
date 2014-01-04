/*
 * Copyright (C) 2006-2008 by Marc Boris Duerner
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

#ifndef PT_SYSTEM_PROCESS_H
#define PT_SYSTEM_PROCESS_H

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/SystemError.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <vector>
#include <cstddef>

namespace Pt {

namespace System {

/** @brief Indicates program failure.

    This exception is thrown, when a process does not terminate
    normally in wait().
*/
class PT_SYSTEM_API ProcessFailed : public SystemError
{
    public:
        ProcessFailed();

        ~ProcessFailed() throw()
        {}
};


//! @brief %Process startup parameters
class ProcessInfo
{
    public:
        enum IOMode
        {
            Keep = 0,
            Close   = 1,
            Redirect = 2,
            ToStdOut = 3  // combine stderr with stdout, only valid for stderr
        };

        //! process info can contain at least the command
        ProcessInfo(const std::string& command);

        const std::string& command() const
        { return _command; }

        /** @brief Adds an argument to the list of arguments
        */
        ProcessInfo& addArg(const std::string& argument)
        { _args.push_back(argument); return *this; }

        std::size_t argCount() const
        { return _args.size(); }

        const std::string& arg(std::size_t idx) const
        { return _args.at(idx); }

        bool isDetached() const
        { return _detach; }

        void setDetached(bool sw)
        { _detach = sw; }

        void setStdInput(IOMode mode)
        { _stdinMode = mode; }

        bool isStdInputClosed() const
        { return (_stdinMode & Close) == Close; }

        bool isStdInputRedirected() const
        { return (_stdinMode & Redirect) == Redirect; }

        void setStdOutput(IOMode mode)
        { _stdoutMode = mode; }

        bool isStdOutputClosed() const
        { return (_stdoutMode & Close) == Close; }

        bool isStdOutputRedirected() const
        { return (_stdoutMode & Redirect) == Redirect; }

        void setStdError(IOMode mode)
        { _stderrMode = mode; }

        bool isStdErrorClosed() const
        { return (_stderrMode & Close) == Close; }

        bool isStdErrorRedirected() const
        { return (_stderrMode & Redirect) == Redirect; }

        bool isStdErrorAsOutput() const
        { return (_stderrMode & ToStdOut) == ToStdOut; }

    private:
        std::string _command;
        std::vector<std::string> _args;
        bool _detach;
        IOMode _stdinMode;
        IOMode _stdoutMode;
        IOMode _stderrMode;
};

//! @brief Executes shell commands
class PT_SYSTEM_API Process : private NonCopyable
{
    public:
        enum State
        {
            Ready    = 0,
            Running  = 1,
            Finished = 2,
            Failed   = 3
        };

    public:
        /** @brief Constructs a Process with a command including its arguments.
        
            @param command Name of the executable along with its arguments
        */
        explicit Process(const std::string& command);

        //! @brief Constructs a Process with a process info structure
        explicit Process(const ProcessInfo& procInfo);

        //! @brief Destructor
        ~Process();

        const ProcessInfo& procInfo() const;

        State state() const;

        /** @brief Start/Create the Process
        
            @throw SystemError
        */
        void start();

        /** @brief Kills the Process
        
            @throw SystemError
        */
        void kill();

        /** @brief Waits until the Process ends
        
            @throw SystemError
        */
        int wait();

        IODevice* stdInput();

        IODevice* stdOutput();

        IODevice* stdError();

    private:
        class ProcessImpl *_impl;
};


inline ProcessInfo::ProcessInfo(const std::string& command)
: _command(command)
, _detach(false)
, _stdinMode(Close)
, _stdoutMode(Close)
, _stderrMode(Close)
{
}

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_PROCESS_H

