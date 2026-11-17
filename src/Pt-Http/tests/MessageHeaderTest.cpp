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
            registerMethod("Empty", *this, &MessageHeaderTest::Empty);
            registerMethod("Upgrade", *this, &MessageHeaderTest::Upgrade);
            registerMethod("FieldName", *this, &MessageHeaderTest::FieldName);
        }

        void setUp()
        { }

        void tearDown()
        { }

        void Value()
        {
            Pt::Http::MessageHeader header;
            header.set("Sec-WebSocket-Version", " 13 ");

            PT_UNIT_ASSERT( header.has("Sec-WebSocket-Version", "13") );

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

            PT_UNIT_ASSERT( header.has("Connection", "keep-alive") );
            PT_UNIT_ASSERT( header.has("Connection", "Upgrade") );
            PT_UNIT_ASSERT( header.has("Connection", "close") );

            Pt::Http::MessageHeader::FieldIterator field =
                header.field("Connection");
            PT_UNIT_ASSERT(field != header.end());

            Pt::Http::MessageHeader::ValueIterator it = field->begin();
            PT_UNIT_ASSERT(it != field->end());
            PT_UNIT_ASSERT( it->equalsIgnoreCase("Keep-Alive") );

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
            PT_UNIT_ASSERT( header.has("Connection", "upgrade") );
            PT_UNIT_ASSERT( header.has("CONNECTION", "Keep-Alive") );
            PT_UNIT_ASSERT( ! header.has("Connection", "websocket") );
            PT_UNIT_ASSERT( ! header.has("Upgrade", "websocket") );
            PT_UNIT_ASSERT( ! header.isSet("Connection", "upgrade") );

            header.add("Connection", "close");
            PT_UNIT_ASSERT( header.has("Connection", "close") );

            header.set("Upgrade", "");
            PT_UNIT_ASSERT( header.has("Upgrade") );
            PT_UNIT_ASSERT( ! header.has("Upgrade", "websocket") );
        }

        void Empty()
        {
            Pt::Http::MessageHeader header;
            header.set("Upgrade", " \t ");

            Pt::Http::MessageHeader::FieldIterator field = header.field("Upgrade");
            PT_UNIT_ASSERT(field != header.end());
            PT_UNIT_ASSERT( field->empty() );
            PT_UNIT_ASSERT( ! header.has("Upgrade", "websocket") );
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
        }

        void FieldName()
        {
            Pt::Http::MessageHeader header;
            PT_UNIT_ASSERT( Pt::Http::MessageHeader::isToken("Sec-WebSocket-Key") );
            PT_UNIT_ASSERT( ! Pt::Http::MessageHeader::isToken("Bad Name") );
            PT_UNIT_ASSERT( ! Pt::Http::MessageHeader::isToken("") );
            PT_UNIT_ASSERT_THROW(header.set("Bad Name", "value"), std::invalid_argument);
        }
};

Pt::Unit::RegisterTest<MessageHeaderTest> register_MessageHeaderTest;
