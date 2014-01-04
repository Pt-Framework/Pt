/*
 * Copyright (C) 2005-2006 by Dr. Marc Boris Duerner
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

#include "Pt/SerializationInfo.h"
#include <Pt/Unit/TextProtocol.h>
#include <fstream>
#include <sstream>
#include <cctype>

namespace Pt {

namespace Unit {

void TextProtocol::run(Pt::Unit::TestSuite& suite)
{
    std::ifstream iniFile(_path.c_str());
    std::string line;
    std::string lineBuffer;
    int lineNumber = 0;

    if(!iniFile)
    {
        throw std::logic_error("Test protocol "  + _path + " not found" + PT_SOURCEINFO);
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

        lineReader >> token;

        // property line
        if(token.compare("property") == 0)
        {
            lineReader >> propertyName;

            char ch;
            lineReader >> ch;
            if(ch == '"')
            {
                std::getline(lineReader, token, '"');
            }
            else
            {
                token.clear();
                lineReader >> token;
                token = ch + token;
            }

            //std::cerr << "Property:" << propertyName << " : " << token << std::endl;
            Pt::SerializationInfo si;
            si.setName(propertyName);
            si.setValue(token);
            suite.setParameter(propertyName, si);
        }
        // method line
        else if(token.compare("method") == 0)
        {
            lineReader >> methodName;

            // SerializationInfo is not copy-constructible
            SerializationInfo siArgs[10];
            std::size_t n = 0;
            while( getline(lineReader, paramType, ':') )
            {
                //paramType.erase(0, paramType.find_first_not_of(", \t"));

                char ch;
                lineReader >> ch;
                if(ch == '"')
                {
                    std::getline(lineReader, token, '"');
                }
                else
                {
                    token.clear();
                    char next = lineReader.peek();
                    if( std::isspace(next) )
                    {
                        token += ch;

                    }
                    else
                    {
                        lineReader >> token;
                        token = ch + token;
                    }
                }

                //std::cerr << "arg:" << token << std::endl;
                siArgs[n].setValue(token);
                if(++n > 9)
                    throw std::out_of_range("Too many method arguments");
            }

            suite.runTest(methodName, siArgs, n );
        }
        // unknown command
        else
        {
            std::stringstream msg;
            msg << "Invalid protocol format in " << _path << ", line " << lineNumber << ": " << token;
            throw std::logic_error(msg.str() + PT_SOURCEINFO);
        }
    }
}

} // namespace Unit

} // namesoace Pt

