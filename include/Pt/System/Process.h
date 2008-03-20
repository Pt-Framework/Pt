/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#if !defined(PTV_Process_H)
#define PTV_Process_H

#include <Pt/System/Api.h>
#include <Pt/Types.h>
#include <Pt/SourceInfo.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/SystemError.h>
#include <string>
#include <stdexcept>

namespace Pt {

namespace System {

//! Process Environment
class PT_SYSTEM_API Process {
    public:
        //! Constructs a Process with a command
        /**
            @param command Name of the executable
            @param suppStdin  [IN] if true, suppresses standard in
            @param suppStdOut [IN] if true, suppresses standard out
            @param suppStdErr [IN] if true, suppresses standard err
        */
        Process(const std::string& command,
                bool supStdIn = false,
                bool supStdOut = false,
                bool supStdErr = false);

        //! Dtor
        ~Process();

        //! Get the Command string
        /**
            @return Name of the executable
        */
        const std::string& command();

        //! Set the Command Arguments
        /**
            @param args Arguments given to the process
        */
        void setArgs(const std::string& args);

        //! Get the Command Arguments
        /**
            @return Arguments of the process
        */
        const std::string& args();

        //! Redirects the standar input, output, error
        /** 
         * @param dev [IN] redirection device
         */
		void setInput( IODevice& dev);

        //! Redirects the standar output
        /** 
         * @param dev [IN] redirection device
         */
		void setOutput( IODevice& dev);

        //! Redirects the standar error
        /** 
         * @param dev [IN] redirection device
         */
        void setErrput( IODevice& dev);

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
        void wait();

    public:
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

    private:
        friend class ProcessImpl;
        class ProcessImpl *_impl;
};

}

}

#endif // PT_Process_H
