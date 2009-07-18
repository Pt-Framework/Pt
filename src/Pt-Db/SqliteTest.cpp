/*
 * Copyright (C) 2007 by Marc Boris Duerner
 * 
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
#undef PT_DB_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <Pt/Db/Connection.h>
#include <Pt/Db/Transaction.h>
#include <Pt/Db/Result.h>
#include <Pt/System/File.h>
#include <Pt/System/Thread.h>

#include <fstream>
#include <cassert>
#include <cstddef>

/**
 * @brief Helper class using for the write or read access test to the database. 
 *
 * Implements a Runnable which runs in a loop until a stop() API is called. Before use this class, a database must be
 * already created and also contains table name, age and salary.
 */
class AccessDbRunnable
{
public:

    /**
     * @brief Constructor.
     *
     * @param accessWrite Indicate whether the helper class is used for the database write access or read access test.
     * @param db The connection instance to the target database.
     * @param accessFreq Number of write or read access to the target database.
     * @param startValue The first value added to "40000" for the field "salary". This value will be incremented depends 
     *        on the accessFreq within the read and write methods.
     */
    AccessDbRunnable(const bool accessWrite, const Pt::Db::Connection& db, const Pt::uint32_t accessFreq, const Pt::uint32_t& startValue = 0)
    : _exit(false)
    , _accessWrite(accessWrite)
    , _db(db)
    , _accessFreq(accessFreq)
    , _startValue(startValue)
    , _success(false)
    {
    }

    /**
     * @brief Alters a member variable to break the loop in the run() method.
     */
    void stop()
    {
        _exit = true; 
    }

    /**
     * @return \c True when the access write or read to the db is successfully done.
     */
    const bool isTestSuccessfully()
    {
        return _success;
    }

    /**
     * @brief Implementation of the Runnable interface.
     *
     * Implements a loop which is abortable by calling the stop() method.
     */
    void run()
    {
        while(false == _exit)
        {
            if (_accessWrite)
            {
                writeDb();
            }
            else
            {
                readDb();
            }
        }
        _db.close();
    }

private:

    /**
     * @brief Specifies if the run() method should be broken and thus returned.
     */
    bool _exit;

    /**
     * @brief Specifies whether the read or write access has to be done.
     */
    bool _accessWrite;

    /**
     * @brief Specifies the data base to access.
     */
    Pt::Db::Connection _db;

    /**
     * @brief Specifies the frequently access to the database.
     */
    Pt::uint32_t _accessFreq;

    /**
     * @brief The first value added to "40000" for the field "salary". This value will be incremented depending 
     *        on the accessFreq within the read and write methods.
     */
    Pt::uint32_t _startValue;

    /**
     * @brief Indicates whether the read or write access is successfully done.
     */
    bool _success;


private:

    /**
     * @brief Write access methods to the database.
     *
     * By each write access the salary value will be incremented and added to value "40000".
     */
    void writeDb()
    {
        try
        {
            _success = false;
            Pt::Db::SqliteTransaction sqliteConn(_db,true,true);

            for (Pt::uint32_t i=0; i < _accessFreq; ++i)
            {
                std::stringstream strStream;
                strStream << "INSERT INTO TestTable (name,age,salary) VALUES ('Thomas0',20, " << 40000 + i + _startValue << ")";
                _db.execute(strStream.str());
            }

           _db.commitTransaction();
           _exit = true;
           _success = true;
       }
       catch (const std::logic_error& error)
       {
           std::cout << "std::logic_error caught while loading UpdateHandler on WriteDb : "  << error.what() << std::endl;
       }
       catch (const std::runtime_error& error)
       {
           std::cout << "std::runtime_error caught while loading UpdateHandler on WriteDb: " << error.what() << std::endl;
       }
       catch (...)
       {
           std::cout << "... caught while loading UpdateHandler on WriteDb" << std::endl;
       }
    }

    /**
     * @brief Read access methods to the database.
     *
     * By each select statement the salary value will be incremented and added to value "40000".
     */
    void readDb()
    {
        try
        {            
            _success = false;
            for (Pt::uint32_t i=0; i < _accessFreq; ++i)
            {
                std::stringstream strStream;
                strStream << "SELECT * FROM TestTable WHERE name = 'Thomas0' AND salary = " << 40000 + i + _startValue << "";
                Pt::Db::Result result = _db.select(strStream.str());
            }
            _success = true;
            _exit = true;
        }
        catch (const std::logic_error& error)
        {
            std::cout << "std::logic_error caught while loading UpdateHandler on ReadDb : "  << error.what() << std::endl;
        }
        catch (const std::runtime_error& error)
        {
            std::cout << "std::runtime_error caught while loading UpdateHandler on ReadDb: " << error.what() << std::endl;
        }
        catch (...)
        {
            std::cout << "... caught while loading UpdateHandler on ReadDb" << std::endl;
        }        
    }
};


class SqliteTest : public Pt::Unit::TestSuite
{
    public:
        SqliteTest()
        : Pt::Unit::TestSuite("SqliteTest")
        {
            Pt::Unit::TestSuite::registerMethod("testConnection", *this, &SqliteTest::Connection );
            Pt::Unit::TestSuite::registerMethod( "testCreateTable", *this, &SqliteTest::CreateTable );
            Pt::Unit::TestSuite::registerMethod( "testInsert", *this, &SqliteTest::Insert );
            Pt::Unit::TestSuite::registerMethod( "testSelect", *this, &SqliteTest::Select );
            Pt::Unit::TestSuite::registerMethod( "testDelete", *this, &SqliteTest::Delete );
            Pt::Unit::TestSuite::registerMethod( "testPragma", *this, &SqliteTest::Pragma );
            Pt::Unit::TestSuite::registerMethod( "Concurrency", *this, &SqliteTest::testConcurrency );
        }

    public:
        void setUp();
        void tearDown();

    protected:	
        void Connection();
        void CreateTable();
        void Insert();
        void Select();
        void Delete();
        void Pragma();
        void testConcurrency();

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


void SqliteTest::Connection()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTest.db");
}


void SqliteTest::CreateTable()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTest.db");
    Pt::Db::Transaction tact(con);

    con.execute("CREATE TABLE TestTable (name,age,salary);");

    Pt::Db::Result result = con.select("SELECT * FROM TestTable");
    PT_UNIT_ASSERT( result.size() == 0);

    tact.commit();
}

void SqliteTest::Insert()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTest.db");
    Pt::Db::Transaction tact(con);

    con.execute("CREATE TABLE TestTable (name,age,salary);");

    fillTable(con);

    Pt::Db::Result result = con.select("SELECT * FROM TestTable");
    PT_UNIT_ASSERT( result.size() == 7);

    tact.commit();
}

void SqliteTest::Select()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTest.db");
    Pt::Db::Transaction tact(con);

    con.execute("CREATE TABLE TestTable (name,age,salary);");

    fillTable(con);

    Pt::Db::Result result = con.select("SELECT * FROM TestTable WHERE age > 30 AND age < 60");
    PT_UNIT_ASSERT( result.size() == 4 );

    Pt::Db::Row row = con.selectRow("SELECT * FROM TestTable WHERE name = 'Angelique'");
    PT_UNIT_ASSERT( row.size() == 3 );

    std::string name;
    row[0].getString(name);
    int age = row[1].getInt();
    unsigned salary = row[2].getUnsigned();
    PT_UNIT_ASSERT(name == "Angelique");
    PT_UNIT_ASSERT(age == 32);
    PT_UNIT_ASSERT(salary == 68400);

    Pt::Db::Value val = con.selectValue("SELECT salary FROM TestTable WHERE age = 48");
    PT_UNIT_ASSERT( val.getUnsigned() == 8000 );

    tact.commit();
}

void SqliteTest::Delete()
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

void SqliteTest::Pragma()
{
    Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteTestPragma.db");
    Pt::Db::Value result;

    // NOTE: setting the page_size only works if its the first statement
    con.execute("PRAGMA page_size=4096;"); // size of one page in bytes
    result = con.selectValue("PRAGMA page_size;");
    PT_UNIT_ASSERT( result.getInt() == 4096 );

    con.execute("PRAGMA auto_vacuum = 1"); // 0 | 1 - reclaim unused space or not
    result = con.selectValue("PRAGMA auto_vacuum");
    PT_UNIT_ASSERT( result.getInt() == 1 );

    con.execute("PRAGMA cache_size = 3500"); // No. of cached pages
    result = con.selectValue("PRAGMA cache_size");
    PT_UNIT_ASSERT( result.getInt() == 3500 );

    con.execute("PRAGMA count_changes = 1"); // 0 | 1 - if set INSERT, UPDATE, DELETE return No. of changes
    result = con.selectValue("PRAGMA count_changes");
    PT_UNIT_ASSERT( result.getInt() == 1 );
}

void SqliteTest::testConcurrency()
{
    bool success = false;
    try
    {
        Pt::Db::Connection con = Pt::Db::connect("sqlite:SqliteConcurrencyTest.db");
        Pt::Db::Connection con1 = Pt::Db::connect("sqlite:SqliteConcurrencyTest.db");
        Pt::Db::Connection con2 = Pt::Db::connect("sqlite:SqliteConcurrencyTest.db");
        
        Pt::Db::SqliteTransaction sqliteConn(con,true,true);
        con.execute("CREATE TABLE TestTable (name,age,salary);");
        con.commitTransaction();

        AccessDbRunnable *runAccessWrite = new AccessDbRunnable(true, con, 20000);
        AccessDbRunnable *runAccessRead = new AccessDbRunnable(false, con1, 5000, 0);
        AccessDbRunnable *runAccessWrite1 = new AccessDbRunnable(true, con2, 10000, 20001);

        //Create the Threads.
        Pt::System::AttachedThread th1(Pt::callable(*runAccessWrite, &AccessDbRunnable::run));
        Pt::System::AttachedThread th2(Pt::callable(*runAccessRead, &AccessDbRunnable::run));
        Pt::System::AttachedThread th3(Pt::callable(*runAccessWrite1, &AccessDbRunnable::run));

        //Start the Threads.
        th1.start();
        th2.start();
        th3.start();

        //Give the Threads so time to work a bit.
        Pt::System::Thread::sleep(3000);

        //Stop the AccessDbRunnable and join the corresponding Threads.
        runAccessWrite->stop();
        runAccessRead->stop();
        runAccessWrite1->stop();
 
        th3.join();
        th2.join();
        th1.join();

        Pt::Db::Connection con3 = Pt::Db::connect("sqlite:SqliteConcurrencyTest.db");
        Pt::Db::Value result = con3.selectValue("SELECT COUNT(name) FROM TestTable where name = 'Thomas0'");
        PT_UNIT_ASSERT(result.getInt() == 30000);

        PT_UNIT_ASSERT(runAccessWrite->isTestSuccessfully());
        PT_UNIT_ASSERT(runAccessRead->isTestSuccessfully());
        PT_UNIT_ASSERT(runAccessWrite1->isTestSuccessfully());

        Pt::Db::Connection conDelete = Pt::Db::connect("sqlite:SqliteConcurrencyTest.db");
        conDelete.execute("DROP TABLE TestTable");

        conDelete.close();
        con3.close();
        con2.close();
        con1.close();
        con.close();

        success = true;
    }
    catch (const std::logic_error& error)
    {
        std::cout << "std::logic_error caught while loading UpdateHandler: "  << error.what() << std::endl;
    }
    catch (const std::runtime_error& error)
    {
        std::cout << "std::runtime_error caught while loading UpdateHandler: " << error.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "... caught while loading UpdateHandler" << std::endl;
    }
    
    PT_UNIT_ASSERT(success);
    Pt::System::File("SqliteConcurrencyTest.db").remove();
}

