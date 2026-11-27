/*
 * Copyright (C) 2009 by Tommi Mäkitalo
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
#include "Parser.h"
#include <Pt/Http/HttpError.h>
#include <Pt/Http/Message.h>
#include <sstream>
#include <string>

class ParserTest : public Pt::Unit::TestSuite, private Pt::Http::HeaderParser::Event
{
    public:
        ParserTest()
        : Pt::Unit::TestSuite("ParserTest")
        {
            registerMethod( "SimpleRequest", *this, &ParserTest::SimpleRequest);
            registerMethod( "RequestWithHeader", *this, &ParserTest::RequestWithHeader);
            registerMethod( "RequestWithMultilineHeader", *this, &ParserTest::RequestWithMultilineHeader);
            registerMethod( "RequestWithWhitespace", *this, &ParserTest::RequestWithWhitespace);
            registerMethod( "RequestWithQueryParam", *this, &ParserTest::RequestWithQueryParam);
            registerMethod( "LongFieldName", *this, &ParserTest::LongFieldName);
            registerMethod( "BodyFraming", *this, &ParserTest::BodyFraming);
            registerMethod( "Response", *this, &ParserTest::Response);
            registerMethod( "SpaceBeforeColon", *this, &ParserTest::SpaceBeforeColon);
            registerMethod( "ObsFold", *this, &ParserTest::ObsFold);
            registerMethod( "FieldValue", *this, &ParserTest::FieldValue);
            registerMethod( "FieldName", *this, &ParserTest::FieldName);
            registerMethod( "EmptyValue", *this, &ParserTest::EmptyValue);
            registerMethod( "HeaderSize", *this, &ParserTest::HeaderSize);
        }

        void setUp()
        {
            events.str(std::string());
            events.clear();
        }

        void tearDown()
        { }

        void SimpleRequest()
        {
            Pt::Http::HeaderParser parser(*this, false);

            std::istringstream msg("GET /foo HTTP/1.0\r\n\r\n");

            parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(std::size_t( msg.tellg() ), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/foo)V(1.0)E()");
        }

        void RequestWithHeader()
        {
            Pt::Http::HeaderParser parser(*this, false);

            std::istringstream msg("GET /foo HTTP/1.0\r\nUser-Agent: Pt-Unit\nContent-Size:47\r\n\r\n");

            parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(std::size_t( msg.tellg() ), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/foo)V(1.0)K(User-Agent)H(Pt-Unit)K(Content-Size)H(47)E()");
        }

        void RequestWithMultilineHeader()
        {
            Pt::Http::HeaderParser parser(*this, false);

            std::istringstream msg("GET /foo HTTP/1.0\r\nFoo: line1\r\nBar: line2\r\n\r\n");

            parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(std::size_t( msg.tellg() ), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/foo)V(1.0)K(Foo)H(line1)K(Bar)H(line2)E()");
        }

        void RequestWithWhitespace()
        {
            Pt::Http::HeaderParser parser(*this, false);

            std::istringstream msg("PUT /foo HTTP/1.0\r\nContent-Size:  47  \r\n\r\n");

            parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(std::size_t( msg.tellg() ), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(PUT)U(/foo)V(1.0)K(Content-Size)H(47)E()");
        }

        void RequestWithQueryParam()
        {
            Pt::Http::HeaderParser parser(*this, false);

            std::istringstream msg("GET /foo+bar?a=4&b=Hello+World HTTP/1.1\r\n\r\n");

            parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(std::size_t( msg.tellg() ), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/foo bar)Q(a=4&b=Hello+World)V(1.1)E()");
        }

        void LongFieldName()
        {
            const std::string name511(511, 'A');
            const std::string name512(512, 'A');

            {
                Pt::Http::MessageHeader header;
                Pt::Http::HeaderParser::MessageHeaderEvent ev(header);
                Pt::Http::HeaderParser parser(ev, false);

                std::istringstream msg("GET / HTTP/1.1\r\n" + name511 + ": v\r\n\r\n");
                parser.advance(msg);

                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(! parser.fail());
                PT_UNIT_ASSERT(header.get(name511.c_str()) != 0);
                PT_UNIT_ASSERT_EQUALS(std::string(header.get(name511.c_str())), std::string("v"));
            }

            {
                Pt::Http::MessageHeader header;
                Pt::Http::HeaderParser::MessageHeaderEvent ev(header);
                Pt::Http::HeaderParser parser(ev, false);

                std::istringstream msg("GET / HTTP/1.1\r\n" + name512 + ": v\r\n\r\n");
                PT_UNIT_ASSERT_THROW(parser.advance(msg), Pt::Http::HttpError);
                PT_UNIT_ASSERT(header.get(name512.c_str()) == 0);
            }
        }

        void parseFraming(const std::string& fields)
        {
            Pt::Http::MessageHeader header;
            Pt::Http::HeaderParser::MessageHeaderEvent ev(header);
            Pt::Http::HeaderParser parser(ev, false);

            std::istringstream msg("GET / HTTP/1.1\r\n" + fields + "\r\n");
            parser.advance(msg);
        }

        void BodyFraming()
        {
            parseFraming("Content-Length: 4\r\nContent-Length: 4\r\n");
            parseFraming("Transfer-Encoding: chunked\r\n");
            parseFraming("Transfer-Encoding: gzip\r\n");
            parseFraming("Transfer-Encoding: gzip, chunked\r\n");
            parseFraming("Transfer-Encoding: chunked\r\nTransfer-Encoding: chunked\r\n");

            PT_UNIT_ASSERT_THROW(parseFraming("Content-Length: 4\r\nTransfer-Encoding: chunked\r\n"),
                                 Pt::Http::HttpError);
            PT_UNIT_ASSERT_THROW(parseFraming("Transfer-Encoding: chunked\r\nContent-Length: 4\r\n"),
                                 Pt::Http::HttpError);
            PT_UNIT_ASSERT_THROW(parseFraming("Content-Length: 4\r\nContent-Length: 5\r\n"),
                                 Pt::Http::HttpError);
            PT_UNIT_ASSERT_THROW(parseFraming("Content-Length: 12abc\r\n"),
                                 Pt::Http::HttpError);
        }

        void Response()
        {
            Pt::Http::HeaderParser parser(*this, true);

            std::istringstream msg("HTTP/1.1 200 OK\r\nConnection:close\nContent-Type: text/xml\r\n\r\n");

            parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(std::size_t( msg.tellg() ), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "V(1.1)R(200,OK)K(Connection)H(close)K(Content-Type)H(text/xml)E()");
        }

        void parseUntilEnd(bool client, const std::string& msg)
        {
            Pt::Http::HeaderParser parser(*this, client);
            for(std::size_t n = 0; n < msg.size(); ++n)
            {
                if( parser.parse(msg[n]) )
                    break;
            }
        }

        void SpaceBeforeColon()
        {
            parseUntilEnd(false, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/)V(1.1)K(Host)H(example.com)E()");

            events.str(std::string());
            events.clear();
            parseUntilEnd(false, "GET / HTTP/1.1\r\nHost : example.com\r\n\r\n");
            PT_UNIT_ASSERT(events.str().find("H(") == std::string::npos);

            {
                Pt::Http::HeaderParser parser(*this, false);
                std::istringstream msg("GET / HTTP/1.1\r\nHost : example.com\r\n\r\n");
                parser.advance(msg);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }

            {
                Pt::Http::HeaderParser parser(*this, false);
                std::istringstream msg("GET / HTTP/1.1\r\nHost\t: example.com\r\n\r\n");
                parser.advance(msg);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }

            {
                Pt::Http::HeaderParser parser(*this, true);
                std::istringstream msg("HTTP/1.1 200 OK\r\nHost : example.com\r\n\r\n");
                parser.advance(msg);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }
        }

        void ObsFold()
        {
            {
                Pt::Http::HeaderParser parser(*this, false);
                std::istringstream msg("GET / HTTP/1.1\r\nFoo: line1\r\n line2\r\n\r\n");
                parser.advance(msg);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }

            {
                Pt::Http::HeaderParser parser(*this, false);
                std::istringstream msg("GET / HTTP/1.1\r\n Foo: line1\r\n\r\n");
                parser.advance(msg);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }

            {
                Pt::Http::HeaderParser parser(*this, true);
                std::istringstream msg("HTTP/1.1 200 OK\r\nFoo: line1\r\n line2\r\n\r\n");
                parser.advance(msg);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }

            {
                Pt::Http::HeaderParser parser(*this, true);
                std::istringstream msg("HTTP/1.1 200 OK\r\n Foo: line1\r\n\r\n");
                parser.advance(msg);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }
        }

        void FieldValue()
        {
            parseUntilEnd(false, std::string("GET / HTTP/1.1\r\nX: a b\tc\x21\x7e\x80\r\n\r\n"));
            PT_UNIT_ASSERT_EQUALS(events.str(), std::string("M(GET)U(/)V(1.1)K(X)H(a b\tc\x21\x7e\x80)E()"));

            const char rejected[] = { '\0', '\x01', '\x7f', '\v', '\f' };
            for(std::size_t n = 0; n < sizeof(rejected); ++n)
            {
                Pt::Http::HeaderParser parser(*this, false);
                std::string msg = std::string("GET / HTTP/1.1\r\nX: a") + rejected[n] + "\r\n\r\n";
                std::istringstream in(msg);
                parser.advance(in);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }
        }

        void FieldName()
        {
            parseUntilEnd(false, "GET / HTTP/1.1\r\nX-Token_1.2: v\r\n!: v\r\n\r\n");
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/)V(1.1)K(X-Token_1.2)H(v)K(!)H(v)E()");

            const char rejected[] = "(),/;<=>?@[\\]{}\"";
            for(const char* p = rejected; *p; ++p)
            {
                Pt::Http::HeaderParser parser(*this, false);
                std::string msg = std::string("GET / HTTP/1.1\r\nX") + *p + ": v\r\n\r\n";
                std::istringstream in(msg);
                parser.advance(in);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }
        }

        void EmptyValue()
        {
            parseUntilEnd(false, "GET / HTTP/1.1\r\nOrigin:\r\n\r\n");
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/)V(1.1)K(Origin)H()E()");

            Pt::Http::MessageHeader header;
            Pt::Http::HeaderParser::MessageHeaderEvent ev(header);
            Pt::Http::HeaderParser parser(ev, false);
            std::istringstream msg("GET / HTTP/1.1\r\nContent-Length: 0 \r\n\r\n");
            parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(! parser.fail());
            PT_UNIT_ASSERT(header.isSet("Content-Length", "0"));
            PT_UNIT_ASSERT_EQUALS(std::string(header.get("Content-Length")), std::string("0"));
        }

        void HeaderSize()
        {
            const std::size_t limit = Pt::Http::MessageHeader::MaxHeaderSize;
            const std::string prefix = "GET / HTTP/1.1\r\nX: ";
            const std::string ending = "\r\n\r\n";

            {
                Pt::Http::HeaderParser parser(*this, false);
                const std::size_t valueSize = limit - std::string("X: ").size() - ending.size();
                std::string msg = prefix + std::string(valueSize, 'a') + ending;
                std::istringstream in(msg);
                parser.advance(in);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(! parser.fail());
            }

            {
                Pt::Http::HeaderParser parser(*this, false);
                const std::size_t valueSize = limit - std::string("X: ").size() - ending.size() + 1;
                std::string msg = prefix + std::string(valueSize, 'a') + ending;
                std::istringstream in(msg);
                parser.advance(in);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }

            {
                Pt::Http::HeaderParser parser(*this, false);
                const std::size_t valueSize = limit - std::string("X: ").size() - ending.size() + 1;
                std::string msg = prefix + std::string(valueSize, 'a') + ending;
                std::istringstream first(msg.substr(0, prefix.size() + 8));
                parser.advance(first);
                PT_UNIT_ASSERT(! parser.end());

                std::istringstream second(msg.substr(prefix.size() + 8));
                parser.advance(second);
                PT_UNIT_ASSERT(parser.end());
                PT_UNIT_ASSERT(parser.fail());
            }
        }

    private:

        std::ostringstream events;

        void onMethod(const std::string& method)
        {
            events << "M(" << method << ')';
        }

        void onUrl(const std::string& url)
        {
            events << "U(" << url << ')';
        }

        void onUrlParam(const std::string& q)
        {
            events << "Q(" << q << ')';
        }

        void onHttpVersion(unsigned major, unsigned minor)
        {
            events << "V(" << major << '.' << minor << ')';
        }

        void onKey(const std::string& key)
        {
            events << "K(" << key << ')';
        }

        void onValue(const std::string& value)
        {
            events << "H(" << value << ')';
        }

        void onHttpReturn(unsigned ret, const std::string& text)
        {
            events << "R(" << ret << ',' << text << ')';
        }

        void onEnd()
        {
            events << "E()";
        }

};

Pt::Unit::RegisterTest<ParserTest> register_HttpParserTest;
