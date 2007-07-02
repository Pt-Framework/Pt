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
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"
#include "../Pt-Log/PropertiesArchive.h"
#include "../Pt-Log/PropertiesReader.h"

#include <fstream>
#include <sstream>


namespace Pt {

namespace Unit {


const Archive& operator>>(const Archive& ar, TextProtocol& suite)
{
    return ar;
}

/*
MyTest.property1 = 5
MyTest.property2 = Hello

[ MyTest.protocol.tests.testMethod1 ]
arg = 10

# Should this create a new testMethod1 Archive?
[ MyTest.protocol.tests.testMethod1 ]
arg = 20
*/

//void TextProtocol::run(Pt::Unit::TestSuite& suite)
//{
/* 
    std::ifstream file(_path.c_str());

    if( ! file )
    {
        throw std::logic_error(_path + " not found" + PT_SOURCEINFO);
    }

    Text::TextIStream ts(file, new Pt::Text::Utf8Codec);
    PropertiesReader reader(ts);

    PropertiesArchive archive;
    reader.read(archive);

    // extract the suite itself
    Pt::String suiteName = Pt::String::widen( suite.name() );
    archive.extract( suite, suiteName );

    // extract this protocol as element of the test
    const Archive* protocol = archive.getArchive(suiteName);
    if( ! protocol )
        return;

    protocol = archive.getArchive(L"protocol");
    if( ! protocol )
        return;

    Archive::ConstIterator it = protocol->begin();
    for( ; it != protocol->end(); ++it )
    {
        
    }
*/
//}

//} // namespace Unit

//} // namespace Pt



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

            //std::string typeName = suite.property(propertyName).typeName();
            //value = AnyFactory::create(typeName, lineReader);

            //suite.setProperty(propertyName, value);
        }
        // method line
        else if(token.compare("method") == 0)
        {
            lineReader >> methodName;

            Settings ar;
            while(getline(lineReader, paramType, ':'))
            {
                paramType.erase( 0, paramType.find_first_not_of(", \t") );

                // old code
                //value.init(paramType);
                //lineReader >> value;

                // TODO: use reflection to get paramType instead of writing
                // the type in the file
                //value = AnyFactory::create(paramType, lineReader);
                //args.push_back(value);

                std::string value;
                lineReader >> value;
                ar.addValue( Pt::String(L"arg"), Pt::String::widen(value) );
            }

            suite.runTest(methodName, ar);
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

}

}
