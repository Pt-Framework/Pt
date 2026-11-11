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
#include "Pt/System/IOError.h"
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
            Pt::Unit::TestSuite::registerMethod( "EmptyDirectory", *this, &DirectoryTest::EmptyDirectory );
            Pt::Unit::TestSuite::registerMethod( "IterateEntries", *this, &DirectoryTest::IterateEntries );
            Pt::Unit::TestSuite::registerMethod( "InvalidDirectory", *this, &DirectoryTest::InvalidDirectory );
        }

        void setUp()
        {
            this->tearDown();
        }

        void tearDown()
        {
            if( Pt::System::FileInfo::exists(_dir1) )
            {
                if( Pt::System::FileInfo::type(_dir1) == Pt::System::FileInfo::Directory )
                    Pt::System::FileInfo::removeAll(_dir1);
                else
                    Pt::System::FileInfo::remove(_dir1);
            }

            if( Pt::System::FileInfo::exists(_dir2) )
            {
                if( Pt::System::FileInfo::type(_dir2) == Pt::System::FileInfo::Directory )
                    Pt::System::FileInfo::removeAll(_dir2);
                else
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
        void EmptyDirectory();
        void IterateEntries();
        void InvalidDirectory();

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
        PT_UNIT_ASSERT( it->exists() );
        PT_UNIT_ASSERT( it->path().fileName() != "." );
        PT_UNIT_ASSERT( it->path().fileName() != ".." );
    }
}


void DirectoryTest::EmptyDirectory()
{
    Pt::System::FileInfo::createDirectory(_dir1);

    Pt::System::DirectoryIterator it(_dir1);
    Pt::System::DirectoryIterator end;
    PT_UNIT_ASSERT(it == end);

    Pt::System::FileInfo dirInfo(_dir1);
    Pt::System::DirectoryIterator it2(dirInfo);
    PT_UNIT_ASSERT(it2 == end);
}


void DirectoryTest::IterateEntries()
{
    using Pt::System::FileInfo;
    using Pt::System::Path;
    using Pt::System::DirectoryIterator;

    FileInfo::createDirectory(_dir1);

    Path file = _dir1 / "entry.txt";
    FileInfo::createFile(file);
    FileInfo::resize(file, 42);

    Path sub = _dir1 / "sub";
    FileInfo::createDirectory(sub);

    DirectoryIterator it(_dir1);
    DirectoryIterator end;

    bool sawFile = false;
    bool sawSub = false;
    std::size_t count = 0;
    FileInfo copied;

    for(; it != end; ++it)
    {
        ++count;
        PT_UNIT_ASSERT( it->path().fileName() != "." );
        PT_UNIT_ASSERT( it->path().fileName() != ".." );
        PT_UNIT_ASSERT( it->exists() );

        if(it->path().fileName() == "entry.txt")
        {
            sawFile = true;
            PT_UNIT_ASSERT(it->path() == file);
            PT_UNIT_ASSERT(it->type() == FileInfo::File);
            PT_UNIT_ASSERT(it->size() == 42);
            copied = *it;
        }
        else if(it->path().fileName() == "sub")
        {
            sawSub = true;
            PT_UNIT_ASSERT(it->path() == sub);
            PT_UNIT_ASSERT(it->type() == FileInfo::Directory);
        }
    }

    PT_UNIT_ASSERT_EQUAL(count, static_cast<std::size_t>(2));
    PT_UNIT_ASSERT(sawFile);
    PT_UNIT_ASSERT(sawSub);
    PT_UNIT_ASSERT(copied.exists());
    PT_UNIT_ASSERT(copied.path() == file);
    PT_UNIT_ASSERT(copied.size() == 42);
}


void DirectoryTest::InvalidDirectory()
{
    PT_UNIT_ASSERT_THROW(
        Pt::System::DirectoryIterator it0(Pt::System::Path("no-such-dir-xxxPT")),
        Pt::System::AccessFailed);

    Pt::System::FileInfo::createFile(_file1);
    PT_UNIT_ASSERT_THROW(
        Pt::System::DirectoryIterator it1(_file1),
        Pt::System::AccessFailed);
}


Pt::Unit::RegisterTest<DirectoryTest> register_DirectoryTest;
