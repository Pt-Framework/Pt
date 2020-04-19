/* Copyright (C) 2020-2020 Aloysius Indrayanto
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

#include <cfloat>

#include <iomanip>
#include <iostream>

#include <Pt/StringStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/System/Clock.h>

#define FMT_HEADER_ONLY
#include "fmt/format.h"
//#include "fmtc11/fmt/format.h"

#include "FormatString.h"


#ifdef PT_WITH_STD_LOCALE
class CustomNumpunct : public std::numpunct<Pt::Char>  {
    public:
        typedef Pt::Char                    char_type;
        typedef std::basic_string<Pt::Char> string_type;

    public:
        CustomNumpunct()
        : _decimal_point(',')
        , _thousands_sep('.')
        , _grouping     ("\3")
        , _truename     ("benar")
        , _falsename    ("salah")
        {}

        virtual ~CustomNumpunct()
        {}

        inline void setDecimalPoint(char_type v)
        { _decimal_point = v; }

        inline void setThousandsSeparator(char_type v)
        { _thousands_sep = v; }

        inline void setGrouping(const std::string& v)
        { _grouping = v; }

        inline void setTruename(const string_type& v)
        { _truename = v; }

        inline void setFalsename(const string_type& v)
        { _falsename = v; }

    protected:
        virtual char_type do_decimal_point() const
        { return _decimal_point; }

        virtual char_type do_thousands_sep() const
        { return _thousands_sep; }

        virtual std::string do_grouping() const
        { return _grouping; }

        virtual string_type do_truename() const
        { return _truename; }

        virtual string_type do_falsename() const
        { return _falsename; }

    private:
        char_type   _decimal_point;
        char_type   _thousands_sep;
        std::string _grouping;
        string_type _truename;
        string_type _falsename;
};
#endif


#ifdef __unix__
#define GRAY    "\u001b[90m"
#define RED     "\u001b[91m"
#define GREEN   "\u001b[92m"
#define YELLOW  "\u001b[93m"
#define BLUE    "\u001b[94m"
#define MAGENTA "\u001b[95m"
#define CYAN    "\u001b[96m"
#define WHITE   "\u001b[97m"
#define RESET   "\u001b[0m"
#else
#define GRAY    ""
#define RED     ""
#define GREEN   ""
#define YELLOW  ""
#define BLUE    ""
#define MAGENTA ""
#define CYAN    ""
#define WHITE   ""
#define RESET   ""
#endif


static bool                             COMPARE_WITH_FMT = true;
static Pt::FormatStringValue::locale_t* customLocale     = 0;

#define LOOP_COUNT (1 * 8000)

#define TEST_AND_BENCHMARK(FMT, ...)                                          \
    do {                                                                      \
        /* Test */                                                            \
        const std::string& fmt =                                              \
            COMPARE_WITH_FMT                                                  \
            ? fmt::format(FMT, __VA_ARGS__)                                   \
            : "";                                                             \
        const Pt::String& ptf =                                               \
            customLocale ? Pt::format(*customLocale, FMT, __VA_ARGS__)        \
                         : Pt::format(FMT, __VA_ARGS__);                      \
        /* Print */                                                           \
        if(COMPARE_WITH_FMT) {                                                \
            const Pt::String& tmp = Pt::String(fmt.c_str());                  \
            std::cerr << Pt::Utf8Codec::encode(tmp) << std::endl;             \
        }                                                                     \
        std::cerr << Pt::Utf8Codec::encode(ptf) << std::endl;                 \
        /* Compare */                                                         \
        if(COMPARE_WITH_FMT) {                                                \
            if(fmt == ptf.narrow())                                           \
                std::cerr << GREEN << "[MATCH] " << RESET;                    \
            else                                                              \
                std::cerr << RED << "[NOT MATCH] " << RESET;                  \
        }                                                                     \
        else {                                                                \
            std::cerr << CYAN << "[NO COMPARE] " << RESET;                    \
        }                                                                     \
        /* Benchmark the reference fmt 4.1.0 library */                       \
        Pt::System::Clock clock;                                              \
        double            bfmt = 0.0;                                         \
        if(COMPARE_WITH_FMT) {                                                \
            clock.start();                                                    \
            for(size_t i = 0; i < LOOP_COUNT; ++i) {                          \
                fmt::format(FMT, __VA_ARGS__);                                \
            }                                                                 \
            bfmt = clock.stop().toUSecs() * 1000.0 / LOOP_COUNT;              \
        }                                                                     \
        /* Benchmark our implementation */                                    \
        double bptf = 0.0;                                                    \
        if(customLocale) {                                                    \
            clock.start();                                                    \
            for(size_t i = 0; i < LOOP_COUNT; ++i) {                          \
                Pt::format(*customLocale, FMT, __VA_ARGS__);                  \
            }                                                                 \
            bptf = clock.stop().toUSecs() * 1000.0 / LOOP_COUNT;              \
        }                                                                     \
        else {                                                                \
            clock.start();                                                    \
            for(size_t i = 0; i < LOOP_COUNT; ++i) {                          \
                Pt::format(FMT, __VA_ARGS__);                                 \
            }                                                                 \
            bptf = clock.stop().toUSecs() * 1000.0 / LOOP_COUNT;              \
        }                                                                     \
        /* Print the benchmark result */                                      \
        double brat = COMPARE_WITH_FMT ? (bptf / bfmt) : 0.0;                 \
        std::cerr << std::fixed << std::setprecision(1);                      \
        std::cerr << std::setw(6) << bfmt << " nS/call - ";                   \
        std::cerr << std::setw(6) << bptf << " nS/call ";                     \
        if(!COMPARE_WITH_FMT) {                                               \
            std::cerr << std::endl << std::endl << std::endl;                 \
            break;                                                            \
        }                                                                     \
        if(brat < 1.0) {                                                      \
            brat = 1.0 / brat;                                                \
            std::cerr << BLUE;                                                \
            std::cerr << "(" << std::setw(3) << brat << "x faster)" << RESET; \
        }                                                                     \
        else {                                                                \
            std::cerr << MAGENTA;                                             \
            std::cerr << "(" << std::setw(3) << brat << "x slower)" << RESET; \
        }                                                                     \
        std::cerr << std::endl << std::endl << std::endl;                     \
   } while(false)


int main(int argc, char* args[])
{
#if 0
    Pt::System::Clock clock;
    const size_t      loopCount = 3 * 1000000;

    Pt::String srcStr = "12345.123047";
    Pt::String dstStr;

    Pt::size_t resN = 0;
    Pt::size_t resO = 0;

    for(size_t i = 0; i < 10; ++i) {
        clock.start();
        for(size_t j = 0; j < loopCount; ++j) {
            finalizePositiveFloatingPointStringFormatN(dstStr, srcStr, ',', '.');
        }
        resN += clock.stop().toUSecs();
        std::cerr << dstStr.narrow() << std::endl;

        clock.start();
        for(size_t j = 0; j < loopCount; ++j) {
            finalizePositiveFloatingPointStringFormatO(dstStr, srcStr, ',', '.');
        }
        resO += clock.stop().toUSecs();
        std::cerr << dstStr.narrow() << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << "N = " << resN << std::endl;
    std::cerr << "O = " << resO << std::endl;
    std::cerr << std::endl;

    return 0;
#endif

    /*
        REFERENCE

        1.81C9P+13
        1.2345E+04
        12345.1235
        1.235E+04

        1.FFFFFE00000000000000P+127
        3.40282346638528859812E+38
        3.4028234663852885981E+38

        1.FFFFFFFFFFFFF0000000P+1023
        1.79769313486231570815E+308
        1.7976931348623157081E+308

        F.FFFFFFFFFFFFFFF00000P+16380
        1.18973149535723176502E+4932
        1.189731495357231765E+4932

        1.000099999999999989
        1000100
        1E+06

        N\A
        N\A
        N\A

        -------------------------

        1.81C9P+13
        1.2345E+04
        12345.1235
        1.235E+04

        1.FFFFFE00000000000000P+127
        3.40282346638528859812E+38
        3.4028234663852885981E+38

        1.FFFFFFFFFFFFF0000000P+1023
        1.79769313486231570815E+308
        1.7976931348623157081E+308

        1.FFFFFFFFFFFFFFFE0000P+16383
        1.18973149535723176502E+4932
        1.189731495357231765E+4932

        1.000099999999999989
        1000100
        1E+06

        1.000099999999999989
        1.
        1.E+06
    */
#if 1
#define FPFP(F, ...) Pt::FormatStringValue::printPositiveFloatingPoint(F, __VA_ARGS__)
    // Test the in-house positive floating-point formatter
    Pt::String s;
    std::cerr << std::endl;
    FPFP(s, 12345.123456789, 4, false, 'A'/* %0.4a */); std::cerr << s.narrow() << std::endl;
    FPFP(s, 12345.123456789, 4, false, 'E'/* %0.4e */); std::cerr << s.narrow() << std::endl;
    FPFP(s, 12345.123456789, 4, false, 'F'/* %0.4f */); std::cerr << s.narrow() << std::endl;
    FPFP(s, 12345.123456789, 4, false, 'G'/* %0.4g */); std::cerr << s.narrow() << std::endl;
    std::cerr << std::endl;
    FPFP(s, FLT_MAX /*3.40282347e+38f*/, 20, false, 'A'/* %0.4a */); std::cerr << s.narrow() << std::endl;
    FPFP(s, FLT_MAX /*3.40282347e+38f*/, 20, false, 'E'/* %0.4e */); std::cerr << s.narrow() << std::endl;
    FPFP(s, FLT_MAX /*3.40282347e+38f*/, 20, false, 'G'/* %0.4g */); std::cerr << s.narrow() << std::endl;
    std::cerr << std::endl;
    FPFP(s, DBL_MAX /*1.7976931348623157e308*/, 20, false, 'A'/* %0.4a */); std::cerr << s.narrow() << std::endl;
    FPFP(s, DBL_MAX /*1.7976931348623157e308*/, 20, false, 'E'/* %0.4e */); std::cerr << s.narrow() << std::endl;
    FPFP(s, DBL_MAX /*1.7976931348623157e308*/, 20, false, 'G'/* %0.4g */); std::cerr << s.narrow() << std::endl;
    std::cerr << std::endl;
    FPFP(s, LDBL_MAX /*1.18973149535723176502e+4932L*/, 20, false, 'A'/* %0.4a */); std::cerr << s.narrow() << std::endl;
    FPFP(s, LDBL_MAX /*1.18973149535723176502e+4932L*/, 20, false, 'E'/* %0.4e */); std::cerr << s.narrow() << std::endl;
    FPFP(s, LDBL_MAX /*1.18973149535723176502e+4932L*/, 20, false, 'G'/* %0.4g */); std::cerr << s.narrow() << std::endl;
    std::cerr << std::endl;
    FPFP(s, 1.00010000, 20, false, 'G'); std::cerr << s.narrow() << std::endl;
    FPFP(s, 1.00010000 * 1000000, 20, false, 'G'); std::cerr << s.narrow() << std::endl;
    FPFP(s, 1.00010000 * 1000000, 0, false, 'G'); std::cerr << s.narrow() << std::endl;
    std::cerr << std::endl;
    FPFP(s, 1.00010000, 20, true, 'G'); std::cerr << s.narrow() << std::endl;
    FPFP(s, 1.00010000, 0, true, 'G'); std::cerr << s.narrow() << std::endl;
    FPFP(s, 1.00010000 * 1000000, 0, true, 'G'); std::cerr << s.narrow() << std::endl;
    std::cerr << std::endl << std::endl;
#else
    // Generate reference results
    printf("\n");
    printf("%.4lA\n", 12345.123456789);
    printf("%.4lE\n", 12345.123456789);
    printf("%.4lF\n", 12345.123456789);
    printf("%.4lG\n", 12345.123456789);
    printf("\n");
    printf("%.20A\n", FLT_MAX /*3.40282347e+38f*/);
    printf("%.20E\n", FLT_MAX /*3.40282347e+38f*/);
    printf("%.20G\n", FLT_MAX /*3.40282347e+38f*/);
    printf("\n");
    printf("%.20lA\n", DBL_MAX /*1.7976931348623157e308*/);
    printf("%.20lE\n", DBL_MAX /*1.7976931348623157e308*/);
    printf("%.20lG\n", DBL_MAX /*1.7976931348623157e308*/);
    printf("\n");
    printf("%.20LA\n", LDBL_MAX /*1.18973149535723176502e+4932L*/);
    printf("%.20LE\n", LDBL_MAX /*1.18973149535723176502e+4932L*/);
    printf("%.20LG\n", LDBL_MAX /*1.18973149535723176502e+4932L*/);
    printf("\n");
    printf("%.20G\n", 1.00010000);
    printf("%.20G\n", 1.00010000 * 1000000);
    printf("%.0G\n", 1.00010000 * 1000000);
    printf("\n");
    printf("N\\A\n");
    printf("N\\A\n");
    printf("N\\A\n");
    printf("\n\n");
#endif
    //return 0;

#ifdef PT_WITH_STD_LOCALE
    // Create a customized locale
    CustomNumpunct* customNumPunct = new CustomNumpunct();
    std::locale clNumpunct = std::locale( std::locale(), customNumPunct );
#endif

    // Format-string only
    TEST_AND_BENCHMARK("   ", 0);
    TEST_AND_BENCHMARK("{{}}", 0);
    TEST_AND_BENCHMARK("{{}} {{}} {{}}", 0);
    TEST_AND_BENCHMARK("{}", "aBc");
    TEST_AND_BENCHMARK("{0} {0} {0}", "aBc");

    //return 0;

    // Strings
    TEST_AND_BENCHMARK("{}", "aBc");
    TEST_AND_BENCHMARK("{0} {0}", "aBc");

    TEST_AND_BENCHMARK("|{}| |{:s}|", "aBc", "dEf");
    TEST_AND_BENCHMARK("|{0:8}| |{0:*<8}| |{0:*>8}| |{0:*^8}| |{0:^8}|", "aBc");

    //return 0;

    // Characters (xA9 == 169 == ©)
    TEST_AND_BENCHMARK("|{}| |{}| |{}|", 'A', 'b', '\xA9');
    TEST_AND_BENCHMARK("|{:*<8}| |{:*>8}| |{:*^8}|", 'A', 'b', '\xA9');

    TEST_AND_BENCHMARK("|{:d}| |{:d}| |{:d}|", 'A', 'b', '\xA9');
    TEST_AND_BENCHMARK("|{:*<8d}| |{:*>8d}| |{:*^8d}|", 'A', 'b', '\xA9');

    //return 0;

    // Pointers
    int dummy = 0;
    TEST_AND_BENCHMARK("|{0:p}| |{0:24p}| |{0:*^24p}|", (void*) &dummy);

    //return 0;

    // Booleans
    TEST_AND_BENCHMARK("|{:08}| |{:08}|", true, false);
    TEST_AND_BENCHMARK("|{:08d}| |{:08d}|", true, false);

    COMPARE_WITH_FMT = false;
    TEST_AND_BENCHMARK("|{:<8L}| |{:>8L}|", true, false);
    COMPARE_WITH_FMT = true;

    TEST_AND_BENCHMARK("|{0:*<8}| |{0:*>8}| |{0:*^8}| |{1:*<8}| |{1:*>8}| |{1:*^8}|", true, false);
    TEST_AND_BENCHMARK("|{0:*<08}| |{0:*>08}| |{0:*^08}| |{1:*<08}| |{1:*>08}| |{1:*^08}|", true, false);

    TEST_AND_BENCHMARK("|{0:*<8d}| |{0:*>8d}| |{0:*^8d}| |{1:*<8d}| |{1:*>8d}| |{1:*^8d}|", true, false);
    TEST_AND_BENCHMARK("|{0:*<08d}| |{0:*>08d}| |{0:*^08d}| |{1:*<08d}| |{1:*>08d}| |{1:*^08d}|", true, false);

    //return 0;

    // Floating-points
    TEST_AND_BENCHMARK("|{0}| |{0:18f}| |{0:.4f}| |{0:18.4f}| |{0:018.4f}| |{0:*^18.4f}| |{0:*^018.4f}|", 12345.123456789f);
    TEST_AND_BENCHMARK("|{0}| |{0:18f}| |{0:.4f}| |{0:18.4f}| |{0:018.4f}| |{0:*^18.4f}| |{0:*^018.4f}|", -12345.123456789f);
    TEST_AND_BENCHMARK("|{0:.4a}| |{0:.4e}| |{0:.4g}|", 12345.123456789f);
    TEST_AND_BENCHMARK("|{0:.4A}| |{0:.4E}| |{0:.4G}|", 12345.123456789f);

    COMPARE_WITH_FMT = false;
    TEST_AND_BENCHMARK("|{0:+Lf}| |{0:+18Lf}| |{0:+.4Lf}| |{0:+18.4Lf}| |{0:+018.4Lf}| |{0:*^+18.4Lf}| |{0:*^+018.4Lf}|", 12345.123456789f);
    TEST_AND_BENCHMARK("|{0:+Lf}| |{0:+18Lf}| |{0:+.4Lf}| |{0:+18.4Lf}| |{0:+018.4Lf}| |{0:*^+18.4Lf}| |{0:*^+018.4Lf}|", -12345.123456789f);
    COMPARE_WITH_FMT = true;

    //return 0;

    // Integers
    TEST_AND_BENCHMARK("|{0:8n}| |{1:8n}| |{0:08n}| |{1:08n}| |{2:8n}| |{3:8n}| |{2:08n}| |{3:08n}|", 123, 1234, -567, -5678);
    TEST_AND_BENCHMARK("|{0:*<8n}| |{1:*<8n}| |{0:*^8n}| |{1:*^8n}| |{2:*<8n}| |{3:*<8n}| |{2:*^8n}| |{3:*^8n}|", 123, 1234, -567, -5678);

    TEST_AND_BENCHMARK("|{0:8x}| |{1:8x}| |{0:08x}| |{1:08x}| |{2:8x}| |{3:8x}| |{2:08x}| |{3:08x}|", 123, 1234, -567, -5678);
    TEST_AND_BENCHMARK("|{0:*<8x}| |{1:*<8x}| |{0:*^8x}| |{1:*^8x}| |{2:*<8x}| |{3:*<8x}| |{2:*^8x}| |{3:*^8x}|", 123, 1234, -567, -5678);

    TEST_AND_BENCHMARK("|{0:#8x}| |{1:#8x}| |{0:#08x}| |{1:#08x}| |{2:#8x}| |{3:#8x}| |{2:#08x}| |{3:#08x}|", 123, 1234, -567, -5678);
    TEST_AND_BENCHMARK("|{0:*<#8x}| |{1:*<#8x}| |{0:*^#8x}| |{1:*^#8x}| |{2:*<#8x}| |{3:*<#8x}| |{2:*^#8x}| |{3:*^#8x}|", 123, 1234, -567, -5678);

    TEST_AND_BENCHMARK("|{:+n}| |{:+n}| |{:n}|", (Pt::int64_t) LLONG_MAX /*9223372036854775807LL*/, (Pt::int64_t) LLONG_MIN /*-9223372036854775807LL - 1*/, (Pt::uint64_t) ULLONG_MAX /*18406740073009501615ULL*/);

    TEST_AND_BENCHMARK("|{:b}|", (Pt::uint64_t) 18406740073009501610ULL);
    TEST_AND_BENCHMARK("|{:o}|", (Pt::uint64_t) 18406740073009501610ULL);
    TEST_AND_BENCHMARK("|{:d}|", (Pt::uint64_t) 18406740073009501610ULL);
    TEST_AND_BENCHMARK("|{:x}|", (Pt::uint64_t) 18406740073009501610ULL);

    //return 0;

    // Mixeds
    TEST_AND_BENCHMARK("{} [{}] - {:d} mS ({:.1f}x) [{:d} loops]", "TEST", "FLAG", 1000, 3.5f, 250);

    //return 0;

    // Custom numpunct
#ifdef PT_WITH_STD_LOCALE
    COMPARE_WITH_FMT = false;

    std::locale oldLocale = std::locale::global(clNumpunct);
    TEST_AND_BENCHMARK("|{:*^8L}| |{:*^8L}| |{:*^20.5Lf}|", true, false, 123456789.123456789);

    std::locale::global(oldLocale);
    TEST_AND_BENCHMARK("|{:*^8L}| |{:*^8L}| |{:*^20.5Lf}|", true, false, 123456789.123456789);

    customLocale = &clNumpunct;
    TEST_AND_BENCHMARK("|{:*^8L}| |{:*^8L}| |{:*^20.5Lf}|", true, false, 123456789.123456789);

    customNumPunct->setTruename ( Pt::Utf8Codec::decode( std::string("真") ) );
    customNumPunct->setFalsename( Pt::Utf8Codec::decode( std::string("偽") ) );
    TEST_AND_BENCHMARK("|{:*^8L}| |{:*^8L}| |{:*^20.5Lf}|", true, false, 123456789.123456789);

    //return 0;

    customNumPunct->setThousandsSeparator(':');
    customNumPunct->setGrouping("\x08");
    TEST_AND_BENCHMARK("|{0:b}| |{0:Lb}|", 0b10101010101010101010101010101010);

    customNumPunct->setGrouping("\3");
    TEST_AND_BENCHMARK("|{0:o}| |{0:Lo}|", 0b10101010101010101010101010101010);

    customNumPunct->setGrouping("\2");
    TEST_AND_BENCHMARK("|{0:x}| |{0:Lx}|", 0b10101010101010101010101010101010);

    customNumPunct->setGrouping("\1\2\3\4\5");
    TEST_AND_BENCHMARK("|{0:b}| |{0:Lb}|", 0b10101010101010101010101010101010);
    customNumPunct->setGrouping("\1\2\3");
    TEST_AND_BENCHMARK("|{0:o}| |{0:Lo}|", 0b10101010101010101010101010101010);
    TEST_AND_BENCHMARK("|{0:d}| |{0:Ld}|", 0b10101010101010101010101010101010);
    TEST_AND_BENCHMARK("|{0:x}| |{0:Lx}|", 0b10101010101010101010101010101010);

    customLocale = 0;

    COMPARE_WITH_FMT = true;

    //return 0;
#endif

    // Output stream
    Pt::OStringStream oss;

    format_to(oss, "Test Pt::OStringStream\n");
    std::cerr << Pt::Utf8Codec::encode(oss.str());

    format_to(oss, "Test Pt::OStringStream : {} {}\n", "aBc", 123.123);
    std::cerr << Pt::Utf8Codec::encode(oss.str());

    std::cerr << std::endl;

    // Done
    return 0;
}
