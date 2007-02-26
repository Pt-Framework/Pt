/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Drner                      *
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

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/Test.h>
#include <Pt/Unit/TestContext.h>

#include <iostream>
#include <fstream>
#include <sstream>


namespace Pt {

namespace Unit {

    /** @brief Test event reporter
        @ingroup UnitTests

        This class is the base class for all reporters for test events. It
        lets the implementor override several virtual methods that are called
        on perticular events during the test. Reporters can be made to print
        information to the console or write XML logs. The default
        implementation will report events to a stream in a simple Text format.
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
        virtual void started(const TestContext& test) = 0;

        /** @brief Finished notification

            This method is called when a test has finished. Every test sends
            this signal at its end no matter if it failed or succeeded.

            @param test The finished test
        */
        virtual void finished(const TestContext& test) = 0;

        /** @brief Message notification

            This method is called when a test has produced an informational
            message.

            @param msg The message
        */
        virtual void message(const std::string& msg) = 0;

        /** @brief Success notification

            This method is called when a test was successful.

            @param test The succeeded test
        */
        virtual void success(const TestContext& test) = 0;

        /** @brief Assertion notification

            This method is called when a an assertion failed during a test. an
            assertion fails when a user defined condition is not met.

            @param test The failed test
        */
        virtual void assertion(const TestContext& test, const Assertion& a) = 0;

        /** @brief Exception notification

            This method is called when a an exception failed during a test. An
            exception usually means that an error occured that was even u
            nexpected in a test scenario

            @param test The failed test
        */
        virtual void exception(const TestContext& test, const std::exception& ex) = 0;

        /** @brief Error notification

            This method is called when a an unknown error occurs during a
            test.

            @param test The failed test
        */
        virtual void error(const TestContext& test) = 0;

    protected:
        /** @brief Ostream to print output to
        */
        std::ostream* _out;
    };


    class BriefTextReporter : public Reporter
    {
    public:
        BriefTextReporter(std::ostream* out = &std::cerr)
        : Reporter(out)
        {}

        virtual ~BriefTextReporter()
        {}

        virtual void started(const TestContext& test)
        {
            *_out << test.testName() << ": ";
        }

        virtual void finished(const TestContext& test)
        {}

        virtual void message(const std::string& msg)
        {
            *_out << "INFO: " << msg << std::endl;
        }

        virtual void success(const TestContext& test)
        {
            *_out << "OK" << std::endl;
        }

        virtual void assertion(const TestContext& test, const Assertion& a)
        {
            *_out << "ASSERTION: " << a.what() << std::endl;
        }

        virtual void exception(const TestContext& test, const std::exception& ex)
        {
            *_out << "EXCEPTION: " << ex.what() << std::endl;
        }

        virtual void error(const TestContext& test)
        {
            *_out << "ERROR" << std::endl;
        }
    };


    class TextReporter : public BriefTextReporter
    {
    public:
        TextReporter(std::ostream* out = &std::cerr)
        : BriefTextReporter(out)
        {}

        virtual ~TextReporter()
        {}

        virtual void started(const TestContext& test)
        {
            BriefTextReporter::started(test);
            *_out << std::endl;
        }

        virtual void finished(const TestContext& test)
        {
            *_out << std::endl;
        }

        virtual void message(const std::string& msg)
        {
            *_out << msg << std::endl;
        }

        virtual void success(const TestContext& test)
        {
            BriefTextReporter::success(test);
        }

        virtual void assertion(const TestContext& test, const Assertion& a)
        {
            BriefTextReporter::assertion(test, a);
            *_out << '\t' << "Condition: " << a.what() << std::endl;
            *_out << '\t' << a.sourceInfo().file() << ":" << a.sourceInfo().line() << std::endl;
        }

        virtual void exception(const TestContext& test, const std::exception& ex)
        {
            BriefTextReporter::exception(test, ex);
            *_out << '\t' << ex.what() << std::endl;
        }

        virtual void error(const TestContext& test)
        {
            BriefTextReporter::error(test);
        }
    };


    class XMLReporter : public Reporter
    {
    public:
        XMLReporter(std::ostream* out = &std::cerr, int indentWidth = 4)
        : Reporter(out)
        , _indentWidth(indentWidth)
        , _indent(0)
        {
            *_out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
            *_out << std::endl;
            *_out << "<ComponentTester>" << std::endl;
            *_out << std::endl;
        }

        virtual ~XMLReporter()
        {
            *_out << std::endl;
            *_out << "</ComponentTester>" << std::endl;
        }

        virtual void started(const TestContext& test)
        {
            beginTag(std::string("test name=\"") + test.testName() + std::string("\""));
        }

        virtual void finished(const TestContext& test)
        {
           endTag("test");
           *_out << std::endl;
        }

        virtual void message(const std::string& msg)
        {
            *_out << "<!--" << std::endl;
            *_out << msg << std::endl;
            *_out << "-->" << std::endl;
        }

        virtual void success(const TestContext& test)
        {
            beginTag("success");
            writeData("OK");
            endTag("success");
        }

        virtual void assertion(const TestContext& test, const Assertion& a)
        {
            beginTag("assertion");

            beginTag("condition");
            writeData(a.what());
            endTag("condition");

            beginTag("file");
            std::stringstream s;
            s << a.sourceInfo().line();
            writeData(std::string(a.sourceInfo().file()) + std::string(":") + s.str());
            endTag("file");

            endTag("assertion");
        }

        virtual void exception(const TestContext& test, const std::exception& ex)
        {
            beginTag("exception");
            writeData(ex.what());
            endTag("exception");
        }

        virtual void error(const TestContext& test)
        {
            beginTag("error");
            writeData("ERROR");
            endTag("error");
        }

    private:
        int _indentWidth;
        int _indent;

        void beginTag(std::string tag)
        {
            *_out << std::string(_indentWidth * _indent, ' ');
            *_out << "<" << tag << ">" << std::endl;
            _indent++;
        }

        void endTag(std::string tag)
        {
            _indent--;

            if(_indent < 0)
            {
                _indent = 0;
            }

            *_out << std::string(_indentWidth * _indent, ' ');
            *_out << "</" << tag << ">" << std::endl;
        }

        void writeData(std::string data)
        {
            *_out << std::string(_indentWidth * _indent, ' ');
            *_out << data << std::endl;
        }
    };


    class CSVReporter : public Reporter
    {
    public:
        CSVReporter(std::ostream* out = &std::cerr, int indentWidth = 4)
        : Reporter(out)
        , _performanceSaved(false)
        {
            *_out << "\"ComponentTester\"" << std::endl;
            *_out << std::endl;
            *_out << std::endl;
            *_out << "\"Timestamp\";\"Test\";\"Duration\";\"Result\"" << std::endl;
            *_out << std::endl;
        }

        virtual ~CSVReporter()
        {}

        virtual void started(const TestContext& test)
        {
            _performanceSaved = false;
            *_out << "\"" << "Time and Date" << "\"";
            *_out << ";\"" << test.testName() << "\"";
        }

        virtual void finished(const TestContext& test)
        {
           *_out << std::endl;
        }

        virtual void message(const std::string& msg)
        {
            if(msg.substr(0, 10).compare("Duration: ") == 0)
            {
                *_out << ";\"" << msg.substr(10) << "\"";
                _performanceSaved = true;
            }
        }

        virtual void success(const TestContext& test)
        {
            if(!_performanceSaved)
            {
                *_out << ";\"\"";
            }

            *_out << ";\"OK\"";
        }

        virtual void assertion(const TestContext& test, const Assertion& a)
        {
            if(!_performanceSaved)
            {
                *_out << ";\"\"";
            }

            *_out << ";\"ASSERTION\"";
            *_out << ";\"Condition: " << a.what() << "\"";
            *_out << ";\"" << a.sourceInfo().file() << ":" << a.sourceInfo().line() << "\"";
        }

        virtual void exception(const TestContext& test, const std::exception& ex)
        {
            if(!_performanceSaved)
            {
                *_out << ";\"\"";
            }

            *_out << ";\"EXCEPTION\"";
            *_out << ";\"" << ex.what() << "\"";
        }

        virtual void error(const TestContext& test)
        {
            if(!_performanceSaved)
            {
                *_out << ";\"\"";
            }

            *_out << ";\"ERROR\"";
        }

    private:
        bool _performanceSaved;
    };

} // namespace Unit

} // namespace Pt

#endif
