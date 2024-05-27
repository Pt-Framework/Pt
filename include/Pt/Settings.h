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

    Many programs need to be able to restore its settings from a persistent
    location, such as a file. The @link Pt::Settings Settings@endlink class
    provides an hierachical organisation of settings entries and an API to
    read and write them in a text format. The following example illustrates
    how settings can be read from a file:

    @code
    std::ifstream ifs("app.settings");
    Pt::TextIStream tis(ifs, new Pt::Utf8codec);

    Pt::Settings settings;
    settings.load(tis);
    @endcode

    Settings can be loaded from any input stream, so the API is not limited to
    files. In this example, a file stream is opened and a text input stream is
    used to read UTF-8 encoded text. Another interesting use-case is to load
    settings from a string stream, which can greatly simplify unit testing.
    Writing settings to a file is just as easy:

    @code
    std::ofstream ofs("app.settings", std::ios::out|std::ios::trunc);
    Pt::TextOStream tos(ofs, new Pt::Utf8codec);

    Pt::Settings settings;
    settings.save(tos);
    @endcode

    Any output stream can be used to save the settings, in this case UTF-8
    encoded text is written to a file. Note, that the file is truncated when
    opened, so the content is replaced.

    Settings are saved in a compact text format, which supports integers,
    floats, strings and booleans as scalar value types and arrays and structs
    as compound types. The next example shows some possibilities:

    @code
    a = 1
    b = 3.14
    c = "Hello World!"
    d = true
    e = [ 1, 2, 3 ]
    f = { red = 255, green = 0, blue = 0 }
    @endcode

    The entry values for a, b, c and d are of type integer, float, string and
    bool, respectively. The entries e and f demonstrate the syntax for arrays
    and structs. The following example shows how such a settings file can be
    loaded and how the entries are accessed:

    @code
    std::ifstream ifs("app.settings");
    Pt::TextIStream tis(ifs, new Pt::Utf8codec);

    Pt::Settings settings;
    settings.load(tis);

    int a = 0;
    bool ok = settings["a"].get(a);

    float b = 0;
    ok = settings.entry("b").get(b);

    Pt::String c;
    ok = settings.entry("c").get(c);

    bool d = false;
    ok = settings.entry("d").get(d);

    std::vector<int> e;
    ok = settings.entry("e").get(e);

    Color f;
    ok = settings.entry("f").get(f);
    @endcode

    The @link Pt::Settings::entry() entry()@endlink method or alternatively,
    the index operator can be used, to access entries and subentries by name.
    If a subentry does not exist, an empty entry object will be returned.
    Values can be retrieved with the @link Pt::Settings::ConstEntry::get()
    get()@endlink method, which returns false, if the value does not exist.
    The data type, which is stored in the settings must be serializable i.e.
    the serialization operators must be defined. The framework defines the
    serialization operators for STL containers, so these work out of the box.
    The @link Pt::Settings::Entry::set() set()@endlink function can be used
    to set an entry to a new value, before the modified settings are saved.
    New subentries can be added using the @link Pt::Settings::addEntry()
    addEntry()@endlink function.

    Settings can be split into sections, to improve the readability of the
    file, using the following syntax:

    @code
    [animals]
    a = "dog"
    b = "cat"

    [plants]
    a = "tulip"
    b = "rose"
    @endcode

    When such a settings file is loaded, it will contain two entries named
    "animals" and "plants". Both entries will have two subentries named "a"
    and "b".

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
