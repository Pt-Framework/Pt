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
#include "Pt/Net/HttpParser.h"
#include <string>

class HttpParserTest : public Pt::Unit::TestSuite, private Pt::Net::HttpHeaderParser::Event
{
    public:
        HttpParserTest()
        : Pt::Unit::TestSuite("HttpParserTest")
        {
            registerMethod( "SimpleRequest", *this, &HttpParserTest::SimpleRequest);
            registerMethod( "RequestWithHeader", *this, &HttpParserTest::RequestWithHeader);
            registerMethod( "RequestWithMultilineHeader", *this, &HttpParserTest::RequestWithMultilineHeader);
            registerMethod( "RequestWithWhitespace", *this, &HttpParserTest::RequestWithWhitespace);
            registerMethod( "RequestWithQueryParam", *this, &HttpParserTest::RequestWithQueryParam);
            registerMethod( "Response", *this, &HttpParserTest::Response);
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
            Pt::Net::HttpHeaderParser parser(*this, false);

            std::istringstream msg("GET /foo HTTP/1.0\r\n\r\n");

            bool end = parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(end);
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(msg.tellg(), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/foo)V(1.0)E()");
        }

        void RequestWithHeader()
        {
            Pt::Net::HttpHeaderParser parser(*this, false);

            std::istringstream msg("GET /foo HTTP/1.0\r\nUser-Agent: Pt-Unit\nContent-Size:47\r\n\r\n");

            bool end = parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(end);
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(msg.tellg(), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/foo)V(1.0)K(User-Agent)H(Pt-Unit)K(Content-Size)H(47)E()");
        }

        void RequestWithMultilineHeader()
        {
            Pt::Net::HttpHeaderParser parser(*this, false);

            std::istringstream msg("GET /foo HTTP/1.0\r\nContent-Size:47\nFoo:line1\r\n line2\n line3\nBar:bar\r\n\r\n");

            bool end = parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(end);
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(msg.tellg(), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/foo)V(1.0)K(Content-Size)H(47)K(Foo)H(line1 line2 line3)K(Bar)H(bar)E()");
        }

        void RequestWithWhitespace()
        {
            Pt::Net::HttpHeaderParser parser(*this, false);

            std::istringstream msg("PUT   /foo   HTTP  /  1  .  0  \r\n  Content-Size  :  47  \r\n\r\n");

            bool end = parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(end);
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(msg.tellg(), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(PUT)U(/foo)V(1.0)K(Content-Size)H(47  )E()");
        }

        void RequestWithQueryParam()
        {
            Pt::Net::HttpHeaderParser parser(*this, false);

            std::istringstream msg("GET /foo+bar?a=4&b=Hello+World HTTP/1.1\r\n\r\n");

            bool end = parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(end);
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(msg.tellg(), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "M(GET)U(/foo bar)Q(a=4&b=Hello+World)V(1.1)E()");
        }

        void Response()
        {
            Pt::Net::HttpHeaderParser parser(*this, true);

            std::istringstream msg("HTTP/1.1 200 OK\r\nConnection:close\nContent-Type : text/xml\r\n\r\n");

            bool end = parser.advance(msg);

            PT_UNIT_ASSERT(parser.end());
            PT_UNIT_ASSERT(end);
            PT_UNIT_ASSERT(!parser.fail());
            PT_UNIT_ASSERT_EQUALS(msg.tellg(), msg.str().size());
            PT_UNIT_ASSERT_EQUALS(events.str(), "V(1.1)R(200,OK)K(Connection)H(close)K(Content-Type)H(text/xml)E()");
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

Pt::Unit::RegisterTest<HttpParserTest> register_HttpParserTest;
