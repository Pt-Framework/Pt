#include <iostream>

#include <Pt/System/Clock.h>
#include <Pt/fmt/format.h>

#include "FormatString.h"


#define LOOP_COUNT 1000

#define TEST_AND_BENCHMARK(FORMAT, ...)                                          \
    do {                                                                         \
        /* Test and print */                                                     \
        const std::string& r = fmt::format       (FORMAT, __VA_ARGS__);          \
        std::cerr << r << std::endl;                                             \
        const std::string& c =  Pt::format_string(FORMAT, __VA_ARGS__).narrow(); \
        std::cerr << c << std::endl;                                             \
        /* Compare */                                                            \
        if(r == c) {                                                             \
            std::cerr << "MATCH : ";                                             \
        }                                                                        \
        else {                                                                   \
            std::cerr << "!!! NOT MATCH !!!" << std::endl << std::endl;          \
            return -1;                                                           \
        }                                                                        \
        /* Benchmark the reference fmt 4.1.0 library */                          \
        Pt::System::Clock clock;                                                 \
        clock.start();                                                           \
        for(int i = 0; i < LOOP_COUNT; ++i) {                                    \
            fmt::format(FORMAT, __VA_ARGS__);                                    \
        }                                                                        \
        const size_t br = clock.stop().toUSecs();                                \
        /* Benchmark our implementation */                                       \
        clock.start();                                                           \
        for(int i = 0; i < LOOP_COUNT; ++i) {                                    \
            Pt::format_string(FORMAT, __VA_ARGS__);                              \
        }                                                                        \
        const size_t bc = clock.stop().toUSecs();                                \
        /* Print the benchmark result */                                         \
        std::cerr << br << " uS - " << bc << " uS (";                            \
        std::cerr << ((float) bc / (float) br) << ")" << std::endl << std::endl; \
    } while(false)


// svn commit -m 'Implementing a simple string formatter ala std::format that uses Pt::String'


int main(int argc, char* args[])
{
    // String
    TEST_AND_BENCHMARK("{{ }}", 0);

    TEST_AND_BENCHMARK("{}", "aBc");

    TEST_AND_BENCHMARK("{0} {0}", "aBc");

//    TEST_AND_BENCHMARK("|{}| |{:s}|", "aBc", "aBc");

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
