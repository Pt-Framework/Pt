/*
 * Copyright (C) 2005-2010 by Dr. Marc Boris Duerner
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
#include <Pt/SerializationInfo.h>
#include <string>
#include <cstddef>

namespace Pt {

/** @brief %Settings Format Error.

    @ingroup Utilities
*/
class PT_API SettingsError : public SerializationError
{
    public:
        //! @brief Constructor.
        SettingsError(const char* what, std::size_t line);

        //! @brief Destructor.
        ~SettingsError() throw()
        {}

        /** @brief Returns the line number where the error occured.
        */
        std::size_t line() const
        { return _line; }

    private:
        //! @internal
        std::size_t _line;
};

/** @brief Store application settings.

    @ingroup Utilities
*/
class PT_API Settings : private SerializationInfo
{
    public:
        /** @brief Modifiable settings entry.
        */
        class Entry
        {
            public:
                explicit Entry(SerializationInfo* si = 0)
                : _si(si)
                {}

                Entry(const Entry& entry)
                : _si(entry._si)
                {}

                Entry& operator=(const Entry& entry)
                {
                    _si = entry._si;
                    return *this;
                }

                /** @brief Gets the value.
                */
                template <typename T>
                bool get(T& value) const
                {
                    if( ! _si )
                        return false;

                    *_si >>= value;
                    return true;
                }

                /** @brief Sets the value.
                */
                template <typename T>
                void set(const T& value)
                {
                    if( _si )
                    {
                        _si->setVoid();
                        *_si <<= value;
                    }
                }

                /** @brief Adds a sub entry.
                */
                Entry add(const std::string& name)
                {
                    if( ! _si )
                        return Entry();

                    SerializationInfo& si = _si->addMember(name);
                    return Entry(&si);
                }

                /** @brief Adds a sub entry.
                */
                Entry add(const char* name)
                {
                    if( ! _si )
                        return Entry();

                    SerializationInfo& si = _si->addMember(name);
                    return Entry(&si);
                }

                /** @brief Removes a sub entry.
                */
                void remove(const std::string& name)
                {
                    if( _si )
                        _si->removeMember(name);
                }
                
                /** @brief Removes a sub entry.
                */
                void remove(const char* name)
                {
                    if( _si )
                        _si->removeMember(name);
                }

                /** @brief Begin of sub entries.
                */
                Entry begin() const
                {
                    if( ! _si )
                        return this->end();

                    SerializationInfo::Iterator it =_si->begin();
                    if( it == _si->end() )
                        return this->end();

                    SerializationInfo& si = *it;
                    return Entry(&si);
                }

                /** @brief End of sub entries.
                */
                Entry end() const
                {
                    return Entry();
                }

                /** @brief Returns a sub entry.
                */
                Entry entry(const std::string& name) const
                {
                    if( ! _si )
                        return this->end();

                    SerializationInfo* si = _si->findMember(name);
                    return Entry(si);
                }
                
                /** @brief Returns a sub entry.
                */
                Entry entry(const char* name) const
                {
                    if( ! _si )
                        return this->end();

                    SerializationInfo* si = _si->findMember(name);
                    return Entry(si);
                }
                
                /** @brief Returns a sub entry.
                */
                Entry operator[] (const std::string& name) const
                {
                    return this->entry(name);
                }

                /** @brief Returns a sub entry.
                */
                Entry operator[] (const char* name) const
                {
                    return this->entry(name);
                }

                /** @brief Returns the entry name.
                */
                const char* name() const
                { return _si->name(); }

                /** @brief Allows using the entry like an iterator.
                */
                Entry& operator*()
                { return *this; }

                /** @brief Allows using the entry like an iterator.
                */
                Entry* operator->()
                { return this; }

                /** @brief Allows using the entry like an iterator.
                */
                Entry& operator++()
                {
                    _si = _si->sibling();
                    return *this;
                }

                /** @brief Allows using the entry like an iterator.
                */
                bool operator!=(const Entry& other) const
                { return _si != other._si; }

                /** @brief Allows using the entry like an iterator.
                */
                bool operator==(const Entry& other) const
                { return _si == other._si; }

                /** @brief Returns true if entry is invalid.
                */
                bool operator!() const
                { return _si == 0; }

            private:
                SerializationInfo* _si;
        };

        /** @brief Constant settings entry.
        */
        class ConstEntry
        {
            public:
                explicit ConstEntry(const SerializationInfo* si = 0)
                : _si(si)
                {}

                /** @brief Gets the value.
                */
                template <typename T>
                bool get(T& value) const
                {
                    if( ! _si )
                        return false;

                    *_si >>= value;
                    return true;
                }

                /** @brief Begin of sub entries.
                */
                ConstEntry begin() const
                {
                    if( ! _si )
                        return this->end();

                    SerializationInfo::ConstIterator it =_si->begin();
                    if(it == _si->end())
                        return this->end();

                    const SerializationInfo& si = *it;
                    return ConstEntry(&si);
                }

                /** @brief End of sub entries.
                */
                ConstEntry end() const
                {
                    return ConstEntry();
                }

                /** @brief Returns a sub entry.
                */
                ConstEntry entry(const std::string& name) const
                {
                    if( ! _si )
                        return end();

                    const SerializationInfo* si = _si->findMember(name);
                    return ConstEntry(si);
                }

                /** @brief Returns a sub entry.
                */
                ConstEntry entry(const char* name) const
                {
                    if( ! _si )
                        return end();

                    const SerializationInfo* si = _si->findMember(name);
                    return ConstEntry(si);
                }

                /** @brief Returns a sub entry.
                */
                ConstEntry operator[] (const std::string& name) const
                {
                    return this->entry(name);
                }

                /** @brief Returns a sub entry.
                */
                ConstEntry operator[] (const char* name) const
                {
                    return this->entry(name);
                }

                /** @brief Returns the entry name.
                */
                const char* name() const
                { return _si->name(); }

                /** @brief Allows using the entry like an iterator.
                */
                const ConstEntry& operator*() const
                { return *this; }

                /** @brief Allows using the entry like an iterator.
                */
                const ConstEntry* operator->() const
                { return this; }

                /** @brief Allows using the entry like an iterator.
                */
                ConstEntry& operator++()
                {
                    _si = _si->sibling();
                    return *this;
                }

                /** @brief Allows using the entry like an iterator.
                */
                bool operator!=(const ConstEntry& other) const
                { return _si != other._si; }

                /** @brief Allows using the entry like an iterator.
                */
                bool operator==(const ConstEntry& other) const
                { return _si == other._si; }

                /** @brief Returns true if entry is invalid.
                */
                bool operator!() const
                { return _si == 0; }

            private:
                const SerializationInfo* _si;
        };

    public:
        /** @brief Default constructor.
        */
        Settings();

        /** @brief Begin of entries.
        */
        ConstEntry begin() const
        { return root().begin(); }

        /** @brief End of entries.
        */
        ConstEntry end() const
        { return root().end(); }

        /** @brief Returns the root entry.
        */
        ConstEntry root() const
        { return ConstEntry(this); }

        /** @brief Begin of entries.
        */
        Entry begin()
        { return root().begin(); }

        /** @brief End of entries.
        */
        Entry end()
        { return root().end(); }

        /** @brief Returns the root entry.
        */
        Entry root()
        { return Entry(this); }

        /** @brief Loads settings from a input stream.
        */
        void load( std::basic_istream<Pt::Char>& is );

        /** @brief Saves settings to a output stream.
        */
        void save( std::basic_ostream<Pt::Char>& os ) const;

        /** @brief Returns a top level entry.
        */
        ConstEntry entry(const std::string& name) const
        {
            return root().entry(name);
        }

        /** @brief Returns a top level entry.
        */
        ConstEntry entry(const char* name) const
        {
            return root().entry(name);
        }

        /** @brief Returns a top level entry.
        */
        ConstEntry operator[] (const std::string& name) const
        {
            return this->entry(name);
        }

        /** @brief Returns a top level entry.
        */
        ConstEntry operator[] (const char* name) const
        {
            return this->entry(name);
        }

        /** @brief Returns a top level entry.
        */
        Entry entry(const std::string& name)
        {
            SerializationInfo* si = this->findMember(name);
            return Entry(si);
        }

        /** @brief Returns a top level entry.
        */
        Entry entry(const char* name)
        {
            SerializationInfo* si = this->findMember(name);
            return Entry(si);
        }

        /** @brief Returns a top level entry.
        */
        Entry operator[] (const std::string& name)
        {
            return this->entry(name);
        }

        /** @brief Returns a top level entry.
        */
        Entry operator[] (const char* name)
        {
            return this->entry(name);
        }
};

} // namespace Pt

#endif
