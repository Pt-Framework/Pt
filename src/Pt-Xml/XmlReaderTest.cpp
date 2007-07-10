#undef PT_XML_API_EXPORT

#include "Pt/Xml/XmlReader.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Comment.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/EndDocument.h"
#include "Pt/Text/Utf8Codec.h"
#include "Pt/Main.h"


#include <sstream>
#include <ctime>

using namespace Pt;
using namespace Pt::Xml;
using namespace std;

/*
void perfTest()
{
    stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    for(int i = 0; i < 50000; ++i)
    {
        input << "<testelem x=\"abc\">";
        input << "0123456789abcdefghijklmnopqrstuvwxyz";
        input << "</testelem>";
    }

    cerr << "PrefTest: ";
    XmlReader reader( input );

    clock_t begin = clock();
    for(XmlReader::Iterator it = reader.current(); it != reader.end(); ++it)
    {}
    cerr << clock() - begin << endl;
}
*/
void test()
{
    stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<!DOCTYPE ui-description SYSTEM \"UIDescription.dtd\">";
    input << "<ui-description>";
    input << "<menu id=\"mainmenu\" label=\"Main-Menu\">";
    input << "<menu-ref menu-id=\"mainmenu_options\"/>";
    input << "</menu>";
    input << "<menu id=\"mainmenu_options\" label=\"Einstellungen\"/>";
    input << "<init-message type=\"info\" priority=\"1\"> <!-- error, info -->";
    input << "Das Navigationssystem sollte nur dann bedient werden, wenn es die Verkehrssituation ";
    input << "wirklich zulaesst. Widmen Sie Ihre Aufmerksamkeit in erster Linie dem Autofahren! ";
    input << "Als Fahrer tragen Sie die volle Verantwortung fuer die Verkehrssicherheit.</init-message>";
    input << "<init-message type=\"warning\" priority=\"3\"> <!-- error, info -->";
    input << "Es gilt immer die Strassen-verkehrsordnung (StVO), auch wenn eine Fahrempfehlung ";
    input << "im Widerspruch zur StVO steht.</init-message>";
    input << "<dialog id=\"mainmenu_dialog\" title=\"Main-Menu\" background-color=\"#ffffe1\" modal=\"true\">";
    input << "<horizontal-fill-position margin-left=\"10\" margin-right=\"10\"/>";
    input << "<components layout=\"gridlayout\">";
    input << "<menu-view id=\"mainmenu_view\" menu-id=\"mainmenu\">";
    input << "<layout-data grid-x=\"0\" grid-y=\"0\" grab-horizontal=\"true\" grab-vertical=\"true\"";
    input << " horizontal-align=\"fill\" vertical-align=\"fill\"/>";
    input << "</menu-view>";
    input << "</components>";
    input << "</dialog>";
    input << "</ui-description>";

    XmlReader reader( input );

    try {
        for(XmlReader::Iterator it = reader.current(); it != reader.end(); ++it)
        {
            const Xml::Node& n = *it;

            if( const Xml::StartElement* e = dynamic_cast<const Xml::StartElement*>(&n) )
            {
                cerr << "StartElement: '" << e->name().narrow() << "'" << endl;
                if( e->attributes().size() > 0 )
                {
                    cerr << "   Attribute: '" << e->attributes().front().name().narrow() << "'"
                         << ": '" << e->attributes().front().value().narrow() << "'" << endl;
                }
            }
            else if( const Xml::Characters* e = dynamic_cast<const Xml::Characters*>(&n) )
            {
                cerr << "Characters: '" << e->content().narrow() << "'" << endl;
            }
            else if( const Xml::Comment* e = dynamic_cast<const Xml::Comment*>(&n) )
            {
                cerr << "Comment: '" << e->text().narrow() << "'" << endl;
            }
            else if( const Xml::EndElement* e = dynamic_cast<const Xml::EndElement*>(&n) )
            {
                cerr << "EndElement: '" << e->name().narrow() << "'" << endl;
            }
            else {
                cerr << "Unknown Node" << endl;
            }
        }
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
    }
}


int main(int argc, char* argv[])
{
    test();
    //perfTest();
    return 0;
}
