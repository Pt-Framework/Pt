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
#include <Pt/System/Path.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/SystemError.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <vector>
#include <cstddef>

namespace Pt {

namespace System {

/** @brief Indicates process failure.

    This exception is thrown, when a process does not terminate
    normally in Process::wait().
*/
class PT_SYSTEM_API ProcessFailed : public SystemError
{
    public:
        /** @brief Default Constructor.
        */
        ProcessFailed();

        /** @brief Destructor.
        */
        ~ProcessFailed() throw()
        {}
};


//! @brief %Process startup parameters
class ProcessInfo
{
    public:
        /** @brief Flags for process I/O.
        */
        enum IOMode
        {
            Keep = 0,     //!< Keep open
            Close   = 1,  //!< Close I/O
            Redirect = 2, //!< Redirect I/O
            ToStdOut = 3  //!< Combine stderr with stdout, only valid for stderr
        };

        /** @brief Construct with command to execute.
        */
        explicit ProcessInfo(const Path& command);

        /** @brief Command to execute.
        */
        const Path& command() const
        { return _command; }

        /** @brief Adds an argument to the list of arguments.
        */
        ProcessInfo& addArg(const std::string& argument)
        { _args.push_back(argument); return *this; }

        /** @brief Number of command line arguments.
        */
        std::size_t argCount() const
        { return _args.size(); }

        /** @brief Returns a command line argument.
        */
        const std::string& arg(std::size_t idx) const
        { return _args.at(idx); }

        /** @brief Returns full command line.
        */
        std::string toString() const;

        /** @brief Returns true if process should detach.
        */
        bool isDetached() const
        { return _detach; }

        /** @brief Process should detach.
        */
        void setDetached(bool sw)
        { _detach = sw; }

        /** @brief Sets I/O flags for stdin.
        */
        void setStdInput(IOMode mode)
        { _stdinMode = mode; }

        /** @brief Returns true if stdin will be closed.
        */
        bool isStdInputClosed() const
        { return (_stdinMode & Close) == Close; }

        /** @brief Returns true if stdin will be redirected.
        */
        bool isStdInputRedirected() const
        { return (_stdinMode & Redirect) == Redirect; }

        /** @brief Sets I/O flags for stdout.
        */
        void setStdOutput(IOMode mode)
        { _stdoutMode = mode; }

        /** @brief Returns true if stdout will be closed.
        */
        bool isStdOutputClosed() const
        { return (_stdoutMode & Close) == Close; }

        /** @brief Returns true if stdout will be redirected.
        */
        bool isStdOutputRedirected() const
        { return (_stdoutMode & Redirect) == Redirect; }

        /** @brief Sets I/O flags for stderr.
        */
        void setStdError(IOMode mode)
        { _stderrMode = mode; }

        /** @brief Returns true if stderr will be closed.
        */
        bool isStdErrorClosed() const
        { return (_stderrMode & Close) == Close; }

        /** @brief Returns true if stderr will be redirected.
        */
        bool isStdErrorRedirected() const
        { return (_stderrMode & Redirect) == Redirect; }

        /** @brief Returns true if stderr and atdout wil be combined.
        */
        bool isStdErrorAsOutput() const
        { return (_stderrMode & ToStdOut) == ToStdOut; }

    private:
        Path _command;
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
        /** @brief State of the process.
        */
        enum State
        {
            Ready    = 0, //!< Ready to run
            Running  = 1, //!< Currently running
            Finished = 2, //!< Finished to run
            Failed   = 3  //!< Execution has failed
        };

    public:
        //! @brief Constructs with a process parameters.
        explicit Process(const ProcessInfo& procInfo);

        //! @brief Destructor.
        ~Process();

        //! @brief Returns the process parameters.
        const ProcessInfo& procInfo() const;

        //! @brief Returns the current state.
        State state() const;

        /** @brief Start/Create the Process.
        
            @throw SystemError
        */
        void start();

        /** @brief Kills the Process.
        
            @throw SystemError
        */
        void kill();

        /** @brief Waits until the Process ends.
        
            @throw SystemError, ProcessFailed
        */
        int wait();

        bool tryWait(int& status);

        //! @brief Returns an I/O device to stdin.
        IODevice* stdInput();

        //! @brief Returns an I/O device to stdout.
        IODevice* stdOutput();

        //! @brief Returns an I/O device to stderr.
        IODevice* stdError();

    private:
        class ProcessImpl *_impl;
};


inline ProcessInfo::ProcessInfo(const Path& command)
: _command(command)
, _detach(false)
, _stdinMode(Close)
, _stdoutMode(Close)
, _stderrMode(Close)
{
}


inline std::string ProcessInfo::toString() const
{
    std::string cmdline = _command.toLocal();

    for( std::size_t i = 0; i < _args.size(); ++i )
        cmdline += ' ' + _args[i];

    return cmdline;
}

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_PROCESS_H

