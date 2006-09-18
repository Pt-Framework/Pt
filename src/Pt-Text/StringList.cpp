#include "Pt/Text/StringList.h"

//#include "Pt/Text/TextStream.h"
//#include "Pt/Text/Utf8Codec.h"
using namespace Pt::Text;

#include "Pt/Any.h"
using namespace Pt;

#include <sstream>
using namespace std;


StringList::StringList()
{
}


/*
void Pt::AnyTraits<StringList>::input(std::istream& is, StringList& value)
{
	Pt::String token;
	std::string buffer;

	while( getline(is, buffer, ';') ) {
		for(size_t pos = buffer.find("&sem!"); pos != std::string::npos; pos = buffer.find("&sem!", pos) ) {
			buffer.replace(pos, 5, ";");
			pos += 5;
		}

		stringstream ss(buffer);
		TextIStream ts(ss, new Utf8Codec());
		getline(ts, token);
		value.push_back(token);
	}
}


void Pt::AnyTraits<StringList>::output(std::ostream& os, const StringList& value)
{
	for(std::list<Pt::String>::const_iterator it = value.begin(); it != value.end(); ++it) {
		Pt::String buffer = *it;
		for(size_t pos = buffer.find(";"); pos != Pt::String::npos; pos = buffer.find(";", pos) ) {
			buffer.replace(pos, 1, "&sem!");
			pos += 1;
		}
		os << buffer.toUtf8() << ";";
	}
}
*/

//static Pt::Any::Bind<Pt::StringList> bind_Pt_StringList;



