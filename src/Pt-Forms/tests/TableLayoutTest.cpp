/* Copyright (C) 2026 Marc Boris Duerner

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 As a special exception, you may use this file as part of a free
 software library without restriction. Specifically, if other files
 instantiate templates or use macros or inline functions from this
 file, or you compile this file and link it with other files to
 produce an executable, this file does not by itself cause the
 resulting executable to be covered by the GNU General Public
 License. This exception does not however invalidate any other
 reasons why the executable file might be covered by the GNU Library
 General Public License.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA 02110-1301 USA
*/

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/TableLayout.h>
#include <cmath>

namespace Pt {

namespace Forms {

class StubControl : public Control
{
    public:
        explicit StubControl(const Gfx::SizeF& size, double wrapArea = 0)
        : _size(size)
        , _wrapArea(wrapArea)
        , _measureCount(0)
        {
            onShow(true);
        }

        std::size_t measureCount() const
        { return _measureCount; }

        const SizePolicy& lastPolicy() const
        { return _lastPolicy; }

        const Gfx::SizeF& lastResize() const
        { return _lastResize; }

        void setShown(bool shown)
        { onShow(shown); }

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy)
        {
            ++_measureCount;
            _lastPolicy = policy;

            if(_wrapArea > 0 && policy.width() > 0)
            {
                return Gfx::SizeF( _size.width(),
                                   _wrapArea / policy.width() );
            }

            return _size;
        }

        virtual void onRequestResize(const Gfx::SizeF& s)
        {
            _lastResize = s;
            Control::onRequestResize(s);
        }

    private:
        Gfx::SizeF  _size;
        double      _wrapArea;
        std::size_t _measureCount;
        SizePolicy  _lastPolicy;
        Gfx::SizeF  _lastResize;
};


class TestTable : public TableLayout2
{
    public:
        void layoutRect(const Gfx::RectF& rect)
        { onLayout(rect); }
};


class TestFlow : public FlowLayout
{
    public:
        TestFlow()
        { onShow(true); }
};


class TableLayoutTest : public Unit::TestSuite
{
    public:
        TableLayoutTest()
        : Unit::TestSuite("Pt::Forms::TableLayoutTest")
        {
            Unit::TestSuite::registerMethod("MultiRowPreferred", *this,
                                            &TableLayoutTest::MultiRowPreferred);
            Unit::TestSuite::registerMethod("VerticalFill", *this,
                                            &TableLayoutTest::VerticalFill);
            Unit::TestSuite::registerMethod("NestedFill", *this,
                                            &TableLayoutTest::NestedFill);
            Unit::TestSuite::registerMethod("Wrap", *this,
                                            &TableLayoutTest::Wrap);
            Unit::TestSuite::registerMethod("HiddenCell", *this,
                                            &TableLayoutTest::HiddenCell);
            Unit::TestSuite::registerMethod("TwoFillRows", *this,
                                            &TableLayoutTest::TwoFillRows);
            Unit::TestSuite::registerMethod("LeftoverClamp", *this,
                                            &TableLayoutTest::LeftoverClamp);
            Unit::TestSuite::registerMethod("RemoveItem", *this,
                                            &TableLayoutTest::RemoveItem);
        }

    protected:
        void MultiRowPreferred()
        {
            Application app;
            TestTable table;
            StubControl top( Gfx::SizeF(80, 10) );
            StubControl bottom( Gfx::SizeF(50, 10) );

            table.addItem(top, 0, 0);
            table.addItem(bottom, 1, 0);

            SizePolicy policy(SizePolicy::Preferred, SizePolicy::Preferred);
            policy.setSize(1000, 1000);
            table.measure(policy);

            PT_UNIT_ASSERT_NEAR(table.preferredSize().width(), 80.0);
            PT_UNIT_ASSERT_NEAR(table.preferredSize().height(), 20.0);
        }

        void VerticalFill()
        {
            Application app;
            TestTable table;
            StubControl preferred( Gfx::SizeF(20, 40) );
            StubControl fill( Gfx::SizeF(20, 10) );

            table.setRow(0, TableLayout2::Preferred);
            table.setRow(1, TableLayout2::Fill);
            table.addItem(preferred, 0, 0);
            table.addItem(fill, 1, 0);

            SizePolicy policy(SizePolicy::Preferred, SizePolicy::Preferred);
            policy.setSize(200, 200);
            table.measure(policy);

            PT_UNIT_ASSERT_NEAR(fill.preferredSize().height(), 160.0);
            PT_UNIT_ASSERT_NEAR(table.preferredSize().height(), 200.0);
        }

        void NestedFill()
        {
            Application app;
            TestTable table;
            StubControl preferred( Gfx::SizeF(20, 40) );
            TestFlow flow;
            StubControl inner( Gfx::SizeF(10, 10) );

            flow.addItem(inner);

            table.setRow(0, TableLayout2::Preferred);
            table.setRow(1, TableLayout2::Fill);
            table.setColumn(0, TableLayout2::Fill);
            table.addItem(preferred, 0, 0);
            table.addItem(flow, 1, 0);

            SizePolicy policy(SizePolicy::Preferred, SizePolicy::Preferred);
            policy.setSize(200, 200);
            table.measure(policy);

            PT_UNIT_ASSERT_NEAR(inner.lastPolicy().height(), 160.0);
            PT_UNIT_ASSERT(inner.lastPolicy().height() < 200.0);
        }

        void Wrap()
        {
            Application app;
            TestTable table;
            StubControl wrap(Gfx::SizeF(10, 10), 700);

            table.setColumn(0, TableLayout2::Fixed, 30);
            table.setColumn(1, TableLayout2::Fill);
            table.setRow(0, TableLayout2::Preferred);
            table.addItem(wrap, 0, 1);

            SizePolicy policy(SizePolicy::Preferred, SizePolicy::Preferred);
            policy.setSize(100, 1000);
            table.measure(policy);

            PT_UNIT_ASSERT_NEAR(wrap.lastPolicy().width(), 70.0);
            PT_UNIT_ASSERT_NEAR(table.preferredSize().height(), 10.0);
            PT_UNIT_ASSERT_NEAR(table.preferredSize().width(), 100.0);
        }

        void HiddenCell()
        {
            Application app;
            TestTable table;
            StubControl hidden( Gfx::SizeF(20, 80) );
            StubControl visible( Gfx::SizeF(20, 10) );

            table.addItem(hidden, 0, 0);
            table.addItem(visible, 1, 0);
            hidden.setShown(false);

            SizePolicy policy(SizePolicy::Preferred, SizePolicy::Preferred);
            policy.setSize(1000, 1000);
            table.measure(policy);

            PT_UNIT_ASSERT_NEAR(table.preferredSize().height(), 10.0);
        }

        void TwoFillRows()
        {
            Application app;
            TestTable table;
            StubControl top( Gfx::SizeF(10, 10) );
            StubControl bottom( Gfx::SizeF(10, 10) );

            table.setRow(0, TableLayout2::Fill);
            table.setRow(1, TableLayout2::Fill);
            table.addItem(top, 0, 0);
            table.addItem(bottom, 1, 0);

            SizePolicy policy(SizePolicy::Preferred, SizePolicy::Preferred);
            policy.setSize(100, 101);
            table.measure(policy);

            const double topH = top.preferredSize().height();
            const double bottomH = bottom.preferredSize().height();
            PT_UNIT_ASSERT_NEAR(topH + bottomH, 101.0);
            PT_UNIT_ASSERT(std::abs(topH - bottomH) <= 1.0);
        }

        void LeftoverClamp()
        {
            Application app;
            TestTable table;
            StubControl preferred( Gfx::SizeF(50, 80) );
            StubControl fill( Gfx::SizeF(50, 10) );

            table.setRow(0, TableLayout2::Preferred);
            table.setRow(1, TableLayout2::Fill);
            table.addItem(preferred, 0, 0);
            table.addItem(fill, 1, 0);

            SizePolicy policy(SizePolicy::Preferred, SizePolicy::Preferred);
            policy.setSize(40, 40);
            table.measure(policy);
            table.layoutRect( Gfx::RectF(40, 40) );

            PT_UNIT_ASSERT(fill.lastResize().width() >= 0);
            PT_UNIT_ASSERT(fill.lastResize().height() >= 0);
        }

        void RemoveItem()
        {
            Application app;
            TestTable table;
            StubControl item( Gfx::SizeF(50, 50) );

            table.addItem(item, 0, 0);
            table.removeItem(item);
            PT_UNIT_ASSERT_NOTHROW( table.removeItem(item) );

            SizePolicy policy(SizePolicy::Preferred, SizePolicy::Preferred);
            policy.setSize(200, 200);
            table.measure(policy);
            PT_UNIT_ASSERT_NEAR(table.preferredSize().width(), 0.0);
            PT_UNIT_ASSERT_NEAR(table.preferredSize().height(), 0.0);

            table.addItem(item, 0, 0);
            table.measure(policy);
            PT_UNIT_ASSERT_NEAR(table.preferredSize().width(), 50.0);
            PT_UNIT_ASSERT_NEAR(table.preferredSize().height(), 50.0);
        }
};

Unit::RegisterTest<TableLayoutTest> _registerTableLayoutTest;

} // namespace Forms

} // namespace Pt
