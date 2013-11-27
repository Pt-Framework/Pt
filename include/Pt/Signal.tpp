
template <class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, 
          class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class Signal : public SignalBase
{
    public:
        typedef Invokable<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> InvokableT;

    public:
        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlots();
        }

        template <typename R>
        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
        {
            this->disconnectSlot(slot); 
        }

        inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
        {
            if(Connectable::connections().empty()) return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot
                if( it->isValid() && it->sender() == this ) {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
                }
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if(!sentry) return;
                // Break if this is the last handler
                if(it == last) break;
                ++it;
            };
        }

        inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
        { this->send(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); }
};


template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
Connection operator+=(Signal<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& signal, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
{
    return signal.connect(slot);
}


template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
void operator-=(Signal<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& signal, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
{
    signal.disconnect(slot);
}


template <class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, 
          class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class SignalSlot : public BasicSlot<void,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
{
    public:
        SignalSlot(Signal<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& signal)
        : _method( signal, &Signal<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>::send )
        {}

        Slot* clone() const
        {
            return new SignalSlot(*this); 
        }

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

        virtual bool equals(const Slot& rhs) const
        {
            const SignalSlot* ss = dynamic_cast<const SignalSlot*>(&rhs);
            return ss ? (_method == ss->_method) : false;
        }

    private:
        Method<void, Signal<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10 > _method;
};


template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
SignalSlot<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> slot( Signal<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> & signal )
{ return SignalSlot<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>( signal ); }


template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class Signal<A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> : public SignalBase
{
    public:
        typedef Invokable<A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> InvokableT;

    public:
        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlots();
        }

        template <typename R>
        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>& slot)
        {
            this->disconnectSlot(slot);
        }

        inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
        {
            if(Connectable::connections().empty()) return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot
                if( it->isValid() && it->sender() == this ) {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke(a1,a2,a3,a4,a5,a6,a7,a8,a9);
                }
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if(!sentry) return;
                // Break if this is the last handler
                if(it == last) break;
                ++it;
            };
        }

        inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
        { this->send(a1,a2,a3,a4,a5,a6,a7,a8,a9); }
};


template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class Signal<A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> : public SignalBase
{
    public:
        typedef Invokable<A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> InvokableT;

    public:
        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlots();
        }

        template <typename R>
        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>& slot)
        {
            this->disconnectSlot(slot);
        }

        inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
        {
            if(Connectable::connections().empty()) return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot
                if( it->isValid() && it->sender() == this ) {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke(a1,a2,a3,a4,a5,a6,a7,a8);
                }
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if(!sentry) return;
                // Break if this is the last handler
                if(it == last) break;
                ++it;
            };
        }

        inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
        { this->send(a1,a2,a3,a4,a5,a6,a7,a8); }
};


template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Signal<A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> : public SignalBase
{
    public:
        typedef Invokable<A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> InvokableT;

    public:
        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlots();
        }

        template <typename R>
        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>& slot)
        {
            this->disconnectSlot(slot);
        }

        inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) 
        {
            if(Connectable::connections().empty()) return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot
                if( it->isValid() && it->sender() == this ) {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke(a1,a2,a3,a4,a5,a6,a7);
                }
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if(!sentry) return;
                // Break if this is the last handler
                if(it == last) break;
                ++it;
            };
        }

        inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) 
        { this->send(a1,a2,a3,a4,a5,a6,a7); }
};


template <class A1, class A2, class A3, class A4, class A5, class A6>
class Signal<A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> : public SignalBase
{
    public:
        typedef Invokable<A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> InvokableT;

    public:
        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlots();
        }

        template <typename R>
        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>& slot)
        {
            this->disconnectSlot(slot);
        }

        inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) 
        {
            if(Connectable::connections().empty()) return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot
                if( it->isValid() && it->sender() == this ) {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke(a1,a2,a3,a4,a5,a6);
                }
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if(!sentry) return;
                // Break if this is the last handler
                if(it == last) break;
                ++it;
            };
        }

        inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) 
        { this->send(a1,a2,a3,a4,a5,a6); }
};


template <class A1, class A2, class A3, class A4, class A5>
class Signal<A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> : public SignalBase
{
    public:
        typedef Invokable<A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> InvokableT;

    public:
        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlots();
        }

        template <typename R>
        void disconnect(const BasicSlot<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>& slot)
        {
            this->disconnectSlot(slot);
        }

        inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) 
        {
            if(Connectable::connections().empty()) return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot
                if( it->isValid() && it->sender() == this ) {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke(a1,a2,a3,a4,a5);
                }
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if(!sentry) return;
                // Break if this is the last handler
                if(it == last) break;
                ++it;
            };
        }

        inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) 
        { this->send(a1,a2,a3,a4,a5); }
};


template <class A1, class A2, class A3, class A4>
class Signal<A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> : public SignalBase
{
    public:
        typedef Invokable<A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> InvokableT;

    public:
        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlots();
        }

        template <typename R>
        void disconnect(const BasicSlot<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>& slot)
        {
            this->disconnectSlot(slot);
        }

        inline void send(A1 a1, A2 a2, A3 a3, A4 a4) 
        {
            if(Connectable::connections().empty()) return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot
                if( it->isValid() && it->sender() == this ) {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke(a1,a2,a3,a4);
                }
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if(!sentry) return;
                // Break if this is the last handler
                if(it == last) break;
                ++it;
            };
        }

        inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4) 
        { this->send(a1,a2,a3,a4); }
};


template <class A1, class A2, class A3>
class Signal<A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> : public SignalBase
{
    public:
        typedef Invokable<A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> InvokableT;

    public:
        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlots();
        }

        template <typename R>
        void disconnect(const BasicSlot<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>& slot)
        {
            this->disconnectSlot(slot);
        }

        inline void send(A1 a1, A2 a2, A3 a3) 
        {
            if(Connectable::connections().empty()) return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot
                if( it->isValid() && it->sender() == this ) {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke(a1,a2,a3);
                }
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if(!sentry) return;
                // Break if this is the last handler
                if(it == last) break;
                ++it;
            };
        }

        inline void operator()(A1 a1, A2 a2, A3 a3) 
        { this->send(a1,a2,a3); }
};


template <class A1, class A2>
class Signal<A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> : public SignalBase
{
    public:
        typedef Invokable<A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> InvokableT;

    public:
        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlot(); 
        }

        template <typename R>
        void disconnect(const BasicSlot<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>& slot)
        {
            this->disconnectSlot(slot);
        }

        inline void send(A1 a1, A2 a2) 
        {
            if(Connectable::connections().empty()) return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot
                if( it->isValid() && it->sender() == this ) {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke(a1,a2);
                }
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if(!sentry) return;
                // Break if this is the last handler
                if(it == last) break;
                ++it;
            };
        }

        inline void operator()(A1 a1, A2 a2) 
        { this->send(a1,a2); }
};


template <class A1>
class Signal<A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public SignalBase
{
    public:
        typedef Invokable<A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> InvokableT;

    public:

        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlots(); 
        }

        template <typename R>
        void disconnect(const BasicSlot<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>& slot)
        {
            this->disconnectSlot(slot);
        }

        inline void send(A1 a1) 
        {
            if(Connectable::connections().empty()) return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot
                if( it->isValid() && it->sender() == this ) {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke(a1);
                }
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if(!sentry) return;
                // Break if this is the last handler
                if(it == last) break;
                ++it;
            };
        }

        inline void operator()(A1 a1) 
        { this->send(a1); }
};


template <>
class Signal<Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public SignalBase 
{
    public:
        typedef Invokable<Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> InvokableT;

    public:
        Signal()
        { }

        Signal(const Signal& rhs)
        {
            Signal::operator=(rhs);
        }

        template <typename R>
        Connection connect(const BasicSlot<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>& slot)
        {
            return Connection(*this, slot.clone() );
        }

        void disconnect()
        {
            this->disconnectSlots();
        }

        template <typename R>
        void disconnect(const BasicSlot<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>& slot)
        {
            this->disconnectSlot(slot); 
        }

        inline void send()
        {
            if( Connectable::connections().empty() ) 
                return;

            // The sentry will set the Signal to the sending state and reset it to not-sending upon destruction.
            // In the sending state, removing connection will leave invalid connections in the connection list
            // to keep the iterator valid, but mark the Signal dirty. If the Signal is dirty, all invalid connections
            // will be removed by the Sentry when it destructs.
            SignalBase::Sentry sentry(this);

            std::list<Connection>::const_iterator it   = Connectable::connections().begin();
            std::list<Connection>::const_iterator last = --Connectable::connections().end();
            while(true) 
            {
                // The following scenarios must be considered when the slot is called:
                // - The slot might get deleted and thus disconnected from this signal
                // - The slot might delete this signal and we must end calling any slots immediately
                // - A new Connection might get added to this Signal in the slot

                if( it->isValid() && it->sender() == this ) 
                {
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot()->callable() );
                    invokable->invoke();
                }
                    
                // If this signal gets deleted by the slot, the Sentry will be detached. In this case we bail out immediately
                if( ! sentry)
                    return;

                // Break if this is the last handler
                if(it == last) 
                    break;
                    
                ++it;
            };
        }

        inline void operator()()
        { this->send(); }
};
