#include "Pt/Xml/Resolver.h"
#include "Pt/Text/TextStream.h"

#include <sstream>
#include <iostream>
using namespace std;


namespace Pt {

namespace Xml {

Resolver::Resolver()
{
    this->clear();
}


Resolver::~Resolver()
{

}


void Resolver::clear()
{
    _entityMap.clear();
    _entityMap.insert( std::make_pair(L"lt", L"<")  );
    _entityMap.insert( std::make_pair(L"gt", L">") );
    _entityMap.insert( std::make_pair(L"amp", L"&") );
    _entityMap.insert( std::make_pair(L"apos", L"\'") );
    _entityMap.insert( std::make_pair(L"quot", L"\"") );
}


void Resolver::addEntity(const String& entity, const String& token)
{
  _entityMap.insert( std::make_pair<String, String>(entity, token) );
}


String Resolver::resolveEntity(const String& entity)
{
    if( entity.find('#') == 0 )
    {
        int code;
        stringstream ss1;
        if( entity.find('x') == 1 )
        {
            ss1 << entity.substr(2).narrow();
            ss1 << hex;
        }
        else
        {
            ss1 << entity.substr(1).narrow();
            String number;
        }
        ss1 >> code;
        return String( 1, Char(code) );
    }

    map<String, String>::iterator it = _entityMap.find(entity);
    if( it == _entityMap.end() )
    {
        return String();
    }

    return it->second;
}

} // namespace Xml

} // namespace Pt
