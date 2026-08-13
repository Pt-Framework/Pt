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

/** @brief Read and extract command-line options.

    Arg objects can be used to process command line options passed to the
    main function of the program. A syntax for short-named and long-named
    options is supported. Short-named options start with a single hypen
    followd by a character (-O). Optionally, a value follows directly (-Ofoo) 
    or separated by whitespace(-O foo). Alternatively, option names can be
    consist of any character sequence to support unix style options(--option)
    and windows style options (/OPTION). An optional value follows either
    separated by whitespace (--option yes) or an equal character (/OPTION=yes).
    Note that constructing an Arg from with the character 'n' or with the
    string "-n" are equivalent.

    The template parameter of the Arg class is the argument value type, which
    must be streamable, i.e. the operator >> (std::istream&, T&) must be defined
    for the type T. When an Arg is constructed, the operator will be used to
    extract the value from the command-line string. the next example
    demonstrates this:
   
    @code
    int main(int argc, char* argv[])
    {
        Pt::Arg<int> option_n(argc, argv, 'n', 0);
        std::cout << "value for -n: " << option_n << endl;
    }
    @endcode

    Options are removed from the option-list, so programs can easily check,
    if there are parameters left, after all options were extracted.
    A specialization exists for boolean parameters. This implements a switch,
    which is on, if the option is present and off, if it is missing. The option
    consists, in this case, only of a command line flag without a value. Boolean
    parameters can also be grouped, so -abc is processed like -a -b -c.

    @code
    Pt::Arg<bool> debug(argc, argv, "--debug");
    if (debug)
        std::cout << "debug flag is set" << std::endl;
    @endcode

    The example shown above not only shows a boolean parameter, but also how
    long-named options are handled, in this case "--debug".

    @ingroup Utilities
 */
template <typename T>
class Arg : public ArgBaseT<T>
{
  public:
      /** @brief Constructor with initial value.
      */
      Arg(const T& def = T())
      : ArgBaseT<T>(def)
      { }

      /** @brief Extract short option.

          \param argc      1. parameter of main
          \param argv      2. of main
          \param ch        optioncharacter
          \param def       default-value

          Example:
          \code
          Pt::Arg<unsigned> offset(argc, argv, 'o', 0);
          unsigned value = offset.getValue();
          \endcode
       */
      Arg(int& argc, char* argv[], char ch, const T& def = T())
      : ArgBaseT<T>(def)
      {
          set(argc, argv, ch);
      }

      /** @brief Extract long option.

          Long option names starting with "--". This (and more) is supported
          here. Instead of giving a single option-character, you specify a string.

          Example:
          \code
          Pt::Arg<int> option_number(argc, argv, "--number", 0);
          std::cout << "number =" << option_number.getValue() << std::endl;
          \endcode
       */
        Arg(int& argc, char* argv[], const char* str, const T& def = T())
        : ArgBaseT<T>(def)
        {
            this->m_isset = set(argc, argv, str);
        }

        /** @brief Extracts the next parameter.
        */
        Arg(int& argc, char* argv[])
        : ArgBaseT<T>(T())
        {
            this->m_isset = set(argc, argv);
        }

        /** @brief Extract short option.

          \param argc      1. parameter of main
          \param argv      2. of main
          \param ch        optioncharacter

          Example:
          \code
          Pt::Arg<unsigned> offset;
          offset.set(argc, argv, 'o');
          unsigned value = offset.getValue();
          \endcode
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

        /** @brief Extract long option.

            GNU defines long options starting with "--". This (and more) is
            supported here. Instead of giving a single option-character, you
            specify a string.

            Example:
            \code
            Pt::Arg<int> option_number;
            number.set(argc, argv, "--number");
            std::cout << "number =" << option_number.getValue() << std::endl;
            \endcode
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

        //! @brief Reads next parameter and removes it.
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

//! @internal
template <>
class Arg<bool> : public ArgBase
{
    public:
        Arg(bool def = false)
        : m_value(def)
        { }

        Arg(int& argc, char* argv[], char ch, bool def = false)
        : m_value(def)
        {
            m_isset = set(argc, argv, ch);
        }

        Arg(int& argc, char* argv[], const char* str, bool def = false)
        : m_value(def)
        {
            m_isset = set(argc, argv, str);
        }

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

        bool get() const
        { return m_value; }

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
