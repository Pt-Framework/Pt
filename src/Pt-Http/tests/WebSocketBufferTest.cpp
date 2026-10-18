/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "WebSocketBuffer.h"
#include <sstream>
#include <string>
#include <cstring>

class WebSocketBufferTest : public Pt::Unit::TestSuite
{
    public:
        WebSocketBufferTest()
        : Pt::Unit::TestSuite("Pt::Http::WebSocketBufferTest")
        {
            Pt::Unit::TestSuite::registerMethod("Complete", *this,
                &WebSocketBufferTest::Complete);

            Pt::Unit::TestSuite::registerMethod("Split", *this,
                &WebSocketBufferTest::Split);

            Pt::Unit::TestSuite::registerMethod("Masked", *this,
                &WebSocketBufferTest::Masked);

            Pt::Unit::TestSuite::registerMethod("PartialPayload", *this,
                &WebSocketBufferTest::PartialPayload);

            Pt::Unit::TestSuite::registerMethod("Fragments", *this,
                &WebSocketBufferTest::Fragments);

            Pt::Unit::TestSuite::registerMethod("Ping", *this,
                &WebSocketBufferTest::Ping);

            Pt::Unit::TestSuite::registerMethod("PingBetween", *this,
                &WebSocketBufferTest::PingBetween);

            Pt::Unit::TestSuite::registerMethod("LengthForm", *this,
                &WebSocketBufferTest::LengthForm);

            Pt::Unit::TestSuite::registerMethod("Rsv", *this,
                &WebSocketBufferTest::Rsv);

            Pt::Unit::TestSuite::registerMethod("MaskDirection", *this,
                &WebSocketBufferTest::MaskDirection);

            Pt::Unit::TestSuite::registerMethod("ControlFin", *this,
                &WebSocketBufferTest::ControlFin);

            Pt::Unit::TestSuite::registerMethod("Continuation", *this,
                &WebSocketBufferTest::Continuation);

            Pt::Unit::TestSuite::registerMethod("MessageSize", *this,
                &WebSocketBufferTest::MessageSize);

            Pt::Unit::TestSuite::registerMethod("Utf8", *this,
                &WebSocketBufferTest::Utf8);
        }

        void Complete()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x81));
            bytes.push_back(static_cast<char>(5));
            bytes += "Hello";

            std::stringbuf buf(bytes);
            Pt::Http::WebSocketBuffer ws(buf);
            ws.setMasking(false);

            ws.import();
            PT_UNIT_ASSERT(ws.header());
            PT_UNIT_ASSERT_EQUALS(ws.opcode(), 0x01u);
            PT_UNIT_ASSERT(ws.finished());
            PT_UNIT_ASSERT_EQUALS(ws.failCode(), 0u);
            PT_UNIT_ASSERT_EQUALS(ws.in_avail(), std::streamsize(5));

            char text[5];
            PT_UNIT_ASSERT_EQUALS(ws.sgetn(text, 5), std::streamsize(5));
            PT_UNIT_ASSERT(std::memcmp(text, "Hello", 5) == 0);
        }

        void Split()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x82));
            bytes.push_back(static_cast<char>(3));
            bytes += "abc";

            std::stringbuf buf(bytes);
            Pt::Http::WebSocketBuffer ws(buf);
            ws.setMasking(false);

            PT_UNIT_ASSERT( ! ws.finished() );
            ws.import(1);
            PT_UNIT_ASSERT( ! ws.finished() );
            ws.import(1);
            PT_UNIT_ASSERT( ! ws.finished() );
            ws.import(1);
            PT_UNIT_ASSERT( ! ws.finished() );
            ws.import(1);
            PT_UNIT_ASSERT( ! ws.finished() );
            ws.import();
            PT_UNIT_ASSERT(ws.finished());
            PT_UNIT_ASSERT_EQUALS(ws.opcode(), 0x02u);
            PT_UNIT_ASSERT_EQUALS(ws.in_avail(), std::streamsize(3));

            char text[3];
            PT_UNIT_ASSERT_EQUALS(ws.sgetn(text, 3), std::streamsize(3));
            PT_UNIT_ASSERT(std::memcmp(text, "abc", 3) == 0);
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
            Pt::Http::WebSocketBuffer ws(buf);

            ws.import();
            PT_UNIT_ASSERT(ws.finished());
            PT_UNIT_ASSERT_EQUALS(ws.in_avail(), std::streamsize(2));

            char out[2];
            PT_UNIT_ASSERT_EQUALS(ws.sgetn(out, 2), std::streamsize(2));
            PT_UNIT_ASSERT(std::memcmp(out, "Hi", 2) == 0);
        }

        void PartialPayload()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x82));
            bytes.push_back(static_cast<char>(5));
            bytes += "abcde";

            std::stringbuf buf(bytes);
            Pt::Http::WebSocketBuffer ws(buf);
            ws.setMasking(false);

            ws.import(3);
            PT_UNIT_ASSERT(ws.header());
            PT_UNIT_ASSERT( ! ws.finished() );
            PT_UNIT_ASSERT_EQUALS(ws.in_avail(), std::streamsize(1));

            PT_UNIT_ASSERT_EQUALS(ws.sbumpc(), 'a');
            PT_UNIT_ASSERT_EQUALS(ws.in_avail(), std::streamsize(0));

            ws.import();
            PT_UNIT_ASSERT(ws.finished());
            PT_UNIT_ASSERT_EQUALS(ws.in_avail(), std::streamsize(4));
            PT_UNIT_ASSERT_EQUALS(ws.sbumpc(), 'b');
        }

        void Fragments()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x01));
            bytes.push_back(static_cast<char>(1));
            bytes.push_back('a');
            bytes.push_back(static_cast<char>(0x80));
            bytes.push_back(static_cast<char>(1));
            bytes.push_back('b');

            std::stringbuf buf(bytes);
            Pt::Http::WebSocketBuffer ws(buf);
            ws.setMasking(false);

            ws.import();
            PT_UNIT_ASSERT(ws.header());
            PT_UNIT_ASSERT_EQUALS(ws.opcode(), 0x01u);
            PT_UNIT_ASSERT(ws.finished());
            PT_UNIT_ASSERT_EQUALS(ws.in_avail(), std::streamsize(2));

            char text[2];
            PT_UNIT_ASSERT_EQUALS(ws.sgetn(text, 2), std::streamsize(2));
            PT_UNIT_ASSERT(std::memcmp(text, "ab", 2) == 0);
        }

        void Ping()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x89));
            bytes.push_back(static_cast<char>(2));
            bytes += "hi";

            std::stringbuf buf(bytes);
            Pt::Http::WebSocketBuffer ws(buf);
            ws.setMasking(false);

            ws.import();
            PT_UNIT_ASSERT(ws.hasControl());
            PT_UNIT_ASSERT_EQUALS(ws.controlOpcode(), 0x09u);
            PT_UNIT_ASSERT_EQUALS(ws.controlSize(), std::size_t(2));
            PT_UNIT_ASSERT(std::memcmp(ws.controlData(), "hi", 2) == 0);
            PT_UNIT_ASSERT_EQUALS(ws.in_avail(), std::streamsize(0));
            PT_UNIT_ASSERT( ! ws.header() );
            PT_UNIT_ASSERT( ! ws.finished() );

            ws.consumeControl();
            PT_UNIT_ASSERT( ! ws.hasControl() );
        }

        void PingBetween()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x01));
            bytes.push_back(static_cast<char>(1));
            bytes.push_back('a');
            bytes.push_back(static_cast<char>(0x89));
            bytes.push_back(static_cast<char>(0));
            bytes.push_back(static_cast<char>(0x80));
            bytes.push_back(static_cast<char>(1));
            bytes.push_back('b');

            std::stringbuf buf(bytes);
            Pt::Http::WebSocketBuffer ws(buf);
            ws.setMasking(false);

            ws.import();
            PT_UNIT_ASSERT(ws.header());
            PT_UNIT_ASSERT_EQUALS(ws.opcode(), 0x01u);
            PT_UNIT_ASSERT( ! ws.finished() );
            PT_UNIT_ASSERT(ws.hasControl());
            PT_UNIT_ASSERT_EQUALS(ws.controlOpcode(), 0x09u);
            PT_UNIT_ASSERT_EQUALS(ws.in_avail(), std::streamsize(1));
            PT_UNIT_ASSERT_EQUALS(ws.sbumpc(), 'a');

            ws.consumeControl();
            ws.import();
            PT_UNIT_ASSERT(ws.finished());
            PT_UNIT_ASSERT_EQUALS(ws.opcode(), 0x01u);
            PT_UNIT_ASSERT_EQUALS(ws.in_avail(), std::streamsize(1));
            PT_UNIT_ASSERT_EQUALS(ws.sbumpc(), 'b');
        }

        void LengthForm()
        {
            std::string short16;
            short16.push_back(static_cast<char>(0x81));
            short16.push_back(static_cast<char>(126));
            short16.push_back(0);
            short16.push_back(10);

            std::stringbuf buf16(short16);
            Pt::Http::WebSocketBuffer ws16(buf16);
            ws16.setMasking(false);
            ws16.import();
            PT_UNIT_ASSERT_EQUALS(ws16.failCode(), 1002u);

            std::string ok16;
            ok16.push_back(static_cast<char>(0x82));
            ok16.push_back(static_cast<char>(126));
            ok16.push_back(0);
            ok16.push_back(static_cast<char>(126));
            ok16.append(126, 'x');

            std::stringbuf bufOk(ok16);
            Pt::Http::WebSocketBuffer wsOk(bufOk);
            wsOk.setMasking(false);
            wsOk.import();
            PT_UNIT_ASSERT_EQUALS(wsOk.failCode(), 0u);
            PT_UNIT_ASSERT(wsOk.finished());
            PT_UNIT_ASSERT_EQUALS(wsOk.in_avail(), std::streamsize(126));

            std::string bad64(2 + 8, '\0');
            bad64[0] = static_cast<char>(0x82);
            bad64[1] = static_cast<char>(127);
            bad64[2] = static_cast<char>(0x80);

            std::stringbuf buf64(bad64);
            Pt::Http::WebSocketBuffer ws64(buf64);
            ws64.setMasking(false);
            ws64.import();
            PT_UNIT_ASSERT_EQUALS(ws64.failCode(), 1002u);
        }

        void Rsv()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x91));
            bytes.push_back(static_cast<char>(0));

            std::stringbuf buf(bytes);
            Pt::Http::WebSocketBuffer ws(buf);
            ws.setMasking(false);
            ws.import();
            PT_UNIT_ASSERT_EQUALS(ws.failCode(), 1002u);
        }

        void MaskDirection()
        {
            std::string masked;
            masked.push_back(static_cast<char>(0x81));
            masked.push_back(static_cast<char>(0x80));
            masked.append(4, '\0');

            std::stringbuf bufMasked(masked);
            Pt::Http::WebSocketBuffer wsMasked(bufMasked);
            wsMasked.setMasking(false);
            wsMasked.import();
            PT_UNIT_ASSERT_EQUALS(wsMasked.failCode(), 1002u);

            std::string plain;
            plain.push_back(static_cast<char>(0x81));
            plain.push_back(static_cast<char>(0));

            std::stringbuf bufPlain(plain);
            Pt::Http::WebSocketBuffer wsPlain(bufPlain);
            wsPlain.setMasking(true);
            wsPlain.import();
            PT_UNIT_ASSERT_EQUALS(wsPlain.failCode(), 1002u);
        }

        void ControlFin()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x09));
            bytes.push_back(static_cast<char>(0));

            std::stringbuf buf(bytes);
            Pt::Http::WebSocketBuffer ws(buf);
            ws.setMasking(false);
            ws.import();
            PT_UNIT_ASSERT_EQUALS(ws.failCode(), 1002u);
        }

        void Continuation()
        {
            std::string orphan;
            orphan.push_back(static_cast<char>(0x80));
            orphan.push_back(static_cast<char>(0));

            std::stringbuf bufOrphan(orphan);
            Pt::Http::WebSocketBuffer wsOrphan(bufOrphan);
            wsOrphan.setMasking(false);
            wsOrphan.import();
            PT_UNIT_ASSERT_EQUALS(wsOrphan.failCode(), 1002u);

            std::string bytes;
            bytes.push_back(static_cast<char>(0x01));
            bytes.push_back(static_cast<char>(1));
            bytes.push_back('a');
            bytes.push_back(static_cast<char>(0x81));
            bytes.push_back(static_cast<char>(1));
            bytes.push_back('b');

            std::stringbuf buf(bytes);
            Pt::Http::WebSocketBuffer ws(buf);
            ws.setMasking(false);
            ws.import();
            PT_UNIT_ASSERT_EQUALS(ws.failCode(), 1002u);
        }

        void MessageSize()
        {
            std::string bytes;
            bytes.push_back(static_cast<char>(0x81));
            bytes.push_back(static_cast<char>(4));
            bytes += "abcd";

            std::stringbuf buf(bytes);
            Pt::Http::WebSocketBuffer ws(buf);
            ws.setMasking(false);
            ws.setMaxMessageSize(3);
            ws.import();
            PT_UNIT_ASSERT_EQUALS(ws.failCode(), 1009u);

            std::string parts;
            parts.push_back(static_cast<char>(0x01));
            parts.push_back(static_cast<char>(2));
            parts += "ab";
            parts.push_back(static_cast<char>(0x80));
            parts.push_back(static_cast<char>(2));
            parts += "cd";

            std::stringbuf bufParts(parts);
            Pt::Http::WebSocketBuffer wsParts(bufParts);
            wsParts.setMasking(false);
            wsParts.setMaxMessageSize(3);
            wsParts.import();
            PT_UNIT_ASSERT(wsParts.header());
            PT_UNIT_ASSERT( ! wsParts.finished() );
            PT_UNIT_ASSERT_EQUALS(wsParts.failCode(), 1009u);

            std::stringbuf bufOpen(bytes);
            Pt::Http::WebSocketBuffer wsOpen(bufOpen);
            wsOpen.setMasking(false);
            wsOpen.setMaxMessageSize(0);
            wsOpen.import();
            PT_UNIT_ASSERT(wsOpen.finished());
            PT_UNIT_ASSERT_EQUALS(wsOpen.failCode(), 0u);
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
            Pt::Http::WebSocketBuffer wsSplit(bufSplit);
            wsSplit.setMasking(false);
            wsSplit.import();
            PT_UNIT_ASSERT(wsSplit.finished());
            PT_UNIT_ASSERT_EQUALS(wsSplit.failCode(), 0u);
            PT_UNIT_ASSERT_EQUALS(wsSplit.in_avail(), std::streamsize(2));

            std::string bad;
            bad.push_back(static_cast<char>(0x81));
            bad.push_back(static_cast<char>(1));
            bad.push_back(static_cast<char>(0x80));

            std::stringbuf bufBad(bad);
            Pt::Http::WebSocketBuffer wsBad(bufBad);
            wsBad.setMasking(false);
            wsBad.import();
            PT_UNIT_ASSERT_EQUALS(wsBad.failCode(), 1007u);

            std::string open;
            open.push_back(static_cast<char>(0x81));
            open.push_back(static_cast<char>(1));
            open.push_back(static_cast<char>(0xC3));

            std::stringbuf bufOpen(open);
            Pt::Http::WebSocketBuffer wsOpen(bufOpen);
            wsOpen.setMasking(false);
            wsOpen.import();
            PT_UNIT_ASSERT_EQUALS(wsOpen.failCode(), 1007u);
        }
};

Pt::Unit::RegisterTest<WebSocketBufferTest> register_WebSocketBufferTest;
