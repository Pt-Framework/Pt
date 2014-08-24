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
#include "Pt/System/FileInfo.h"
#include "Pt/System/Path.h"
#include <algorithm>
#include <iostream>
#include <iterator>

class DirectoryTest : public Pt::Unit::TestSuite
{
    public:
        DirectoryTest()
        : Pt::Unit::TestSuite("DirectoryTest")
        , _dir1("xxxDIR")
        , _dir2("yyyDIR")
        , _file1("TestFile1")
        , _file2("TestFile2")
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
            if( Pt::System::FileInfo::exists(_dir1) )
            {
                Pt::System::FileInfo::remove(_dir1);
            }

            if( Pt::System::FileInfo::exists(_dir2) )
            {
                Pt::System::FileInfo::remove(_dir2);
            }

            if( Pt::System::FileInfo::exists(_file1) )
            {
                Pt::System::FileInfo::remove(_file1);
            }

            if( Pt::System::FileInfo::exists(_file2) )
            {
                Pt::System::FileInfo::remove(_file2);
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

    private:
        Pt::System::Path _dir1;
        Pt::System::Path _dir2;
        Pt::System::Path _file1;
        Pt::System::Path _file2;
};


void DirectoryTest::createDir()
{
    Pt::System::FileInfo::createDirectory(_dir1);
    PT_UNIT_ASSERT( Pt::System::FileInfo::exists(_dir1) );
}


void DirectoryTest::moveDir()
{
    Pt::System::FileInfo::createDirectory(_dir1);
    Pt::System::FileInfo::move(_dir1, _dir2);

    PT_UNIT_ASSERT( ! Pt::System::FileInfo::exists(_dir1) );
    PT_UNIT_ASSERT( Pt::System::FileInfo::exists(_dir2) );
}

void DirectoryTest::removeDir()
{
    Pt::System::FileInfo::createDirectory(_dir2);
    PT_UNIT_ASSERT( Pt::System::FileInfo::exists(_dir2) );

    Pt::System::FileInfo::remove(_dir2);
    PT_UNIT_ASSERT( false == Pt::System::FileInfo::exists(_dir2) );
}



void DirectoryTest::createFile()
{
    Pt::System::FileInfo::createFile(_file1);
    PT_UNIT_ASSERT( 0 == Pt::System::FileInfo::size(_file1) );
    PT_UNIT_ASSERT( true == Pt::System::FileInfo::exists(_file1) );
}


void DirectoryTest::resizeFile()
{
    Pt::System::FileInfo::createFile(_file1);

    Pt::System::FileInfo::resize(_file1, 1000);
    PT_UNIT_ASSERT( 1000 == Pt::System::FileInfo::size(_file1) );
}


void DirectoryTest::moveFile()
{
   
    Pt::System::FileInfo::createFile(_file1);
    Pt::System::FileInfo::move(_file1, _file2);
    PT_UNIT_ASSERT( Pt::System::FileInfo::exists(_file2) );
}


void DirectoryTest::removeFile()
{
    Pt::System::FileInfo::createFile(_file1);
    Pt::System::FileInfo::remove(_file1);
    PT_UNIT_ASSERT( Pt::System::FileInfo::exists(_file1) == false );
}


void DirectoryTest::DirectoryIterator()
{
    Pt::System::Path curdir(".");
    Pt::System::DirectoryIterator it(curdir);
    Pt::System::DirectoryIterator end;
    for(; it != end; ++it)
    {
        const Pt::System::Path& path = it->path();
        if(path.fileName() != ".." && path.fileName() != ".")
        {
            PT_UNIT_ASSERT( Pt::System::FileInfo::exists(path) );
        }
    }
}


Pt::Unit::RegisterTest<DirectoryTest> register_DirectoryTest;
