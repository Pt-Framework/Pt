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

#include <Pt/Unit/TextProtocol.h>
#include <fstream>
#include <sstream>


using namespace Pt;
using namespace Unit;


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
        std::cout << "WARNING: method is not fully implemented:\n\tvoid TextProtocol::run(Pt::Unit::TestSuite& suite)" << std::endl;

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

            // old code
            //value.init(suite.property(propertyName).typeName());
            //lineReader >> value;

            std::string typeName = suite.property(propertyName).typeName();
            value = AnyFactory::create(typeName, lineReader);

            suite.setProperty(propertyName, value);
        }
        // method line
        else if(token.compare("method") == 0)
        {
            lineReader >> methodName;

            while(getline(lineReader, paramType, ':'))
            {
                paramType.erase( 0, paramType.find_first_not_of(", \t") );

                // old code
                //value.init(paramType);
                //lineReader >> value;

                // TODO: use reflection to get paramType instead of writing
                // the type in the file
                value = AnyFactory::create(paramType, lineReader);

                args.push_back(value);
            }

            suite.runTest(methodName, args);
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
