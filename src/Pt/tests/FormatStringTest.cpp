#include <iomanip>
#include <iostream>

#include <Pt/System/Clock.h>

#include "fmt/format.h"

#include "FormatString.h"


#define COMPARE_WITH_FMT 1

#if !!COMPARE_WITH_FMT
#define FMT_FORMAT(...) fmt::format(__VA_ARGS__)
#else
#define FMT_FORMAT(...) ""
#endif


#define LOOP_COUNT 15000

#define TEST_AND_BENCHMARK(FORMAT, ...)                                          \
    do {                                                                         \
        /* Test and print */                                                     \
        const std::string& r = FMT_FORMAT(FORMAT, __VA_ARGS__);                  \
        if(COMPARE_WITH_FMT) std::cerr << r << std::endl;                        \
        const std::string& c = Pt::format_string(FORMAT, __VA_ARGS__).narrow();  \
        std::cerr << c << std::endl;                                             \
        /* Compare */                                                            \
        std::cerr << std::endl;                                                  \
        if(COMPARE_WITH_FMT) {                                                   \
            if(r == c) {                                                         \
                std::cerr << ">>> [MATCH] ";                                     \
            }                                                                    \
            else {                                                               \
                std::cerr << "[!!! NOT MATCH !!!] " << std::endl << std::endl;   \
                return -1;                                                       \
            }                                                                    \
        }                                                                        \
        else {                                                                   \
            std::cerr << ">>> [NO COMPARE] ";                                    \
        }                                                                        \
        /* Benchmark the reference fmt 4.1.0 library */                          \
        Pt::System::Clock clock;                                                 \
        clock.start();                                                           \
        for(int i = 0; i < LOOP_COUNT; ++i) {                                    \
            fmt::format(FORMAT, __VA_ARGS__);                                    \
        }                                                                        \
        const double br = clock.stop().toUSecs() * 1000.0 / LOOP_COUNT;          \
        /* Benchmark our implementation */                                       \
        clock.start();                                                           \
        for(int i = 0; i < LOOP_COUNT; ++i) {                                    \
            Pt::format_string(FORMAT, __VA_ARGS__);                              \
        }                                                                        \
        const double bc = clock.stop().toUSecs() * 1000.0 / LOOP_COUNT;          \
        /* Print the benchmark result */                                         \
        std::cerr << std::fixed << std::setprecision(1);                         \
        std::cerr << std::setw(6) << br << " nS/call - ";                        \
        std::cerr << std::setw(6) << bc << " nS/call (";                         \
        std::cerr << std::setw(3) << (bc / br) << "x slower)";                   \
        std::cerr << std::endl << std::endl << std::endl;                        \
   } while(false)


// svn commit -m 'Implementing a simple string formatter ala std::format that uses Pt::String'


int main(int argc, char* args[])
{
    // String
    TEST_AND_BENCHMARK("{{ }}", 0);

    TEST_AND_BENCHMARK("{}", "aBc");

    TEST_AND_BENCHMARK("{0} {0}", "aBc");

    TEST_AND_BENCHMARK("|{}| |{:s}|", "aBc", "dEf");

    TEST_AND_BENCHMARK("|{0:8}| |{0:*<8}| |{0:*>8}| |{0:*^8}| |{0:^8}|", "aBc");

    return 0;

    /*
    std::cerr << fmt::format(
                     "{} {:d} {:.1f} {:p} {}\n",
                     "Test", 123, 456.789, (void*) 1234567890, true
                 );

    std::cerr << Pt::format_string(
                     "{} {:d} {:.1f} {:p} {}\n",
                     "Test", 123, 456.789, (void*) 1234567890, true
                 ).narrow();
    */
}
