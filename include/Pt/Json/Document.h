/*
   Copyright (C) 2015-2023 by Dr. Marc Boris Duerner
  
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#ifndef PT_JSON_DOCUMENT_H
#define PT_JSON_DOCUMENT_H

#include <Pt/Json/Api.h>
#include <Pt/SerializationInfo.h>
#include <Pt/String.h>
#include <string>
#include <cstddef>

namespace Pt {

namespace Json {

/** @brief JSON Document.
*/
class PT_JSON_API Document
{
    public:
        /** @brief Modifiable document element.
        */
        class Element
        {
            public:
                explicit Element(SerializationInfo* si = 0)
                : _si(si)
                {}

                Element(const Element& elem)
                : _si(elem._si)
                {}

                Element& operator=(const Element& elem)
                {
                    _si = elem._si;
                    return *this;
                }

                /** @brief Returns the element name.
                */
                const char* name() const
                { return _si->name(); }

                /** @brief Gets the value.
                */
                template <typename T>
                bool getValue(T& value) const
                {
                    if( ! _si )
                        return false;

                    *_si >>= value;
                    return true;
                }

                /** @brief Sets the value.
                */
                template <typename T>
                void setValue(const T& value)
                {
                    if( _si )
                    {
                        _si->setVoid();
                        *_si <<= value;
                    }
                }

                /** @brief Returns true if element is null.
                */
                bool isNull() const
                {
                  return _si ? _si->isVoid() : true;
                }

                /** @brief Sets element to null.
                */
                void setNull()
                {
                    if( _si )
                    {
                        _si->setVoid();
 
                    }
                }

                /** @brief Returns the parent element.
                */
                Element parent() const
                {
                  if( ! _si )
                    return Element();

                  return Element( _si->parent() );
                }

                /** @brief Begin of sub elements.
                */
                Element begin() const
                {
                    if( ! _si )
                        return this->end();

                    SerializationInfo::Iterator it =_si->begin();
                    if( it == _si->end() )
                        return this->end();

                    SerializationInfo& si = *it;
                    return Element(&si);
                }

                /** @brief End of sub elements.
                */
                Element end() const
                {
                    return Element();
                }

                /** @brief Returns a sub element.
                */
                Element getMember(const std::string& name) const
                {
                    if( ! _si )
                        return this->end();

                    SerializationInfo* si = _si->findMember(name);
                    return Element(si);
                }
                
                /** @brief Returns a sub element.
                */
                Element getMember(const char* name) const
                {
                    if( ! _si )
                        return this->end();

                    SerializationInfo* si = _si->findMember(name);
                    return Element(si);
                }
                
                /** @brief Returns a sub element.
                */
                Element operator[] (const std::string& name) const
                {
                    return this->getMember(name);
                }

                /** @brief Returns a sub element.
                */
                Element operator[] (const char* name) const
                {
                    return this->getMember(name);
                }

                /** @brief Adds a sub element.
                */
                Element addMember(const char* name)
                {
                    if( ! _si )
                        return Element();

                    SerializationInfo* si = _si->findMember(name);
                    if( ! si )
                        si = &_si->addMember(name);
                    
                    return Element(si);
                }

                /** @brief Adds a sub element.
                */
                Element addMember(const std::string& name)
                {
                    if( ! _si )
                        return Element();

                    SerializationInfo* si = _si->findMember(name);
                    if( ! si )
                        si = &_si->addMember(name);
                    
                    return Element(si);
                }

                /** @brief Adds a sub element.
                */
                Element addElement()
                {
                    if( ! _si )
                        return Element();

                    SerializationInfo& si = _si->addElement();
                    return Element(&si);
                }

                /** @brief Removes a sub element.
                */
                void removeMember(const std::string& name)
                {
                    if( _si )
                        _si->removeMember(name);
                }
                
                /** @brief Removes a sub element.
                */
                void removeMember(const char* name)
                {
                    if( _si )
                        _si->removeMember(name);
                }

                /** @brief Removes a sub element.
                */
                void removeElement(const Element& e)
                {
                    if( ! _si || ! e._si )
                      return;

                    _si->removeMember( *e._si );
                }

                /** @brief Allows using the element like an iterator.
                */
                Element& operator*()
                { return *this; }

                /** @brief Allows using the element like an iterator.
                */
                Element* operator->()
                { return this; }

                /** @brief Allows using the element like an iterator.
                */
                Element& operator++()
                {
                    _si = _si->sibling();
                    return *this;
                }

                /** @brief Allows using the element like an iterator.
                */
                bool operator!=(const Element& other) const
                { return _si != other._si; }

                /** @brief Allows using the element like an iterator.
                */
                bool operator==(const Element& other) const
                { return _si == other._si; }

                /** @brief Returns true if element is invalid.
                */
                bool operator!() const
                { return _si == 0; }

            private:
                SerializationInfo* _si;
        };

        /** @brief Constant document element.
        */
        class ConstElement
        {
            public:
                explicit ConstElement(const SerializationInfo* si = 0)
                : _si(si)
                {}

                /** @brief Returns the element name.
                */
                const char* name() const
                { return _si->name(); }

                /** @brief Gets the value.
                */
                template <typename T>
                bool getValue(T& value) const
                {
                    if( ! _si )
                        return false;

                    *_si >>= value;
                    return true;
                }

                /** @brief Returns true if element is null.
                */
                bool isNull() const
                {
                  return _si ? _si->isVoid() : true;
                }

                /** @brief Begin of sub elements.
                */
                ConstElement begin() const
                {
                    if( ! _si )
                        return this->end();

                    SerializationInfo::ConstIterator it =_si->begin();
                    if(it == _si->end())
                        return this->end();

                    const SerializationInfo& si = *it;
                    return ConstElement(&si);
                }

                /** @brief End of sub elements.
                */
                ConstElement end() const
                {
                    return ConstElement();
                }

                /** @brief Returns a sub element.
                */
                ConstElement getMember(const std::string& name) const
                {
                    if( ! _si )
                        return end();

                    const SerializationInfo* si = _si->findMember(name);
                    return ConstElement(si);
                }

                /** @brief Returns a sub element.
                */
                ConstElement getMember(const char* name) const
                {
                    if( ! _si )
                        return end();

                    const SerializationInfo* si = _si->findMember(name);
                    return ConstElement(si);
                }

                /** @brief Returns a sub element.
                */
                ConstElement operator[] (const std::string& name) const
                {
                    return this->getMember(name);
                }

                /** @brief Returns a sub element.
                */
                ConstElement operator[] (const char* name) const
                {
                    return this->getMember(name);
                }

                /** @brief Allows using the element like an iterator.
                */
                const ConstElement& operator*() const
                { return *this; }

                /** @brief Allows using the element like an iterator.
                */
                const ConstElement* operator->() const
                { return this; }

                /** @brief Allows using the element like an iterator.
                */
                ConstElement& operator++()
                {
                    _si = _si->sibling();
                    return *this;
                }

                /** @brief Allows using the element like an iterator.
                */
                bool operator!=(const ConstElement& other) const
                { return _si != other._si; }

                /** @brief Allows using the element like an iterator.
                */
                bool operator==(const ConstElement& other) const
                { return _si == other._si; }

                /** @brief Returns true if element is invalid.
                */
                bool operator!() const
                { return _si == 0; }

            private:
                const SerializationInfo* _si;
        };

    public:
        /** @brief Default constructor.
        */
        Document();

        /** @brief Clears the settings.
        */
        void clear();

        /** @brief Returns true if settings are empty.
        */
        bool isEmpty() const;

        /** @brief Loads a document from a input stream.
        */
        void load( std::basic_istream<Pt::Char>& is );

        /** @brief Saves a document to a output stream.
        */
        void save( std::basic_ostream<Pt::Char>& os ) const;

        /** @brief Returns the root element.
        */
        Element root()
        { return Element(&_root); }

        /** @brief Returns the root element.
        */
        ConstElement root() const
        { return ConstElement(&_root); }

        /** @brief Begin of elements.
        */
        Element begin()
        { return root().begin(); }

        /** @brief Begin of elements.
        */
        ConstElement begin() const
        { return root().begin(); }

        /** @brief End of elements.
        */
        Element end()
        { return root().end(); }

        /** @brief End of elements.
        */
        ConstElement end() const
        { return root().end(); }

        /** @brief Returns a top level element.
        */
        Element getMember(const std::string& name)
        {
            return root().getMember(name);
        }

        /** @brief Returns a top level element.
        */
        ConstElement getMember(const std::string& name) const
        {
            return root().getMember(name);
        }

        /** @brief Returns a top level element.
        */
        Element getMember(const char* name)
        {
            return root().getMember(name);
        }

        /** @brief Returns a top level element.
        */
        ConstElement getMember(const char* name) const
        {
            return root().getMember(name);
        }

        /** @brief Returns a top level element.
        */
        Element operator[] (const std::string& name)
        {
            return root().getMember(name);
        }

        /** @brief Returns a top level element.
        */
        ConstElement operator[] (const std::string& name) const
        {
            return root().getMember(name);
        }

        /** @brief Returns a top level element.
        */
        Element operator[] (const char* name)
        {
            return root().getMember(name);
        }

        /** @brief Returns a top level element.
        */
        ConstElement operator[] (const char* name) const
        {
            return root().getMember(name);
        }

        /** @brief Adds a top level element.
        */
        Element addMember(const char* name)
        {
            return root().addMember(name);
        }

        /** @brief Adds a top level element.
        */
        Element addMember(const std::string& name)
        {
            return root().addMember(name);
        }

        /** @brief Adds a top level element.
        */
        Element addElement()
        {
            return root().addElement();
        }

        /** @brief Removes a top level element.
        */
        void removeMember(const char* name)
        {
            root().removeMember(name);
        }

        /** @brief Removes a top level element.
        */
        void removeMember(const std::string& name)
        {
            root().removeMember(name);
        }

        /** @brief Removes a top level element.
        */
        void removeElement(const Element& e)
        {
            root().removeElement(e);
        }

    private:
        SerializationInfo _root;
};

} // namespace

} // namespace

#endif
