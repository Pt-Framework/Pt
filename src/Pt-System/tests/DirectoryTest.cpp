/*
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
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/Directory.h"
#include "Pt/System/File.h"
#include "Pt/System/FileInfo.h"
#include <algorithm>
#include <iostream>
#include <iterator>

class DirectoryTest : public Pt::Unit::TestSuite
{
    public:
        DirectoryTest()
        : Pt::Unit::TestSuite("DirectoryTest")
        { 
            Pt::Unit::TestSuite::registerMethod( "createDir", *this, &DirectoryTest::createDir );
            Pt::Unit::TestSuite::registerMethod( "moveDir", *this, &DirectoryTest::moveDir );
            Pt::Unit::TestSuite::registerMethod( "removeDir", *this, &DirectoryTest::removeDir );
            Pt::Unit::TestSuite::registerMethod( "createFile", *this, &DirectoryTest::createFile );
            Pt::Unit::TestSuite::registerMethod( "resizeFile", *this, &DirectoryTest::resizeFile );
            Pt::Unit::TestSuite::registerMethod( "moveFile", *this, &DirectoryTest::moveFile );
            Pt::Unit::TestSuite::registerMethod( "removeFile", *this, &DirectoryTest::removeFile );
            Pt::Unit::TestSuite::registerMethod( "DirectoryIterator", *this, &DirectoryTest::DirectoryIterator );
        }

        void setUp()
        {
            this->tearDown();
        }

        void tearDown()
        {
            if( Pt::System::FileInfo::exists("xxxDIR") )
            {
                Pt::System::FileInfo::remove("xxxDIR");
            }

            if( Pt::System::FileInfo::exists("yyyDIR") )
            {
                Pt::System::FileInfo::remove("yyyDIR");
            }

            if( Pt::System::FileInfo::exists("TestFile1") )
            {
                Pt::System::FileInfo::remove("TestFile1");
            }

            if( Pt::System::FileInfo::exists("TestFile2") )
            {
                Pt::System::FileInfo::remove("TestFile2");
            }
        }

    protected:
        void createDir();
        void moveDir();
        void removeDir();
        void createFile();
        void resizeFile();
        void moveFile();
        void removeFile();
        void DirectoryIterator();
};


void DirectoryTest::createDir()
{
    Pt::System::FileInfo::createDirectory("xxxDIR");
    PT_UNIT_ASSERT( Pt::System::FileInfo::exists("xxxDIR") );
}


void DirectoryTest::moveDir()
{
    Pt::System::FileInfo::createDirectory("xxxDIR");
    Pt::System::FileInfo::move("xxxDIR", "yyyDIR");

    PT_UNIT_ASSERT( ! Pt::System::FileInfo::exists("xxxDIR") );
    PT_UNIT_ASSERT( Pt::System::FileInfo::exists("yyyDIR") );
}

void DirectoryTest::removeDir()
{
    Pt::System::FileInfo::createDirectory("yyyDIR");
    PT_UNIT_ASSERT( Pt::System::FileInfo::exists("yyyDIR") );

    Pt::System::FileInfo::remove("yyyDIR");
    PT_UNIT_ASSERT( false == Pt::System::FileInfo::exists("yyyDIR") );
}



void DirectoryTest::createFile()
{
    std::string name = "TestFile1";
    Pt::System::FileInfo::createFile(name);
    PT_UNIT_ASSERT( 0 == Pt::System::FileInfo::size(name) );
    PT_UNIT_ASSERT( true == Pt::System::FileInfo::exists(name) );
}


void DirectoryTest::resizeFile()
{
    std::string name = "TestFile1";
    Pt::System::FileInfo::createFile(name);

    Pt::System::FileInfo::resize(name, 1000);
    PT_UNIT_ASSERT( 1000 == Pt::System::FileInfo::size(name) );
}


void DirectoryTest::moveFile()
{
    std::string name1 = "TestFile1";
    std::string name2 = "TestFile2";
    
    Pt::System::FileInfo::createFile(name1);
    Pt::System::FileInfo::move(name1, name2);
    PT_UNIT_ASSERT( Pt::System::FileInfo::exists(name2) );
}


void DirectoryTest::removeFile()
{
    std::string name1 = "TestFile1";

    Pt::System::FileInfo::createFile(name1);
    Pt::System::FileInfo::remove(name1);
    PT_UNIT_ASSERT( Pt::System::FileInfo::exists(name1) == false );
}


void DirectoryTest::DirectoryIterator()
{
    Pt::System::DirectoryIterator it( Pt::System::FileInfo::curdir() );
    Pt::System::DirectoryIterator end;
    for(; it != end; ++it)
    {
        std::string name = *it;
    }
}


Pt::Unit::RegisterTest<DirectoryTest> register_DirectoryTest;
