/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#ifndef Pt_Deserializer_h
#define Pt_Deserializer_h

#include <Pt/Api.h>
#include <Pt/SerializationInfo.h>
#include <Pt/SerializationContext.h>
#include <map>

namespace Pt {

class Deserializer;

class IComposer
{
    public:
        virtual ~IComposer()
        {}

        virtual void setName(const std::string& name) = 0;

        virtual void setId(const std::string& id) = 0;

        virtual void setValue(const Pt::String& value)
        { throw SerializationError("unexpected value"); }

        virtual void setBool(bool value)
        { throw SerializationError("unexpected bool value"); }

        virtual void setInt(long value)
        { throw SerializationError("unexpected integer value"); }

        virtual void setUInt(unsigned long value)
        { throw SerializationError("unexpected unsigned value"); }

        virtual void setFloat(double value)
        { throw SerializationError("unexpected float value"); }

        virtual void setReference(const std::string& id)
        { throw SerializationError("unexpected reference"); }

        virtual IComposer* beginMember(const std::string& name)
        { throw SerializationError("unexpected struct"); }

        virtual IComposer* beginElement()
        { throw SerializationError("unexpected sequence"); }

        virtual IComposer* finish() = 0;

        virtual IComposer* finish(Deserializer& deser) = 0;

    protected:
        IComposer()
        {}
};


template <typename T>
class Composer;


class Deserializer
{
    public:
        typedef void (*LoadRule)(SerializationInfo& si, Deserializer& deser);

    public:
        Deserializer()
        : _context(0)
        , _current(0)
        {}

        virtual ~Deserializer()
        {}

        SerializationContext& context()
        { return *_context; }

        const SerializationContext& context() const
        { return *_context; }

        void setContext(SerializationContext* context)
        { _context = context; }

        /** @brief Deserialize an object

            This method will deserialize the object \a type.
            The type \a type must be serializable.
        */
        template <typename T>
        void deserialize(T& type)
        {
            Composer<T> deser;
            deser.begin(type, _context);

            this->get(&deser);
        }

        void deserialize(IComposer& deser)
        {
            this->get(&deser);
            deser.finish(*this);
        }

        template <typename T>
        void begin(T& type)
        {
            Composer<T>* deser = new Composer<T>;
            deser->begin(type, _context);
            _current = deser;
        }

        bool advance()
        {
            if( ! _current )
                return false;

            _current = this->advance(_current);

            if( ! _current )
                return false;
        }

        virtual IComposer* advance(IComposer* deser) = 0;

        void finish()
        { _context->fixup(); }

        void setLoadRule(const std::string& typeName, LoadRule rule)
        {
            _loadRules[typeName] = rule;
        }

        void load(SerializationInfo& si)
        {
            std::map<std::string, LoadRule>::const_iterator it;
            it = _loadRules.find( si.typeName() );
            if( it != _loadRules.end() )
            {
                it->second(si, *this);
            }
        }

    protected:
        virtual void get(IComposer* deser) = 0;

    private:
        std::map<std::string, LoadRule> _loadRules;
        SerializationContext* _context;
        IComposer* _current;
};


template <typename T>
class Composer : public IComposer
{
    public:
        Composer()
        : _parent(0)
        , _type(0)
        , _current(&_si)
        {}

        void setParent(IComposer* parent)
        { _parent = parent; }

        void begin(T& type, SerializationContext* context = 0)
        {
            _si.clear();
            _si.setContext(context);
            _type = &type;
            _current = &_si;
        }

        virtual void setName(const std::string& name)
        {
            _current->setName(name);
        }

        virtual void setId(const std::string& id)
        {
            _current->setId(id);
        }

        virtual void setValue(const Pt::String& value)
        {
            _current->setValue(value);
        }

        virtual void setBool(bool value)
        {
            _current->setValue(value);
        }

        virtual void setInt(long value)
        {
            _current->setValue(value);
        }

        virtual void setUInt(unsigned long value)
        {
            _current->setValue(value);
        }

        virtual void setFloat(double value)
        {
            _current->setValue(value);
        }

        virtual void setReference(const std::string& id)
        {
           _current->setReference(id);
        }

        virtual IComposer* beginMember(const std::string& name)
        {
            SerializationInfo& child = _current->addMember(name);
            _current = &child;
            return this;
        }

        virtual IComposer* beginElement()
        {
            SerializationInfo& child = _current->addElement();
            _current = &child;
            return this;
        }

        virtual IComposer* finish()
        {
            if( ! _current->parent() )
            {
                *_current >>= Pt::load() >>= *_type;
                return _parent;
            }

            _current = _current->parent();
            return this;
        }

        virtual IComposer* finish(Deserializer& deser)
        {
            // TODO: pass Deserializer to finish() so a format rule
            //       can be applied to the _current SerializationInfo
            //       to expand for example a date iso string to date
            //       member attributes.
            //
            //       deser.setLoadRule("Pt::Date", DateFromIsoString);
            //       composer.finish(deser);

            deser.load(*_current);

            if( ! _current->parent() )
            {
                *_current >>= Pt::load() >>= *_type;
                return _parent;
            }

            _current = _current->parent();
            return this;
        }

    private:
        IComposer* _parent;
        T* _type;
        Pt::SerializationInfo _si;
        Pt::SerializationInfo* _current;
};

} // namespace Pt

#endif
