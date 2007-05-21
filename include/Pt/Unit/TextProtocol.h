#ifndef PT_UNIT_TEXTPROTOCOL_H
#define PT_UNIT_TEXTPROTOCOL_H

#include <Pt/Unit/TestSuite.h>


namespace Pt
{

namespace Unit
{

/**
 * The ComponentTestProtocol is used for running component tests
 * defined in component test protocol files (ctp).
 */
class TextProtocol : public Pt::Unit::TestProtocol
    {
    public:

        /**
         * Constructor.
         *
         * @param iniFileName initializes the component test protocol file name
         */
        TextProtocol(const std::string& iniFileName)
        : m_iniFileName(iniFileName)
        {
        }

        /**
         * Loads and parses the component test protocol file, then runs the tests on the specified suite.
         *
         * @param suite the test suite to run the tests on
         */
        virtual void run(Pt::Unit::TestSuite& suite)
        {
            std::ifstream iniFile(m_iniFileName.c_str());
            std::string line;
            std::string lineBuffer;
            int lineNumber = 0;

            if(!iniFile)
            {
                throw std::logic_error("Test protocol "  + m_iniFileName + " not found" + PT_SOURCEINFO);
            }

            while(getline(iniFile, line))
            {
                lineNumber++;

                // remove '\r' (Windows files on Linux)
                while(line.find(13, 0) != std::string::npos)
                {
                    line.erase(line.find(13, 0), 1);
                }

                // next line is NOT empty and NOT a comment line
                if(!line.empty()
                    && (line.at(line.find_first_not_of(" \t")) != '#'))
                {
                    // add new line to lineBuffer
                    lineBuffer += line;

                    // there is NO backslash and NO right brace at the end of the lineBuffer
                    // and the lineBuffer starts with a left brace
                    if((lineBuffer.at(lineBuffer.length() - 1) != '\\')
                        && (lineBuffer.at(lineBuffer.find_first_not_of(" \t")) == '{')
                        && (lineBuffer.at(lineBuffer.length() - 1) != '}'))
                    {
                        // add carriage return and line feed to lineBuffer
                        lineBuffer += "\r\n";
                        continue;
                    }
                    // else: lineBuffer has a backslash at the end
                    else if(lineBuffer.at(lineBuffer.length() - 1) == '\\')
                    {
                        // erase the backslash at the end of lineBuffer
                        lineBuffer.erase(lineBuffer.length() - 1);
                        continue;
                    }

                    // lineBuffer is in braces
                    if((lineBuffer.at(lineBuffer.find_first_not_of(" \t")) == '{')
                        && (lineBuffer.at(lineBuffer.length() - 1) == '}'))
                    {
                        // erase braces at the beginning...
                        lineBuffer.erase(lineBuffer.find_first_not_of(" \t"), 1);
                        // ...and at the end
                        lineBuffer.erase(lineBuffer.length() - 1);
                    }
                }
                // next line is empty or a comment line
                // and lineBuffer is empty
                else if(lineBuffer.empty())
                {
                    continue;
                }


                std::stringstream lineReader(lineBuffer);
                lineBuffer.clear();
                std::string token;
                std::string propertyName;
                std::string methodName;
                std::string paramType;
                Pt::Any value;
                Pt::Args args;

                lineReader >> token;

                // property line
                if(token.compare("property") == 0)
                {
                    lineReader >> propertyName;

                    value.init(suite.property(propertyName).typeName());
                    lineReader >> value;

                    suite.setProperty(propertyName, value);
                }
                // method line
                else if(token.compare("method") == 0)
                {
                    lineReader >> methodName;

                    while(getline(lineReader, paramType, ':'))
                    {
                        paramType.erase(0, paramType.find_first_not_of(", \t"));
                        value.init(paramType);
                        lineReader >> value;
                        args.push_back(value);
                    }

                    suite.runTest(methodName, args);
                }
                // unknown command
                else
                {
                    std::stringstream msg;
                    msg << "Invalid protocol format in " << m_iniFileName << ", line " << lineNumber << ": " << token;
                    throw std::logic_error(msg.str() + PT_SOURCEINFO);
                }
            }
        }

    private:
        std::string m_iniFileName;
    };

}   // namespace Unit

}   // namespace Pt


#endif  // PT_UNIT_TEXTPROTOCOL_H
