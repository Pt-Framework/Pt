/***************************************************************************
 *   Copyright (C) 2006-2008 by Marc Boris Duerner                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef PT_SYSTEM_PROCESS_H
#define PT_SYSTEM_PROCESS_H

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <string>
#include <vector>

namespace Pt {

namespace System {

//! Process parameters
class PT_SYSTEM_API ProcessInfo
{
    public:
        //! process info can contain at least the command
        ProcessInfo(const std::string& command);

        const std::string& command() const;

        /** @brief Adds an argument to the list of arguments
        */
        void addArg(const std::string& argument);

        unsigned argCount() const;

        const std::string& arg(unsigned idx) const;

        //! @brief Replaces or, if null, closes the standard input
        void setStdin(IODevice* dev);

        IODevice* stdin() const;

        bool stdinClosed() const;

        //! @brief Replaces or, if null, closes the standard output
        void setStdout(IODevice* dev);

        bool stdoutClosed() const;

        IODevice* stdout() const;

        //! @brief Replaces or, if null, closes the standard error
        void setStderr(IODevice* dev);

        IODevice* stderr() const;

        bool stderrClosed() const;

    private:
        std::string _command;
        std::vector<std::string> _args;
        IODevice* _stdin;
        bool _stdinClosed;
        IODevice* _stdout;
        bool _stdoutClosed;
        IODevice* _stderr;
        bool _stderrClosed;
};

//! Process Environment
class PT_SYSTEM_API Process
{
    public:
        //! Constructs a Process with a command including its arguments
        /**
            @param command Name of the executable along with its arguments
        */
        Process(const std::string& commandline);

        //! Constructs a Process with a process info structure
        Process(const ProcessInfo& procInfo);

        //! Dtor
        ~Process();

        const ProcessInfo& procInfo() const;

        //! Start/Create the Process
        /**
            @throw SystemError
        */
        void start();

        //! Kills the Process
        /**
            @throw SystemError
        */
        void kill();

        //! Waits until the Process ends
        /**
            @throw SystemError
        */
        int wait();

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

        //! Called Process Sleeps milliSec Seconds
        /**
            @throw SystemError
        */
        static void sleep(size_t milliSec);

        static unsigned long usedMemory();

    private:
        class ProcessImpl *_impl;
};

}

}

#endif // PT_SYSTEM_PROCESS_H
