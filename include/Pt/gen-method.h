// Main instantiation
template < typename R,typename ClassT,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class Method : public Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10);
        typedef R (ClassT::*ConstMemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); }

        Method<R, ClassT, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>(obj,ptr);
}
// Main instantiation
template < typename R, typename ClassT,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class MethodSlot : public BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
{
    public:
        MethodSlot(const Method<R, ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& method)
        : _method( method )
        {}

        Slot* clone() const
        { return new MethodSlot(*this); }

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
        Method<R, ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> _method;
};
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>( callable( obj, memFunc ) );
}

// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class Method<R,ClassT, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8,A9);
        typedef R (ClassT::*ConstMemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8,A9) const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5,a6,a7,a8,a9); }

        Method<R, ClassT, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9>(obj,ptr);
}
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7,A8,A9> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9>( callable( obj, memFunc ) );
}

// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class Method<R,ClassT, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8);
        typedef R (ClassT::*ConstMemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8) const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5,a6,a7,a8); }

        Method<R, ClassT, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8>(obj,ptr);
}
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7,A8> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8>( callable( obj, memFunc ) );
}

// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Method<R,ClassT, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7);
        typedef R (ClassT::*ConstMemFuncT)(A1,A2,A3,A4,A5,A6,A7) const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5,a6,a7); }

        Method<R, ClassT, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7>(obj,ptr);
}
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7>( callable( obj, memFunc ) );
}

// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
class Method<R,ClassT, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6);
        typedef R (ClassT::*ConstMemFuncT)(A1,A2,A3,A4,A5,A6) const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5,a6); }

        Method<R, ClassT, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
Method<R,ClassT,A1,A2,A3,A4,A5,A6> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5,A6>(obj,ptr);
}
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6>( callable( obj, memFunc ) );
}

// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5>
class Method<R,ClassT, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5);
        typedef R (ClassT::*ConstMemFuncT)(A1,A2,A3,A4,A5) const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5); }

        Method<R, ClassT, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5>
Method<R,ClassT,A1,A2,A3,A4,A5> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5>(obj,ptr);
}
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5>( callable( obj, memFunc ) );
}

// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3, class A4>
class Method<R,ClassT, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4);
        typedef R (ClassT::*ConstMemFuncT)(A1,A2,A3,A4) const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
        { return (_object->*_memFunc)(a1,a2,a3,a4); }

        Method<R, ClassT, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT,class A1, class A2, class A3, class A4>
Method<R,ClassT,A1,A2,A3,A4> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4>(obj,ptr);
}
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4>
MethodSlot<R,BaseT,A1,A2,A3,A4> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4>( callable( obj, memFunc ) );
}

// Specialization
template < typename R, typename ClassT,class A1, class A2, class A3>
class Method<R,ClassT, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)(A1,A2,A3);
        typedef R (ClassT::*ConstMemFuncT)(A1,A2,A3) const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2, A3 a3) const
        { return (_object->*_memFunc)(a1,a2,a3); }

        Method<R, ClassT, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT,class A1, class A2, class A3>
Method<R,ClassT,A1,A2,A3> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3)) throw()
{
    return Method<R,ClassT,A1,A2,A3>(obj,ptr);
}
template <class R, class BaseT, class ClassT,class A1, class A2, class A3>
MethodSlot<R,BaseT,A1,A2,A3> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3>( callable( obj, memFunc ) );
}

// Specialization
template < typename R, typename ClassT,class A1, class A2>
class Method<R,ClassT, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)(A1,A2);
        typedef R (ClassT::*ConstMemFuncT)(A1,A2) const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1, A2 a2) const
        { return (_object->*_memFunc)(a1,a2); }

        Method<R, ClassT, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT,class A1, class A2>
Method<R,ClassT,A1,A2> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2)) throw()
{
    return Method<R,ClassT,A1,A2>(obj,ptr);
}
template <class R, class BaseT, class ClassT,class A1, class A2>
MethodSlot<R,BaseT,A1,A2> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2>( callable( obj, memFunc ) );
}

// Specialization
template < typename R, typename ClassT,class A1>
class Method<R,ClassT, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)(A1);
        typedef R (ClassT::*ConstMemFuncT)(A1) const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()(A1 a1) const
        { return (_object->*_memFunc)(a1); }

        Method<R, ClassT, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT,class A1>
Method<R,ClassT,A1> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1)) throw()
{
    return Method<R,ClassT,A1>(obj,ptr);
}
template <class R, class BaseT, class ClassT,class A1>
MethodSlot<R,BaseT,A1> slot( ClassT & obj, R (BaseT::*memFunc)(A1) ) throw()
{
    return MethodSlot<R,ClassT,A1>( callable( obj, memFunc ) );
}

// Specialization
template < typename R, typename ClassT>
class Method<R,ClassT, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        typedef ClassT InterfaceType;
        typedef R (ClassT::*MemFuncT)();
        typedef R (ClassT::*ConstMemFuncT)() const;


        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        Method(const Method& method) throw()
        : Callable<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object;}

        const ClassT& object() const
        { return *_object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()() const
        { return (_object->*_memFunc)(); }

        Method<R, ClassT, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        Method& operator=(const Method& method)
        {
            _object = method._object;
            _memFunc = method._memFunc;
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

template <class R, class ClassT>
Method<R,ClassT> callable( ClassT & obj, R (ClassT::*ptr)()) throw()
{
    return Method<R,ClassT>(obj,ptr);
}
template <class R, class BaseT, class ClassT>
MethodSlot<R,BaseT> slot( ClassT & obj, R (BaseT::*memFunc)() ) throw()
{
    return MethodSlot<R,ClassT>( callable( obj, memFunc ) );
}

