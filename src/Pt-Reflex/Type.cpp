#include "Pt/Reflex/Type.h"
#include <stdexcept>
#include <string>

namespace Pt {

namespace Reflex {

class BoolType : public Type
{
    public:
        BoolType(Context& context)
        : Type( typeid(bool), context )
        { }

        virtual void registerMethods()
        {
            this->registerProxy("=", &BoolType::assign);
            this->registerProxy("==", &BoolType::eq);
            this->registerProxy("<", &BoolType::lt);
        }

        static bool assign(bool& self, bool b)
        {
            self = b;
            return self;
        }

        static bool eq(bool& self, bool b)
        {
            return self == b;
        }

        static bool lt(bool& self, bool b)
        {
            return self < b;
        }
};


class IntType : public Type
{
    public:
        IntType(Context& context)
        : Type( typeid(int), context )
        { }

        virtual void registerMethods()
        {
            this->registerProxy("=", &IntType::assign);
            this->registerProxy("==", &IntType::eq);
            this->registerProxy("<", &IntType::lt);
        }

        static int assign(int& self, int b)
        {
            self = b;
            return self;
        }

        static bool eq(int& self, int b)
        {
            return self == b;
        }

        static bool lt(int& self, int b)
        {
            return self < b;
        }
};


class LongType : public Type
{
    public:
        LongType(Context& context)
        : Type( typeid(long), context )
        { }

        virtual void registerMethods()
        {
            this->registerProxy("=", &LongType::assign);
            this->registerProxy("==", &LongType::eq);
            this->registerProxy("<", &LongType::lt);
        }

        static long assign(long& self, long b)
        {
            self = b;
            return self;
        }

        static bool eq(long& self, long b)
        {
            return self == b;
        }

        static bool lt(long& self, long b)
        {
            return self < b;
        }
};


Context::Context()
{
    _builtins.push_back( new BoolType(*this) );
    _builtins.push_back( new IntType(*this) );
    _builtins.push_back( new LongType(*this) );
}


Context::~Context()
{
    std::vector<Type*>::iterator it;
    for(it = _builtins.begin(); it != _builtins.end(); ++it)
    {
        delete *it;
    }
}


void Context::registerMethods()
{
    TypeMap::iterator it;
    for(it = _types.begin(); it != _types.end(); ++it)
    {
        it->second->registerMethods();
    }
}


Type::Type(const std::type_info& ti, Context& context)
: _context(&context)
{
    context.registerType(ti, *this);
}


Type::~Type()
{
    MethodMap::iterator mit;
    for(mit = _methods.begin(); mit != _methods.end(); ++mit)
    {
        delete mit->second;
    }

    PropertyMap::iterator pit;
    for(pit = _properties.begin(); pit != _properties.end(); ++pit)
    {
        delete pit->second;
    }
}


CallableInfo& Type::method(const char* method)
{
    MethodMap::iterator it;
    it = _methods.find(method);

    if( it == _methods.end() )
        throw std::logic_error( std::string("no such method: ") + method );

    return *(it->second);
}


PropInfo& Type::property(const char* method)
{
    PropertyMap::iterator it;
    it = _properties.find(method);

    if( it == _properties.end() )
        throw std::logic_error( std::string("no such property: ") + method );

    return *(it->second);
}

}

}
