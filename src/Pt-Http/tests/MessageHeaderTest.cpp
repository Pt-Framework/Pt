/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Http/Message.h"
#include <stdexcept>
#include <string>

class MessageHeaderTest : public Pt::Unit::TestSuite
{
    public:
        MessageHeaderTest()
        : Pt::Unit::TestSuite("MessageHeaderTest")
        {
            registerMethod("Value", *this, &MessageHeaderTest::Value);
            registerMethod("Values", *this, &MessageHeaderTest::Values);
            registerMethod("HasValue", *this, &MessageHeaderTest::HasValue);
            registerMethod("Contains", *this, &MessageHeaderTest::Contains);
            registerMethod("Empty", *this, &MessageHeaderTest::Empty);
            registerMethod("Upgrade", *this, &MessageHeaderTest::Upgrade);
            registerMethod("FieldName", *this, &MessageHeaderTest::FieldName);
            registerMethod("Field", *this, &MessageHeaderTest::Field);
            registerMethod("MatchesValue", *this, &MessageHeaderTest::MatchesValue);
        }

        void setUp()
        { }

        void tearDown()
        { }

        void Value()
        {
            Pt::Http::MessageHeader header;
            header.set("Sec-WebSocket-Version", " 13 ");

            PT_UNIT_ASSERT( header.contains("Sec-WebSocket-Version", "13") );
            PT_UNIT_ASSERT( header.matches("Sec-WebSocket-Version", "13") );

            Pt::Http::MessageHeader::FieldIterator field =
                header.field("sec-websocket-version");
            PT_UNIT_ASSERT(field != header.end());

            Pt::Http::MessageHeader::ValueIterator it = field->begin();
            PT_UNIT_ASSERT(it != field->end());
            PT_UNIT_ASSERT( it->equals("13") );
            PT_UNIT_ASSERT_EQUALS(it->size(), std::size_t(2));
            ++it;
            PT_UNIT_ASSERT(it == field->end());
        }

        void Values()
        {
            Pt::Http::MessageHeader header;
            header.set("Connection", "keep-alive, Upgrade");
            header.add("Connection", " close ");

            PT_UNIT_ASSERT( header.contains("Connection", "keep-alive") );
            PT_UNIT_ASSERT( header.contains("Connection", "Upgrade") );
            PT_UNIT_ASSERT( header.contains("Connection", "close") );
            PT_UNIT_ASSERT( ! header.matches("Connection", "keep-alive") );

            Pt::Http::MessageHeader::FieldIterator field =
                header.field("Connection");
            PT_UNIT_ASSERT(field != header.end());

            Pt::Http::MessageHeader::ValueIterator it = field->begin();
            PT_UNIT_ASSERT(it != field->end());
            PT_UNIT_ASSERT( it->matches("Keep-Alive") );

            ++it;
            PT_UNIT_ASSERT(it != field->end());
            PT_UNIT_ASSERT( it->equals("Upgrade") );

            ++it;
            PT_UNIT_ASSERT(it == field->end());

            ++field;
            PT_UNIT_ASSERT(field != header.end());
            it = field->begin();
            PT_UNIT_ASSERT(it != field->end());
            PT_UNIT_ASSERT( it->equals("close") );
            ++it;
            PT_UNIT_ASSERT(it == field->end());
        }

        void HasValue()
        {
            Pt::Http::MessageHeader header;
            header.set("Connection", "keep-alive, Upgrade");

            PT_UNIT_ASSERT( header.has("Connection") );
            PT_UNIT_ASSERT( header.contains("Connection", "upgrade") );
            PT_UNIT_ASSERT( header.contains("CONNECTION", "Keep-Alive") );
            PT_UNIT_ASSERT( ! header.contains("Connection", "websocket") );
            PT_UNIT_ASSERT( ! header.contains("Upgrade", "websocket") );
            PT_UNIT_ASSERT( ! header.matches("Connection", "upgrade") );
            PT_UNIT_ASSERT( ! header.isSet("Connection", "upgrade") );

            header.add("Connection", "close");
            PT_UNIT_ASSERT( header.contains("Connection", "close") );

            header.set("Upgrade", "");
            PT_UNIT_ASSERT( header.has("Upgrade") );
            PT_UNIT_ASSERT( ! header.contains("Upgrade", "websocket") );
        }

        void Contains()
        {
            Pt::Http::MessageHeader header;
            header.set("Connection", "keep-alive, Upgrade");

            PT_UNIT_ASSERT( header.contains("Connection", "Keep-Alive") );
            PT_UNIT_ASSERT( header.contains("Connection", "Upgrade") );
            PT_UNIT_ASSERT( ! header.matches("Connection", "keep-alive") );

            header.set("Transfer-Encoding", "Chunked");
            PT_UNIT_ASSERT( header.matches("Transfer-Encoding", "chunked") );

            header.set("Transfer-Encoding", "gzip, chunked");
            PT_UNIT_ASSERT( header.contains("Transfer-Encoding", "chunked") );
            PT_UNIT_ASSERT( ! header.matches("Transfer-Encoding", "chunked") );

            header.set("X-Name", "Foo");
            PT_UNIT_ASSERT( header.isSet("X-Name", "Foo") );
            PT_UNIT_ASSERT( ! header.isSet("X-Name", "foo") );
            PT_UNIT_ASSERT( ! header.isSet("Connection", "upgrade") );
        }

        void Empty()
        {
            Pt::Http::MessageHeader header;
            header.set("Upgrade", " \t ");

            Pt::Http::MessageHeader::FieldIterator field = header.field("Upgrade");
            PT_UNIT_ASSERT(field != header.end());
            PT_UNIT_ASSERT( field->empty() );
            PT_UNIT_ASSERT( ! header.contains("Upgrade", "websocket") );
            PT_UNIT_ASSERT(header.field("Missing") == header.end());
        }

        void Upgrade()
        {
            Pt::Http::MessageHeader header;
            header.setVersion(1, 1);
            PT_UNIT_ASSERT( ! header.isUpgrade() );

            header.set("Connection", "keep-alive, Upgrade");
            header.set("Upgrade", " websocket ");
            PT_UNIT_ASSERT( header.isUpgrade() );
            PT_UNIT_ASSERT( header.isKeepAlive() );
            PT_UNIT_ASSERT( ! header.isChunked() );

            header.set("Transfer-Encoding", "gzip, chunked");
            PT_UNIT_ASSERT( header.isChunked() );

            header.set("Transfer-Encoding", "chunked, gzip");
            PT_UNIT_ASSERT( ! header.isChunked() );

            header.set("Content-Length", "12abc");
            PT_UNIT_ASSERT(header.contentLength() == 0);

            header.set("Content-Length", "18");
            PT_UNIT_ASSERT(header.contentLength() == 18);
            PT_UNIT_ASSERT( Pt::Http::MessageHeader::isDigits("18") );
            PT_UNIT_ASSERT( ! Pt::Http::MessageHeader::isDigits("12abc") );
            PT_UNIT_ASSERT( ! Pt::Http::MessageHeader::isDigits("") );
        }

        void Field()
        {
            Pt::Http::MessageHeader header;
            header.set("Content-Length", "4");

            Pt::Http::MessageHeader::FieldIterator field = header.field("content-length");
            PT_UNIT_ASSERT(field != header.end());
            PT_UNIT_ASSERT( field->matches("Content-Length") );
            PT_UNIT_ASSERT( field->equals("Content-Length") );
            PT_UNIT_ASSERT( ! field->equals("content-length") );
            PT_UNIT_ASSERT( ! field->matches("Content-Lengths") );
            PT_UNIT_ASSERT( ! field->matches(0) );
            PT_UNIT_ASSERT( ! field->equals(0) );
        }

        void FieldName()
        {
            Pt::Http::MessageHeader header;
            PT_UNIT_ASSERT( Pt::Http::MessageHeader::isToken("Sec-WebSocket-Key") );
            PT_UNIT_ASSERT( ! Pt::Http::MessageHeader::isToken("Bad Name") );
            PT_UNIT_ASSERT( ! Pt::Http::MessageHeader::isToken("") );
            PT_UNIT_ASSERT_THROW(header.set("Bad Name", "value"), std::invalid_argument);
        }

        void MatchesValue()
        {
            using Pt::Http::MessageHeader;
            PT_UNIT_ASSERT( MessageHeader::isMatch("https://Example.com", "HTTPS://example.COM") );
            PT_UNIT_ASSERT( ! MessageHeader::isMatch("https://example.com", "https://example.com:8443") );
            PT_UNIT_ASSERT( ! MessageHeader::isMatch("https://example.com:8443", "https://example.com") );
            PT_UNIT_ASSERT( MessageHeader::isMatch("", "") );
            PT_UNIT_ASSERT( ! MessageHeader::isMatch(0, "a") );
            PT_UNIT_ASSERT( ! MessageHeader::isMatch("a", 0) );
        }
};

Pt::Unit::RegisterTest<MessageHeaderTest> register_MessageHeaderTest;
