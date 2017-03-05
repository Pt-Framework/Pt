// g++ -O2 bench-pc.cpp -o bench-pc && ./bench-pc && rm -f bench-pc

// Results are below the program

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

double mygettime()
{
  struct timeval tv;
  if(gettimeofday(&tv, 0) < 0) {
    perror("oops");
  }
  return (double)tv.tv_sec + (0.000001 * (double)tv.tv_usec);
}

template< typename Type >
void my_test(const char* name) {
  volatile Type v  = 0;
  // Do not use constants or repeating values
  //  to avoid loop unroll optimizations.
  // All values >0 to avoid division by 0
  Type v0 = (Type)(rand() % 128)/16 + 1;
  Type v1 = (Type)(rand() % 128)/16 + 1;
  Type v2 = (Type)(rand() % 128)/16 + 1;
  Type v3 = (Type)(rand() % 128)/16 + 1;
  Type v4 = (Type)(rand() % 128)/16 + 1;
  Type v5 = (Type)(rand() % 128)/16 + 1;
  Type v6 = (Type)(rand() % 128)/16 + 1;
  Type v7 = (Type)(rand() % 128)/16 + 1;

  double t1 = mygettime();
  for (size_t i = 0; i < 100000000; ++i) {
    v += v0;
    v += v2;
    v += v4;
    v += v6;
  }
  printf("%s add = %7.3f uS\n", name, mygettime() - t1);

  t1 = mygettime();
  for (size_t i = 0; i < 100000000; ++i) {
    v -= v1;
    v -= v3;
    v -= v5;
    v -= v7;
  }
  printf("%s sub = %7.3f uS\n", name, mygettime() - t1);

  t1 = mygettime();
  for (size_t i = 0; i < 100000000; ++i) {
    v *= v0;
    v *= v2;
    v *= v4;
    v *= v6;
  }
  printf("%s mul = %7.3f uS\n", name, mygettime() - t1);

  t1 = mygettime();
  for (size_t i = 0; i < 100000000; ++i) {
    v /= v1;
    v /= v3;
    v /= v5;
    v /= v7;
  }
  printf("%s div = %7.3f uS\n", name, mygettime() - t1);
}

int main() {
  my_test<     int8_t>("     int8_t");
  my_test<    uint8_t>("    uint8_t");

  my_test<    int16_t>("    int16_t");
  my_test<   uint16_t>("   uint16_t");

  my_test<    int32_t>("    int32_t");
  my_test<   uint32_t>("   uint32_t");

  my_test<    int64_t>("    int64_t");
  my_test<   uint64_t>("   uint64_t");

  my_test<      float>("      float");
  my_test<     double>("     double");
  my_test<long double>("long double");

  return 0;
}

/*
    Core i5 64-bit Mode
    -------------------

    RaspberryPi 3 32-bit Mode
    -------------------------

 */