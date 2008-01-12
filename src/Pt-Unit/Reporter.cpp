/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
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

#include <Pt/Unit/Reporter.h>


namespace Pt {

namespace Unit {

void BriefReporter::started(const TestContext& test)
{
    *m_out << test.testName() << ": ";
}

void BriefReporter::finished(const TestContext& test)
{
}

void BriefReporter::message(const std::string& msg)
{
    *m_out << msg << std::endl;
}

void BriefReporter::success(const TestContext& test)
{
    *m_out << "OK" << std::endl;
}

void BriefReporter::assertion(const TestContext& test, const Assertion& a)
{
    *m_out << "ASSERTION at " << a.sourceInfo().file() << ":" << a.sourceInfo().line() << std::endl;
    *m_out << '\t' << a.what() << std::endl;
}

void BriefReporter::exception(const TestContext& test, const std::exception& ex)
{
    *m_out << "EXCEPTION" << std::endl;
    *m_out << '\t' << ex.what() << std::endl;
}

void BriefReporter::error(const TestContext& test)
{
    *m_out << "ERROR" << std::endl;
}


void XMLReporter::started(const TestContext& test)
{
    beginTag(std::string("test name=\"") + test.testName() + std::string("\""));
}

void XMLReporter::finished(const TestContext& test)
{
   endTag("test");
   *m_out << std::endl;
}

void XMLReporter::message(const std::string& msg)
{
    *m_out << "<!--" << std::endl;
    *m_out << msg << std::endl;
    *m_out << "-->" << std::endl;
}

void XMLReporter::success(const TestContext& test)
{
    beginTag("success");
    writeData("OK");
    endTag("success");
}

void XMLReporter::assertion(const TestContext& test, const Assertion& a)
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

void XMLReporter::exception(const TestContext& test, const std::exception& ex)
{
    beginTag("exception");
    writeData(ex.what());
    endTag("exception");
}

void XMLReporter::error(const TestContext& test)
{
    beginTag("error");
    writeData("ERROR");
    endTag("error");
}

void XMLReporter::beginTag(std::string tag)
{
    *m_out << std::string(m_indentWidth * m_indent, ' ');
    *m_out << "<" << tag << ">" << std::endl;
    m_indent++;
}

void XMLReporter::endTag(std::string tag)
{
    m_indent--;

    if(m_indent < 0)
    {
        m_indent = 0;
    }

    *m_out << std::string(m_indentWidth * m_indent, ' ');
    *m_out << "</" << tag << ">" << std::endl;
}

void XMLReporter::writeData(std::string data)
{
    *m_out << std::string(m_indentWidth * m_indent, ' ');
    *m_out << data << std::endl;
}



void CSVReporter::started(const TestContext& test)
{
    m_allMessages = "";
    m_timestampSaved = false;
    m_performanceSaved = false;

    m_testName = test.testName();
}

void CSVReporter::finished(const TestContext& test)
{
    if(m_allMessages.length() >= 3)
    {
        m_allMessages.erase(m_allMessages.length() - 3, 3);
    }

    std::string::size_type pos = std::string::npos;
    while((pos = m_allMessages.find('\n', 0)) != std::string::npos)
    {
        m_allMessages.replace(pos, 1, "; ");
    }

    *m_out << ";\"" << m_allMessages << "\"" << std::endl;
}

void CSVReporter::message(const std::string& msg)
{
    if(!m_timestampSaved && extractData("Timestamp: ", msg, true))
    {
        m_timestampSaved = true;
        *m_out << ";\"" << m_testName << "\"";
    }

    else if(extractData("Duration: ", msg) || extractData("Used mem: ", msg))
    {
        m_performanceSaved = true;
    }

    else
    {
        m_allMessages += msg;
        m_allMessages += " | ";
    }
}

void CSVReporter::success(const TestContext& test)
{
    if(!m_performanceSaved)
    {
        *m_out << ";\"\"";
    }

    *m_out << ";\"OK\"";
}

void CSVReporter::assertion(const TestContext& test, const Assertion& a)
{
    if(!m_performanceSaved)
    {
        *m_out << ";\"\"";
    }

    *m_out << ";\"ASSERTION\"";
    *m_out << ";\"Condition: " << a.what() << "\"";
    *m_out << ";\"" << a.sourceInfo().file() << ":" << a.sourceInfo().line() << "\"";
}

void CSVReporter::exception(const TestContext& test, const std::exception& ex)
{
    if(!m_performanceSaved)
    {
        *m_out << ";\"\"";
    }

    *m_out << ";\"EXCEPTION\"";
    *m_out << ";\"" << ex.what() << "\"";
}

void CSVReporter::error(const TestContext& test)
{
    if(!m_performanceSaved)
    {
        *m_out << ";\"\"";
    }

    *m_out << ";\"ERROR\"";
}

bool CSVReporter::extractData(std::string key, const std::string& msg, bool isFirstColumn)
{
    if(msg.find(key, 0) != std::string::npos)
    {
        if(!isFirstColumn)
        {
            *m_out << ";";
        }

        *m_out << "\"" << msg.substr(key.size()) << "\"";
        return true;
    }

    return false;
}

}

}
