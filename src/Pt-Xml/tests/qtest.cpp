#include "qtest.h"

#include <iostream>
#include <sstream>
#include <string>

#include <QtCore/QString>
#include <QtCore/QBuffer>
#include <QtCore/QXmlStreamReader>
#include <Pt/System/Clock.h>

void MyObject::go()
{
  emit exec();
}


void runTest()
{
    std::string input;
    input += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input += "<benchmark>";
    for(int i = 0; i < 50000; ++i)
    {
        input += "<test>";
        input += "<number>";
        input += "3.1415";
        input += "</number>";
        input += "<text>";
        input += "hello world!";
        input += "</text>";
        input += "</test>";
    }
    input += "</benchmark>";

    QBuffer buffer;
    buffer.setData( input.c_str(), input.size() );
    buffer.open(QBuffer::ReadWrite);
    
    Pt::System::Clock c;
    c.start();

    QXmlStreamReader reader(&buffer);
    
    int n = 0;
    for(;;) 
    {
        ++n;
        
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::EndDocument)
            break;

        if(token == QXmlStreamReader::EndDocument)
            break;
    }

    Pt::Timespan ts = c.stop();
    std::cerr << "parsed " << n << " nodes in " << ts.toMSecs()  << " msecs." << std::endl;
}


int main(int argc, char* argv[])
{
    MyObject mob;
    QObject::connect(&mob, SIGNAL(exec()), &mob, SLOT(done()));
    mob.go();

    for(unsigned n = 0; n < 10; ++n)
        runTest();

    return 0;
}
