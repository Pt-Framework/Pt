/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/IOError.h"
#include "FrameReader.h"
#include <sstream>
#include <string>
#include <cstring>
#include <stdexcept>

class FrameReaderTest : public Pt::Unit::TestSuite
{
    public:
        FrameReaderTest()
        : Pt::Unit::TestSuite("Pt::Http::FrameReaderTest")
        {
            Pt::Unit::TestSuite::registerMethod("Complete", *this,
                &FrameReaderTest::Complete);

            Pt::Unit::TestSuite::registerMethod("Split", *this,
                &FrameReaderTest::Split);

            Pt::Unit::TestSuite::registerMethod("Masked", *this,
                &FrameReaderTest::Masked);

            Pt::Unit::TestSuite::registerMethod("LengthForm", *this,
                &FrameReaderTest::LengthForm);

            Pt::Unit::TestSuite::registerMethod("Rsv", *this,
                &FrameReaderTest::Rsv);

            Pt::Unit::TestSuite::registerMethod("MaskDirection", *this,
                &FrameReaderTest::MaskDirection);

            Pt::Unit::TestSuite::registerMethod("ControlFin", *this,
                &FrameReaderTest::ControlFin);

            Pt::Unit::TestSuite::registerMethod("Continuation", *this,
                &FrameReaderTest::Continuation);

            Pt::Unit::TestSuite::registerMethod("MessageSize", *this,
                &FrameReaderTest::MessageSize);

            Pt::Unit::TestSuite::registerMethod("Utf8", *this,
                &FrameReaderTest::Utf8);
        }

        void Complete()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x81));
            bytes.push_back(static_cast<char>(5));
            bytes += "Hello";

            std::stringbuf buf(bytes);
            Pt::Http::FrameReader reader(buf);
            reader.setMasking(false);

            Pt::Http::Frame* frame = reader.advance();
            PT_UNIT_ASSERT(frame != 0);
            PT_UNIT_ASSERT_EQUALS(frame->type(), Pt::Http::Frame::Data);
            PT_UNIT_ASSERT_EQUALS(frame->opcode(), 0x01u);
            PT_UNIT_ASSERT(frame->fin());
            PT_UNIT_ASSERT_EQUALS(frame->size(), std::size_t(5));
            PT_UNIT_ASSERT(std::memcmp(frame->data(), "Hello", 5) == 0);
            PT_UNIT_ASSERT(reader.advance() == 0);
        }

        void Split()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x82));
            bytes.push_back(static_cast<char>(3));
            bytes += "abc";

            std::stringbuf buf(bytes);
            Pt::Http::FrameReader reader(buf);
            reader.setMasking(false);

            PT_UNIT_ASSERT(reader.advance(1) == 0);
            PT_UNIT_ASSERT(reader.advance(1) == 0);
            PT_UNIT_ASSERT(reader.advance(1) == 0);

            Pt::Http::Frame* frame = reader.advance(1);
            PT_UNIT_ASSERT(frame == 0);
            frame = reader.advance();
            PT_UNIT_ASSERT(frame != 0);
            PT_UNIT_ASSERT_EQUALS(frame->opcode(), 0x02u);
            PT_UNIT_ASSERT(std::memcmp(frame->data(), "abc", 3) == 0);
        }

        void Masked()
        {
            char mask[4] = { 0x11, 0x22, 0x33, 0x44 };
            const char* text = "Hi";

            std::string bytes;
            bytes.push_back(static_cast<char>(0x81));
            bytes.push_back(static_cast<char>(0x80 | 2));
            bytes.append(mask, 4);
            bytes.push_back(static_cast<char>(text[0] ^ mask[0]));
            bytes.push_back(static_cast<char>(text[1] ^ mask[1]));

            std::stringbuf buf(bytes);
            Pt::Http::FrameReader reader(buf);

            Pt::Http::Frame& frame = reader.get();
            PT_UNIT_ASSERT_EQUALS(frame.type(), Pt::Http::Frame::Data);
            PT_UNIT_ASSERT_EQUALS(frame.size(), std::size_t(2));
            PT_UNIT_ASSERT(std::memcmp(frame.data(), "Hi", 2) == 0);
        }

        void LengthForm()
        {
            std::string short16;
            short16.push_back(static_cast<char>(0x81));
            short16.push_back(static_cast<char>(126));
            short16.push_back(0);
            short16.push_back(10);

            std::stringbuf buf16(short16);
            Pt::Http::FrameReader reader16(buf16);
            reader16.setMasking(false);
            PT_UNIT_ASSERT_THROW(reader16.advance(), Pt::IOError);

            std::string ok16;
            ok16.push_back(static_cast<char>(0x82));
            ok16.push_back(static_cast<char>(126));
            ok16.push_back(0);
            ok16.push_back(static_cast<char>(126));
            ok16.append(126, 'x');

            std::stringbuf bufOk(ok16);
            Pt::Http::FrameReader readerOk(bufOk);
            readerOk.setMasking(false);
            Pt::Http::Frame* frame = readerOk.advance();
            PT_UNIT_ASSERT(frame != 0);
            PT_UNIT_ASSERT_EQUALS(frame->size(), std::size_t(126));

            std::string bad64(2 + 8, '\0');
            bad64[0] = static_cast<char>(0x82);
            bad64[1] = static_cast<char>(127);
            bad64[2] = static_cast<char>(0x80);

            std::stringbuf buf64(bad64);
            Pt::Http::FrameReader reader64(buf64);
            reader64.setMasking(false);
            PT_UNIT_ASSERT_THROW(reader64.advance(), Pt::IOError);
        }

        void Rsv()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x91));
            bytes.push_back(static_cast<char>(0));

            std::stringbuf buf(bytes);
            Pt::Http::FrameReader reader(buf);
            reader.setMasking(false);
            PT_UNIT_ASSERT_THROW(reader.advance(), Pt::IOError);
        }

        void MaskDirection()
        {
            std::string masked;
            masked.push_back(static_cast<char>(0x81));
            masked.push_back(static_cast<char>(0x80));
            masked.append(4, '\0');

            std::stringbuf bufMasked(masked);
            Pt::Http::FrameReader readerMasked(bufMasked);
            readerMasked.setMasking(false);
            PT_UNIT_ASSERT_THROW(readerMasked.advance(), Pt::IOError);

            std::string plain;
            plain.push_back(static_cast<char>(0x81));
            plain.push_back(static_cast<char>(0));

            std::stringbuf bufPlain(plain);
            Pt::Http::FrameReader readerPlain(bufPlain);
            readerPlain.setMasking(true);
            PT_UNIT_ASSERT_THROW(readerPlain.advance(), Pt::IOError);
        }

        void ControlFin()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x09));
            bytes.push_back(static_cast<char>(0));

            std::stringbuf buf(bytes);
            Pt::Http::FrameReader reader(buf);
            reader.setMasking(false);
            PT_UNIT_ASSERT_THROW(reader.advance(), Pt::IOError);
        }

        void Continuation()
        {
            std::string orphan;
            orphan.push_back(static_cast<char>(0x80));
            orphan.push_back(static_cast<char>(0));

            std::stringbuf bufOrphan(orphan);
            Pt::Http::FrameReader readerOrphan(bufOrphan);
            readerOrphan.setMasking(false);
            PT_UNIT_ASSERT_THROW(readerOrphan.advance(), Pt::IOError);

            std::string bytes;
            bytes.push_back(static_cast<char>(0x01));
            bytes.push_back(static_cast<char>(1));
            bytes.push_back('a');
            bytes.push_back(static_cast<char>(0x81));
            bytes.push_back(static_cast<char>(1));
            bytes.push_back('b');

            std::stringbuf buf(bytes);
            Pt::Http::FrameReader reader(buf);
            reader.setMasking(false);

            Pt::Http::Frame* first = reader.advance();
            PT_UNIT_ASSERT(first != 0);
            PT_UNIT_ASSERT( ! first->fin() );
            PT_UNIT_ASSERT_THROW(reader.advance(), Pt::IOError);
        }

        void MessageSize()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x81));
            bytes.push_back(static_cast<char>(4));
            bytes += "abcd";

            std::stringbuf buf(bytes);
            Pt::Http::FrameReader reader(buf);
            reader.setMasking(false);
            reader.setMaxMessageSize(3);
            PT_UNIT_ASSERT_THROW(reader.advance(), Pt::IOError);

            std::string parts;
            parts.push_back(static_cast<char>(0x01));
            parts.push_back(static_cast<char>(2));
            parts += "ab";
            parts.push_back(static_cast<char>(0x80));
            parts.push_back(static_cast<char>(2));
            parts += "cd";

            std::stringbuf bufParts(parts);
            Pt::Http::FrameReader readerParts(bufParts);
            readerParts.setMasking(false);
            readerParts.setMaxMessageSize(3);

            Pt::Http::Frame* first = readerParts.advance();
            PT_UNIT_ASSERT(first != 0);
            PT_UNIT_ASSERT_THROW(readerParts.advance(), Pt::IOError);

            std::stringbuf bufOpen(bytes);
            Pt::Http::FrameReader readerOpen(bufOpen);
            readerOpen.setMasking(false);
            readerOpen.setMaxMessageSize(0);
            PT_UNIT_ASSERT(readerOpen.advance() != 0);
        }

        void Utf8()
        {
            std::string split;
            split.push_back(static_cast<char>(0x01));
            split.push_back(static_cast<char>(1));
            split.push_back(static_cast<char>(0xC3));
            split.push_back(static_cast<char>(0x80));
            split.push_back(static_cast<char>(1));
            split.push_back(static_cast<char>(0xA9));

            std::stringbuf bufSplit(split);
            Pt::Http::FrameReader readerSplit(bufSplit);
            readerSplit.setMasking(false);

            Pt::Http::Frame* first = readerSplit.advance();
            PT_UNIT_ASSERT(first != 0);
            Pt::Http::Frame* second = readerSplit.advance();
            PT_UNIT_ASSERT(second != 0);
            PT_UNIT_ASSERT(second->fin());

            std::string bad;
            bad.push_back(static_cast<char>(0x81));
            bad.push_back(static_cast<char>(1));
            bad.push_back(static_cast<char>(0x80));

            std::stringbuf bufBad(bad);
            Pt::Http::FrameReader readerBad(bufBad);
            readerBad.setMasking(false);
            PT_UNIT_ASSERT_THROW(readerBad.advance(), Pt::IOError);

            std::string open;
            open.push_back(static_cast<char>(0x81));
            open.push_back(static_cast<char>(1));
            open.push_back(static_cast<char>(0xC3));

            std::stringbuf bufOpen(open);
            Pt::Http::FrameReader readerOpen(bufOpen);
            readerOpen.setMasking(false);
            PT_UNIT_ASSERT_THROW(readerOpen.advance(), Pt::IOError);
        }
};

Pt::Unit::RegisterTest<FrameReaderTest> register_FrameReaderTest;
