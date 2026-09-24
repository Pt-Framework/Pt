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
#include <Pt/DateTime.h>
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

    // Foreign UStar header: identity lives only in the fixed fields.
    void addOwnedFile(const char* name,
                      unsigned long mode,
                      unsigned long uid,
                      unsigned long gid,
                      const char* uname,
                      const char* gname,
                      unsigned long mtime,
                      const void* content,
                      std::size_t size)
    {
        char hdr[512] = {};
        std::strncpy(hdr, name, 99);
        setOctal(hdr + 100, 8, mode);
        setOctal(hdr + 108, 8, uid);
        setOctal(hdr + 116, 8, gid);
        setOctal(hdr + 124, 12, static_cast<unsigned long>(size));
        setOctal(hdr + 136, 12, mtime);
        hdr[156] = '0';
        std::strncpy(hdr + 265, uname, 31);
        std::strncpy(hdr + 297, gname, 31);
        std::memcpy(hdr + 257, "ustar", 5);
        std::memcpy(hdr + 263, "00", 2);
        computeChksum(hdr);
        bytes.insert(bytes.end(), hdr, hdr + 512);

        const char* p = static_cast<const char*>(content);
        bytes.insert(bytes.end(), p, p + size);
        std::size_t pad = (512 - (size % 512)) % 512;
        bytes.insert(bytes.end(), pad, '\0');
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
        registerMethod("UStarIdentity", *this, &TarReaderTest::UStarIdentity);
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

    // Identity stored only in the UStar header, with no Pax records.
    void UStarIdentity()
    {
        const std::string content = "owned";

        TarBuilder b;
        b.addOwnedFile("owned.txt",
                       0644 | 04000,
                       1000,
                       100,
                       "marc",
                       "staff",
                       1717243200,
                       content.data(),
                       content.size());
        b.finalize();
        auto ss = b.stream();

        Pt::System::TarReader reader(ss);
        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->path().toString() == "owned.txt");
        PT_UNIT_ASSERT_EQUAL(entry->ownerId(), static_cast<Pt::uint32_t>(1000));
        PT_UNIT_ASSERT_EQUAL(entry->groupId(), static_cast<Pt::uint32_t>(100));
        PT_UNIT_ASSERT(entry->ownerName() == "marc");
        PT_UNIT_ASSERT(entry->groupName() == "staff");
        PT_UNIT_ASSERT_EQUAL(
            static_cast<unsigned>(entry->permissions()),
            static_cast<unsigned>(0644 | 04000));
        PT_UNIT_ASSERT(entry->mtime() == Pt::DateTime(2024, 6, 1, 12, 0, 0));
        PT_UNIT_ASSERT(entry->isEnd());
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
        registerMethod("WriteIdentity",          *this, &TarWriterTest::WriteIdentity);
        registerMethod("WriteUnsetIdentity",     *this, &TarWriterTest::WriteUnsetIdentity);
        registerMethod("WriteRootIdentity",      *this, &TarWriterTest::WriteRootIdentity);
        registerMethod("WritePaxIdentity",       *this, &TarWriterTest::WritePaxIdentity);
        registerMethod("WriteLinkMetadata",      *this, &TarWriterTest::WriteLinkMetadata);
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
        Pt::System::TarEntry file;
        file.setPath(Pt::System::Path("hello.txt"));
        file.setSize(content.size());
        writer.addFile(file, content.data(), content.size());
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
        Pt::System::TarEntry dir;
        dir.setPath(Pt::System::Path("subdir"));
        writer.addDirectory(dir);
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
        Pt::System::TarEntry link;
        link.setPath(Pt::System::Path("link.txt"));
        link.setLinkTarget(Pt::System::Path("target.txt"));
        writer.addSymlink(link);
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
        Pt::System::TarEntry link;
        link.setPath(Pt::System::Path("link.bin"));
        link.setLinkTarget(Pt::System::Path("original.bin"));
        writer.addHardlink(link);
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
        Pt::System::TarEntry a;
        a.setPath(Pt::System::Path("a.txt"));
        a.setSize(first.size());
        writer.addFile(a, first.data(), first.size());

        Pt::System::TarEntry b;
        b.setPath(Pt::System::Path("b.txt"));
        b.setSize(second.size());
        writer.addFile(b, second.data(), second.size());
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
        Pt::System::TarEntry file;
        file.setPath(Pt::System::Path("large.bin"));
        file.setSize(fileSize);
        writer.beginFile(file);

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
        Pt::System::TarEntry file;
        file.setPath(Pt::System::Path("f.bin"));
        file.setSize(4);
        writer.beginFile(file);
        const char data[5] = {};
        PT_UNIT_ASSERT_THROW(writer.writeFile(data, 5), Pt::IOError);
    }

    // endFile before all declared bytes are written → IOError.
    void WriteStreamingUnderrun()
    {
        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        Pt::System::TarEntry file;
        file.setPath(Pt::System::Path("f.bin"));
        file.setSize(10);
        writer.beginFile(file);
        const char data[5] = {};
        writer.writeFile(data, 5);
        PT_UNIT_ASSERT_THROW(writer.endFile(), Pt::IOError);
    }

    // addFile while bytes are still pending (endFile was not called) → IOError.
    void MissingEndFile()
    {
        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        Pt::System::TarEntry file;
        file.setPath(Pt::System::Path("f.bin"));
        file.setSize(10);
        writer.beginFile(file);
        const char data[5] = {};
        writer.writeFile(data, 5);

        Pt::System::TarEntry other;
        other.setPath(Pt::System::Path("g.bin"));
        other.setSize(5);
        PT_UNIT_ASSERT_THROW(writer.addFile(other, data, 5), Pt::IOError);
    }

    // uid, gid, ASCII names, setuid and an explicit mtime stay in UStar.
    void WriteIdentity()
    {
        const std::string content = "owned";
        const Pt::DateTime when(2024, 6, 1, 12, 0, 0);

        Pt::System::TarEntry file;
        file.setPath(Pt::System::Path("owned.txt"));
        file.setSize(content.size());
        file.setPermissions(static_cast<Pt::System::FileInfo::Perms>(0644 | 04000));
        file.setMtime(when);
        file.setOwnerId(1000);
        file.setOwnerName(Pt::String("marc"));
        file.setGroupId(100);
        file.setGroupName(Pt::String("staff"));

        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.addFile(file, content.data(), content.size());
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);
        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT_EQUAL(entry->ownerId(), static_cast<Pt::uint32_t>(1000));
        PT_UNIT_ASSERT_EQUAL(entry->groupId(), static_cast<Pt::uint32_t>(100));
        PT_UNIT_ASSERT(entry->ownerName() == "marc");
        PT_UNIT_ASSERT(entry->groupName() == "staff");
        PT_UNIT_ASSERT_EQUAL(
            static_cast<unsigned>(entry->permissions()),
            static_cast<unsigned>(0644 | 04000));
        PT_UNIT_ASSERT(entry->mtime() == when);
    }

    // An entry that never sets an identity stores neither id nor name.
    void WriteUnsetIdentity()
    {
        const std::string content = "plain";

        Pt::System::TarEntry file;
        file.setPath(Pt::System::Path("plain.txt"));
        file.setSize(content.size());

        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.addFile(file, content.data(), content.size());
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);
        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT_EQUAL(entry->ownerId(), Pt::System::TarEntry::NoId);
        PT_UNIT_ASSERT_EQUAL(entry->groupId(), Pt::System::TarEntry::NoId);
        PT_UNIT_ASSERT(entry->ownerName().empty());
        PT_UNIT_ASSERT(entry->groupName().empty());
        PT_UNIT_ASSERT(entry->mtime() == Pt::DateTime(1970, 1, 1));
    }

    // Numeric id 0 and the name "root" are real values, not sentinels.
    void WriteRootIdentity()
    {
        const std::string content = "root-owned";

        Pt::System::TarEntry file;
        file.setPath(Pt::System::Path("root.txt"));
        file.setSize(content.size());
        file.setOwnerId(0);
        file.setOwnerName(Pt::String("root"));
        file.setGroupId(0);
        file.setGroupName(Pt::String("root"));

        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.addFile(file, content.data(), content.size());
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);
        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT_EQUAL(entry->ownerId(), static_cast<Pt::uint32_t>(0));
        PT_UNIT_ASSERT_EQUAL(entry->groupId(), static_cast<Pt::uint32_t>(0));
        PT_UNIT_ASSERT(entry->ownerName() == "root");
        PT_UNIT_ASSERT(entry->groupName() == "root");
    }

    // An id past seven octal digits and a long name travel in a Pax header.
    void WritePaxIdentity()
    {
        const std::string content = "pax";
        const Pt::uint32_t bigId = 010000000u;
        const Pt::String longName("abcdefghijklmnopqrstuvwxyz012345");

        Pt::System::TarEntry file;
        file.setPath(Pt::System::Path("pax.txt"));
        file.setSize(content.size());
        file.setOwnerId(bigId);
        file.setOwnerName(longName);
        file.setGroupId(bigId);
        Pt::String groupName("m");
        groupName += Pt::Char(0x00fc);
        groupName += "ller";
        file.setGroupName(groupName);

        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.addFile(file, content.data(), content.size());
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);
        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT_EQUAL(entry->ownerId(), bigId);
        PT_UNIT_ASSERT_EQUAL(entry->groupId(), bigId);
        PT_UNIT_ASSERT(entry->ownerName() == longName);
        PT_UNIT_ASSERT(entry->groupName() == groupName);
    }

    // Links carry permissions, mtime, owner and group like a file does.
    void WriteLinkMetadata()
    {
        const Pt::DateTime when(2024, 6, 1, 12, 0, 0);

        Pt::System::TarEntry link;
        link.setPath(Pt::System::Path("link.txt"));
        link.setLinkTarget(Pt::System::Path("target.txt"));
        link.setPermissions(Pt::System::FileInfo::OwnerRead);
        link.setMtime(when);
        link.setOwnerId(1000);
        link.setOwnerName(Pt::String("marc"));
        link.setGroupId(100);
        link.setGroupName(Pt::String("staff"));

        std::ostringstream oss;
        Pt::System::TarWriter writer(oss);
        writer.addSymlink(link);

        Pt::System::TarEntry hard;
        hard.setPath(Pt::System::Path("hard.bin"));
        hard.setLinkTarget(Pt::System::Path("original.bin"));
        hard.setPermissions(Pt::System::FileInfo::OwnerWrite);
        hard.setMtime(when);
        hard.setOwnerId(7);
        writer.addHardlink(hard);
        writer.finish();

        std::istringstream iss(oss.str(), std::ios::binary);
        Pt::System::TarReader reader(iss);

        const Pt::System::TarEntry* entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->type() == Pt::System::TarEntry::Link);
        PT_UNIT_ASSERT(entry->permissions() == Pt::System::FileInfo::OwnerRead);
        PT_UNIT_ASSERT(entry->mtime() == when);
        PT_UNIT_ASSERT_EQUAL(entry->ownerId(), static_cast<Pt::uint32_t>(1000));
        PT_UNIT_ASSERT(entry->ownerName() == "marc");
        PT_UNIT_ASSERT_EQUAL(entry->groupId(), static_cast<Pt::uint32_t>(100));

        entry = reader.advance(4096);
        PT_UNIT_ASSERT(entry != 0);
        PT_UNIT_ASSERT(entry->type() == Pt::System::TarEntry::Hardlink);
        PT_UNIT_ASSERT(entry->permissions() == Pt::System::FileInfo::OwnerWrite);
        PT_UNIT_ASSERT(entry->mtime() == when);
        PT_UNIT_ASSERT_EQUAL(entry->ownerId(), static_cast<Pt::uint32_t>(7));
        PT_UNIT_ASSERT(entry->groupName().empty());
    }
};

Pt::Unit::RegisterTest<TarWriterTest> register_TarWriterTest;
