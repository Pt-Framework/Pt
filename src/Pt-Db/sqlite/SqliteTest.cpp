/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <Pt/Db/Connection.h>
#include <Pt/Db/Transaction.h>
#include <Pt/Db/Result.h>
#include <Pt/System/File.h>

#include <fstream>
#include <cassert>
#include <stddef.h>


class SqliteTest : public Pt::Unit::TestSuite
{

    public:
        SqliteTest()
        : Pt::Unit::TestSuite("SqliteTest")
        {
            this->registerMethod( "Connection", *this, &SqliteTest::testConnection );
            this->registerMethod( "CreateTable", *this, &SqliteTest::testCreateTable );
            this->registerMethod( "Insert", *this, &SqliteTest::testInsert );
            this->registerMethod( "Select", *this, &SqliteTest:: testSelect );
            this->registerMethod( "Delete", *this, &SqliteTest::testDelete );
            this->registerMethod( "Pragma", *this, &SqliteTest::testPragma );
        }

    public:
        void setUp();
        void tearDown();

    protected:
        void testConnection();
        void testCreateTable();
        void testInsert();
        void testSelect();
        void testDelete();
        void testPragma();

    private:
        void fillTable(Pt::Db::Connection& con);
};

Pt::Unit::RegisterTest<SqliteTest> register_SqliteTest;



void SqliteTest::setUp()
{

}

void SqliteTest::tearDown()
{
    try
    {
        Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTest.db");
        con.execute("DROP TABLE TestTable");
    }
    catch(...){}

    try
    {
        Pt::System::File("SqliteTest.db").remove();
    }
    catch(...){}

    try
    {
        Pt::System::File("SqliteTestPragma.db").remove();
    }
    catch(...){}
}


void SqliteTest::fillTable(Pt::Db::Connection& con)
{
    con.execute("INSERT INTO TestTable (name,age,salary) VALUES ('Hans',50,40000)");
    con.execute("INSERT INTO TestTable (name,age,salary) VALUES ('Fritz',26,20000)");
    con.execute("INSERT INTO TestTable (name,age,salary) VALUES ('Karl',18,39500)");
    con.execute("INSERT INTO TestTable (name,age,salary) VALUES ('Ines',63,25800)");
    con.execute("INSERT INTO TestTable (name,age,salary) VALUES ('Albert',48,8000)");
    con.execute("INSERT INTO TestTable (name,age,salary) VALUES ('Jennifer',35,15800)");
    con.execute("INSERT INTO TestTable (name,age,salary) VALUES ('Angelique',32,68400)");
}


void SqliteTest::testConnection()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTest.db");
}


void SqliteTest::testCreateTable()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTest.db");
    Pt::Db::Transaction tact(con);

    con.execute("CREATE TABLE TestTable (name,age,salary);");

    Pt::Db::Result result = con.select("SELECT * FROM TestTable");
    PT_UNIT_ASSERT( result.size() == 0);

    tact.commit();
}

void SqliteTest::testInsert()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTest.db");
    Pt::Db::Transaction tact(con);

    con.execute("CREATE TABLE TestTable (name,age,salary);");

    fillTable(con);

    Pt::Db::Result result = con.select("SELECT * FROM TestTable");
    PT_UNIT_ASSERT( result.size() == 7);

    tact.commit();
}

void SqliteTest::testSelect()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTest.db");
    Pt::Db::Transaction tact(con);

    con.execute("CREATE TABLE TestTable (name,age,salary);");

    fillTable(con);

    Pt::Db::Result result = con.select("SELECT * FROM TestTable WHERE age > 30 AND age < 60");
    PT_UNIT_ASSERT( result.size() == 4 );

    Pt::Db::Row row = con.selectRow("SELECT * FROM TestTable WHERE name = 'Angelique'");
    PT_UNIT_ASSERT( row.size() == 3 );

    Pt::Db::Value val = con.selectValue("SELECT salary FROM TestTable WHERE age = 48");
    PT_UNIT_ASSERT( val.getUnsigned() == 8000 );

    tact.commit();
}

void SqliteTest::testDelete()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTest.db");
    Pt::Db::Transaction tact(con);

    con.execute("CREATE TABLE TestTable (name,age,salary);");

    fillTable(con);

    //delet one data table entry
    con.execute("DELETE FROM TestTable WHERE salary > 50000");    
    Pt::Db::Result result = con.select("SELECT * FROM TestTable");
    PT_UNIT_ASSERT( result.size() == 6 );

    //delete whole table
    con.execute("DELETE FROM TestTable");    
    result = con.select("SELECT * FROM TestTable");
    PT_UNIT_ASSERT( result.size() == 0 );

    tact.commit();
}

void SqliteTest::testPragma()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTestPragma.db");
    Pt::Db::Transaction tact(con);
    Pt::Db::Value result;

    // Note that testing against PRAGMAs is not a good idea because PRAGMAs are
    // "volitile", in that they can change from one release to the next with
    // no warning. --stephan

    con.execute("PRAGMA auto_vacuum = 1"); // 0 | 1 - reclaim unused space or not
    result = con.selectValue("PRAGMA auto_vacuum");
    PT_UNIT_ASSERT( result.getInt() == 1 );

    con.execute("PRAGMA cache_size = 3500"); // No. of cached pages
    result = con.selectValue("PRAGMA cache_size");
    PT_UNIT_ASSERT( result.getInt() == 3500 );

    con.execute("PRAGMA count_changes = 1"); // 0 | 1 - if set INSERT, UPDATE, DELETE return No. of changes
    result = con.selectValue("PRAGMA count_changes");
    PT_UNIT_ASSERT( result.getInt() == 1 );

#if 0
    // This test fails in sqlite 3.5.2:
    con.execute("PRAGMA page_size = 4096"); // size of one page in bytes
    result = con.selectValue("PRAGMA page_size");
    PT_UNIT_ASSERT( result.getInt() == 4096 );
#endif // 0|1

    tact.commit();
}
