/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
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
#ifndef PT_UNIT_REPORTER_H
#define PT_UNIT_REPORTER_H

//#include <Pt/System/Clock.h> // NOTE: should not be here !!!
#include <Pt/Unit/Assertion.h>

#include <iostream>
#include <fstream>
#include <sstream>


namespace Pt {

namespace Unit {

    /** @brief Test event reporter

        This class is the base class for all reporters for test events. It
        lets the implementor override several virtual methods that are called
        on perticular events during the test. Reporters can be made to print 
        information to the console or write XML logs. The default 
        implementation will report events to a stream in a simple text format.
    */
    class Reporter
    {
    public:
        /** @brief Constructs a reporter to use an ostream

            This conctructor creates a reporter that uses the passed
            ostream to print messgages for the events that happen during a 
            test. By default, std::cerr is used to print the messages to.

            @param reporter Reporeter to be used
        */
        Reporter(std::ostream* out = &std::cerr)
        : _out(out)
        {}

        /** @brief Destructor
        */
        virtual ~Reporter()
        {}

        /** @brief Start notification

            This method is called when a test has started. Every test sends
            this signal at startup.

            @param test The started test
        */
        virtual void started(const std::string& testName)
        {
            *_out << testName << ": ";
        }

        /** @brief Finished notification

            This method is called when a test has finished. Every test sends
            this signal at its end no matter if it failed or succeeded.

            @param test The finished test
        */
        virtual void finished(const Test& test)
        {
            //*_out << std::endl;
        }

        /** @brief Message notification

            This method is called when a test has produced an informational
            message.

            @param msg The message
        */
        virtual void message(const std::string& msg)
        {
            *_out << msg << std::endl;
        }

        /** @brief Success notification

            This method is called when a test was successful.

            @param test The succeeded test
        */
        virtual void success(const Test& test)
        {
            *_out << "OK"<< std::endl;
        }

        /** @brief Assertion notification

            This method is called when a an assertion failed during a test. an
            assertion fails when a user defined condition is not met.

            @param test The failed test
        */
        virtual void assertion(const Test& test, const Assertion& a)
        {
            *_out << "ASSERTION" << std::endl;
            *_out << '\t' << a.sourceInfo().file() << ":" << a.sourceInfo().line() << std::endl;
            *_out << '\t' << a.sourceInfo().func() << std::endl;
        }

        /** @brief Exception notification

            This method is called when a an exception failed during a test. An
            exception usually means that an error occured that was even u
            nexpected in a test scenario

            @param test The failed test
        */
        virtual void exception(const Test& test, const std::exception& ex)
        {
            *_out << test.name() << "EXCEPTION" << std::endl;
            *_out << '\t' << ex.what() << std::endl;
        }

        /** @brief Error notification

            This method is called when a an unknown error occurs during a 
            test.

            @param test The failed test
        */
        virtual void error(const Test& test)
        {
            *_out << test.name() << ": ERROR" << std::endl;
        }

    protected:
        /** @brief Ostream to print output to
        */
        std::ostream* _out;
    };


    //TODO: is this obsolete? Andreas suspected so before and I have a 
    //       feeling that he is right.
    //       -Marc
	class FileReporter : public Reporter
	{
	public:

		FileReporter(std::string outFileName, std::ofstream::openmode mode = std::ofstream::out | std::ofstream::trunc, bool writeTimestamp = false)
		{
			outFileStream.open(outFileName.c_str(), mode);

			if(outFileStream)
			{
				_out = &outFileStream;
			}

			else
			{
				_out = &std::cerr;

				std::cerr << std::endl;
				std::cerr << "FileReporter::FileReporter(std::string outFileName)" << std::endl;
				std::cerr << "--> Cannot open out file: " << outFileName << std::endl;
				std::cerr << "--> Using std::cerr instead" << std::endl;
				std::cerr << std::endl;
			}


			if(writeTimestamp)
			{
				std::stringstream msg;

				msg << std::endl;
				msg << std::endl;
				msg << "--------------------------------------------------" << std::endl;
				msg << outFileName << std::endl;
				//msg << "Test started: " << Pt::System::Clock::getCurrentTime().toIsoString() << std::endl;
				//msg << "Test started: " << "TODO: Insert current time" << std::endl;
				msg << "--------------------------------------------------" << std::endl;
				msg << std::endl;

				message(msg.str());
			}
		}


		virtual ~FileReporter()
		{
			if(outFileStream)
			{
				outFileStream.close();
			}
		}


	private:

		std::ofstream outFileStream;
	};

} // namespace Unit

} // namespace Pt

#endif
