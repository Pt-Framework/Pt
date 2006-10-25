#ifndef PT_ATOMICINT_H
#define PT_ATOMICINT_H

#ifdef _MSC_VER
    #include <windows.h>
#else
    #include <csignal>
#endif


namespace Pt {

#ifdef _MSC_VER

    typedef long atomic_t;

    class AtomicInt
    {
        public:
            AtomicInt(atomic_t value = 0)
            : _value(value)
            {}

            atomic_t value() const
            { return _value; }

            void operator+=(atomic_t n)
            { ::InterlockedExchangeAdd(const_cast<long*>(&_value), n); }

            void operator-=(atomic_t n)
            { ::InterlockedExchangeAdd(const_cast<long*>(&_value), -n); }

            void operator=(atomic_t n)
            { ::InterlockedExchange(const_cast<long*>(&_value), n); }

            bool compareExchange(atomic_t cmp, atomic_t ex)
            { return (::InterlockedCompareExchange(const_cast<long*>(&_value), ex, cmp) == 1); }

        private:
            volatile atomic_t _value;
    };


#elif __GNUC__

	#ifdef PT_X86 // pentium and higher...

		typedef std::sig_atomic_t atomic_t;

		class AtomicInt
		{
			public:
				AtomicInt(atomic_t value = 0)
				: _value(value)
				{}

				atomic_t value() const
				{ return _value; }

				void operator+=(atomic_t n)
				{
					register int result;
					asm volatile ( "lock; xadd{l} {%0,%1|%1,%0}"
								: "=r" (result), "=m" (_value)
								: "0" (n), "m" (_value) );
				}

				void operator-=(atomic_t n)
				{
					register int result;
					asm volatile ( "lock; xadd{l} {%0,%1|%1,%0}"
								: "=r" (result), "=m" (_value)
								: "0" (-n), "m" (_value) );
				}

				void operator=(atomic_t n)
				{
					register volatile int ret;
					asm volatile("xchgl %0, %1"
					             : "=r"(ret), "=m"(_value)
					             : "0"(n), "m"(_value)
					             : "memory");
				}

				bool compareExchange(atomic_t oldval, atomic_t newval)
				{
					std::sig_atomic_t result;

					asm volatile( "lock; cmpxchgl %2, %1"
					              : "=a" (result), "=m" (_value)
					              : "r" (newval), "m" (_value), "0" (oldval) );

					return result == oldval;
				}

			private:
				volatile atomic_t _value;
		};

	#elif PT_ARM

		typedef std::sig_atomic_t atomic_t;

		class AtomicInt
		{
			public:
				AtomicInt(atomic_t value = 0)
				: _value(value)
				{}

				atomic_t value() const
				{ return _value; }

				void operator+=(atomic_t n)
				{
					register volatile unsigned long tmp;
					register volatile sig_atomic_t result;

					asm volatile("@ atomic_add_return\n"
								"1:     ldrex   %0, [%2]\n"
								"       add     %0, %0, %3\n"
								"       strex   %1, %0, [%2]\n"
								"       teq     %1, #0\n"
								"       bne     1b"
								: "=&r" (result), "=&r" (tmp)
								: "r" (&_value), "Ir" (n)
								: "cc");

					//return result;
				}

				void operator-=(atomic_t n)
				{
					register volatile unsigned long tmp;
					register volatile sig_atomic_t result;

					asm volatile("@ atomic_sub_return\n"
								"1:     ldrex   %0, [%2]\n"
								"       sub     %0, %0, %3\n"
								"       strex   %1, %0, [%2]\n"
								"       teq     %1, #0\n"
								"       bne     1b"
								: "=&r" (result), "=&r" (tmp)
								: "r" (&_value), "Ir" (n)
								: "cc");

					//return result;
				}

				void operator=(atomic_t n)
				{
					register volatile unsigned long tmp;

					asm volatile("@ atomic_set\n"
								"1:     ldrex   %0, [%1]\n"
								"       strex   %0, %2, [%1]\n"
								"       teq     %0, #0\n"
								"       bne     1b"
								: "=&r" (tmp)
								: "r" (&_value), "r" (n)
								: "cc");
				}

				bool compareExchange(atomic_t oldval, atomic_t newval)
				{
					std::sig_atomic_t result, tmp;

					asm volatile("\n"
					             "0:\tldr\t%1,[%2]\n\t"
					             "cmp\t%1,%4\n\t"
					             "movne\t%0,%1\n\t"
					             "bne\t1f\n\t"
					             "swp\t%0,%3,[%2]\n\t"
					             "cmp\t%1,%0\n\t"
					             "swpne\t%1,%0,[%2]\n\t"
					             "bne\t0b\n\t"
					             "1:"
					             : "=&r" (result), "=&r" (tmp)
					             : "r" (&_value), "r" (newval), "r" (oldval)
					             : "cc", "memory");
				}

			private:
				volatile atomic_t _value;
		};

	#elif PT_PPC

		typedef std::sig_atomic_t atomic_t;

		class AtomicInt
		{
			public:
				AtomicInt(atomic_t value = 0)
				: _value(value)
				{}

				atomic_t value() const
				{ return _value;  }

				void operator+=(atomic_t n)
				{
					int ret = 0;
					int zero = 0;

					asm volatile(
						"0:    lwarx %0, %3, %1\n"
						"      add %0, %2, %0\n"
						"      stwcx. %0, %3, %1\n"
						"      bne- 0b\n"
						"      isync\n"
						: "=&r" (ret)
						: "r"(&_value), "r"(n), "r"(zero)
						: "cr0", "memory", "r0"
					);
				}

				void operator-=(atomic_t n)
				{
					int ret = 0;
					int zero = 0;

					asm volatile(
						"0:    lwarx %0, %3, %1\n"
						"      add %0, %2, %0\n"
						"      stwcx. %0, %3, %1\n"
						"      bne- 0b\n"
						"      isync\n"
						: "=&r" (ret)
						: "r"(&_value), "r"(-n), "r"(zero)
						: "cr0", "memory", "r0"
					);
				}

				void operator=(atomic_t n)
				{
					atomic_t ret = 0;
					asm volatile("0:    lwarx %0,0,%1\n"
					             "      stwcx. %2,0,%1\n"
					            "      bne- 0b\n"
					            "      isync\n"
					            : "=&r"(ret)
					            : "r"(&_value), "r"(n)
					            : "cr0","memory","r0");
				}

				bool atomic_compare_and_exchange(atomic_t oldval, atomic_t newval)
				{
					std::sig_atomic_t result;

					asm volatile ("sync\n"
					              "1: lwarx   %0,0,%1\n"
					              "   subf.   %0,%2,%0\n"
					              "   bne     2f\n"
					              "   stwcx.  %3,0,%1\n"
					              "   bne-    1b\n"
					              "2: isync"
					              : "=&r" (result)
					              : "b" (&_value), "r" (oldval), "r" (newval)
					              : "cr0", "memory");

					return result == 0;
				}

			private:
				volatile atomic_t _value;
		};

	#endif

#endif

}

#endif
