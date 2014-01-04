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
#include <Pt/String.h>
#include <sstream>

//namespace Pt {

///** @brief Unicode string stream buffer.
//
//    @ingroup Unicode
//*/
//class PT_API StringBuffer : public std::basic_stringbuf<Pt::Char>
//{
//    public:
//        explicit StringBuffer(std::ios::openmode mode = std::ios::in | std::ios::out);
//
//        explicit StringBuffer(const Pt::String& str,
//                              std::ios::openmode mode = std::ios::in | std::ios::out);
//};
//
//} // namespace Pt
//
//namespace std {
//
//template<>
//class PT_API basic_istringstream<Pt::Char> : public basic_istream<Pt::Char>
//{
//    public:
//        typedef Pt::Char char_type;
//        typedef std::char_traits<Pt::Char> traits_type;
//        typedef std::allocator<Pt::Char> allocator_type;
//        typedef traits_type::int_type int_type;
//        typedef traits_type::pos_type pos_type;
//        typedef traits_type::off_type off_type;
//
//    public:
//        explicit basic_istringstream(ios_base::openmode mode = ios_base::in);
//
//        explicit basic_istringstream(const Pt::String& str,
//                                    std::ios_base::openmode mode = std::ios_base::in);
//
//        basic_stringbuf<Pt::Char>* rdbuf() const
//        { return const_cast<Pt::StringBuffer*>(&_buffer); }
//
//        Pt::String str() const
//        { return _buffer.str(); }
//
//        void str(const Pt::String& str)
//        { _buffer.str(str); }
//
//    private:
//        Pt::StringBuffer _buffer;
//};
//
//template<>
//class PT_API basic_ostringstream<Pt::Char> : public basic_ostream<Pt::Char>
//{
//    public:
//        typedef Pt::Char char_type;
//        typedef std::char_traits<Pt::Char> traits_type;
//        typedef std::allocator<Pt::Char> allocator_type;
//        typedef traits_type::int_type int_type;
//        typedef traits_type::pos_type pos_type;
//        typedef traits_type::off_type off_type;
//
//    public:
//        explicit basic_ostringstream(ios_base::openmode mode = ios_base::out);
//
//        explicit basic_ostringstream(const Pt::String& str,
//                                    std::ios_base::openmode mode = std::ios_base::out);
//
//        basic_stringbuf<Pt::Char>* rdbuf() const
//        { return const_cast<Pt::StringBuffer*>(&_buffer); }
//
//        Pt::String str() const
//        { return _buffer.str(); }
//
//        void str(const Pt::String& str)
//        { _buffer.str(str); }
//
//    private:
//        Pt::StringBuffer _buffer;
//};
//
//template<>
//class PT_API basic_stringstream<Pt::Char> : public basic_iostream<Pt::Char>
//{
//    public:
//        typedef Pt::Char char_type;
//        typedef std::char_traits<Pt::Char> traits_type;
//        typedef std::allocator<Pt::Char> allocator_type;
//        typedef traits_type::int_type int_type;
//        typedef traits_type::pos_type pos_type;
//        typedef traits_type::off_type off_type;
//
//    public:
//        explicit basic_stringstream(ios_base::openmode mode = ios_base::in | ios_base::out);
//
//        explicit basic_stringstream(const Pt::String& str,
//                                    std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);
//
//        basic_stringbuf<Pt::Char>* rdbuf() const
//        { return const_cast<Pt::StringBuffer*>(&_buffer); }
//
//        Pt::String str() const
//        { return _buffer.str(); }
//
//        void str(const Pt::String& str)
//        { _buffer.str(str); }
//
//    private:
//        Pt::StringBuffer _buffer;
//};
//
//} // namespace std


namespace Pt {

/** @brief Unicode string stream.

    @ingroup Unicode
*/
typedef std::basic_stringstream<Pt::Char> StringStream;

/** @brief Unicode string stream.

    @ingroup Unicode
*/
typedef std::basic_istringstream<Pt::Char> IStringStream;

/** @brief Unicode string stream.

    @ingroup Unicode
*/
typedef std::basic_ostringstream<Pt::Char> OStringStream;

} // namespace Pt

#endif
