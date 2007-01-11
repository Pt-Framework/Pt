/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"

#include <ptv/db/Connection.h>
#include <ptv/db/Transaction.h>
#include <ptv/db/Result.h>
#include <ptv/system/File.h>

#include <fstream>
#include <cassert>
#include <stddef.h>

using namespace ptv;
using namespace std;


class SqliteTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( SqliteTest );
	CPPUNIT_TEST( testConnection );
	CPPUNIT_TEST( testCreateTable );
	CPPUNIT_TEST( testInsert );
	CPPUNIT_TEST( testSelect );
	CPPUNIT_TEST( testDelete );
	CPPUNIT_TEST( testPragma );
	CPPUNIT_TEST_SUITE_END();

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
	void fillTable(ptv::db::Connection& con);
};


CPPUNIT_TEST_SUITE_REGISTRATION( SqliteTest );


void SqliteTest::setUp()
{

}

void SqliteTest::tearDown()
{
	try
	{
		ptv::db::Connection con = ptv::db::connect("sqlite:SqliteTest.db");
		con.execute("DROP TABLE TestTable");
	}
	catch(...){}
	
	try
	{
		ptv::system::File("SqliteTest.db").remove();
	}
	catch(...){}
	
	try
	{
		ptv::system::File("SqliteTestPragma.db").remove();
	}
	catch(...){}
}


void SqliteTest::fillTable(ptv::db::Connection& con)
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
	ptv::db::Connection con = ptv::db::connect("sqlite:SqliteTest.db");
}


void SqliteTest::testCreateTable()
{
	ptv::db::Connection con = ptv::db::connect("sqlite:SqliteTest.db");
	ptv::db::Transaction tact(con);
	
	con.execute("CREATE TABLE TestTable (name,age,salary);");
	
	ptv::db::Result result = con.select("SELECT * FROM TestTable");
	CPPUNIT_ASSERT( result.size() == 0);
	
	tact.commit();
}

void SqliteTest::testInsert()
{
	ptv::db::Connection con = ptv::db::connect("sqlite:SqliteTest.db");
	ptv::db::Transaction tact(con);
	
	con.execute("CREATE TABLE TestTable (name,age,salary);");
	
	fillTable(con);

	ptv::db::Result result = con.select("SELECT * FROM TestTable");
	CPPUNIT_ASSERT( result.size() == 7);
	
	tact.commit();
}

void SqliteTest::testSelect()
{
	ptv::db::Connection con = ptv::db::connect("sqlite:SqliteTest.db");
	ptv::db::Transaction tact(con);
	
	con.execute("CREATE TABLE TestTable (name,age,salary);");
	
	fillTable(con);
	
	ptv::db::Result result = con.select("SELECT * FROM TestTable WHERE age > 30 AND age < 60");
	CPPUNIT_ASSERT( result.size() == 4 );
	
	ptv::db::Row row = con.selectRow("SELECT * FROM TestTable WHERE name = 'Angelique'");
	CPPUNIT_ASSERT( row.size() == 3 );
	
	ptv::db::Value val = con.selectValue("SELECT salary FROM TestTable WHERE age = 48");
	CPPUNIT_ASSERT( val.getUnsigned() == 8000 );

	tact.commit();
}

void SqliteTest::testDelete()
{
	ptv::db::Connection con = ptv::db::connect("sqlite:SqliteTest.db");
	ptv::db::Transaction tact(con);
	
	con.execute("CREATE TABLE TestTable (name,age,salary);");
	
	fillTable(con);
	
	//delet one data table entry
	con.execute("DELETE FROM TestTable WHERE salary > 50000");	
	ptv::db::Result result = con.select("SELECT * FROM TestTable");
	CPPUNIT_ASSERT( result.size() == 6 );
	
	//delete whole table
	con.execute("DELETE FROM TestTable");	
	result = con.select("SELECT * FROM TestTable");
	CPPUNIT_ASSERT( result.size() == 0 );
	
	tact.commit();
}

void SqliteTest::testPragma()
{
	ptv::db::Connection con = ptv::db::connect("sqlite:SqliteTestPragma.db");
	ptv::db::Transaction tact(con);
	ptv::db::Value result;
	
	con.execute("PRAGMA auto_vacuum = 1"); // 0 | 1 - reclaim unused space or not
	result = con.selectValue("PRAGMA auto_vacuum");
	CPPUNIT_ASSERT( result.getInt() == 1 );
		
	con.execute("PRAGMA cache_size = 3500"); // No. of cached pages
	result = con.selectValue("PRAGMA cache_size");
	CPPUNIT_ASSERT( result.getInt() == 3500 );

	con.execute("PRAGMA count_changes = 1"); // 0 | 1 - if set INSERT, UPDATE, DELETE return No. of changes
	result = con.selectValue("PRAGMA count_changes");
	CPPUNIT_ASSERT( result.getInt() == 1 );

	con.execute("PRAGMA page_size = 4096"); // size of one page in bytes
	result = con.selectValue("PRAGMA page_size");
	CPPUNIT_ASSERT( result.getInt() == 4096 );

	tact.commit();
}
