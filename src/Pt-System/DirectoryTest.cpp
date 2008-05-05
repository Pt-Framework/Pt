#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/Directory.h"
#include "Pt/System/File.h"

class DirectoryTest : public Pt::Unit::TestSuite
{
    public:
        DirectoryTest()
        : Pt::Unit::TestSuite("DirectoryTest")
        {
            Pt::Unit::TestSuite::registerMethod( "allocDir", *this, &DirectoryTest::allocDir );
            Pt::Unit::TestSuite::registerMethod( "moveDir", *this, &DirectoryTest::moveDir );
            Pt::Unit::TestSuite::registerMethod( "removeDir", *this, &DirectoryTest::removeDir );
            Pt::Unit::TestSuite::registerMethod( "createFile", *this, &DirectoryTest::createFile );
            Pt::Unit::TestSuite::registerMethod( "resizeFile", *this, &DirectoryTest::resizeFile );
            Pt::Unit::TestSuite::registerMethod( "moveFile", *this, &DirectoryTest::moveFile );
            Pt::Unit::TestSuite::registerMethod( "removeFile", *this, &DirectoryTest::removeFile );
            Pt::Unit::TestSuite::registerMethod( "DirectoryIterator", *this, &DirectoryTest::DirectoryIterator );
        }

        void setUp()
        {
            this->tearDown();
        }

        void tearDown()
        {
            Pt::System::Directory dirx("xxxDIR");
            if( dirx.exists() )
                dirx.remove();

            Pt::System::Directory diry("yyyDIR");
            if( diry.exists() )
                diry.remove();

            Pt::System::File f1("TestFile1");
            if( f1.exists() )
                f1.remove();

            Pt::System::File f2("TestFile2");
            if( f2.exists() )
                f2.remove();
        }

    protected:
        void allocDir();
        void moveDir();
        void removeDir();
        void createFile();
        void resizeFile();
        void moveFile();
        void removeFile();
        void DirectoryIterator();
};


void DirectoryTest::allocDir()
{
    bool ok = true;
    try {
        Pt::System::Directory dir1("xxxDIR");
        dir1.create();
    } catch (...) {
        ok = false;
    }
    PT_UNIT_ASSERT( true == ok );
    Pt::System::Directory dir2("xxxDIR");

    try {
        ok = dir2.exists();
    } catch (...) {
        ok = false;
    }
    PT_UNIT_ASSERT( true == ok );
}


void DirectoryTest::moveDir()
{

    bool ok = true;
    Pt::System::Directory dir1("xxxDIR");
    dir1.create();
    try {
        dir1.move("yyyDIR");
    } catch (...) {
        ok = false;
    }

    PT_UNIT_ASSERT(true == ok);

    Pt::System::Directory dir2("yyyDIR");

    try {
        ok = dir2.exists();
    } catch (...) {
        ok = false;
    }
    PT_UNIT_ASSERT( true == ok );

}

void DirectoryTest::removeDir()
{
    bool ok = true;
    Pt::System::Directory dir1("yyyDIR");
    dir1.create();
    try {
        dir1.remove();
    } catch (...) {
        ok = false;
    }

    PT_UNIT_ASSERT( true == ok );
    PT_UNIT_ASSERT( false == dir1.exists() );
}



void DirectoryTest::createFile()
{
    std::string name = "TestFile1";

    Pt::System::File file1(name.c_str());
    file1.create();
    PT_UNIT_ASSERT( 0 == file1.size() );
}


void DirectoryTest::resizeFile()
{
    std::string name = "TestFile1";
    Pt::System::File file1(name);
    file1.create();
    file1.resize(1000);

    PT_UNIT_ASSERT( 1000 == file1.size() );
}


void DirectoryTest::moveFile()
{
    std::string name = "TestFile1";
    bool result = true;
    Pt::System::File file1(name);
    file1.create();
    file1.move("TestFile2");
    PT_UNIT_ASSERT( true == result );

    Pt::System::File file2("TestFile2");
    PT_UNIT_ASSERT( file2.exists() );
}


void DirectoryTest::removeFile()
{
    bool result = true;
    Pt::System::File file1("TestFile1");

    try {
        file1.remove();
    } catch (...) {
        result = false;
    }

    PT_UNIT_ASSERT( true == result );
    PT_UNIT_ASSERT( file1.exists() == false );
}


void DirectoryTest::DirectoryIterator()
{
/*	std::string dir("yyyDIR");
    std::string file(dir + Pt::System::Directory::separator() + "file");

    std::set<std::string> names;

    names.insert(file + "1");
    names.insert(file + "2");
    names.insert(file + "3");
    names.insert(dir + Directory::separator() + ".");
    names.insert(dir + Directory::separator() + "..");

    try {
        Pt::System::Directory dir1(dir, Directory::Create);
        Pt::System::File f1(file + "1", File::Create);
        Pt::System::File f2(file + "2", File::Create);
        Pt::System::File f3(file + "3", File::Create);
    } catch (...) {
        PT_UNIT_ASSERT(false);
    }

    Pt::System::Directory dir1(dir);
    Pt::System::DirectoryIterator it = dir1.begin();
    for (; it != dir1.end(); ++it)
    {
        std::string name = (*it).path();
        PT_UNIT_ASSERT(names.find(name) != names.end());
    }

    Pt::System::File f1(file+"1");
    f1.remove();
    Pt::System::File f2(file+"2");
    f2.remove();
    Pt::System::File f3(file+"3");
    f3.remove();
    dir1.remove();
*/
}


Pt::Unit::RegisterTest<DirectoryTest> register_DirectoryTest;
