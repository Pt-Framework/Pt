/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_API_CONVERT_H
#define PT_API_CONVERT_H

/** @addtogroup Pt-Convert

    @brief Conversion between strings, numbers and byte orders.

    The framework includes functions for fast conversion between strings and
    numbers. The overloaded functions Pt::parseInt() and Pt::formatInt()
    convert between strings and integers, and Pt::parseFloat() and
    Pt::formatFloat() convert between strings and floats. The functions work
    with iterators as input or output instead of string objects, so they can
    be used with simple buffers or even streams, as shown in the following
    example:

    @code
    #include <Pt/Convert.h>
    #include <iterator>
    #include <iostream>

    std::ostream_iterator<char> it(std::cout);
    Pt::formatInt(it, 42);

    const char* buf = "42";
    const char* bufend = buf + 2;
    int n = 0;
    Pt::parseInt(buf, bufend, n);
    @endcode

    A stream iterator is used to format a number directly to std::cout and
    then a number is parsed from a raw character buffer. Floating point
    numbers can be formatted and parsed in a similar way like integers:

    @code
    #include <Pt/Convert.h>
    #include <iterator>
    #include <iostream>

    std::ostream_iterator<char> it(std::cout);
    Pt::formatFloat(it, 42.123);

    const char* buf = "42.123";
    const char* bufend = buf + 6;
    float f = 0;
    Pt::parseFloat(buf, bufend, f);
    @endcode

    By default, decimal format is used when numbers are parsed and formatted,
    but overloads exist that accept an additional format object. A few format
    objects are already provided by the framework, named Pt::DecimalFormat,
    Pt::OctalFormat, Pt::HexFormat and Pt::BinaryFormat, which allow numeric
    conversion in a different base. The next example shows how integers in
    hex format can be parsed and formatted:

    @code
    #include <Pt/Convert.h>
    #include <iterator>
    #include <iostream>

    Pt::HexFormat<char> fmt;

    std::ostream_iterator<char> it(std::cout);
    Pt::formatInt(it, 0x42, fmt);

    const char* buf = "0x42";
    const char* bufend = buf + 4;
    int n = 0;
    Pt::parseInt(buf, bufend, n, fmt);
    @endcode

    All parse functions used so far throw an exception of type
    Pt::ConversionError, if the conversion failed. Overloads of the parse
    functions are available, which set a bool flag instead, to indicate a
    conversion error. Both, the format and parse functions return an
    iterator pointing to the position after the last character that was
    written or read, respectively. Partial consumption of the input is not
    treated as an error.

    Numeric assignments can lead to loss of data, if the operation narrows
    the data type to a smaller one. For example, numeric conversion from int
    to short can be an error, if the assigned value exceeds the maximum or
    minimum value possible for shorts. Pt::narrow() can be used instead of a
    normal assignment, to protect against this. In case of an error, an
    exception of type Pt::ConversionError is thrown.

    @code
    #include <Pt/Convert.h>
    #include <limits>
    #include <iostream>

    long l = ...;
    short s = 0;

    try
    {
        s = Pt::narrow<short>(l);
    }
    catch(const Pt::ConversionError& e)
    {
        std::cerr << "numeric value is out of range: " << l << std::endl;
    }
    @endcode

    The byte order conversion API consists of two sets of functions.
    Pt::swab() swaps the byte order of a type by bytewise copying, and is
    overloaded for all fixed-size integer types such as Pt::uint32_t:

    @code
    #include <Pt/Byteorder.h>

    Pt::uint32_t value = ...;
    Pt::uint32_t swapped = Pt::swab(value);
    @endcode

    A second set of functions can be used to convert from a specific
    external byte order to the native host byte order: Pt::beToHost(),
    Pt::hostToBe(), Pt::leToHost() and Pt::hostToLe(). For example
    Pt::beToHost() converts from big-endian to the host byte order:

    @code
    #include <Pt/Byteorder.h>

    Pt::uint32_t beVal = ...;
    Pt::uint32_t value = Pt::beToHost(beVal);
    @endcode
*/

#endif
