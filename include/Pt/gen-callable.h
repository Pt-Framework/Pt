    // main instantiation
    template <typename R, class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void>
    class Callable : public Invokable<A1,A2,A3,A4,A5,A6,A7,A8> {
		public:
		typedef A1 Arg1T;
		typedef A2 Arg2T;
		typedef A3 Arg3T;
		typedef A4 Arg4T;
		typedef A5 Arg5T;
		typedef A6 Arg6T;
		typedef A7 Arg7T;
		typedef A8 Arg8T;
            typedef R ReturnT;
            enum { NumArgs = 8 };

        public:
            virtual Callable* clone() const = 0;

            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const = 0;

            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { return this->operator()(a1,a2,a3,a4,a5,a6,a7,a8); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { this->operator()(a1,a2,a3,a4,a5,a6,a7,a8); }
    };

    // specialization
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    class Callable<R, A1,A2,A3,A4,A5,A6,A7,Void> : public Invokable<A1,A2,A3,A4,A5,A6,A7> {
		public:
		typedef A1 Arg1T;
		typedef A2 Arg2T;
		typedef A3 Arg3T;
		typedef A4 Arg4T;
		typedef A5 Arg5T;
		typedef A6 Arg6T;
		typedef A7 Arg7T;
		typedef Void Arg8T;
            typedef R ReturnT;
            enum { NumArgs = 7 };

        public:
            virtual Callable* clone() const = 0;

            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const = 0;

            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            { return this->operator()(a1,a2,a3,a4,a5,a6,a7); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            { this->operator()(a1,a2,a3,a4,a5,a6,a7); }
    };

    // specialization
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6>
    class Callable<R, A1,A2,A3,A4,A5,A6,Void,Void> : public Invokable<A1,A2,A3,A4,A5,A6> {
		public:
		typedef A1 Arg1T;
		typedef A2 Arg2T;
		typedef A3 Arg3T;
		typedef A4 Arg4T;
		typedef A5 Arg5T;
		typedef A6 Arg6T;
		typedef Void Arg7T;
		typedef Void Arg8T;
            typedef R ReturnT;
            enum { NumArgs = 6 };

        public:
            virtual Callable* clone() const = 0;

            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const = 0;

            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            { return this->operator()(a1,a2,a3,a4,a5,a6); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            { this->operator()(a1,a2,a3,a4,a5,a6); }
    };

    // specialization
    template <typename R,class A1, class A2, class A3, class A4, class A5>
    class Callable<R, A1,A2,A3,A4,A5,Void,Void,Void> : public Invokable<A1,A2,A3,A4,A5> {
		public:
		typedef A1 Arg1T;
		typedef A2 Arg2T;
		typedef A3 Arg3T;
		typedef A4 Arg4T;
		typedef A5 Arg5T;
		typedef Void Arg6T;
		typedef Void Arg7T;
		typedef Void Arg8T;
            typedef R ReturnT;
            enum { NumArgs = 5 };

        public:
            virtual Callable* clone() const = 0;

            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const = 0;

            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { return this->operator()(a1,a2,a3,a4,a5); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { this->operator()(a1,a2,a3,a4,a5); }
    };

    // specialization
    template <typename R,class A1, class A2, class A3, class A4>
    class Callable<R, A1,A2,A3,A4,Void,Void,Void,Void> : public Invokable<A1,A2,A3,A4> {
		public:
		typedef A1 Arg1T;
		typedef A2 Arg2T;
		typedef A3 Arg3T;
		typedef A4 Arg4T;
		typedef Void Arg5T;
		typedef Void Arg6T;
		typedef Void Arg7T;
		typedef Void Arg8T;
            typedef R ReturnT;
            enum { NumArgs = 4 };

        public:
            virtual Callable* clone() const = 0;

            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4) const = 0;

            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4) const
            { return this->operator()(a1,a2,a3,a4); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4) const
            { this->operator()(a1,a2,a3,a4); }
    };

    // specialization
    template <typename R,class A1, class A2, class A3>
    class Callable<R, A1,A2,A3,Void,Void,Void,Void,Void> : public Invokable<A1,A2,A3> {
		public:
		typedef A1 Arg1T;
		typedef A2 Arg2T;
		typedef A3 Arg3T;
		typedef Void Arg4T;
		typedef Void Arg5T;
		typedef Void Arg6T;
		typedef Void Arg7T;
		typedef Void Arg8T;
            typedef R ReturnT;
            enum { NumArgs = 3 };

        public:
            virtual Callable* clone() const = 0;

            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3) const = 0;

            ReturnT call(A1 a1, A2 a2, A3 a3) const
            { return this->operator()(a1,a2,a3); }

            void invoke(A1 a1, A2 a2, A3 a3) const
            { this->operator()(a1,a2,a3); }
    };

    // specialization
    template <typename R,class A1, class A2>
    class Callable<R, A1,A2,Void,Void,Void,Void,Void,Void> : public Invokable<A1,A2> {
		public:
		typedef A1 Arg1T;
		typedef A2 Arg2T;
		typedef Void Arg3T;
		typedef Void Arg4T;
		typedef Void Arg5T;
		typedef Void Arg6T;
		typedef Void Arg7T;
		typedef Void Arg8T;
            typedef R ReturnT;
            enum { NumArgs = 2 };

        public:
            virtual Callable* clone() const = 0;

            virtual ReturnT operator()(A1 a1, A2 a2) const = 0;

            ReturnT call(A1 a1, A2 a2) const
            { return this->operator()(a1,a2); }

            void invoke(A1 a1, A2 a2) const
            { this->operator()(a1,a2); }
    };

    // specialization
    template <typename R,class A1>
    class Callable<R, A1,Void,Void,Void,Void,Void,Void,Void> : public Invokable<A1> {
		public:
		typedef A1 Arg1T;
		typedef Void Arg2T;
		typedef Void Arg3T;
		typedef Void Arg4T;
		typedef Void Arg5T;
		typedef Void Arg6T;
		typedef Void Arg7T;
		typedef Void Arg8T;
            typedef R ReturnT;
            enum { NumArgs = 1 };

        public:
            virtual Callable* clone() const = 0;

            virtual ReturnT operator()(A1 a1) const = 0;

            ReturnT call(A1 a1) const
            { return this->operator()(a1); }

            void invoke(A1 a1) const
            { this->operator()(a1); }
    };

    // specialization
    template <typename R>
    class Callable<R, Void,Void,Void,Void,Void,Void,Void,Void> : public Invokable<> {
		public:
		typedef Void Arg1T;
		typedef Void Arg2T;
		typedef Void Arg3T;
		typedef Void Arg4T;
		typedef Void Arg5T;
		typedef Void Arg6T;
		typedef Void Arg7T;
		typedef Void Arg8T;
            typedef R ReturnT;
            enum { NumArgs = 0 };

        public:
            virtual Callable* clone() const = 0;

            virtual ReturnT operator()() const = 0;

            ReturnT call() const
            { return this->operator()(); }

            void invoke() const
            { this->operator()(); }
    };

