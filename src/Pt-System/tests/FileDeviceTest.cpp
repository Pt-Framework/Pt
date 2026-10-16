/*
 * Copyright (C) 2026 Marc Boris Duerner
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
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/FileDevice.h"
#include "Pt/System/FileInfo.h"
#include "Pt/System/IOError.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Path.h"

class FileDeviceTest : public Pt::Unit::TestSuite
{
    public:
        FileDeviceTest()
        : Pt::Unit::TestSuite("Pt::System::FileDeviceTest")
        , _file("xxxFileDeviceTest")
        {
            Pt::Unit::TestSuite::registerMethod( "ConcurrentReadWrite",
                                                 *this,
                                                 &FileDeviceTest::ConcurrentReadWrite );
            Pt::Unit::TestSuite::registerMethod( "Peek",
                                                 *this,
                                                 &FileDeviceTest::Peek );
        }

        void setUp()
        {
            this->tearDown();
        }

        void tearDown()
        {
            if( Pt::System::FileInfo::exists(_file) )
                Pt::System::FileInfo::remove(_file);
        }

    protected:
        void ConcurrentReadWrite()
        {
            const char content[] = "abcdefghijklmnopqrst";
            const char written[] = "XYZ";
            char readBuffer[100];

            Pt::System::FileDevice initial(_file, std::ios::out | std::ios::trunc);
            PT_UNIT_ASSERT_EQUAL(initial.write(content, sizeof(content) - 1),
                                 sizeof(content) - 1);
            initial.close();

            Pt::System::MainLoop loop;
            Pt::System::FileDevice file(_file, std::ios::in | std::ios::out);
            file.setActive(loop);

            file.beginRead(readBuffer, sizeof(readBuffer));
            file.beginWrite(written, sizeof(written) - 1);

            Pt::System::FileDevice::off_type position = sizeof(readBuffer) + sizeof(written) - 1;
            PT_UNIT_ASSERT(file.position() == position);
            PT_UNIT_ASSERT_THROW(file.seek(0, std::ios::beg), Pt::System::IOError);

            PT_UNIT_ASSERT_EQUAL(file.endRead(), sizeof(content) - 1);
            PT_UNIT_ASSERT_EQUAL(file.endWrite(), sizeof(written) - 1);
            PT_UNIT_ASSERT(file.position() == position);

            file.seek(sizeof(readBuffer), std::ios::beg);
            char writtenBuffer[sizeof(written) - 1];
            PT_UNIT_ASSERT_EQUAL(file.read(writtenBuffer, sizeof(writtenBuffer)),
                                 sizeof(writtenBuffer));
            PT_UNIT_ASSERT_EQUAL(std::string(writtenBuffer, sizeof(writtenBuffer)), written);
        }

        void Peek()
        {
            const char content[] = "abcdefgh";
            char peekBuffer[3];
            char readBuffer[3];

            Pt::System::FileDevice initial(_file, std::ios::out | std::ios::trunc);
            PT_UNIT_ASSERT_EQUAL(initial.write(content, sizeof(content) - 1),
                                 sizeof(content) - 1);
            initial.close();

            Pt::System::FileDevice file(_file, std::ios::in);
            PT_UNIT_ASSERT_EQUAL(file.peek(peekBuffer, sizeof(peekBuffer)), sizeof(peekBuffer));
            PT_UNIT_ASSERT_EQUAL(std::string(peekBuffer, sizeof(peekBuffer)), "abc");
            PT_UNIT_ASSERT_EQUAL(file.position(), Pt::System::FileDevice::pos_type(0));

            PT_UNIT_ASSERT_EQUAL(file.read(readBuffer, sizeof(readBuffer)), sizeof(readBuffer));
            PT_UNIT_ASSERT_EQUAL(std::string(readBuffer, sizeof(readBuffer)), "abc");
            PT_UNIT_ASSERT_EQUAL(file.position(),
                                 Pt::System::FileDevice::pos_type(sizeof(readBuffer)));
        }

    private:
        Pt::System::Path _file;
};

Pt::Unit::RegisterTest<FileDeviceTest> register_FileDeviceTest;