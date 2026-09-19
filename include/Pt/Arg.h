/*
  Copyright (C) 2006,2010 by Tommi Maekitalo
  Copyright (C) 2006-2016 by Marc Duerner
 
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

#ifndef Pt_Arg_h
#define Pt_Arg_h

#include <Pt/Api.h>
#include <sstream>
#include <cstring>

namespace Pt {

//! @internal
class ArgBase
{
    public:
        ArgBase()
          : m_isset(false)
        { }

        //! @brief Returns true if the option is set, false if default is used.
        bool isSet() const
        { return m_isset; }

    protected:
        bool m_isset;

        //! @internal
        static void removeArg(int& argc, char* argv[], int pos, int n)
        {
          for ( ; pos < argc - n; ++pos)
              argv[pos] = argv[pos + n];
          
          argc -= n;
          argv[argc] = 0;
        }
};

//! @internal
template <typename T>
class ArgBaseT : public ArgBase
{
    protected:
        explicit ArgBaseT(const T& def)
        : m_value(def)
        { }

        //! @internal
        bool extract(const char* str, int& argc, char* argv[], int i, int n)
        {
            std::istringstream s(str);
            s >> m_value;
            if( ! s.fail() )
            {
                m_isset = true;
                removeArg(argc, argv, i, n);
                return true;
            }
            
            return false;
        }

    public:
        //! @brief Returns the value.
        const T& get() const
        { return m_value; }

    private:
       T m_value;
};

//! @internal
template <>
class ArgBaseT<const char*> : public ArgBase
{
    protected:
        explicit ArgBaseT(const char* def)
        : m_value(def)
        { }

        //! @internal
        bool extract(const char* str, int& argc, char* argv[], int i, int n)
        {
            m_value = str;
            m_isset = true;
            removeArg(argc, argv, i, n);
            return true;
        }

    public:
        //! @internal
        const char* get() const
        { return m_value; }

    private:
        const char* m_value;
};

//! @internal
template <>
class ArgBaseT<std::string> : public ArgBase
{
    protected:
        explicit ArgBaseT(const std::string& def)
        : m_value(def)
        { }

        //! @internal
        bool extract(const char* str, int& argc, char* argv[], int i, int n)
        {
            m_value = str;
            m_isset = true;
            removeArg(argc, argv, i, n);
            return true;
        }

    public:
        //! @internal
        const std::string& get() const
        { return m_value; }

    private:
        std::string m_value;
};

/** @brief Command-line option extracted from argc and argv.

    %Arg reads one option out of the argument vector of @c main and
    removes it, so later parameters are whatever is left. Construct
    it with @a argc, @a argv, the option name, and a default value.
    After construction, %get() is the extracted value or the default.
    %isSet() is true only when the option was present. Streaming an
    %Arg writes %get().

    @a T is the value type. Extraction uses @c operator>> on an
    @c istringstream, so @a T must be default-constructible and
    extractable. @c int, @c unsigned, @c std::string and
    @c const char* are the usual cases. @c const char* and
    @c std::string copy the argument text without parsing.

    A short option is a hyphen and one character: @c -n. The value
    may follow in the same argument (@c -n42) or in the next
    (@c -n 42). A long option is any prefix string the caller
    passes, commonly @c --name or @c /NAME. The value may follow
    after whitespace (@c --name 42) or after an equals sign
    (@c --name=42). Passing the character @c 'n' and passing the
    string @c "-n" select the same short option.

    Each successful extraction deletes that option, and its value
    if it occupied a separate argument, from @a argv and shrinks
    @a argc. Unrecognized arguments stay in place. After every
    %Arg of interest has been constructed, @a argv still holds
    the program name and any leftover operands.

    @code
    int main(int argc, char* argv[])
    {
        Pt::Arg<int> n(argc, argv, 'n', 0);
        std::cout << "value for -n: " << n << std::endl;
    }
    @endcode

    %Arg<bool> is a switch, not a parsed value. Presence of the
    flag sets it to true. There is no option argument. Short
    switches group: @c -abc is @c -a, @c -b and @c -c. A short
    switch can be turned off explicitly with @c -x-.

    @code
    Pt::Arg<bool> debug(argc, argv, "--debug");
    if (debug)
        std::cout << "debug flag is set" << std::endl;
    @endcode

    Constructing without @a argc and @a argv leaves the default.
    Call %set() later to extract. %set() does nothing if %isSet()
    is already true, so the first match wins. A constructor that
    takes only @a argc and @a argv extracts the next positional
    argument, not a named option.

    @ingroup Pt-Core
*/
template <typename T>
class Arg : public ArgBaseT<T>
{
  public:
      /** @brief Constructs with a default value.
      */
      Arg(const T& def = T())
      : ArgBaseT<T>(def)
      { }

      /** @brief Extracts a short option from @a argc and @a argv.

          @param argc Argument count from @c main.
          @param argv Argument vector from @c main.
          @param ch Short option character.
          @param def Value used when the option is absent.
      */
      Arg(int& argc, char* argv[], char ch, const T& def = T())
      : ArgBaseT<T>(def)
      {
          set(argc, argv, ch);
      }

      /** @brief Extracts a long option from @a argc and @a argv.

          @param argc Argument count from @c main.
          @param argv Argument vector from @c main.
          @param str Option prefix, such as @c "--number".
          @param def Value used when the option is absent.
      */
        Arg(int& argc, char* argv[], const char* str, const T& def = T())
        : ArgBaseT<T>(def)
        {
            this->m_isset = set(argc, argv, str);
        }

        /** @brief Extracts the next positional argument from @a argv.
        */
        Arg(int& argc, char* argv[])
        : ArgBaseT<T>(T())
        {
            this->m_isset = set(argc, argv);
        }

        /** @brief Extracts a short option from @a argc and @a argv.

            @param argc Argument count from @c main.
            @param argv Argument vector from @c main.
            @param ch Short option character.
        */
        bool set(int& argc, char* argv[], char ch)
        {
            // don't extract value, when already found
            if(this->m_isset)
                return false;

            for(int i = 1; i < argc; ++i)
            {
                if (argv[i] && argv[i][0] == '-' && argv[i][1] == ch)
                {
                    if(argv[i][2] == '\0' && i < argc - 1)
                    {
                      // -O foo
                      if (this->extract(argv[i + 1], argc, argv, i, 2))
                        return true;
                    }

                  // -Ofoo
                  if( this->extract(argv[i] + 2, argc, argv, i, 1) )
                      return true;
                }
            }

            return false;
        }

        /** @brief Extracts a long option from @a argc and @a argv.

            @param argc Argument count from @c main.
            @param argv Argument vector from @c main.
            @param str Option prefix, such as @c "--number".
        */
        bool set(int& argc, char* argv[], const char* str)
        {
            // don't extract value, when already found
            if (this->m_isset)
                return false;

            std::size_t n = std::strlen(str);
            for (int i = 1; i < argc; ++i)
            {
                if(argv[i] && (std::strncmp(argv[i], str, n) == 0))
                {
                    if (i < argc - 1 && argv[i][n] == '\0')
                    {
                        // --option value
                        if (this->extract(argv[i + 1], argc, argv, i, 2))
                            return true;
                    }

                    if (argv[i][n] == '=')
                    {
                        // --option=value
                        if (this->extract(argv[i] + n + 1, argc, argv, i, 1))
                            return true;
                    }
                }
            }

            return false;
        }

        /** @brief Extracts the next positional argument from @a argv.
        */
        bool set(int& argc, char* argv[])
        {
            // don't extract value, when already found
            if (this->m_isset)
                return false;

            if (argc > 1)
                this->extract(argv[1], argc, argv, 1, 1);

            return this->m_isset;
        }
};

/** @brief Boolean command-line switch.

    Presence of the flag sets the value to true. Short switches group
    in one argument, and @c -x- turns a short switch off. See %Arg
    for the extraction rules.

    @ingroup Pt-Core
*/
template <>
class Arg<bool> : public ArgBase
{
    public:
        /** @brief Constructs with a default value.
        */
        Arg(bool def = false)
        : m_value(def)
        { }

        /** @brief Extracts a short switch from @a argc and @a argv.
        */
        Arg(int& argc, char* argv[], char ch, bool def = false)
        : m_value(def)
        {
            m_isset = set(argc, argv, ch);
        }

        /** @brief Extracts a long switch from @a argc and @a argv.
        */
        Arg(int& argc, char* argv[], const char* str, bool def = false)
        : m_value(def)
        {
            m_isset = set(argc, argv, str);
        }

        /** @brief Extracts a short switch from @a argc and @a argv.
        */
        bool set(int& argc, char* argv[], char ch)
        {
            // don't extract value, when already found
            if (m_isset)
                return false;

            for (int i = 1; i < argc; ++i)
            {
                if (argv[i][0] == '-' && argv[i][1] != '-')
                {
                    // starts with a '-', but not with "--"
                    if (argv[i][1] == ch && argv[i][2] == '\0')
                    {
                        // single option found
                        m_value = true;
                        m_isset = true;
                        removeArg(argc, argv, i, 1);
                        return true;
                    }
                    else if(argv[i][1] == ch && 
                            argv[i][2] == '-' && 
                            argv[i][3] == '\0')
                    {
                        // Option was explicitly disabled with -x-
                        m_value = false;
                        m_isset = true;
                        removeArg(argc, argv, i, 1);
                        return true;
                    }
                    else
                    {
                        // check for optiongroup
                        for (char* p = argv[i] + 1; *p != '\0'; ++p)
                        {
                            if (*p == ch)
                            {
                                // here it is - extract it
                                m_value = true;
                                m_isset = true;
                                do
                                {
                                    *p = *(p + 1);
                                } 
                                while (*p++ != '\0');

                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

        /** @brief Extracts a long switch from @a argc and @a argv.
        */
        bool set(int& argc, char* argv[], const char* str)
        {
            // don't extract value, when already found
            if(m_isset)
                return false;

            for (int i = 1; i < argc; ++i)
            {
                if (std::strcmp(argv[i], str) == 0)
                {
                    m_value = true;
                    m_isset = true;
                    removeArg(argc, argv, i, 1);
                    return true;
                }
            }

            return false;
        }

        /** @brief Returns the switch value.
        */
        bool get() const
        { return m_value; }

        /** @brief Returns the switch value.
        */
        operator bool() const
        { return m_value; }

    private:
        bool m_value;
};

/** @brief Write %Arg to an std::ostream.

    @related Arg
*/
template <typename T>
inline std::ostream& operator<<(std::ostream& out, const Arg<T>& arg)
{
  return out << arg.get();
}

} // namespace Pt

#endif
