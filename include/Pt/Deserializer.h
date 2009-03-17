/*
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
#include <map>
#include <list>

namespace Pt {

class IDeserializationHandler
{
    public:
        IDeserializationHandler()
        : _parent(0)
        {}

        virtual ~IDeserializationHandler()
        {}

        void setParent(IDeserializationHandler* parent)
        { _parent = parent; }

        IDeserializationHandler* parent()
        { return _parent; }

        virtual void setValue(const Pt::String& value) = 0;

        virtual IDeserializationHandler* beginMember(const std::string& name) = 0;

        virtual IDeserializationHandler* leaveMember() = 0;

        virtual void finish() = 0;

    private:
        IDeserializationHandler* _parent;
};


template <typename T>
class DeserializationHandler : public IDeserializationHandler
{
    public:
        DeserializationHandler(T& type)
        : _type(type)
        , _current(&_si)
        {}

        virtual void setValue(const Pt::String& value)
        {
            _current->setValue(value);
        }

        virtual IDeserializationHandler* beginMember(const std::string& name)
        {
            SerializationInfo& child = _current->addMember(name);
            _current = &child;
            return this;
        }

        virtual IDeserializationHandler* leaveMember()
        {
            if( ! _current->parent() )
            {
                this->finish();

                if( ! this->parent() )
                    throw std::runtime_error("invalid member");

                return this->parent();
            }

            _current = _current->parent();
            return this;
        }

        virtual void finish()
        { 
            *_current >>= *_type;
        }

    private:
        T* _type;
        Pt::SerializationInfo _si;
        Pt::SerializationInfo* _current;
};


class PT_API Deserializer
{
    public:
        typedef void (*Fixup)(void**, const std::type_info&, void*);

        virtual ~Deserializer();

        /** @brief Deserialize an object

            This method will deserialize the object \a type from an
            XML format. The type \a type must be serializable.
        */
        template <typename T>
        void deserialize(T& type)
        {
            SerializationInfo& si = this->get();
            si >>= type;
            this->markFixup(si, &type, &Deserializer::do_fixup<T>);
        }

        template <typename T>
        void deserialize2(T& type)
        {
            DeserializationHandler<T> handler(type);
            this->onDeserialize(handler)
        }

        virtual void onDeserialize(IDeserializationHandler& dh)
        {
        }
        //SerializationInfo& peek();

        void finish();

    protected:
        Deserializer();

        virtual void read(SerializationInfo& si) = 0;

    private:
        Pt::SerializationInfo& get();

        void markFixup(Pt::SerializationInfo& si, void* type, Fixup fixup);

        void fixup(const Pt::SerializationInfo& si);

    private:
        std::list<Pt::SerializationInfo> _stack;

        bool _peeking;

        std::map<std::string, void*> _objects;

        std::map<std::string, Fixup> _fixups;

        std::map<void*, std::string> _pointers;

        template <typename T>
        static void do_fixup(void** fixme, const std::type_info& fixmeInfo , void* obj)
        {
            if( fixmeInfo != typeid(T) )
                throw SerializationError( PT_ERROR_MSG("reference fixup failed, type mismatch") );

            *( (T**)(fixme) ) = (T*)(obj);
        }
};

} // namespace Pt

#endif
