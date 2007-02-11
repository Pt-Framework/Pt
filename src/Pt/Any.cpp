#include "Pt/Any.h"
using namespace Pt;

#include <iostream>
using namespace std;


//map<string, void (Any::*)()> Any::_initMap;
std::map<std::string, void (Any::*)()>& Any::initMap()
{
    static std::map<std::string, void (Any::*)()>* _initMap = new std::map<std::string, void (Any::*)()>();
    return *_initMap;
}


Any::Any()
: _value(0)
{ }


Any::Any(const Any& val)
: _value(val._value ? val._value->clone() : 0)
{
}


Any::~Any()
{
    if(_value)
        delete _value;
}


void Any::clear()
{
    if(_value) {
        delete _value;
        _value = 0;
    }
}


void  Any::init(const std::string& typeName)
{
    std::map<std::string, void (Any::*)()>::iterator it;
    it = initMap().find(typeName);

    if( it == initMap().end() ) {
        return;
    }

    (this->*(it->second))();
}


Any& Any::swap(Any& rhs)
{
    std::swap(_value, rhs._value);
    return *this;
}


void Any::output(std::ostream& os) const
{
    if(_value)
        _value->output(os);
}


void Any::input(std::istream& is)
{
    if(_value)
        _value->input(is);
}


Any& Any::operator=(const Any& rhs)
{
    Any(rhs).swap(*this);
    return *this;
}


bool Any::operator==(const Any& a) const
{
    if(_value && a._value)
    {
       return _value->equal( *(a._value) );
    }

    // if one or both of the Anys is not initialised
    // they are considered equal if both have NULL values.
    return _value == a._value;
}


bool Any::operator!=(const Any& a) const
{
    return !( this->operator==(a) );
}


bool Any::operator<(const Any& a) const
{
    if(_value && a._value)
    {
       return _value->lt( *(a._value) );
    }

    // if one of the Anys is not initialised the
    //one having a NULL valueis considered less.
    return _value < a._value;
}


std::ostream& Pt::operator<<(std::ostream& os, const Pt::Any& any)
{
    any.output(os);
    return os;
}


std::istream& Pt::operator>>(std::istream& is, Pt::Any& any)
{
    any.input(is);
    return is;
}




static Pt::Any::Bind<bool> bind_bool;
static Pt::Any::Bind<int> bind_int;
static Pt::Any::Bind<float> bind_float;






