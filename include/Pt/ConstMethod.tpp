// BEGIN_ConstMethod 10
// Main instantiation
template < typename R,typename ClassT,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class ConstMethod : public Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
{
    public:
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
        { return (_object->*_method)(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); }

        ConstMethod<R, ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
ConstMethod<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const ) throw()
{ return ConstMethod<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>(obj,ptr); }

// Main instantiation
template < typename R, typename ClassT,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class ConstMethodSlot : public BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
{
    public:
        ConstMethodSlot(const ConstMethod<R, ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& method)
        : _method( method )
        {}

        Slot* clone() const
        { return new ConstMethodSlot(*this); }

        virtual const void* callable() const
        { return &_method; }

        virtual bool opened(const Connection& c)
        {
            Connectable& connectable = _method.object();
            return connectable.opened(c);
        }

        virtual void closed(const Connection& c) 
        {
            Connectable& connectable = _method.object();
            connectable.closed(c);
        }

    private:
        ConstMethod<R, ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> _method;
};
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
ConstMethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) const ) throw()
{
    return ConstMethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>( callable( obj, memFunc ) );
}
// END_ConstMethod 10
// BEGIN_ConstMethod 9
// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class ConstMethod<R,ClassT, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>
{
    public:
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8,A9) const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
        { return (_object->*_method)(a1,a2,a3,a4,a5,a6,a7,a8,a9); }

        ConstMethod<R, ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
ConstMethod<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const ) throw()
{ return ConstMethod<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9>(obj,ptr); }

template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
ConstMethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7,A8,A9> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9) const ) throw()
{
    return ConstMethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9>( callable( obj, memFunc ) );
}
// END_ConstMethod 9
// BEGIN_ConstMethod 8
// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class ConstMethod<R,ClassT, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>
{
    public:
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8) const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
        { return (_object->*_method)(a1,a2,a3,a4,a5,a6,a7,a8); }

        ConstMethod<R, ClassT,A1,A2,A3,A4,A5,A6,A7,A8>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
ConstMethod<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const ) throw()
{ return ConstMethod<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8>(obj,ptr); }

template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
ConstMethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7,A8> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8) const ) throw()
{
    return ConstMethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8>( callable( obj, memFunc ) );
}
// END_ConstMethod 8
// BEGIN_ConstMethod 7
// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class ConstMethod<R,ClassT, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>
{
    public:
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7) const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
        { return (_object->*_method)(a1,a2,a3,a4,a5,a6,a7); }

        ConstMethod<R, ClassT,A1,A2,A3,A4,A5,A6,A7>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
ConstMethod<R,ClassT,A1,A2,A3,A4,A5,A6,A7> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const ) throw()
{ return ConstMethod<R,ClassT,A1,A2,A3,A4,A5,A6,A7>(obj,ptr); }

template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
ConstMethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7) const ) throw()
{
    return ConstMethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7>( callable( obj, memFunc ) );
}
// END_ConstMethod 7
// BEGIN_ConstMethod 6
// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
class ConstMethod<R,ClassT, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>
{
    public:
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6) const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
        { return (_object->*_method)(a1,a2,a3,a4,a5,a6); }

        ConstMethod<R, ClassT,A1,A2,A3,A4,A5,A6>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
ConstMethod<R,ClassT,A1,A2,A3,A4,A5,A6> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const ) throw()
{ return ConstMethod<R,ClassT,A1,A2,A3,A4,A5,A6>(obj,ptr); }

template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
ConstMethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6) const ) throw()
{
    return ConstMethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6>( callable( obj, memFunc ) );
}
// END_ConstMethod 6
// BEGIN_ConstMethod 5
// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5>
class ConstMethod<R,ClassT, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>
{
    public:
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5) const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
        { return (_object->*_method)(a1,a2,a3,a4,a5); }

        ConstMethod<R, ClassT,A1,A2,A3,A4,A5>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5>
ConstMethod<R,ClassT,A1,A2,A3,A4,A5> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const ) throw()
{ return ConstMethod<R,ClassT,A1,A2,A3,A4,A5>(obj,ptr); }

template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5>
ConstMethodSlot<R,BaseT,A1,A2,A3,A4,A5> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5) const ) throw()
{
    return ConstMethodSlot<R,ClassT,A1,A2,A3,A4,A5>( callable( obj, memFunc ) );
}
// END_ConstMethod 5
// BEGIN_ConstMethod 4
// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4>
class ConstMethod<R,ClassT, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4) const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
        { return (_object->*_method)(a1,a2,a3,a4); }

        ConstMethod<R, ClassT,A1,A2,A3,A4>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4>
ConstMethod<R,ClassT,A1,A2,A3,A4> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4) const ) throw()
{ return ConstMethod<R,ClassT,A1,A2,A3,A4>(obj,ptr); }

template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4>
ConstMethodSlot<R,BaseT,A1,A2,A3,A4> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4) const ) throw()
{
    return ConstMethodSlot<R,ClassT,A1,A2,A3,A4>( callable( obj, memFunc ) );
}
// END_ConstMethod 4
// BEGIN_ConstMethod 3
// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3>
class ConstMethod<R,ClassT, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef R (ClassT::*MemFuncT)(A1,A2,A3) const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3) const
        { return (_object->*_method)(a1,a2,a3); }

        ConstMethod<R, ClassT,A1,A2,A3>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT,class A1, class A2, class A3>
ConstMethod<R,ClassT,A1,A2,A3> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3) const ) throw()
{ return ConstMethod<R,ClassT,A1,A2,A3>(obj,ptr); }

template <class R, class BaseT, class ClassT,class A1, class A2, class A3>
ConstMethodSlot<R,BaseT,A1,A2,A3> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3) const ) throw()
{
    return ConstMethodSlot<R,ClassT,A1,A2,A3>( callable( obj, memFunc ) );
}
// END_ConstMethod 3
// BEGIN_ConstMethod 2
// Specialization
template < typename R, typename ClassT,class A1, class A2>
class ConstMethod<R,ClassT, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef R (ClassT::*MemFuncT)(A1,A2) const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2) const
        { return (_object->*_method)(a1,a2); }

        ConstMethod<R, ClassT,A1,A2>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT,class A1, class A2>
ConstMethod<R,ClassT,A1,A2> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2) const ) throw()
{ return ConstMethod<R,ClassT,A1,A2>(obj,ptr); }

template <class R, class BaseT, class ClassT,class A1, class A2>
ConstMethodSlot<R,BaseT,A1,A2> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2) const ) throw()
{
    return ConstMethodSlot<R,ClassT,A1,A2>( callable( obj, memFunc ) );
}
// END_ConstMethod 2
// BEGIN_ConstMethod 1
// Specialization
template < typename R, typename ClassT,class A1>
class ConstMethod<R,ClassT, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef R (ClassT::*MemFuncT)(A1) const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()(A1 a1) const
        { return (_object->*_method)(a1); }

        ConstMethod<R, ClassT,A1>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT,class A1>
ConstMethod<R,ClassT,A1> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1) const ) throw()
{ return ConstMethod<R,ClassT,A1>(obj,ptr); }

template <class R, class BaseT, class ClassT,class A1>
ConstMethodSlot<R,BaseT,A1> slot( ClassT & obj, R (BaseT::*memFunc)(A1) const ) throw()
{
    return ConstMethodSlot<R,ClassT,A1>( callable( obj, memFunc ) );
}
// END_ConstMethod 1
// BEGIN_ConstMethod 0
// Specialization
template < typename R, typename ClassT>
class ConstMethod<R,ClassT, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef R (ClassT::*MemFuncT)() const;

        ConstMethod(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        R operator()() const
        { return (_object->*_method)(); }

        ConstMethod<R, ClassT>* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _method;
};

template <class R, class ClassT>
ConstMethod<R,ClassT> callable( ClassT & obj, R (ClassT::*ptr)() const ) throw()
{ return ConstMethod<R,ClassT>(obj,ptr); }

template <class R, class BaseT, class ClassT>
ConstMethodSlot<R,BaseT> slot( ClassT & obj, R (BaseT::*memFunc)() const ) throw()
{
    return ConstMethodSlot<R,ClassT>( callable( obj, memFunc ) );
}
// END_ConstMethod 0
