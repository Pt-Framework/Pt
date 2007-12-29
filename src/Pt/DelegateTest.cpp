/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
 *   Copyright (C) 2005 Stephan Beal                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#undef PT_API_EXPORT

#include "Pt/Delegate.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"


class Callee : public Pt::Connectable
{
    public:
        Callee()
        : _count(0)
        {}

        int slot0()
        { ++_count; return 9;}

        int destroySelf()
        {
            delete this;
            return 0;
        }

        int count() const
        { return _count; }

        void reset()
        { _count = 0; }

    private:
        int _count;
};


class DelegateTest : public Pt::Unit::TestSuite, public Pt::Connectable
{
    public:
        DelegateTest()
        : Pt::Unit::TestSuite("DelegateTest")
        , _caller(0)
        {
            Pt::Unit::TestSuite::registerMethod( "DeleteWhileCall", *this, &DelegateTest::DeleteWhileCall );
            Pt::Unit::TestSuite::registerMethod( "Copy", *this, &DelegateTest::Copy );
            Pt::Unit::TestSuite::registerMethod( "Call0", *this, &DelegateTest::Call0 );
            Pt::Unit::TestSuite::registerMethod( "ChainDelegates", *this, &DelegateTest::ChainDelegates );
        }

        virtual void setUp()
        {
            _caller = new Pt::Delegate<int>;
        }

        virtual void tearDown()
        {
            delete _caller;
            _caller = 0;
        }

    protected:
        void Call0()
        {
            // A connect must lead to a new connection
            Callee* recv = new Callee;
            Pt::Delegate<int> delegate;
            connect( delegate, *recv, &Callee::slot0 );
            PT_UNIT_ASSERT(delegate.connectionCount() == 1)

            // A deleted receiver must remove itself from a signal
            delete recv;
            delegate.invoke();
            PT_UNIT_ASSERT(delegate.connectionCount() == 0)

            // A delegate must call its slot when connected
            recv = new Callee;
            Pt::Connection connection = connect(delegate, *recv, &Callee::slot0 );
            int ret = delegate.call();
            PT_UNIT_ASSERT( recv->count() == 1)
            PT_UNIT_ASSERT( ret == 9)
            recv->reset();

            // Closing connections must remove them
            connection.close();
            delegate.invoke();
            PT_UNIT_ASSERT( recv->count() == 0 )
            PT_UNIT_ASSERT( delegate.connectionCount() == 0)

            delete recv;
        }

        void ChainDelegates()
        {
            Callee* recv = new Callee;
            Pt::Delegate<int> d1;
            Pt::Delegate<int> d2;

            connect( d1, slot(d2) );
            PT_UNIT_ASSERT( d1.connectionCount() == 1)

            connect( d2, slot(*recv, &Callee::slot0) );
            PT_UNIT_ASSERT( d2.connectionCount() == 2)

            // Slot must be called via delegate chain
            d1.call();
            PT_UNIT_ASSERT( recv->count() == 1 )

            delete recv;
        }

        void Copy()
        {
            Callee callee;
            Pt::Delegate<int>* d1 = new Pt::Delegate<int>;
            Pt::Delegate<int> d2;

            Pt::Connection connection1 = connect(*d1, callee, &Callee::slot0);
            PT_UNIT_ASSERT( d1->connectionCount() == 1)

            d2 = *d1;
            PT_UNIT_ASSERT( d2.connectionCount() == 1)

            connection1.close();
            PT_UNIT_ASSERT( d1->connectionCount() == 0)
            delete d1;

            d2.call();
            PT_UNIT_ASSERT( callee.count() == 1 )
        }

        void DeleteWhileCall()
        {
            // will get deleted by slot
            Callee* callee = new Callee;

            connect(*_caller, *callee, &Callee::destroySelf );
            _caller->call();

            connect(*_caller, *this, &DelegateTest::deleteCaller);
            _caller->call();
        }

        int deleteCaller()
        {
            delete _caller;
            _caller = 0;
            return 0;
        }

    private:
        Pt::Delegate<int>* _caller;
};

Pt::Unit::RegisterTest<DelegateTest> register_DelegateTest;
