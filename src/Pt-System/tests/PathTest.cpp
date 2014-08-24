/*
 * Copyright (C) 2014 Marc Boris Duerner
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

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/System/Path.h>

class PathTest : public Pt::Unit::TestSuite
{
    public:
        PathTest()
        : Pt::Unit::TestSuite("PathTest")
        { 
            Pt::Unit::TestSuite::registerMethod( "Append", *this, &PathTest::Append );
            Pt::Unit::TestSuite::registerMethod( "Concat", *this, &PathTest::Concat );
            Pt::Unit::TestSuite::registerMethod( "Compare", *this, &PathTest::Compare );
            Pt::Unit::TestSuite::registerMethod( "FileName", *this, &PathTest::FileName );
            Pt::Unit::TestSuite::registerMethod( "DirName", *this, &PathTest::DirName );
            Pt::Unit::TestSuite::registerMethod( "BaseName", *this, &PathTest::BaseName );
            Pt::Unit::TestSuite::registerMethod( "Extension", *this, &PathTest::Extension );
            Pt::Unit::TestSuite::registerMethod( "Extension", *this, &PathTest::Extension );
        }

    protected:
        void Append()
        {
            Pt::System::Path path;
            path /= "";
            path /= "abc";
            path /= Pt::String("xyz");
            path /= "";

            std::string pathName("abc");
            pathName += Pt::System::Path::dirsep().narrow();
            pathName += "xyz";
            PT_UNIT_ASSERT_EQUALS( path.toString(), pathName.c_str() );
        }

        void Concat()
        {
            Pt::System::Path path;
            path += "abc/";
            path += Pt::String("xyz\\");
            path += "123";
            PT_UNIT_ASSERT_EQUALS( path.toString(), "abc/xyz\\123");
        }

        void Compare()
        {
            Pt::System::Path path1("aaa");
            Pt::System::Path path2("bbb");
            path2 /= "xyz";

            PT_UNIT_ASSERT(path1 == path1);
            PT_UNIT_ASSERT(path1 != path2);
            PT_UNIT_ASSERT(path1 < path2);
        }

        void FileName()
        {
            Pt::System::Path path;
            PT_UNIT_ASSERT( path.fileName().empty() );

            path = ".";
            PT_UNIT_ASSERT_EQUALS( path.fileName(), "." );

            path = "abc";
            path /= "xyz.txt";
            PT_UNIT_ASSERT_EQUALS( path.fileName(), "xyz.txt" );

            path = "abc";
            path /= "xyz.";
            PT_UNIT_ASSERT_EQUALS( path.fileName(), "xyz." );

            path = "abc";
            path /= "xyz";
            PT_UNIT_ASSERT_EQUALS( path.fileName(), "xyz" );
            
            path = "abc.txt";
            PT_UNIT_ASSERT_EQUALS( path.fileName(), "abc.txt" );

            path = "abc.";
            PT_UNIT_ASSERT_EQUALS( path.fileName(), "abc." );

            path = "abc";
            PT_UNIT_ASSERT_EQUALS( path.fileName(), "abc" );
        }

        void DirName()
        {
            Pt::System::Path path;
            PT_UNIT_ASSERT( path.dirName().empty() );

            path = ".";
            PT_UNIT_ASSERT( path.dirName().empty() );

            std::string dir("abc");
            dir += Pt::System::Path::dirsep();

            path = "abc";
            path /= "xyz.txt";
            PT_UNIT_ASSERT_EQUALS( path.dirName(), dir.c_str() );

            path = "abc";
            path /= "xyz.";
            PT_UNIT_ASSERT_EQUALS( path.dirName(), dir.c_str() );

            path = "abc";
            path /= "xyz";
            PT_UNIT_ASSERT_EQUALS( path.dirName(), dir.c_str() );
            
            path = "abc.txt";
            PT_UNIT_ASSERT( path.dirName().empty() );

            path = "abc.";
            PT_UNIT_ASSERT( path.dirName().empty() );

            path = "abc";
            PT_UNIT_ASSERT( path.dirName().empty() );
        }

        void BaseName()
        {
            Pt::System::Path path;
            PT_UNIT_ASSERT( path.baseName().empty() );

            path = ".";
            PT_UNIT_ASSERT( path.baseName().empty() );

            path = "abc";
            path /= "xyz.txt";
            PT_UNIT_ASSERT_EQUALS( path.baseName(), "xyz" );

            path = "abc";
            path /= "xyz.";
            PT_UNIT_ASSERT_EQUALS( path.baseName(), "xyz" );

            path = "abc";
            path /= "xyz";
            PT_UNIT_ASSERT_EQUALS( path.baseName(), "xyz" );
            
            path = "abc.txt";
            PT_UNIT_ASSERT_EQUALS( path.baseName(), "abc" );

            path = "abc.";
            PT_UNIT_ASSERT_EQUALS( path.baseName(), "abc" );

            path = "abc";
            PT_UNIT_ASSERT_EQUALS( path.baseName(), "abc" );
        }

        void Extension()
        {
            Pt::System::Path path;
            PT_UNIT_ASSERT( path.extension().empty() );

            path = ".";
            PT_UNIT_ASSERT( path.extension().empty() );

            path = "abc";
            path /= "xyz.txt";
            PT_UNIT_ASSERT_EQUALS( path.extension(), "txt" );

            path = "abc";
            path /= "xyz.";
            PT_UNIT_ASSERT( path.extension().empty() );

            path = "abc";
            path /= "xyz";
            PT_UNIT_ASSERT( path.extension().empty() );
            
            path = "abc.txt";
            PT_UNIT_ASSERT_EQUALS( path.extension(), "txt" );

            path = "abc.";
            PT_UNIT_ASSERT( path.extension().empty() );

            path = "abc";
            PT_UNIT_ASSERT( path.extension().empty() );
        }
};

Pt::Unit::RegisterTest<PathTest> register_PathTest;
