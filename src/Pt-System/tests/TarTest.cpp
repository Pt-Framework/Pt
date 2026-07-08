/*
 * Copyright (C) 2024 Marc Boris Duerner
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
#include <Pt/System/TarReader.h>
#include <Pt/System/TarWriter.h>
#include <Pt/System/TarEntry.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/Path.h>
#include <Pt/IOError.h>

#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <algorithm>

// --------------------------------------------------------------------------
// Minimal UStar archive builder (in-memory)
// --------------------------------------------------------------------------

namespace {

static void setOctal(char* field, int len, unsigned long value)
{
    char fmt[16];
    std::snprintf(fmt, sizeof(fmt), "%%0%dlo", len - 1);
    std::snprintf(field, len, fmt, value);
}

static void computeChksum(char block[512])
{
    std::memset(block + 148, ' ', 8);
    unsigned int sum = 0;
    for(int i = 0; i < 512; ++i)
        sum += static_cast<unsigned char>(block[i]);
    std::snprintf(block + 148, 8, "%06o", sum);
    block[154] = '\0';
    block[155] = ' ';
}

struct TarBuilder
{
    std::vector<char> bytes;

    void addFile(const char* name, const void* content, std::size_t size)
    {
        char hdr[512] = {};
        std::strncpy(hdr, name, 99);
        setOctal(hdr + 100, 8, 0644);
        setOctal(hdr + 124, 12, static_cast<unsigned long>(size));
        hdr[156] = '0';
        std::memcpy(hdr + 257, "ustar", 5);
        std::memcpy(hdr + 263, "00", 2);
        computeChksum(hdr);
        bytes.insert(bytes.end(), hdr, hdr + 512);

        const char* p = static_cast<const char*>(content);
        bytes.insert(bytes.end(), p, p + size);
        std::size_t pad = (512 - (size % 512)) % 512;
        bytes.insert(bytes.end(), pad, '\0');
    }

    void addDirectory(const char* name)
    {
        std::string n = name;
        if( n.empty() || n.back() != '/' )
            n += '/';

        char hdr[512] = {};
        std::strncpy(hdr, n.c_str(), 99);
        setOctal(hdr + 100, 8, 0755);
        hdr[156] = '5';
        std::memcpy(hdr + 257, "ustar", 5);
        std::memcpy(hdr + 263, "00", 2);
        computeChksum(hdr);
        bytes.insert(bytes.end(), hdr, hdr + 512);
    }

    void addSymlink(const char* name, const char* target)
    {
        char hdr[512] = {};
        std::strncpy(hdr, name, 99);
        setOctal(hdr + 100, 8, 0777);
        hdr[156] = '2';
        std::strncpy(hdr + 157, target, 99);
        std::memcpy(hdr + 257, "ustar", 5);
        std::memcpy(hdr + 263, "00", 2);
        computeChksum(hdr);
        bytes.insert(bytes.end(), hdr, hdr + 512);
    }

    void addHardlink(const char* name, const char* target)
    {
        char hdr[512] = {};
        std::strncpy(hdr, name, 99);
        setOctal(hdr + 100, 8, 0644);
        hdr[156] = '1';
        std::strncpy(hdr + 157, target, 99);
        std::memcpy(hdr + 257, "ustar", 5);
        std::memcpy(hdr + 263, "00", 2);
        computeChksum(hdr);
        bytes.insert(bytes.end(), hdr, hdr + 512);
    }

    void finalize()
    {
        bytes.insert(bytes.end(), 1024, '\0');
    }

    std::istringstream stream() const
    {
        return std::istringstream(std::string(bytes.begin(), bytes.end()),
                                  std::ios::binary);
    }
};

} // namespace

// --------------------------------------------------------------------------
// TarReaderTest
// --------------------------------------------------------------------------

class TarReaderTest : public Pt::Unit::TestSuite
{
  public:
    TarReaderTest()
    : Pt::Unit::TestSuite("Pt::System::TarReaderTest")
    {
        registerMethod("EmptyArchive",  *this, &TarReaderTest::EmptyArchive);
        registerMethod("SingleFile",    *this, &TarReaderTest::SingleFile);
        registerMethod("Directory",     *this, &TarReaderTest::Directory);
        registerMethod("SymLink",       *this, &TarReaderTest::SymLink);
        registerMethod("MultipleFiles", *this, &TarReaderTest::MultipleFiles);
        registerMethod("LargeFile",     *this, &TarReaderTest::LargeFile);
        registerMethod("HardLink",      *this, &TarReaderTest::HardLink);
    }

  protected:
    // Two null blocks only — advance() returns null immediately.
    void EmptyArchive()
    {
        TarBuilder b;
        b.finalize();
        auto ss = b.stream();

        Pt::System::TarReader reader(ss);
        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // Single regular file — verify metadata and full content.
    void SingleFile()
    {
        const std::string content = "Hello, World!";

        TarBuilder b;
        b.addFile("hello.txt", content.data(), content.size());
        b.finalize();
        auto ss = b.stream();

        Pt::System::TarReader reader(ss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "hello.txt");
        PT_UNIT_ASSERT_EQUAL(entry->size(), content.size());
        PT_UNIT_ASSERT(entry->type() == Pt::System::TarEntry::File);

        std::string got;
        while(entry->avail() > 0)
        {
            got.append(entry->data(), entry->avail());
            if( entry->isEnd() )
                break;
            entry = reader.advance(4096);
            PT_UNIT_ASSERT(entry != 0);
        }
        PT_UNIT_ASSERT_EQUAL(got, content);

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // Directory entry — trailing slash is stripped by TarReader.
    void Directory()
    {
        TarBuilder b;
        b.addDirectory("subdir");
        b.finalize();
        auto ss = b.stream();

        Pt::System::TarReader reader(ss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "subdir");
        PT_UNIT_ASSERT(entry->type() == Pt::System::TarEntry::Directory);
        PT_UNIT_ASSERT_EQUAL(entry->size(), std::size_t(0));
        PT_UNIT_ASSERT(entry->isEnd());

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // Symbolic link — verify path and link target.
    void SymLink()
    {
        TarBuilder b;
        b.addSymlink("link.txt", "target.txt");
        b.finalize();
        auto ss = b.stream();

        Pt::System::TarReader reader(ss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "link.txt");
        PT_UNIT_ASSERT(entry->type() == Pt::System::TarEntry::Link);
        PT_UNIT_ASSERT(entry->linkTarget().toString() == "target.txt");
        PT_UNIT_ASSERT(entry->isEnd());

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // Hard link — verify type and link target.
    void HardLink()
    {
        TarBuilder b;
        b.addHardlink("link.bin", "original.bin");
        b.finalize();
        auto ss = b.stream();

        Pt::System::TarReader reader(ss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "link.bin");
        PT_UNIT_ASSERT(entry->type() == Pt::System::TarEntry::Hardlink);
        PT_UNIT_ASSERT(entry->linkTarget().toString() == "original.bin");
        PT_UNIT_ASSERT_EQUAL(entry->size(), std::size_t(0));
        PT_UNIT_ASSERT(entry->isEnd());

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // Two files — verify sequential reading does not mix content.
    void MultipleFiles()
    {
        const std::string first  = "content of file one";
        const std::string second = "content of file two";

        TarBuilder b;
        b.addFile("a.txt", first.data(),  first.size());
        b.addFile("b.txt", second.data(), second.size());
        b.finalize();
        auto ss = b.stream();

        Pt::System::TarReader reader(ss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "a.txt");

        std::string got;
        while(entry->avail() > 0)
        {
            got.append(entry->data(), entry->avail());
            if( entry->isEnd() )
                break;
            entry = reader.advance(4096);
            PT_UNIT_ASSERT(entry != 0);
        }
        PT_UNIT_ASSERT_EQUAL(got, first);

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "b.txt");

        got.clear();
        while(entry->avail() > 0)
        {
            got.append(entry->data(), entry->avail());
            if( entry->isEnd() )
                break;
            entry = reader.advance(4096);
            PT_UNIT_ASSERT(entry != 0);
        }
        PT_UNIT_ASSERT_EQUAL(got, second);

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // File larger than TarReader's 8192-byte internal buffer — exercises
    // multi-block advance() and auto-consume logic.
    void LargeFile()
    {
        const std::size_t fileSize = 20000;
        std::string content(fileSize, '\0');
        for(std::size_t i = 0; i < fileSize; ++i)
            content[i] = static_cast<char>(i & 0xFF);

        TarBuilder b;
        b.addFile("large.bin", content.data(), fileSize);
        b.finalize();
        auto ss = b.stream();

        Pt::System::TarReader reader(ss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT_EQUAL(entry->size(), fileSize);

        std::string got;
        got.reserve(fileSize);
        while(entry->avail() > 0)
        {
            got.append(entry->data(), entry->avail());
            if( entry->isEnd() )
                break;
            entry = reader.advance(4096);
            PT_UNIT_ASSERT(entry != 0);
        }

        PT_UNIT_ASSERT_EQUAL(got.size(), fileSize);
        PT_UNIT_ASSERT(got == content);

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }
};

Pt::Unit::RegisterTest<TarReaderTest> register_TarReaderTest;

// --------------------------------------------------------------------------
// TarWriterTest
// --------------------------------------------------------------------------

class TarWriterTest : public Pt::Unit::TestSuite
{
  public:
    TarWriterTest()
    : Pt::Unit::TestSuite("Pt::System::TarWriterTest")
    {
        registerMethod("WriteEmptyArchive",  *this, &TarWriterTest::WriteEmptyArchive);
        registerMethod("WriteFile",          *this, &TarWriterTest::WriteFile);
        registerMethod("WriteDirectory",     *this, &TarWriterTest::WriteDirectory);
        registerMethod("WriteSymlink",       *this, &TarWriterTest::WriteSymlink);
        registerMethod("WriteHardlink",      *this, &TarWriterTest::WriteHardlink);
        registerMethod("WriteMultipleFiles", *this, &TarWriterTest::WriteMultipleFiles);
        registerMethod("WriteStreamingFile", *this, &TarWriterTest::WriteStreamingFile);
        registerMethod("WriteStreamingOverrun",  *this, &TarWriterTest::WriteStreamingOverrun);
        registerMethod("WriteStreamingUnderrun", *this, &TarWriterTest::WriteStreamingUnderrun);
        registerMethod("MissingEndFile",         *this, &TarWriterTest::MissingEndFile);
    }

  protected:
    // finish() only — reader returns null immediately.
    void WriteEmptyArchive()
    {
        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);
        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // addFile() roundtrip — verify path, size, type and content.
    void WriteFile()
    {
        const std::string content = "Hello, World!";

        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.addFile(Pt::System::Path("hello.txt"),
                       content.data(), content.size(),
                       Pt::System::FileInfo::NoPerms);
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "hello.txt");
        PT_UNIT_ASSERT_EQUAL(entry->size(), content.size());
        PT_UNIT_ASSERT(entry->type() == Pt::System::TarEntry::File);

        std::string got;
        while(entry->avail() > 0)
        {
            got.append(entry->data(), entry->avail());
            if( entry->isEnd() )
                break;
            entry = reader.advance(4096);
            PT_UNIT_ASSERT(entry != 0);
        }
        PT_UNIT_ASSERT_EQUAL(got, content);

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // addDirectory() roundtrip — verify path and type.
    void WriteDirectory()
    {
        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.addDirectory(Pt::System::Path("subdir"),
                            Pt::System::FileInfo::NoPerms);
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "subdir");
        PT_UNIT_ASSERT(entry->type() == Pt::System::TarEntry::Directory);
        PT_UNIT_ASSERT(entry->isEnd());

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // addSymlink() roundtrip — verify path and link target.
    void WriteSymlink()
    {
        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.addSymlink(Pt::System::Path("link.txt"),
                          Pt::System::Path("target.txt"));
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "link.txt");
        PT_UNIT_ASSERT(entry->type() == Pt::System::TarEntry::Link);
        PT_UNIT_ASSERT(entry->linkTarget().toString() == "target.txt");
        PT_UNIT_ASSERT(entry->isEnd());

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // addHardlink() roundtrip — verify path, type and link target.
    void WriteHardlink()
    {
        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.addHardlink(Pt::System::Path("link.bin"),
                           Pt::System::Path("original.bin"));
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "link.bin");
        PT_UNIT_ASSERT(entry->type() == Pt::System::TarEntry::Hardlink);
        PT_UNIT_ASSERT(entry->linkTarget().toString() == "original.bin");
        PT_UNIT_ASSERT_EQUAL(entry->size(), std::size_t(0));
        PT_UNIT_ASSERT(entry->isEnd());

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // Two files — verify sequential writing does not mix content on read-back.
    void WriteMultipleFiles()
    {
        const std::string first  = "content of file one";
        const std::string second = "content of file two";

        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.addFile(Pt::System::Path("a.txt"),
                       first.data(), first.size(),
                       Pt::System::FileInfo::NoPerms);
        writer.addFile(Pt::System::Path("b.txt"),
                       second.data(), second.size(),
                       Pt::System::FileInfo::NoPerms);
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "a.txt");

        std::string got;
        while(entry->avail() > 0)
        {
            got.append(entry->data(), entry->avail());
            if( entry->isEnd() )
                break;
            entry = reader.advance(4096);
            PT_UNIT_ASSERT(entry != 0);
        }
        PT_UNIT_ASSERT_EQUAL(got, first);

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "b.txt");

        got.clear();
        while(entry->avail() > 0)
        {
            got.append(entry->data(), entry->avail());
            if( entry->isEnd() )
                break;
            entry = reader.advance(4096);
            PT_UNIT_ASSERT(entry != 0);
        }
        PT_UNIT_ASSERT_EQUAL(got, second);

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // beginFile/writeFileData/endFile — 20 KB written in 4 KB chunks, read
    // back and verified byte-for-byte.
    void WriteStreamingFile()
    {
        const std::size_t fileSize = 20000;
        std::string content(fileSize, '\0');
        for(std::size_t i = 0; i < fileSize; ++i)
            content[i] = static_cast<char>(i & 0xFF);

        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.beginFile(Pt::System::Path("large.bin"),
                         fileSize,
                         Pt::System::FileInfo::NoPerms);

        const std::size_t chunkSize = 4096;
        for(std::size_t offset = 0; offset < fileSize; offset += chunkSize)
        {
            std::size_t n = std::min(chunkSize, fileSize - offset);
            writer.writeFile(content.data() + offset, n);
        }
        writer.endFile();
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT_EQUAL(entry->size(), fileSize);

        std::string got;
        got.reserve(fileSize);
        while(entry->avail() > 0)
        {
            got.append(entry->data(), entry->avail());
            if( entry->isEnd() )
                break;
            entry = reader.advance(4096);
            PT_UNIT_ASSERT(entry != 0);
        }

        PT_UNIT_ASSERT_EQUAL(got.size(), fileSize);
        PT_UNIT_ASSERT(got == content);

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry == 0);
        PT_UNIT_ASSERT(reader.isEnd());
    }

    // writeFileData with more bytes than declared in beginFile → IOError.
    void WriteStreamingOverrun()
    {
        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.beginFile(Pt::System::Path("f.bin"), 4,
                         Pt::System::FileInfo::NoPerms);
        const char data[5] = {};
        PT_UNIT_ASSERT_THROW(writer.writeFile(data, 5), Pt::IOError);
    }

    // endFile before all declared bytes are written → IOError.
    void WriteStreamingUnderrun()
    {
        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.beginFile(Pt::System::Path("f.bin"), 10,
                         Pt::System::FileInfo::NoPerms);
        const char data[5] = {};
        writer.writeFile(data, 5);
        PT_UNIT_ASSERT_THROW(writer.endFile(), Pt::IOError);
    }

    // addFile while bytes are still pending (endFile was not called) → IOError.
    void MissingEndFile()
    {
        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.beginFile(Pt::System::Path("f.bin"), 10,
                         Pt::System::FileInfo::NoPerms);
        const char data[5] = {};
        writer.writeFile(data, 5);
        PT_UNIT_ASSERT_THROW(
            writer.addFile(Pt::System::Path("g.bin"), data, 5,
                           Pt::System::FileInfo::NoPerms),
            Pt::IOError);
    }
};

Pt::Unit::RegisterTest<TarWriterTest> register_TarWriterTest;
