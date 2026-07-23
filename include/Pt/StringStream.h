/*
 * Copyright (C) 2007 Marc Boris Duerner
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

#ifndef PT_STRINGSTREAM_H
#define PT_STRINGSTREAM_H

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/StreamBuffer.h>
#include <Pt/IOStream.h>
#include <sstream>

namespace Pt {

/** @brief Unicode string stream buffer.

    @ingroup Unicode
*/
class PT_API StringBuffer : public BasicStreamBuffer<Char>
{
    public:
        typedef BasicStreamBuffer<Char> Base;

        //! @brief Internal character type
        typedef Char char_type;

        //! @brief Internal character traits
        typedef typename std::char_traits<Char> traits_type;

        //! @brief Integer type
        typedef typename traits_type::int_type int_type;

        //! @brief Stream position type
        typedef typename traits_type::pos_type pos_type;

        //! @brief Stream offset type
        typedef typename traits_type::off_type off_type;

    public:
        explicit StringBuffer(std::ios::openmode mode = std::ios::in|std::ios::out);

        explicit StringBuffer(const Pt::String& str,
                              std::ios::openmode mode = std::ios::in|std::ios::out);

        ~StringBuffer();

        Pt::String str() const;

        void str(const Pt::String& str);

#if __cplusplus >= 201103L
        void str(Pt::String&& s);

        void swap(StringBuffer& other);
#endif

    protected:
        virtual std::streamsize showfull() override;

        virtual std::streamsize showmanyc() override;

        virtual int sync() override;

        virtual int_type pbackfail(int_type ch = traits_type::eof()) override;

        virtual int_type underflow() override;

        virtual int_type overflow( int_type ch = traits_type::eof() ) override;

        virtual std::streamsize xsgetn(char_type* s, std::streamsize n) override;

        virtual std::streamsize xsputn(const char_type* s, std::streamsize n) override;

        virtual pos_type seekoff(off_type off, std::ios_base::seekdir way,
                                 std::ios_base::openmode m = std::ios_base::in|std::ios_base::out) override;

        virtual pos_type seekpos(pos_type sp,
                                 std::ios_base::openmode m = std::ios_base::in|std::ios_base::out) override;

    private:
        std::ios_base::openmode _mode;
        Pt::String              _str;
        std::streamsize         _hwm;
};

#if defined(PT_WITH_STD_STRINGSTREAM)

namespace Pt {

typedef std::basic_stringstream<Pt::Char> StringStream;
typedef std::basic_istringstream<Pt::Char> IStringStream;
typedef std::basic_ostringstream<Pt::Char> OStringStream;

#else

/** @brief Unicode string input stream.

    @ingroup Unicode
*/
class PT_API IStringStream : public std::basic_istream<Pt::Char>
{
    public:
        typedef Pt::Char char_type;
        typedef std::char_traits<Pt::Char> traits_type;
        typedef std::allocator<Pt::Char> allocator_type;
        typedef traits_type::int_type int_type;
        typedef traits_type::pos_type pos_type;
        typedef traits_type::off_type off_type;

    public:
        explicit IStringStream(std::ios_base::openmode mode = ios_base::in);

        explicit IStringStream(const Pt::String& str,
                               std::ios_base::openmode mode = std::ios_base::in);

        StringBuffer* rdbuf() const
        { return const_cast<Pt::StringBuffer*>(&_buffer); }

        Pt::String str() const
        { return _buffer.str(); }

        void str(const Pt::String& str)
        { _buffer.str(str); }

    private:
        Pt::StringBuffer _buffer;
};

/** @brief Unicode string output stream.

    @ingroup Unicode
*/
class PT_API OStringStream : public std::basic_ostream<Pt::Char>
{
    public:
        typedef Pt::Char char_type;
        typedef std::char_traits<Pt::Char> traits_type;
        typedef std::allocator<Pt::Char> allocator_type;
        typedef traits_type::int_type int_type;
        typedef traits_type::pos_type pos_type;
        typedef traits_type::off_type off_type;

    public:
        explicit OStringStream(std::ios_base::openmode mode = ios_base::out);

        explicit OStringStream(const Pt::String& str,
                               std::ios_base::openmode mode = std::ios_base::out);

        StringBuffer* rdbuf() const
        { return const_cast<Pt::StringBuffer*>(&_buffer); }

        Pt::String str() const
        { return _buffer.str(); }

        void str(const Pt::String& str)
        { _buffer.str(str); }

    private:
        Pt::StringBuffer _buffer;
};

/** @brief Unicode string stream.

    @ingroup Unicode
*/
class PT_API StringStream : public std::basic_iostream<Pt::Char>
{
    public:
        typedef Pt::Char char_type;
        typedef std::char_traits<Pt::Char> traits_type;
        typedef std::allocator<Pt::Char> allocator_type;
        typedef traits_type::int_type int_type;
        typedef traits_type::pos_type pos_type;
        typedef traits_type::off_type off_type;

    public:
        explicit StringStream(std::ios_base::openmode mode = ios_base::in | ios_base::out);

        explicit StringStream(const Pt::String& str,
                              std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

        StringBuffer* rdbuf() const
        { return const_cast<Pt::StringBuffer*>(&_buffer); }

        Pt::String str() const
        { return _buffer.str(); }

        void str(const Pt::String& str)
        { _buffer.str(str); }

    private:
        Pt::StringBuffer _buffer;
};

#endif // PT_WITH_STD_STRINGSTREAM

} // namespace Pt

#endif
