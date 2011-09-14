/*
 * Copyright (C) 2010 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "Pt/System/Uri.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

class UriTest : public Pt::Unit::TestSuite
{
    public:
        UriTest()
        : Pt::Unit::TestSuite("UriTest")
        {
            registerMethod("testUri_UPHP", *this, &UriTest::testUri_UPHP);
            registerMethod("testUri_UHP", *this, &UriTest::testUri_UHP);
            registerMethod("testUri_UPH", *this, &UriTest::testUri_UPH);
            registerMethod("testUri_HP", *this, &UriTest::testUri_HP);
            registerMethod("testUri_H", *this, &UriTest::testUri_H);
            registerMethod("testUri_UPH6P", *this, &UriTest::testUri_UPH6P);
            registerMethod("testUri_UH6P", *this, &UriTest::testUri_UH6P);
            registerMethod("testUri_UPH6", *this, &UriTest::testUri_UPH6);
            registerMethod("testUri_H6P", *this, &UriTest::testUri_H6P);
            registerMethod("testUri_H6", *this, &UriTest::testUri_H6);
            registerMethod("testQuery", *this, &UriTest::testQuery);
            registerMethod("testFragment", *this, &UriTest::testFragment);
            registerMethod("testQueryFragment", *this, &UriTest::testQueryFragment);
            registerMethod("testHttpPort", *this, &UriTest::testHttpPort);
            registerMethod("testHttpsPort", *this, &UriTest::testHttpsPort);
            registerMethod("testFtpPort", *this, &UriTest::testFtpPort);
            registerMethod("testUriStr", *this, &UriTest::testUriStr);
        }

        void testUri_UPHP()
        {
            Pt::System::Uri uri("http://user:password@host:56/blah.html");

            PT_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            PT_UNIT_ASSERT_EQUALS(uri.user(), "user");
            PT_UNIT_ASSERT_EQUALS(uri.password(), "password");
            PT_UNIT_ASSERT_EQUALS(uri.host(), "host");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 56);
            PT_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_UHP()
        {
            Pt::System::Uri uri("http://user@host:56/blah.html");

            PT_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            PT_UNIT_ASSERT_EQUALS(uri.user(), "user");
            PT_UNIT_ASSERT_EQUALS(uri.password(), "");
            PT_UNIT_ASSERT_EQUALS(uri.host(), "host");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 56);
            PT_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_UPH()
        {
            Pt::System::Uri uri("http://user:password@host/blah.html");

            PT_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            PT_UNIT_ASSERT_EQUALS(uri.user(), "user");
            PT_UNIT_ASSERT_EQUALS(uri.password(), "password");
            PT_UNIT_ASSERT_EQUALS(uri.host(), "host");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 80);
            PT_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_HP()
        {
            Pt::System::Uri uri("http://host:56/blah.html");

            PT_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            PT_UNIT_ASSERT_EQUALS(uri.user(), "");
            PT_UNIT_ASSERT_EQUALS(uri.password(), "");
            PT_UNIT_ASSERT_EQUALS(uri.host(), "host");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 56);
            PT_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_H()
        {
            Pt::System::Uri uri("http://host/blah.html");

            PT_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            PT_UNIT_ASSERT_EQUALS(uri.user(), "");
            PT_UNIT_ASSERT_EQUALS(uri.password(), "");
            PT_UNIT_ASSERT_EQUALS(uri.host(), "host");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 80);
            PT_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_UPH6P()
        {
            Pt::System::Uri uri("http://user:password@[::1]:56/blah.html");

            PT_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            PT_UNIT_ASSERT_EQUALS(uri.user(), "user");
            PT_UNIT_ASSERT_EQUALS(uri.password(), "password");
            PT_UNIT_ASSERT_EQUALS(uri.host(), "::1");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 56);
            PT_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_UH6P()
        {
            Pt::System::Uri uri("http://user@[::1]:56/blah.html");

            PT_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            PT_UNIT_ASSERT_EQUALS(uri.user(), "user");
            PT_UNIT_ASSERT_EQUALS(uri.password(), "");
            PT_UNIT_ASSERT_EQUALS(uri.host(), "::1");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 56);
            PT_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_UPH6()
        {
            Pt::System::Uri uri("http://user:password@[::1]/blah.html");

            PT_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            PT_UNIT_ASSERT_EQUALS(uri.user(), "user");
            PT_UNIT_ASSERT_EQUALS(uri.password(), "password");
            PT_UNIT_ASSERT_EQUALS(uri.host(), "::1");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 80);
            PT_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_H6P()
        {
            Pt::System::Uri uri("http://[::1]:56/blah.html");

            PT_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            PT_UNIT_ASSERT_EQUALS(uri.user(), "");
            PT_UNIT_ASSERT_EQUALS(uri.password(), "");
            PT_UNIT_ASSERT_EQUALS(uri.host(), "::1");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 56);
            PT_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_H6()
        {
            Pt::System::Uri uri("http://[::1]/blah.html");

            PT_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            PT_UNIT_ASSERT_EQUALS(uri.user(), "");
            PT_UNIT_ASSERT_EQUALS(uri.password(), "");
            PT_UNIT_ASSERT_EQUALS(uri.host(), "::1");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 80);
            PT_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testQuery()
        {
            Pt::System::Uri uri("http://host/?abc=1");
            PT_UNIT_ASSERT_EQUALS(uri.query(), "abc=1");
        }

        void testFragment()
        {
            Pt::System::Uri uri("http://host/#foo");
            PT_UNIT_ASSERT_EQUALS(uri.fragment(), "foo");
        }

        void testQueryFragment()
        {
            Pt::System::Uri uri("http://host/?abc=1#foo");
            PT_UNIT_ASSERT_EQUALS(uri.query(), "abc=1");
            PT_UNIT_ASSERT_EQUALS(uri.fragment(), "foo");
        }

        void testHttpPort()
        {
            Pt::System::Uri uri("http://host/");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 80);
        }

        void testHttpsPort()
        {
            Pt::System::Uri uri("https://host/");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 443);
        }

        void testFtpPort()
        {
            Pt::System::Uri uri("ftp://host/");
            PT_UNIT_ASSERT_EQUALS(uri.port(), 21);
        }

        void testUriStr()
        {
            Pt::System::Uri uri("http://user:password@host:80/blah.html");
            PT_UNIT_ASSERT_EQUALS(uri.str(), "http://user:password@host/blah.html");
        }

};

Pt::Unit::RegisterTest<UriTest> register_UriTest;
