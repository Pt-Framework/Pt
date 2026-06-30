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
#include "Pt/System/FileInfo.h"
#include "Pt/System/Path.h"

class FileInfoTest : public Pt::Unit::TestSuite
{
    public:
        FileInfoTest()
        : Pt::Unit::TestSuite("Pt::System::FileInfoTest")
        , _file("xxxPermTestFile")
        {
            Pt::Unit::TestSuite::registerMethod( "QueryPerms", *this, &FileInfoTest::QueryPerms );
            Pt::Unit::TestSuite::registerMethod( "ReplacePerms", *this, &FileInfoTest::ReplacePerms );
            Pt::Unit::TestSuite::registerMethod( "AddPerms", *this, &FileInfoTest::AddPerms );
            Pt::Unit::TestSuite::registerMethod( "RemovePerms", *this, &FileInfoTest::RemovePerms );
            Pt::Unit::TestSuite::registerMethod( "BitwiseOperators", *this, &FileInfoTest::BitwiseOperators );
        }

        void setUp()
        {
            this->tearDown();

            Pt::System::FileInfo::createFile(_file);
        }

        void tearDown()
        {
            if( Pt::System::FileInfo::exists(_file) )
            {
                // ensure writable so we can delete
                Pt::System::FileInfo::permissions(_file,
                                                  Pt::System::FileInfo::AllPerms,
                                                  Pt::System::FileInfo::PermReplace);
                Pt::System::FileInfo::remove(_file);
            }
        }

    protected:
        void QueryPerms();
        void ReplacePerms();
        void AddPerms();
        void RemovePerms();
        void BitwiseOperators();

    private:
        Pt::System::Path _file;
};

Pt::Unit::RegisterTest<FileInfoTest> register_FileInfoTest;


void FileInfoTest::QueryPerms()
{
    Pt::System::FileInfo::Perms p = Pt::System::FileInfo::permissions(_file);

    // newly created file must have some permissions
    PT_UNIT_ASSERT(p != Pt::System::FileInfo::NoPerms);

    // owner read must be set
    PT_UNIT_ASSERT( (p & Pt::System::FileInfo::OwnerRead) != Pt::System::FileInfo::NoPerms );
}


void FileInfoTest::ReplacePerms()
{
    using Pt::System::FileInfo;

    FileInfo::Perms target = FileInfo::OwnerRead | FileInfo::OwnerWrite;

    FileInfo::permissions(_file, target, FileInfo::PermReplace);
    FileInfo::Perms p = FileInfo::permissions(_file);

    PT_UNIT_ASSERT( (p & FileInfo::OwnerRead) != FileInfo::NoPerms );
    PT_UNIT_ASSERT( (p & FileInfo::OwnerWrite) != FileInfo::NoPerms );
}


void FileInfoTest::AddPerms()
{
    using Pt::System::FileInfo;

    // start with read-only
    FileInfo::permissions(_file, FileInfo::OwnerRead, FileInfo::PermReplace);

    // add write
    FileInfo::permissions(_file, FileInfo::OwnerWrite, FileInfo::PermAdd);

    FileInfo::Perms p = FileInfo::permissions(_file);
    PT_UNIT_ASSERT( (p & FileInfo::OwnerRead) != FileInfo::NoPerms );
    PT_UNIT_ASSERT( (p & FileInfo::OwnerWrite) != FileInfo::NoPerms );
}


void FileInfoTest::RemovePerms()
{
    using Pt::System::FileInfo;

    // start with read+write
    FileInfo::permissions(_file, FileInfo::OwnerRead | FileInfo::OwnerWrite,
                          FileInfo::PermReplace);

    // remove write
    FileInfo::permissions(_file, FileInfo::OwnerWrite, FileInfo::PermRemove);

    FileInfo::Perms p = FileInfo::permissions(_file);
    PT_UNIT_ASSERT( (p & FileInfo::OwnerRead) != FileInfo::NoPerms );
    PT_UNIT_ASSERT( (p & FileInfo::OwnerWrite) == FileInfo::NoPerms );
}


void FileInfoTest::BitwiseOperators()
{
    using Pt::System::FileInfo;

    // OR combines bits
    FileInfo::Perms combined = FileInfo::OwnerRead | FileInfo::GroupRead;
    PT_UNIT_ASSERT( (combined & FileInfo::OwnerRead) != FileInfo::NoPerms );
    PT_UNIT_ASSERT( (combined & FileInfo::GroupRead) != FileInfo::NoPerms );
    PT_UNIT_ASSERT( (combined & FileInfo::OthersRead) == FileInfo::NoPerms );

    // XOR flips bits
    FileInfo::Perms xored = combined ^ FileInfo::OwnerRead;
    PT_UNIT_ASSERT( (xored & FileInfo::OwnerRead) == FileInfo::NoPerms );
    PT_UNIT_ASSERT( (xored & FileInfo::GroupRead) != FileInfo::NoPerms );

    // NOT inverts
    FileInfo::Perms inverted = ~FileInfo::NoPerms;
    PT_UNIT_ASSERT( (inverted & FileInfo::OwnerRead) != FileInfo::NoPerms );
    PT_UNIT_ASSERT( (inverted & FileInfo::AllPerms) == FileInfo::AllPerms );

    // compound assignment
    FileInfo::Perms p = FileInfo::OwnerRead;
    p |= FileInfo::OwnerWrite;
    PT_UNIT_ASSERT( (p & FileInfo::OwnerWrite) != FileInfo::NoPerms );

    p &= FileInfo::OwnerRead;
    PT_UNIT_ASSERT( (p & FileInfo::OwnerWrite) == FileInfo::NoPerms );
    PT_UNIT_ASSERT( (p & FileInfo::OwnerRead) != FileInfo::NoPerms );
}
