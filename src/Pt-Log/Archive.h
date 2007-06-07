/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Drner                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_Archive_h
#define Pt_Archive_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/Reflectable.h>


namespace Pt {

class Archive
{
    public:
        Archive()
        {}

        virtual ~Archive()
        {}

        virtual const Pt::String* value(const Pt::String& name) const = 0;

        virtual void addValue(const Pt::String& name, const Pt::String& value) = 0;

        virtual const Archive* findArchive(const Pt::String& name) const = 0;

        virtual Archive& addArchive(const Pt::String& name) = 0;

        template <typename T>
        bool extract(T& type, const Pt::String& typeName)
        {
            const Archive* archive = this->_extract(typeName);
            if(archive == 0)
                return false;

            return *archive >> type;
        }

    protected:
        virtual const Archive* _extract(const Pt::String& typeName) = 0;
};


    struct AnyBuilder
    {
        virtual ~AnyBuilder()
        {}

        virtual void build(Pt::Any& a, const Archive& archive) const = 0;

        virtual void build(Pt::Any& a, const Pt::String& value) const = 0;
    };


    template <typename T>
    struct BasicAnyBuilder : public AnyBuilder
    {
        virtual void build(Pt::Any& a, const Archive& archive) const = 0;

        virtual void build(Pt::Any& a, const Pt::String& value) const = 0;
    };


    template <>
    struct BasicAnyBuilder<bool> : public AnyBuilder
    {
        virtual void build(Pt::Any& a, const Archive& archive) const
        {
            throw std::logic_error("Type not buildable from archive" + PT_SOURCEINFO);
        }

        virtual void build(Pt::Any& a, const Pt::String& value) const
        {
            value == L"true"? a = true : a = false;
        }
    };


    template <>
    struct BasicAnyBuilder<std::string> : public AnyBuilder
    {
        virtual void build(Pt::Any& a, const Archive& archive) const
        {
            throw std::logic_error("Type not buildable from archive" + PT_SOURCEINFO);
        }

        virtual void build(Pt::Any& a, const Pt::String& value) const
        {
            std::string s = value.narrow();
            a = s;
        }
    };


    class PT_API AnyFactory2 : public Singleton<AnyFactory2>
    {
        friend class Singleton<AnyFactory2>;

        public:
            static Any create(const std::string typeName, const Pt::String& value)
            {
                const AnyBuilder* builder = AnyFactory2::findBuilder(typeName);
                if( builder == 0 )
                    throw std::invalid_argument("No such builder (" + typeName + ")" + PT_SOURCEINFO);

                Pt::Any a;
                builder->build(a, value);
                return a;
            }

            static Any create(const std::string typeName, const Archive& archive)
            {
                const AnyBuilder* builder = AnyFactory2::findBuilder(typeName);
                if( builder == 0 )
                    throw std::invalid_argument("No such builder (" + typeName + ")" + PT_SOURCEINFO);

                Pt::Any a;
                builder->build(a, archive);
                return a;
            }

        protected:
            AnyFactory2()
            {
                _builder.insert( std::make_pair("bool", new BasicAnyBuilder<bool>) );
                _builder.insert( std::make_pair("std::string", new BasicAnyBuilder<std::string>) );
            }

            static const AnyBuilder* findBuilder(const std::string typeName)
            {
                std::multimap<std::string, AnyBuilder*>::iterator it;
                it = AnyFactory2::instance()._builder.find(typeName);

                if( it != AnyFactory2::instance()._builder.end() )
                    return it->second;

                return 0;
            }

        private:
            //! @internal
            std::multimap<std::string, AnyBuilder*> _builder;
    };


inline bool operator>>(const Archive& archive, Reflectable& r)
{
    PropertyMap& pmap = r.properties();
    for(PropertyMap::iterator it = pmap.begin(); it != pmap.end(); ++it)
    {
        PropertyInfo& propInfo = *( it->second );
        Pt::String propName = Pt::String::widen( it->first );

        const Pt::String* value = archive.value(propName);
        if(value)
        {
            Any a = AnyFactory2::create( propInfo.typeName(), *value );
            it->second->setValue(a);
        }

        const Archive* subarchive = archive.findArchive(propName);
        if(subarchive)
        {
            Any a = AnyFactory2::create( propInfo.typeName(), *subarchive );
            it->second->setValue(a);
        }
    }

    return true;
}

}

#endif

