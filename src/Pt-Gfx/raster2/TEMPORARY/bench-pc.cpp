// clear && g++ -O2 -fno-loop-optimize -fno-rerun-loop-opt bench-pc.cpp -o bench-pc && ./bench-pc && rm -f bench-pc
//
// Results are below the program (MOPS = million operations per second)
//

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <typeinfo>

#define LOOP_COUNT 100000000

static inline double getTime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (double) tv.tv_sec + ((double) tv.tv_usec * 0.000001);
}

static inline       float xsqrt(      float v) { return sqrtf(v); }
static inline      double xsqrt(     double v) { return sqrt (v); }
static inline long double xsqrt(long double v) { return sqrtl(v); }

template<typename TYPE>
static void benchSALR(const char* name)
{
    volatile TYPE v = 0;

    // Do not use constants or repeating values to avoid loop unroll optimizations
    // All values > 0 to avoid division by 0
    volatile TYPE v0 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v1 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v2 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v3 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v4 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v5 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v6 = (TYPE) (rand() % 128) / 16 + 1;
    volatile TYPE v7 = (TYPE) (rand() % 128) / 16 + 1;

    volatile size_t i;
    volatile double t1, td;

    // Loop and assignment overhead
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v0;
        v = v2;
        v = v4;
        v = v6;
    }
    td = (getTime() - t1);

    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v1;
        v = v3;
        v = v5;
        v = v7;
    }
    td += (getTime() - t1);

    td /= (LOOP_COUNT * 8.0);
    printf("%s LOOP = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);

    // Shift-arithmetic-left by a constant
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v0 << 4;
        v = v2 << 4;
        v = v4 << 4;
        v = v6 << 4;
    }
    td = (getTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s sal  = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);

    // Shift-arithmetic-right by a constant
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v1 >> 4;
        v = v3 >> 4;
        v = v5 >> 4;
        v = v7 >> 4;
    }
    td = (getTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s sar  = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);

    // Shift-arithmetic-left by a variable
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v <<= v0;
        v <<= v2;
        v <<= v4;
        v <<= v6;
    }
    td = (getTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s salx = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);

    // Shift-arithmetic-right by a variable
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v >>= v1;
        v >>= v3;
        v >>= v5;
        v >>= v7;
    }
    td = (getTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s sarx = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);
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

    volatile size_t i;
    volatile double t1, td;

    // Loop and assignment overhead
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v0;
        v = v2;
        v = v4;
        v = v6;
    }
    td = (getTime() - t1);

    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v1;
        v = v3;
        v = v5;
        v = v7;
    }
    td += (getTime() - t1);

    td /= (LOOP_COUNT * 8.0);
    printf("%s LOOP = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);

    // Addition
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v += v0;
        v += v2;
        v += v4;
        v += v6;
    }
    td = (getTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s add  = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);

    // Subtraction
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v -= v1;
        v -= v3;
        v -= v5;
        v -= v7;
    }
    td = (getTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s sub  = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);

    // Multiplication
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v *= v0;
        v *= v2;
        v *= v4;
        v *= v6;
    }
    td = (getTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s mul  = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);

    // Division
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v /= v1;
        v /= v3;
        v /= v5;
        v /= v7;
    }
    td = (getTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s div  = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);
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

    volatile size_t i;
    volatile double t1, td;

    // Loop and assignment overhead
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v0;
        v = v2;
        v = v4;
        v = v6;
    }
    td = (getTime() - t1);

    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = v1;
        v = v3;
        v = v5;
        v = v7;
    }
    td += (getTime() - t1);

    td /= (LOOP_COUNT * 8.0);
    printf("%s LOOP = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);

    // Square-root
    t1 = getTime();
    for(i = 0; i < LOOP_COUNT; ++i) {
        v = xsqrt(v0);
        v = xsqrt(v2);
        v = xsqrt(v4);
        v = xsqrt(v6);
    }
    td = (getTime() - t1) / LOOP_COUNT / 4.0;
    printf("%s sqrt = %8.6f uS (%8.3f MOPS)\n", name, td * 1000000, 1.0 / td / 1000000.0);
}

int main()
{
    printf("sizeof(      float) = %2zd\n", sizeof(      float));
    printf("sizeof(     double) = %2zd\n", sizeof(     double));
    printf("sizeof(long double) = %2zd\n", sizeof(long double));
    printf("\n");

    benchSALR<     int8_t>("     int8_t");
    benchASMD<     int8_t>("     int8_t");
    benchSALR<    uint8_t>("    uint8_t");
    benchASMD<    uint8_t>("    uint8_t");
    printf("\n");

    benchSALR<    int16_t>("    int16_t");
    benchASMD<    int16_t>("    int16_t");
    benchSALR<   uint16_t>("   uint16_t");
    benchASMD<   uint16_t>("   uint16_t");
    printf("\n");

    benchSALR<    int32_t>("    int32_t");
    benchASMD<    int32_t>("    int32_t");
    benchSALR<   uint32_t>("   uint32_t");
    benchASMD<   uint32_t>("   uint32_t");
    printf("\n");

    benchSALR<    int64_t>("    int64_t");
    benchASMD<    int64_t>("    int64_t");
    benchSALR<   uint64_t>("   uint64_t");
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
    ---------------------
    Core i5 - 64-bit Mode
    ---------------------
    sizeof(      float) =  4
    sizeof(     double) =  8
    sizeof(long double) = 16

         int8_t LOOP = 0.000443 uS (2255.364 MOPS)
         int8_t sal  = 0.000443 uS (2256.203 MOPS)
         int8_t sar  = 0.000443 uS (2255.633 MOPS)
         int8_t salx = 0.002076 uS ( 481.784 MOPS)
         int8_t sarx = 0.002077 uS ( 481.486 MOPS)
         int8_t LOOP = 0.000443 uS (2256.667 MOPS)
         int8_t add  = 0.002120 uS ( 471.600 MOPS)
         int8_t sub  = 0.002121 uS ( 471.507 MOPS)
         int8_t mul  = 0.002362 uS ( 423.289 MOPS)
         int8_t div  = 0.008036 uS ( 124.435 MOPS)
        uint8_t LOOP = 0.000443 uS (2255.516 MOPS)
        uint8_t sal  = 0.000444 uS (2254.396 MOPS)
        uint8_t sar  = 0.000444 uS (2252.861 MOPS)
        uint8_t salx = 0.002075 uS ( 481.964 MOPS)
        uint8_t sarx = 0.002080 uS ( 480.853 MOPS)
        uint8_t LOOP = 0.000443 uS (2256.915 MOPS)
        uint8_t add  = 0.002120 uS ( 471.636 MOPS)
        uint8_t sub  = 0.002119 uS ( 471.906 MOPS)
        uint8_t mul  = 0.002363 uS ( 423.125 MOPS)
        uint8_t div  = 0.007403 uS ( 135.073 MOPS)

        int16_t LOOP = 0.000443 uS (2256.947 MOPS)
        int16_t sal  = 0.000443 uS (2256.203 MOPS)
        int16_t sar  = 0.000443 uS (2256.242 MOPS)
        int16_t salx = 0.002090 uS ( 478.452 MOPS)
        int16_t sarx = 0.002078 uS ( 481.147 MOPS)
        int16_t LOOP = 0.000443 uS (2257.100 MOPS)
        int16_t add  = 0.002124 uS ( 470.828 MOPS)
        int16_t sub  = 0.002059 uS ( 485.710 MOPS)
        int16_t mul  = 0.002365 uS ( 422.898 MOPS)
        int16_t div  = 0.008037 uS ( 124.418 MOPS)
       uint16_t LOOP = 0.000443 uS (2257.062 MOPS)
       uint16_t sal  = 0.000443 uS (2255.872 MOPS)
       uint16_t sar  = 0.000443 uS (2256.188 MOPS)
       uint16_t salx = 0.002091 uS ( 478.316 MOPS)
       uint16_t sarx = 0.002077 uS ( 481.531 MOPS)
       uint16_t LOOP = 0.000443 uS (2257.584 MOPS)
       uint16_t add  = 0.002062 uS ( 484.870 MOPS)
       uint16_t sub  = 0.002124 uS ( 470.726 MOPS)
       uint16_t mul  = 0.002362 uS ( 423.368 MOPS)
       uint16_t div  = 0.007749 uS ( 129.046 MOPS)

        int32_t LOOP = 0.000444 uS (2254.754 MOPS)
        int32_t sal  = 0.000444 uS (2251.413 MOPS)
        int32_t sar  = 0.000444 uS (2253.040 MOPS)
        int32_t salx = 0.002076 uS ( 481.734 MOPS)
        int32_t sarx = 0.002071 uS ( 482.825 MOPS)
        int32_t LOOP = 0.000444 uS (2252.290 MOPS)
        int32_t add  = 0.001807 uS ( 553.426 MOPS)
        int32_t sub  = 0.002126 uS ( 470.381 MOPS)
        int32_t mul  = 0.002363 uS ( 423.116 MOPS)
        int32_t div  = 0.008039 uS ( 124.397 MOPS)
       uint32_t LOOP = 0.000443 uS (2256.349 MOPS)
       uint32_t sal  = 0.000444 uS (2253.333 MOPS)
       uint32_t sar  = 0.000444 uS (2254.018 MOPS)
       uint32_t salx = 0.002082 uS ( 480.413 MOPS)
       uint32_t sarx = 0.002082 uS ( 480.294 MOPS)
       uint32_t LOOP = 0.000443 uS (2256.534 MOPS)
       uint32_t add  = 0.001808 uS ( 552.974 MOPS)
       uint32_t sub  = 0.002063 uS ( 484.726 MOPS)
       uint32_t mul  = 0.002362 uS ( 423.360 MOPS)
       uint32_t div  = 0.007758 uS ( 128.893 MOPS)

        int64_t LOOP = 0.000443 uS (2256.739 MOPS)
        int64_t sal  = 0.000444 uS (2252.417 MOPS)
        int64_t sar  = 0.000444 uS (2250.945 MOPS)
        int64_t salx = 0.002352 uS ( 425.106 MOPS)
        int64_t sarx = 0.002157 uS ( 463.514 MOPS)
        int64_t LOOP = 0.000443 uS (2256.343 MOPS)
        int64_t add  = 0.002123 uS ( 470.949 MOPS)
        int64_t sub  = 0.002111 uS ( 473.660 MOPS)
        int64_t mul  = 0.002363 uS ( 423.277 MOPS)
        int64_t div  = 0.013182 uS (  75.859 MOPS)
       uint64_t LOOP = 0.000443 uS (2255.777 MOPS)
       uint64_t sal  = 0.000444 uS (2252.583 MOPS)
       uint64_t sar  = 0.000444 uS (2253.585 MOPS)
       uint64_t salx = 0.002345 uS ( 426.491 MOPS)
       uint64_t sarx = 0.002080 uS ( 480.699 MOPS)
       uint64_t LOOP = 0.000444 uS (2254.595 MOPS)
       uint64_t add  = 0.002123 uS ( 471.121 MOPS)
       uint64_t sub  = 0.002056 uS ( 486.355 MOPS)
       uint64_t mul  = 0.002363 uS ( 423.225 MOPS)
       uint64_t div  = 0.010685 uS (  93.593 MOPS)

          float LOOP = 0.000443 uS (2255.217 MOPS)
          float add  = 0.002657 uS ( 376.341 MOPS)
          float sub  = 0.002658 uS ( 376.286 MOPS)
          float mul  = 0.003250 uS ( 307.713 MOPS)
          float div  = 0.004905 uS ( 203.889 MOPS)
          float LOOP = 0.000443 uS (2255.860 MOPS)
          float sqrt = 0.000408 uS (2452.465 MOPS)

         double LOOP = 0.000443 uS (2256.388 MOPS)
         double add  = 0.002658 uS ( 376.254 MOPS)
         double sub  = 0.002657 uS ( 376.295 MOPS)
         double mul  = 0.003248 uS ( 307.913 MOPS)
         double div  = 0.004878 uS ( 204.983 MOPS)
         double LOOP = 0.000443 uS (2256.960 MOPS)
         double sqrt = 0.000470 uS (2126.913 MOPS)

    long double LOOP = 0.001846 uS ( 541.657 MOPS)
    long double add  = 0.003545 uS ( 282.090 MOPS)
    long double sub  = 0.003545 uS ( 282.067 MOPS)
    long double mul  = 0.097509 uS (  10.255 MOPS)
    long double div  = 0.101641 uS (   9.839 MOPS)
    long double LOOP = 0.001772 uS ( 564.381 MOPS)
    long double sqrt = 0.002215 uS ( 451.376 MOPS)

    ---------------------------
    RaspberryPi 3 - 32-bit Mode
    ---------------------------
    sizeof(      float) =  4
    sizeof(     double) =  8
    sizeof(long double) =  8

         int8_t LOOP = 0.003964 uS ( 252.288 MOPS)
         int8_t sal  = 0.005633 uS ( 177.532 MOPS)
         int8_t sar  = 0.006467 uS ( 154.626 MOPS)
         int8_t salx = 0.007301 uS ( 136.959 MOPS)
         int8_t sarx = 0.007301 uS ( 136.959 MOPS)
         int8_t LOOP = 0.002295 uS ( 435.779 MOPS)
         int8_t add  = 0.005424 uS ( 184.353 MOPS)
         int8_t sub  = 0.005424 uS ( 184.369 MOPS)
         int8_t mul  = 0.007093 uS ( 140.984 MOPS)
         int8_t div  = 0.016898 uS (  59.178 MOPS)
        uint8_t LOOP = 0.003964 uS ( 252.301 MOPS)
        uint8_t sal  = 0.005633 uS ( 177.531 MOPS)
        uint8_t sar  = 0.004798 uS ( 208.416 MOPS)
        uint8_t salx = 0.006467 uS ( 154.629 MOPS)
        uint8_t sarx = 0.006467 uS ( 154.631 MOPS)
        uint8_t LOOP = 0.002295 uS ( 435.771 MOPS)
        uint8_t add  = 0.005424 uS ( 184.374 MOPS)
        uint8_t sub  = 0.005424 uS ( 184.364 MOPS)
        uint8_t mul  = 0.007093 uS ( 140.989 MOPS)
        uint8_t div  = 0.017113 uS (  58.433 MOPS)

        int16_t LOOP = 0.003964 uS ( 252.291 MOPS)
        int16_t sal  = 0.006467 uS ( 154.625 MOPS)
        int16_t sar  = 0.006467 uS ( 154.625 MOPS)
        int16_t salx = 0.007302 uS ( 136.958 MOPS)
        int16_t sarx = 0.007301 uS ( 136.964 MOPS)
        int16_t LOOP = 0.002295 uS ( 435.788 MOPS)
        int16_t add  = 0.005424 uS ( 184.361 MOPS)
        int16_t sub  = 0.005424 uS ( 184.361 MOPS)
        int16_t mul  = 0.007093 uS ( 140.991 MOPS)
        int16_t div  = 0.017941 uS (  55.738 MOPS)
       uint16_t LOOP = 0.003964 uS ( 252.283 MOPS)
       uint16_t sal  = 0.005633 uS ( 177.540 MOPS)
       uint16_t sar  = 0.004798 uS ( 208.413 MOPS)
       uint16_t salx = 0.006467 uS ( 154.632 MOPS)
       uint16_t sarx = 0.006467 uS ( 154.634 MOPS)
       uint16_t LOOP = 0.002295 uS ( 435.769 MOPS)
       uint16_t add  = 0.005424 uS ( 184.374 MOPS)
       uint16_t sub  = 0.005424 uS ( 184.370 MOPS)
       uint16_t mul  = 0.007927 uS ( 126.146 MOPS)
       uint16_t div  = 0.015229 uS (  65.665 MOPS)

        int32_t LOOP = 0.003129 uS ( 319.581 MOPS)
        int32_t sal  = 0.004798 uS ( 208.413 MOPS)
        int32_t sar  = 0.004798 uS ( 208.416 MOPS)
        int32_t salx = 0.005632 uS ( 177.543 MOPS)
        int32_t sarx = 0.005633 uS ( 177.535 MOPS)
        int32_t LOOP = 0.002295 uS ( 435.769 MOPS)
        int32_t add  = 0.004590 uS ( 217.887 MOPS)
        int32_t sub  = 0.004590 uS ( 217.881 MOPS)
        int32_t mul  = 0.006258 uS ( 159.793 MOPS)
        int32_t div  = 0.017745 uS (  56.354 MOPS)
       uint32_t LOOP = 0.003129 uS ( 319.582 MOPS)
       uint32_t sal  = 0.004798 uS ( 208.429 MOPS)
       uint32_t sar  = 0.004798 uS ( 208.415 MOPS)
       uint32_t salx = 0.005632 uS ( 177.542 MOPS)
       uint32_t sarx = 0.005632 uS ( 177.542 MOPS)
       uint32_t LOOP = 0.002295 uS ( 435.753 MOPS)
       uint32_t add  = 0.004589 uS ( 217.890 MOPS)
       uint32_t sub  = 0.004589 uS ( 217.891 MOPS)
       uint32_t mul  = 0.006259 uS ( 159.781 MOPS)
       uint32_t div  = 0.017108 uS (  58.453 MOPS)

        int64_t LOOP = 0.003129 uS ( 319.562 MOPS)
        int64_t sal  = 0.005632 uS ( 177.546 MOPS)
        int64_t sar  = 0.005632 uS ( 177.546 MOPS)
        int64_t salx = 0.008970 uS ( 111.477 MOPS)
        int64_t sarx = 0.010222 uS (  97.828 MOPS)
        int64_t LOOP = 0.002295 uS ( 435.773 MOPS)
        int64_t add  = 0.005424 uS ( 184.372 MOPS)
        int64_t sub  = 0.005424 uS ( 184.363 MOPS)
        int64_t mul  = 0.009596 uS ( 104.206 MOPS)
        int64_t div  = 0.054867 uS (  18.226 MOPS)
       uint64_t LOOP = 0.003129 uS ( 319.588 MOPS)
       uint64_t sal  = 0.005633 uS ( 177.517 MOPS)
       uint64_t sar  = 0.005633 uS ( 177.537 MOPS)
       uint64_t salx = 0.008970 uS ( 111.482 MOPS)
       uint64_t sarx = 0.008971 uS ( 111.476 MOPS)
       uint64_t LOOP = 0.002295 uS ( 435.748 MOPS)
       uint64_t add  = 0.005424 uS ( 184.371 MOPS)
       uint64_t sub  = 0.005424 uS ( 184.375 MOPS)
       uint64_t mul  = 0.009596 uS ( 104.206 MOPS)
       uint64_t div  = 0.044853 uS (  22.295 MOPS)

          float LOOP = 0.002295 uS ( 435.780 MOPS)
          float add  = 0.007927 uS ( 126.144 MOPS)
          float sub  = 0.007927 uS ( 126.143 MOPS)
          float mul  = 0.007927 uS ( 126.146 MOPS)
          float div  = 0.015437 uS (  64.779 MOPS)
          float LOOP = 0.002295 uS ( 435.778 MOPS)
          float sqrt = 0.003964 uS ( 252.290 MOPS)

         double LOOP = 0.002295 uS ( 435.773 MOPS)
         double add  = 0.007927 uS ( 126.146 MOPS)
         double sub  = 0.007927 uS ( 126.151 MOPS)
         double mul  = 0.007927 uS ( 126.146 MOPS)
         double div  = 0.022947 uS (  43.578 MOPS)
         double LOOP = 0.002295 uS ( 435.797 MOPS)
         double sqrt = 0.004173 uS ( 239.662 MOPS)

    long double LOOP = 0.002295 uS ( 435.769 MOPS)
    long double add  = 0.007927 uS ( 126.147 MOPS)
    long double sub  = 0.007927 uS ( 126.145 MOPS)
    long double mul  = 0.007927 uS ( 126.148 MOPS)
    long double div  = 0.022948 uS (  43.577 MOPS)
    long double LOOP = 0.002295 uS ( 435.782 MOPS)
    long double sqrt = 0.003964 uS ( 252.281 MOPS)
 */
