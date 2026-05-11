/* Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
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
