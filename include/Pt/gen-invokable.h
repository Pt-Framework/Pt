// main instance
template <class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class Invokable {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const = 0;
};
// specialization
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class Invokable<A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const = 0;
};
// specialization
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class Invokable<A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const = 0;
};
// specialization
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Invokable<A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const = 0;
};
// specialization
template <class A1, class A2, class A3, class A4, class A5, class A6>
class Invokable<A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const = 0;
};
// specialization
template <class A1, class A2, class A3, class A4, class A5>
class Invokable<A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const = 0;
};
// specialization
template <class A1, class A2, class A3, class A4>
class Invokable<A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4) const = 0;
};
// specialization
template <class A1, class A2, class A3>
class Invokable<A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1 a1, A2 a2, A3 a3) const = 0;
};
// specialization
template <class A1, class A2>
class Invokable<A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1 a1, A2 a2) const = 0;
};
// specialization
template <class A1>
class Invokable<A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke(A1 a1) const = 0;
};
// specialization
template <>
class Invokable<Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> {
    public:
        virtual ~Invokable()
        {}

        virtual void invoke() const = 0;
};
