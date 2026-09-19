/* Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_UNIT_API_H
#define PT_UNIT_API_H

#include <Pt/Api.h>

#define PT_UNIT_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_UNIT_VERSION_MINOR PT_VERSION_MINOR
#define PT_UNIT_VERSION_REVISION PT_VERSION_REVISION
#define PT_UNIT_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_UNIT_API_EXPORT)
#    define PT_UNIT_API PT_EXPORT
#  else
#    define PT_UNIT_API PT_IMPORT
#  endif

#endif

namespace Pt {

/** @defgroup Pt-Unit Unit Testing
    @brief Protocol and data driven unit testing framework.

    @copydetails Pt::Unit
*/

/** @namespace Pt::Unit
    
    @brief Protocol and data driven unit testing framework.

    The %Pt::Unit module provides a complete framework for effective unit
    testing. Simple tests are implemented by deriving from %TestCase,
    multi-method suites by deriving from %TestSuite. Setup and teardown
    of resources is provided by the %TestFixture interface. Test
    conditions are verified through the %Assertion class and the
    PT_UNIT_ASSERT macros. Execution order and data-driven repetition
    are controlled by a %TestProtocol. Tests are auto-registered with
    %RegisterTest and run by the %Application class, which reports
    results through %Reporter. A ready-made %main() function is
    available by including %TestMain.h.
*/
namespace Unit {

    class Application;
    class Assertion;
    class Reporter;
    class Test;
    class TestCase;
    class TestContext;
    class TestFixture;
    class TestProtocol;
    class TextProtocol;
    class TestSuite;

}

}
