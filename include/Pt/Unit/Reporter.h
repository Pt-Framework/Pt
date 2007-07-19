/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Duerner                     *
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

        This class is the base class for all reporters for test events. It
        lets the implementor override several virtual methods that are called
        on perticular events during the test. Reporters can be made to print
        information to the console or write XML logs. The default
        implementation will report events to a stream in a simple Text format.
    */
    class PT_UNIT_API Reporter
    {
        public:
            /** @brief Constructs a reporter to use an ostream

                This conctructor creates a reporter that uses the passed
                ostream to print messgages for the events that happen during a
                test. By default, std::cerr is used to print the messages to.

                @param reporter Reporeter to be used
            */
            Reporter(std::ostream* out = &std::cerr)
            : m_out(out)
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
            std::ostream* m_out;
    };


    class PT_UNIT_API BriefTextReporter : public Reporter
    {
        public:
            BriefTextReporter(std::ostream* out = &std::cerr)
            : Reporter(out)
            {}

            virtual ~BriefTextReporter()
            {}

            virtual void started(const TestContext& test);

            virtual void finished(const TestContext& test);

            virtual void message(const std::string& msg);

            virtual void success(const TestContext& test);

            virtual void assertion(const TestContext& test, const Assertion& a);

            virtual void exception(const TestContext& test, const std::exception& ex);

            virtual void error(const TestContext& test);
    };


    class PT_UNIT_API TextReporter : public BriefTextReporter
    {
    public:
        TextReporter(std::ostream* out = &std::cerr)
        : BriefTextReporter(out)
        {}

        virtual ~TextReporter()
        {}

        virtual void started(const TestContext& test);

        virtual void finished(const TestContext& test);

        virtual void message(const std::string& msg);

        virtual void success(const TestContext& test);

        virtual void assertion(const TestContext& test, const Assertion& a);

        virtual void exception(const TestContext& test, const std::exception& ex);

        virtual void error(const TestContext& test);
    };


    class PT_UNIT_API XMLReporter : public Reporter
    {
    public:
        XMLReporter(std::ostream* out = &std::cerr, int indentWidth = 4)
        : Reporter(out)
        , m_indentWidth(indentWidth)
        , m_indent(0)
        {
            *m_out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
            *m_out << std::endl;
            *m_out << "<ComponentTester>" << std::endl;
            *m_out << std::endl;
        }

        virtual ~XMLReporter()
        {
            *m_out << std::endl;
            *m_out << "</ComponentTester>" << std::endl;
        }

        virtual void started(const TestContext& test);

        virtual void finished(const TestContext& test);

        virtual void message(const std::string& msg);

        virtual void success(const TestContext& test);

        virtual void assertion(const TestContext& test, const Assertion& a);

        virtual void exception(const TestContext& test, const std::exception& ex);

        virtual void error(const TestContext& test);

    private:
        int m_indentWidth;
        int m_indent;

        void beginTag(std::string tag);

        void endTag(std::string tag);

        void writeData(std::string data);
    };


    class PT_UNIT_API CSVReporter : public Reporter
    {
    public:
        CSVReporter(std::ostream* out = &std::cerr, int indentWidth = 4)
        : Reporter(out)
        , m_timestampSaved(false)
        , m_performanceSaved(false)
        {
            *m_out << "\"ComponentTester\"" << std::endl;
            *m_out << std::endl;
            *m_out << std::endl;
            *m_out << "\"Timestamp\";\"Test\";\"Performance\";\"Result\"" << std::endl;
            *m_out << std::endl;
        }

        virtual ~CSVReporter()
        {}

        virtual void started(const TestContext& test);

        virtual void finished(const TestContext& test);

        virtual void message(const std::string& msg);

        virtual void success(const TestContext& test);

        virtual void assertion(const TestContext& test, const Assertion& a);

        virtual void exception(const TestContext& test, const std::exception& ex);

        virtual void error(const TestContext& test);

    private:
        std::string m_testName;
        std::string m_allMessages;
        bool m_timestampSaved;
        bool m_performanceSaved;

        bool extractData(std::string key, const std::string& msg, bool isFirstColumn = false);
    };

} // namespace Unit

} // namespace Pt

#endif
