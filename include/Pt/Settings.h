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

/** @brief Invalid settings text.

    @ingroup Pt-Core
*/
class PT_API SettingsError : public SerializationError
{
    public:
        /** @brief Constructs with a message and the line of the error.
        */
        SettingsError(const char* what, std::size_t line);

        /** @brief Destructor.
        */
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

/** @brief Hierarchical application settings loaded from text.

    %Settings is a tree of named entries that a program can load, change,
    and save. The unit of persistence is the whole tree. %load() replaces
    it from a @c std::basic_istream of %Pt::Char or from a %Formatter.
    %save() writes it the same way. A file is a typical source, but a
    string stream is equally valid, which is useful in tests.

    @code
    std::ifstream ifs("app.settings");
    Pt::TextIStream tis(ifs, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(tis);
    @endcode

    The text format stores scalars, arrays, and structs. Integers,
    floating-point values, strings, and booleans are scalars. An array
    is a bracketed list. A struct is a brace-delimited list of named
    members. A @c [section] line is a top-level struct: the names that
    follow become its members until the next section.

    @code
    a = 1
    b = 3.14
    c = "Hello World!"
    d = true
    e = [ 1, 2, 3 ]
    f = { red = 255, green = 0, blue = 0 }

    [animals]
    a = "dog"
    b = "cat"
    @endcode

    After a load, that file has top-level entries @c a through @c f
    and a top-level entry @c animals with subentries @c a and @c b.
    %entry() and @c operator[] return a %ConstEntry or %Entry by name.
    A missing name yields an empty entry. Empty entries are false in
    boolean context. %get() extracts a serializable value and returns
    false when the entry is empty. %set() replaces the value of an
    existing entry. %addEntry() and %makeEntry() create members;
    %makeEntry() returns the member if it already exists.
    %removeEntry() drops a member.

    The stored type must have serialization operators. STL containers
    already do. A user type needs @c operator<<= and @c operator>>=
    for %SerializationInfo, the same operators the rest of the
    serialization framework uses. %Settings privately inherits
    %SerializationInfo; do not use that base as a public API.

    @code
    int a = 0;
    bool ok = settings["a"].get(a);

    std::vector<int> e;
    ok = settings.entry("e").get(e);

    settings.makeEntry("port").set(8080);
    settings.save(tos);
    @endcode

    %load() from malformed text throws %SettingsError, which reports
    the line of the failure. Saving truncates nothing by itself; open
    the destination stream with @c ios::trunc when the file should be
    replaced. %Entry iterates like a sibling walk: %begin() / %end()
    and @c operator++ move to the next member of the same parent.

    @ingroup Pt-Core
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
                Entry addEntry(const std::string& name)
                {
                    if( ! _si )
                        return Entry();

                    SerializationInfo& si = _si->addMember(name);
                    return Entry(&si);
                }

                /** @brief Adds a sub entry.
                */
                Entry addEntry(const char* name)
                {
                    if( ! _si )
                        return Entry();

                    SerializationInfo& si = _si->addMember(name);
                    return Entry(&si);
                }

                /** @brief Adds a sub entry.
                */
                Entry addEntry()
                {
                    if( ! _si )
                        return Entry();

                    SerializationInfo& si = _si->addElement();
                    return Entry(&si);
                }

                /** @brief Removes a sub entry.
                */
                void removeEntry(const std::string& name)
                {
                    if( _si )
                        _si->removeMember(name);
                }
                
                /** @brief Removes a sub entry.
                */
                void removeEntry(const char* name)
                {
                    if( _si )
                        _si->removeMember(name);
                }

                /** @brief Removes a sub entry.
                */
                void removeEntry(const Entry& e)
                {
                    if( ! _si || ! e._si )
                      return;

                    _si->removeMember( *e._si );
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
                Entry makeEntry(const char* name)
                {
                    if( ! _si )
                        return this->end();

                    SerializationInfo* si = _si->findMember(name);
                    if( ! si )
                        si = &_si->addMember(name);
                    
                    return Entry(si);
                }

                /** @brief Returns a sub entry.
                */
                Entry makeEntry(const std::string& name)
                {
                    if( ! _si )
                        return this->end();

                    SerializationInfo* si = _si->findMember(name);
                    if( ! si )
                        si = &_si->addMember(name);
                    
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

        /** @brief Clears the settings.
        */
        void clear();

        /** @brief Returns true if settings are empty.
        */
        bool isEmpty() const;
        
        /** @brief Sets the name of the settings root.
        */
        void setName(const char* name);

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
        void load(std::basic_istream<Pt::Char>& is);

        /** @brief Loads settings using a formatter.
        */
        void load(Pt::Formatter& formatter);

        /** @brief Saves settings to a output stream.
        */
        void save(std::basic_ostream<Pt::Char>& os) const;

        /** @brief Saves settings using a formatter.
        */
        void save(Pt::Formatter& formatter) const;

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

        /** @brief Adds a top level entry.
        */
        Entry addEntry(const char* name)
        {
            return root().addEntry(name);
        }

        /** @brief Adds a top level entry.
        */
        Entry addEntry(const std::string& name)
        {
            return root().addEntry(name);
        }

        /** @brief Makes a top level entry.
        */
        Entry makeEntry(const char* name)
        {
            return root().makeEntry(name);
        }

        /** @brief Makes a top level entry.
        */
        Entry makeEntry(const std::string& name)
        {
            return root().makeEntry(name);
        }

        /** @brief Removes a top level entry.
        */
        void removeEntry(const char* name)
        {
            root().removeEntry(name);
        }

        /** @brief Removes a top level entry.
        */
        void removeEntry(const std::string& name)
        {
            root().removeEntry(name);
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
