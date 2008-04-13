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
#include <vector>
#include <stdexcept>
#include <bitset>

namespace Pt {

namespace System {

//! Process parameters
class PT_SYSTEM_API ProcessInfo
{
    public:
        //! process info can contain at least the command
        ProcessInfo( const std::string& command);

	const std::string& command() const;
		
        /** 
	 * @brief adds an argument to the list of arguments
	 *
	 * An argument can contain white spaces
	 * 
	 * @param argument [IN] string containing the argument
	 */
        void addArgument( const std::string& argument);

        unsigned argCount() const;
        std::string getArgument( unsigned idx) const;

        //! replaces or, if null, closes the standard input
        void setStdInput( IODevice* dev);
        IODevice* getStdInput() const;

        //! replaces or, if null, closes the standard output
        void setStdOutput( IODevice* dev);
        IODevice* getStdOutput() const;

        //! replaces or, if null, closes the standard error
        void setStdError( IODevice* dev);
        IODevice* getStdError() const;

        std::bitset<3> mask() const;
        
    private:
        std::string m_command;
		std::bitset<3> m_mask;
        std::vector< std::string> m_argList;

        IODevice* m_devInput;
        IODevice* m_devOutput;
        IODevice* m_devError;
};

//! Process Environment
class PT_SYSTEM_API Process {
    public:
        //! Constructs a Process with a command including its arguments
        /**
            @param command Name of the executable along with its arguments
        */
        Process( const std::string& commandline);

        //! Constructs a Process with a process info structure
        Process( const ProcessInfo& procInfo);

        //! Dtor
        ~Process();

        //! Get the Command string
        /**
            @return Name of the executable
        */
        const std::string& command();

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
