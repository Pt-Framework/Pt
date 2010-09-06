/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
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
#ifndef Pt_Settings_h
#define Pt_Settings_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/SerializationInfo.h>
#include <iostream>
#include <sstream>
#include <cassert>

namespace Pt {

class PT_API SettingsError : public SerializationError
{
    public:
        SettingsError(const char* what, unsigned line);

        //! @brief Destructor
        ~SettingsError() throw()
        {}

        unsigned line() const
        { return _line; }

    private:
        unsigned _line;
};

/** @brief Store application settings
*/
class PT_API Settings : public SerializationInfo
{
    public:
        class Iterator;
        class ConstIterator;

        class Entry
        {
            public:
                explicit Entry(SerializationInfo* si = 0)
                : _si(si)
                {}

                template <typename T>
                bool get(T& value) const
                {
                    if( ! _si )
                        return false;

                    *_si >>= value;
                    return true;
                }

                template <typename T>
                void set(const T& value)
                {
                    if( _si )
                        *_si <<= value;
                }

                Entry add(const std::string& name)
                {
                    if( ! _si )
                        return Entry();

                    SerializationInfo& si = _si->addMember(name);
                    return Entry(&si);
                }

                void remove(const std::string& name)
                {
                    if( _si )
                        _si->removeMember(name);
                }

                Iterator begin() const
                {
                    if( ! _si )
                        return Iterator();

                    Iterator it;
                    it.init( _si->begin() );
                    return it;
                }

                Iterator end() const
                {
                    return Iterator();
                }

                Entry entry(const std::string& name) const
                {
                    if( ! _si )
                        return Entry();

                    SerializationInfo* si = _si->findMember(name);
                    return Entry(si);
                }

                Entry operator[] (const std::string& name) const
                {
                       return this->entry(name);
                }

                const std::string& name() const
                { return _si->name(); }

            private:
                SerializationInfo* _si;
        };

        class ConstEntry
        {
            public:
                explicit ConstEntry(const SerializationInfo* si = 0)
                : _si(si)
                {}

                template <typename T>
                bool get(T& value) const
                {
                    if( ! _si )
                        return false;

                    *_si >>= value;
                    return true;
                }

                ConstIterator begin() const
                {
                    if( ! _si )
                        return ConstIterator();

                    ConstIterator it;
                    it.init( _si->begin() );
                    return it;
                }

                ConstIterator end() const
                {
                    return ConstIterator();
                }

                ConstEntry entry(const std::string& name) const
                {
                    if( ! _si )
                        return ConstEntry();

                    const SerializationInfo* si = _si->findMember(name);
                    return ConstEntry(si);
                }

                ConstEntry operator[] (const std::string& name) const
                {
                    return this->entry(name);
                }

                const std::string& name() const
                { return _si->name(); }

            private:
                const SerializationInfo* _si;
        };

        class Iterator
        {
            public:
                Iterator()
                {}

                void init(const SerializationInfo::Iterator& it)
                {
                    _it = it;
                }

                Iterator(const Iterator& other)
                : _it(other._it)
                {}

                Iterator& operator=(const Iterator& other)
                {
                    _it = other._it;
                    return *this;
                }

                Entry operator*() const
                {
                    SerializationInfo& si = *_it;
                    return Entry(&si);
                }

                Iterator& operator++()
                {
                    ++_it;
                    return *this;
                }

                bool operator!=(const Iterator& other) const
                { return _it != other._it; }

                bool operator==(const Iterator& other) const
                { return _it == other._it; }

            private:
                SerializationInfo::Iterator _it;
        };

        class ConstIterator
        {
            public:
                ConstIterator()
                {}

                void init(const SerializationInfo::ConstIterator& it)
                {
                    _it = it;
                }

                ConstIterator(const ConstIterator& other)
                : _it(other._it)
                {}

                ConstIterator& operator=(const ConstIterator& other)
                {
                    _it = other._it;
                    return *this;
                }

                ConstEntry operator*() const
                {
                    const SerializationInfo& si = *_it;
                    return ConstEntry(&si);
                }

                ConstIterator& operator++()
                {
                    ++_it;
                    return *this;
                }

                bool operator!=(const ConstIterator& other) const
                { return _it != other._it; }

                bool operator==(const ConstIterator& other) const
                { return _it == other._it; }

            private:
                SerializationInfo::ConstIterator _it;
        };

    public:
        Settings();

        ConstIterator begin() const
        {
            ConstIterator it;
            it.init( SerializationInfo::begin() );
            return it;
        }

        ConstIterator end() const
        { return ConstIterator(); }

        ConstEntry root() const
        {
            const SerializationInfo* si = this;
            return ConstEntry(si);
        }

        Iterator begin()
        {
            Iterator it;
            it.init( SerializationInfo::begin() );
            return it;
        }

        Iterator end()
        { return Iterator(); }

        Entry root()
        {
            SerializationInfo* si = this;
            return Entry(si);
        }

        void load( std::basic_istream<Pt::Char>& is );

        void save( std::basic_ostream<Pt::Char>& os ) const;

        ConstEntry entry(const std::string& name) const
        {
            const SerializationInfo* si = this->findMember(name);
            return ConstEntry(si);
        }

        ConstEntry operator[] (const std::string& name) const
        {
            return this->entry(name);
        }

        Entry entry(const std::string& name)
        {
            SerializationInfo* si = this->findMember(name);
            return Entry(si);
        }

        Entry operator[] (const std::string& name)
        {
            return this->entry(name);
        }

        template <typename T>
        bool getObject(T& type, const std::string& name) const
        {
            return this->entry(name).get(type);
        }

        template <typename T>
        void setObject(const T& type, const std::string& name)
        {
            SerializationInfo& si = this->addMember(name);
            si <<= type;
        }
};

}

#endif
