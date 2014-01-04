
template < typename R, class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, 
                       class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class Delegate : public DelegateBase
{
    public:
        typedef Callable<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
        {
            if( !_target.isValid() ) {
                throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>::call(): Delegate not connected");
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
        }

        inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
        {
            if( !_target.isValid() ) {
                return;
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
        }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
        { return this->call(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); }
};


template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
Connection operator +=(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
{
    return delegate.connect(slot);
}


template < typename R, class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, 
                       class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class DelegateSlot : public BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
{
    public:
        DelegateSlot(Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate)
        : _method( delegate, &Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>::call )
        {}

        Slot* clone() const
        { return new DelegateSlot(*this); }

        virtual const void* callable() const
        {
            return &_method;
        }

        virtual void onConnect(const Connection& c)
        {
            _method.object().onConnectionOpen(c);
        }

        virtual void onDisconnect(const Connection& c)
        {
            _method.object().onConnectionClose(c);
        }

        virtual bool equals(const Slot& slot) const
        {
            const DelegateSlot* ds = dynamic_cast<const DelegateSlot*>(&slot);
            return ds ? (_method == ds->_method) : false;
        }

    private:
        Method<R, Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10 > _method;
};


template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
DelegateSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> slot( Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate )
{ return DelegateSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>( delegate ); }


template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> : public DelegateBase
{
    public:
        typedef Callable<R,A1,A2,A3,A4,A5,A6,A7,A8,A9> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
        {
            if( !_target.isValid() ) {
                throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>::call(): Delegate not connected");
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9);
        }

        inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
        {
            if( !_target.isValid() ) {
                return;
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9);
        }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
        { return this->call(a1,a2,a3,a4,a5,a6,a7,a8,a9); }
};


template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> : public DelegateBase
{
    public:
        typedef Callable<R,A1,A2,A3,A4,A5,A6,A7,A8> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
        {
            if( !_target.isValid() ) {
                throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>::call(): Delegate not connected");
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call(a1,a2,a3,a4,a5,a6,a7,a8);
        }

        inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
        {
            if( !_target.isValid() ) {
                return;
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call(a1,a2,a3,a4,a5,a6,a7,a8);
        }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
        { return this->call(a1,a2,a3,a4,a5,a6,a7,a8); }
};


template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Delegate<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> : public DelegateBase
{
    public:
        typedef Callable<R,A1,A2,A3,A4,A5,A6,A7> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
        {
            if( !_target.isValid() ) {
                throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5,A6,A7>::call(): Delegate not connected");
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call(a1,a2,a3,a4,a5,a6,a7);
        }

        inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
        {
            if( !_target.isValid() ) {
                return;
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call(a1,a2,a3,a4,a5,a6,a7);
        }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
        { return this->call(a1,a2,a3,a4,a5,a6,a7); }
};



template < typename R,class A1, class A2, class A3, class A4, class A5, class A6>
class Delegate<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> : public DelegateBase
{
    public:
        typedef Callable<R,A1,A2,A3,A4,A5,A6> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
        {
            if( !_target.isValid() ) {
                throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5,A6>::call(): Delegate not connected");
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call(a1,a2,a3,a4,a5,a6);
        }

        inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
        {
            if( !_target.isValid() ) {
                return;
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call(a1,a2,a3,a4,a5,a6);
        }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
        { return this->call(a1,a2,a3,a4,a5,a6); }
};


template < typename R,class A1, class A2, class A3, class A4, class A5>
class Delegate<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> : public DelegateBase
{
    public:
        typedef Callable<R,A1,A2,A3,A4,A5> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) 
        {
            if( !_target.isValid() ) {
                throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5>::call(): Delegate not connected");
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call(a1,a2,a3,a4,a5);
        }

        inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) 
        {
            if( !_target.isValid() ) {
                return;
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call(a1,a2,a3,a4,a5);
        }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) 
        { return this->call(a1,a2,a3,a4,a5); }
};


template < typename R,class A1, class A2, class A3, class A4>
class Delegate<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> : public DelegateBase
{
    public:
        typedef Callable<R,A1,A2,A3,A4> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R,A1,A2,A3,A4>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call(A1 a1, A2 a2, A3 a3, A4 a4)
        {
            if( !_target.isValid() ) {
                throw std::logic_error("Delegate<R,A1,A2,A3,A4>::call(): Delegate not connected");
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call(a1,a2,a3,a4);
        }

        inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4)
        {
            if( !_target.isValid() ) {
                return;
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call(a1,a2,a3,a4);
        }

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4)
        { return this->call(a1,a2,a3,a4); }
};


template < typename R,class A1, class A2, class A3>
class Delegate<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
{
    public:
        typedef Callable<R,A1,A2,A3> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R,A1,A2,A3>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call(A1 a1, A2 a2, A3 a3)
        {
            if( !_target.isValid() ) {
                throw std::logic_error("Delegate<R,A1,A2,A3>::call(): Delegate not connected");
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call(a1,a2,a3);
        }

        inline void invoke(A1 a1, A2 a2, A3 a3)
        {
            if( !_target.isValid() ) {
                return;
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call(a1,a2,a3);
        }

        R operator()(A1 a1, A2 a2, A3 a3)
        { return this->call(a1,a2,a3); }
};


template < typename R,class A1, class A2>
class Delegate<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
{
    public:
        typedef Callable<R,A1,A2> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R,A1,A2>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call(A1 a1, A2 a2)
        {
            if( !_target.isValid() ) {
                throw std::logic_error("Delegate<R,A1,A2>::call(): Delegate not connected");
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call(a1,a2);
        }

        inline void invoke(A1 a1, A2 a2)
        {
            if( !_target.isValid() ) {
                return;
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call(a1,a2);
        }

        R operator()(A1 a1, A2 a2)
        { return this->call(a1,a2); }
};


template < typename R,class A1>
class Delegate<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
{
    public:
        typedef Callable<R,A1> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R,A1>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call(A1 a1)
        {
            if( !_target.isValid() ) {
                throw std::logic_error("Delegate<R,A1>::call(): Delegate not connected");
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call(a1);
        }

        inline void invoke(A1 a1)
        {
            if( !_target.isValid() ) {
                return;
            }
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call(a1);
        }

        R operator()(A1 a1)
        { return this->call(a1); }
};


template < typename R>
class Delegate<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
{
    public:
        typedef Callable<R> CallableT;

    public:
        Delegate()
        { }

        Delegate(const Delegate& rhs)
        {
            DelegateBase::operator=(rhs);
        }

        Connection connect(const BasicSlot<R>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        { DelegateBase::disconnectSlot(); }

        void disconnect(const BasicSlot<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>& slot)
        { DelegateBase::disconnectSlot(slot); }

        inline R call()
        {
            if( ! _target.isValid() )
                throw std::logic_error("Delegate not connected");

            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            return cb->call();
        }

        inline void invoke()
        {
            if( ! _target.isValid() )
                return;
 
            const CallableT* cb = static_cast<const CallableT*>( _target.slot()->callable() );
            cb->call();
        }

        R operator()()
        { return this->call(); }
};
