// g++ -march=native -O3 bench-pc.cpp -o bench-pc && ./bench-pc && rm -f bench-pc
//
// Results are below the program
//

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <typeinfo>

#define LOOP_COUNT 100000000

static inline double getUTime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (double) tv.tv_sec + ((double) tv.tv_usec * 0.000001);
}

static inline       float xsqrt(      float v) { return sqrtf(v); }
static inline      double xsqrt(     double v) { return sqrt (v); }
static inline long double xsqrt(long double v) { return sqrtl(v); }

template<typename TYPE>
static void benchSHIF(const char* name)
{
    volatile TYPE v = 0;

    // Do not use constants or repeating values to avoid loop unroll optimizations
    // All values > 0 to avoid division by 0
    TYPE v0 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v1 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v2 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v3 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v4 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v5 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v6 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v7 = (TYPE) (rand() % 128) / 16 + 1;

    double t1, td;

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v = v0 << 4;
        v = v2 << 4;
        v = v4 << 4;
        v = v6 << 4;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s shl  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v = v1 >> 4;
        v = v3 >> 4;
        v = v5 >> 4;
        v = v7 >> 4;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s shr  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);


    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v <<= v0;
        v <<= v2;
        v <<= v4;
        v <<= v6;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s shlx = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v >>= v1;
        v >>= v3;
        v >>= v5;
        v >>= v7;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s shrx = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);
}

template<typename TYPE>
static void benchASMD(const char* name)
{
    volatile TYPE v = 0;

    // Do not use constants or repeating values to avoid loop unroll optimizations
    // All values > 0 to avoid division by 0
    TYPE v0 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v1 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v2 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v3 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v4 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v5 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v6 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v7 = (TYPE) (rand() % 128) / 16 + 1;

    double t1, td;

    t1 = getUTime();
    for (size_t i = 0; i < LOOP_COUNT; ++i) {
        v += v0;
        v += v2;
        v += v4;
        v += v6;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s add  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for(size_t i = 0; i < LOOP_COUNT; ++i) {
        v -= v1;
        v -= v3;
        v -= v5;
        v -= v7;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s sub  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for(size_t i = 0; i < LOOP_COUNT; ++i) {
        v *= v0;
        v *= v2;
        v *= v4;
        v *= v6;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s mul  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);

    t1 = getUTime();
    for(size_t i = 0; i < LOOP_COUNT; ++i) {
        v /= v1;
        v /= v3;
        v /= v5;
        v /= v7;
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s div  = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);
}

template<typename TYPE>
static void benchSQRT(const char* name)
{
    volatile TYPE v = 0;

    // Do not use constants or repeating values to avoid loop unroll optimizations
    // All values > 0 to avoid division by 0
    TYPE v0 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v1 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v2 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v3 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v4 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v5 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v6 = (TYPE) (rand() % 128) / 16 + 1;
    TYPE v7 = (TYPE) (rand() % 128) / 16 + 1;

    double t1, td;

    t1 = getUTime();
    for(size_t i = 0; i < LOOP_COUNT; ++i) {
        v = xsqrt(v0);
        v = xsqrt(v2);
        v = xsqrt(v4);
        v = xsqrt(v6);
    }
    td = (getUTime() - t1) / 4.0;
    printf("%s sqrt = %6.3f uS (%6.3f MIPS)\n", name, td, 1.0 / td);
}


int main()
{
    benchSHIF<     int8_t>("     int8_t");
    benchASMD<     int8_t>("     int8_t");
    benchSHIF<    uint8_t>("     int8_t");
    benchASMD<    uint8_t>("    uint8_t");
    printf("\n");

    benchSHIF<    int16_t>("    int16_t");
    benchASMD<    int16_t>("    int16_t");
    benchSHIF<   uint16_t>("   uint16_t");
    benchASMD<   uint16_t>("   uint16_t");
    printf("\n");

    benchSHIF<    int32_t>("    int32_t");
    benchASMD<    int32_t>("    int32_t");
    benchSHIF<   uint32_t>("   uint32_t");
    benchASMD<   uint32_t>("   uint32_t");
    printf("\n");

    benchSHIF<    int64_t>("    int64_t");
    benchASMD<    int64_t>("    int64_t");
    benchSHIF<   uint64_t>("   uint64_t");
    benchASMD<   uint64_t>("   uint64_t");
    printf("\n");

    benchASMD<      float>("      float");
    benchSQRT<      float>("      float");
    printf("\n");

    benchASMD<     double>("     double");
    benchSQRT<     double>("     double");
    printf("\n");

    benchASMD<long double>("long double");
    benchSQRT<long double>("long double");
    printf("\n");

    return 0;
}

/*
    Core i5 64-bit Mode
    -------------------

    RaspberryPi 3 32-bit Mode
    -------------------------
 */
