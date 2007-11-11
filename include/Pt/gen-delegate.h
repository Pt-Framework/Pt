// Main instantiation
template < typename R,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class Delegate : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
            }

            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
            }

            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
            { return this->call(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); }
    };
// Main instantiation
template < typename R,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class DelegateSlot : public BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
{
        public:
            DelegateSlot(Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate)
            : _method( delegate, &Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>::call )
            {}

            BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>* clone() const
            { return new DelegateSlot(*this); }

            virtual const void* callable() const
            {
                return &_method;
            }

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
            mutable ConstMethod<R, Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10 > _method;
};

    template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    DelegateSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> slot( Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate )
    { return DelegateSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>( delegate ); }

    /** Connect a Delegate to another Delegate
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate, Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate, R(*func)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// Specialization
template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5,A6,A7,A8,A9> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9);
            }

            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9);
            }

            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
            { return this->call(a1,a2,a3,a4,a5,a6,a7,a8,a9); }
    };
    /** Connect a Delegate to another Delegate
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& delegate, Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& delegate, R(*func)(A1,A2,A3,A4,A5,A6,A7,A8,A9))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// Specialization
template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5,A6,A7,A8> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5,a6,a7,a8);
            }

            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5,a6,a7,a8);
            }

            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { return this->call(a1,a2,a3,a4,a5,a6,a7,a8); }
    };
    /** Connect a Delegate to another Delegate
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>& delegate, Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>& delegate, R(*func)(A1,A2,A3,A4,A5,A6,A7,A8))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// Specialization
template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Delegate<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5,A6,A7> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5,a6,a7);
            }

            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5,a6,a7);
            }

            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            { return this->call(a1,a2,a3,a4,a5,a6,a7); }
    };
    /** Connect a Delegate to another Delegate
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7>& delegate, Delegate<R,A1,A2,A3,A4,A5,A6,A7>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7>& delegate, R(*func)(A1,A2,A3,A4,A5,A6,A7))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// Specialization
template < typename R,class A1, class A2, class A3, class A4, class A5, class A6>
class Delegate<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5,A6> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5,a6);
            }

            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5,a6);
            }

            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            { return this->call(a1,a2,a3,a4,a5,a6); }
    };
    /** Connect a Delegate to another Delegate
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6>& delegate, Delegate<R,A1,A2,A3,A4,A5,A6>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6>& delegate, R(*func)(A1,A2,A3,A4,A5,A6))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// Specialization
template < typename R,class A1, class A2, class A3, class A4, class A5>
class Delegate<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5);
            }

            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5);
            }

            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { return this->call(a1,a2,a3,a4,a5); }
    };
    /** Connect a Delegate to another Delegate
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5>& delegate, Delegate<R,A1,A2,A3,A4,A5>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5>& delegate, R(*func)(A1,A2,A3,A4,A5))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// Specialization
template < typename R,class A1, class A2, class A3, class A4>
class Delegate<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R,A1,A2,A3,A4>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1, A2 a2, A3 a3, A4 a4) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4);
            }

            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4);
            }

            R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
            { return this->call(a1,a2,a3,a4); }
    };
    /** Connect a Delegate to another Delegate
    */
    template <typename R,class A1, class A2, class A3, class A4>
    Connection connect(Delegate<R,A1,A2,A3,A4>& delegate, Delegate<R,A1,A2,A3,A4>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R,class A1, class A2, class A3, class A4>
    Connection connect(Delegate<R,A1,A2,A3,A4>& delegate, const BasicSlot<R,A1,A2,A3,A4>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R,class A1, class A2, class A3, class A4>
    Connection connect(Delegate<R,A1,A2,A3,A4>& delegate, R(*func)(A1,A2,A3,A4))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4>
    Connection connect(Delegate<R,A1,A2,A3,A4>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// Specialization
template < typename R,class A1, class A2, class A3>
class Delegate<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R,A1,A2,A3>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1, A2 a2, A3 a3) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3);
            }

            inline void invoke(A1 a1, A2 a2, A3 a3) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3);
            }

            R operator()(A1 a1, A2 a2, A3 a3) const
            { return this->call(a1,a2,a3); }
    };
    /** Connect a Delegate to another Delegate
    */
    template <typename R,class A1, class A2, class A3>
    Connection connect(Delegate<R,A1,A2,A3>& delegate, Delegate<R,A1,A2,A3>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R,class A1, class A2, class A3>
    Connection connect(Delegate<R,A1,A2,A3>& delegate, const BasicSlot<R,A1,A2,A3>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R,class A1, class A2, class A3>
    Connection connect(Delegate<R,A1,A2,A3>& delegate, R(*func)(A1,A2,A3))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3>
    Connection connect(Delegate<R,A1,A2,A3>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT,class A1, class A2, class A3>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// Specialization
template < typename R,class A1, class A2>
class Delegate<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R,A1,A2>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1, A2 a2) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2);
            }

            inline void invoke(A1 a1, A2 a2) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2);
            }

            R operator()(A1 a1, A2 a2) const
            { return this->call(a1,a2); }
    };
    /** Connect a Delegate to another Delegate
    */
    template <typename R,class A1, class A2>
    Connection connect(Delegate<R,A1,A2>& delegate, Delegate<R,A1,A2>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R,class A1, class A2>
    Connection connect(Delegate<R,A1,A2>& delegate, const BasicSlot<R,A1,A2>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R,class A1, class A2>
    Connection connect(Delegate<R,A1,A2>& delegate, R(*func)(A1,A2))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2>
    Connection connect(Delegate<R,A1,A2>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT,class A1, class A2>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// Specialization
template < typename R,class A1>
class Delegate<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R,A1>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call(A1 a1) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1);
            }

            inline void invoke(A1 a1) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1);
            }

            R operator()(A1 a1) const
            { return this->call(a1); }
    };
    /** Connect a Delegate to another Delegate
    */
    template <typename R,class A1>
    Connection connect(Delegate<R,A1>& delegate, Delegate<R,A1>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R,class A1>
    Connection connect(Delegate<R,A1>& delegate, const BasicSlot<R,A1>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R,class A1>
    Connection connect(Delegate<R,A1>& delegate, R(*func)(A1))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT,class A1>
    Connection connect(Delegate<R,A1>& delegate, BaseT& object, R(ClassT::*memFunc)(A1))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT,class A1>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// Specialization
template < typename R>
class Delegate<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R> Callable;

        public:
            Delegate()
            { }

            Delegate(const Delegate& other)
            {
                DelegateBase::operator=(other);
            }

            Connection connect(const BasicSlot<R>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            inline R call() const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call();
            }

            inline void invoke() const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call();
            }

            R operator()() const
            { return this->call(); }
    };
    /** Connect a Delegate to another Delegate
    */
    template <typename R>
    Connection connect(Delegate<R>& delegate, Delegate<R>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a slot
    */
    template <typename R>
    Connection connect(Delegate<R>& delegate, const BasicSlot<R>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function
    */
    template <typename R>
    Connection connect(Delegate<R>& delegate, R(*func)())
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function
    */
    template <typename R, class BaseT, class ClassT>
    Connection connect(Delegate<R>& delegate, BaseT& object, R(ClassT::*memFunc)())
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function
    */
    template <typename R, class ClassT>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)() const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


