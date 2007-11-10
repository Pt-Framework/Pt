template < typename R,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void>
class Function : public Callable<R, A1,A2,A3,A4,A5,A6,A7,A8>
{
    public:
        typedef R (*FuncT)(A1,A2,A3,A4,A5,A6,A7,A8);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
        { return (*_funcPtr)(a1,a2,a3,a4,a5,a6,a7,a8); }

        Function<R,A1,A2,A3,A4,A5,A6,A7,A8>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};
template < typename R,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void>
class FunctionSlot : public BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8>
{
    public:
        FunctionSlot(const Function<R,A1,A2,A3,A4,A5,A6,A7,A8>& func)
        : _func( func )
        {}

        virtual const void* callable() const
        { return &_func; }

        Slot* clone() const
        { return new FunctionSlot(*this); }

        virtual bool opened(const Connection& c)
        { return true; }

        virtual void closed(const Connection& c)
        { }

    private:
        Function<R,A1,A2,A3,A4,A5,A6,A7,A8> _func;
}; // FunctionSlot


template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
Function<R,A1,A2,A3,A4,A5,A6,A7,A8> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)) throw()
{ return Function<R,A1,A2,A3,A4,A5,A6,A7,A8>(func); }

template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
FunctionSlot<R,A1,A2,A3,A4,A5,A6,A7,A8> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) ) throw()
{ return FunctionSlot<R,A1,A2,A3,A4,A5,A6,A7,A8>( callable(func) ); }

template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Function<R,A1,A2,A3,A4,A5,A6,A7> : public Callable<R, A1,A2,A3,A4,A5,A6,A7,Void>
{
    public:
        typedef R (*FuncT)(A1,A2,A3,A4,A5,A6,A7);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
        { return (*_funcPtr)(a1,a2,a3,a4,a5,a6,a7); }

        Function<R,A1,A2,A3,A4,A5,A6,A7>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};

template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
Function<R,A1,A2,A3,A4,A5,A6,A7> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)) throw()
{ return Function<R,A1,A2,A3,A4,A5,A6,A7>(func); }

template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
FunctionSlot<R,A1,A2,A3,A4,A5,A6,A7> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) ) throw()
{ return FunctionSlot<R,A1,A2,A3,A4,A5,A6,A7>( callable(func) ); }

template < typename R,class A1, class A2, class A3, class A4, class A5, class A6>
class Function<R,A1,A2,A3,A4,A5,A6> : public Callable<R, A1,A2,A3,A4,A5,A6,Void,Void>
{
    public:
        typedef R (*FuncT)(A1,A2,A3,A4,A5,A6);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
        { return (*_funcPtr)(a1,a2,a3,a4,a5,a6); }

        Function<R,A1,A2,A3,A4,A5,A6>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};

template <typename R,class A1, class A2, class A3, class A4, class A5, class A6>
Function<R,A1,A2,A3,A4,A5,A6> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)) throw()
{ return Function<R,A1,A2,A3,A4,A5,A6>(func); }

template <typename R,class A1, class A2, class A3, class A4, class A5, class A6>
FunctionSlot<R,A1,A2,A3,A4,A5,A6> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) ) throw()
{ return FunctionSlot<R,A1,A2,A3,A4,A5,A6>( callable(func) ); }

template < typename R,class A1, class A2, class A3, class A4, class A5>
class Function<R,A1,A2,A3,A4,A5> : public Callable<R, A1,A2,A3,A4,A5,Void,Void,Void>
{
    public:
        typedef R (*FuncT)(A1,A2,A3,A4,A5);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
        { return (*_funcPtr)(a1,a2,a3,a4,a5); }

        Function<R,A1,A2,A3,A4,A5>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};

template <typename R,class A1, class A2, class A3, class A4, class A5>
Function<R,A1,A2,A3,A4,A5> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)) throw()
{ return Function<R,A1,A2,A3,A4,A5>(func); }

template <typename R,class A1, class A2, class A3, class A4, class A5>
FunctionSlot<R,A1,A2,A3,A4,A5> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) ) throw()
{ return FunctionSlot<R,A1,A2,A3,A4,A5>( callable(func) ); }

template < typename R,class A1, class A2, class A3, class A4>
class Function<R,A1,A2,A3,A4> : public Callable<R, A1,A2,A3,A4,Void,Void,Void,Void>
{
    public:
        typedef R (*FuncT)(A1,A2,A3,A4);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
        { return (*_funcPtr)(a1,a2,a3,a4); }

        Function<R,A1,A2,A3,A4>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};

template <typename R,class A1, class A2, class A3, class A4>
Function<R,A1,A2,A3,A4> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4)) throw()
{ return Function<R,A1,A2,A3,A4>(func); }

template <typename R,class A1, class A2, class A3, class A4>
FunctionSlot<R,A1,A2,A3,A4> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4) ) throw()
{ return FunctionSlot<R,A1,A2,A3,A4>( callable(func) ); }

template < typename R,class A1, class A2, class A3>
class Function<R,A1,A2,A3> : public Callable<R, A1,A2,A3,Void,Void,Void,Void,Void>
{
    public:
        typedef R (*FuncT)(A1,A2,A3);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1, A2 a2, A3 a3) const
        { return (*_funcPtr)(a1,a2,a3); }

        Function<R,A1,A2,A3>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};

template <typename R,class A1, class A2, class A3>
Function<R,A1,A2,A3> callable(R (*func)(A1 a1, A2 a2, A3 a3)) throw()
{ return Function<R,A1,A2,A3>(func); }

template <typename R,class A1, class A2, class A3>
FunctionSlot<R,A1,A2,A3> slot( R (*func)(A1 a1, A2 a2, A3 a3) ) throw()
{ return FunctionSlot<R,A1,A2,A3>( callable(func) ); }

template < typename R,class A1, class A2>
class Function<R,A1,A2> : public Callable<R, A1,A2,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef R (*FuncT)(A1,A2);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1, A2 a2) const
        { return (*_funcPtr)(a1,a2); }

        Function<R,A1,A2>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};

template <typename R,class A1, class A2>
Function<R,A1,A2> callable(R (*func)(A1 a1, A2 a2)) throw()
{ return Function<R,A1,A2>(func); }

template <typename R,class A1, class A2>
FunctionSlot<R,A1,A2> slot( R (*func)(A1 a1, A2 a2) ) throw()
{ return FunctionSlot<R,A1,A2>( callable(func) ); }

template < typename R,class A1>
class Function<R,A1> : public Callable<R, A1,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef R (*FuncT)(A1);

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()(A1 a1) const
        { return (*_funcPtr)(a1); }

        Function<R,A1>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};

template <typename R,class A1>
Function<R,A1> callable(R (*func)(A1 a1)) throw()
{ return Function<R,A1>(func); }

template <typename R,class A1>
FunctionSlot<R,A1> slot( R (*func)(A1 a1) ) throw()
{ return FunctionSlot<R,A1>( callable(func) ); }

template < typename R>
class Function<R> : public Callable<R, Void,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef R (*FuncT)();

        Function(FuncT func) throw()
        : _funcPtr(func) { }

        Function(const Function& f) throw()
        { this->operator=(f); }

        R operator()() const
        { return (*_funcPtr)(); }

        Function<R>* clone() const
        { return new Function(*this); }

        Function& operator=(const Function& function)
        {
            _funcPtr = function._funcPtr;
            return (*this);
        }

    private:
        FuncT _funcPtr;
};

template <typename R>
Function<R> callable(R (*func)()) throw()
{ return Function<R>(func); }

template <typename R>
FunctionSlot<R> slot( R (*func)() ) throw()
{ return FunctionSlot<R>( callable(func) ); }

