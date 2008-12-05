/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Duerner                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_STRINGSTREAM_H
#define PT_STRINGSTREAM_H

#include <Pt/Api.h>
#include <Pt/Char.h>
#include <Pt/String.h>
#include <sstream>

namespace Pt {

namespace detail {

template <class CharT, class TraitsT>
class basic_ios : public std::ios_base
{
  friend class std::ios_base;

    public:
        typedef CharT                      char_type;
        typedef typename TraitsT::int_type int_type;
        typedef typename TraitsT::pos_type pos_type;
        typedef typename TraitsT::off_type off_type;
        typedef TraitsT                    traits_type;

    public:
        explicit basic_ios(std::basic_streambuf<CharT, TraitsT>* sb)
        : ios_base()
        , PT_fill(0)
        , PT_streambuf(0)
        , PT_tied_ostream(0)
        {
            basic_ios<CharT, TraitsT>::init(sb);
        }

        virtual ~basic_ios()
        {}

    public:
        std::basic_ostream<CharT, TraitsT>* tie() const
        {
            return PT_tied_ostream;
        }

        std::basic_ostream<CharT, TraitsT>* tie(std::basic_ostream<char_type, traits_type>* os)
        {
            std::basic_ostream<char_type, traits_type>* __tmp = PT_tied_ostream;
            PT_tied_ostream = os;
            return __tmp;
        }

        std::basic_streambuf<CharT, TraitsT>* rdbuf() const
        { return PT_streambuf; }

        std::basic_streambuf<CharT, TraitsT>* rdbuf(std::basic_streambuf<char_type, traits_type>* sb)
        {
            std::basic_streambuf<char_type, traits_type>* __tmp = PT_streambuf;
            PT_streambuf = sb;
            this->clear();
            return __tmp;
        }

        // Copies __x's state to *this.
        basic_ios<CharT, TraitsT>& copyfmt(const basic_ios<CharT, TraitsT>& __x)
        {
            //PT_invoke_callbacks(erase_event);
            PT_copy_state(__x); // Inherited from ios_base.
            PT_fill = __x.PT_fill;
            PT_tied_ostream = __x.PT_tied_ostream;
            //PT_invoke_callbacks(copyfmt_event);
            this->PT_set_exception_mask(__x.exceptions());
            return *this;
        }

        char_type fill() const
        { return PT_fill; }

        char_type fill(char_type f)
        {
            char_type __tmp(PT_fill);
            PT_fill = f;
            return __tmp;
        }

    public:
        void clear(iostate __state = goodbit)
        {
            PT_clear_nothrow(this->rdbuf() ? __state : iostate(__state|ios_base::badbit));
            PT_check_exception_mask();
        }

        void setstate(iostate __state)
        { this->clear(rdstate() | __state); }

        iostate exceptions() const
        { return this->PT_get_exception_mask(); }

        void exceptions(iostate __mask)
        {
            this->PT_set_exception_mask(__mask);
            this->clear(this->rdstate());
        }

        operator void*() const
        { return ! fail() ? this : (void*) 0; }

        bool operator!() const
        { return fail(); }

        iostate rdstate() const
        { return PT_iostate; }

        bool good() const
        { return PT_iostate == 0; }

        bool eof() const
        { return (PT_iostate & eofbit) != 0; }

        bool fail() const
        { return (PT_iostate & (failbit | badbit)) != 0; }

        bool bad() const
        { return (PT_iostate & badbit) != 0; }

    public:
        std::locale imbue(const std::locale& __loc)
        {
            //locale __tmp = ios_base::imbue(__loc);
            //try
            //{
            //    if (PT_streambuf)
            //        PT_streambuf->pubimbue(__loc);
            //
            //    // no throwing here
            //    this->PT_cached_ctype = __loc.PT_get_facet(ctype<char_type>::id);
            //    this->PT_cached_numpunct = __loc.PT_get_facet(numpunct<char_type>::id);
            //    this->PT_cached_grouping = ((numpunct<char_type>*)PT_cached_numpunct)->grouping();
            //}
            //catch(...)
            //{
            //    __tmp = ios_base::imbue(__tmp);
            //    this->PT_setstate_nothrow(__flag);
            //    if (this->PT_get_exception_mask() & ios_base::failbit)
            //        throw;
            //}
            //
            //return __tmp;
            return __loc;
        }


        char narrow(CharT ch, char __default) const
        {
            return ch.narrow(__default);
        }

        inline CharT widen(char ch) const
        {
            return Pt::Char(ch);
        }

        static bool is_eof(int_type ch)
        {
            const int_type char_eof = TraitsT::eof();
            return TraitsT::eq_int_type(ch, char_eof);
        }

    protected:
        basic_ios()
        : ios_base()
        , PT_fill(0)
        , PT_streambuf(0)
        , PT_tied_ostream(0)
        {}

        void init(std::basic_streambuf<CharT, TraitsT>* __sb)
        {
            this->rdbuf(__sb);
            this->imbue( std::locale() );
            this->tie(0);

            this->PT_set_exception_mask(ios_base::goodbit);
            this->PT_clear_nothrow(__sb != 0 ? ios_base::goodbit : ios_base::badbit);

            ios_base::flags(ios_base::skipws | ios_base::dec);
            ios_base::width(0);
            ios_base::precision(6);
            this->fill(widen(' '));
            // We don't need to worry about any of the three arrays: they are
            // initialized correctly in ios_base's constructor.
        }

    private:
        iostate PT_get_exception_mask() const
        { return PT_exception_mask; }

        void PT_set_exception_mask(iostate __mask)
        { PT_exception_mask = __mask; }

        void PT_check_exception_mask()
        {
            if (PT_iostate & PT_exception_mask)
                PT_throw_failure();
        }

        void PT_clear_nothrow(iostate __state)
        { PT_iostate = __state; }

        void PT_invoke_callbacks(event)
        {}

        void PT_copy_state(const ios_base& __x);

        void PT_throw_failure();

        iostate PT_exception_mask;
        iostate PT_iostate;
        char_type PT_fill;
        std::basic_streambuf<CharT, TraitsT>* PT_streambuf;
        std::basic_ostream<CharT, TraitsT>*   PT_tied_ostream;
};

}

class PT_API BasicStringStreamBuffer : public std::basic_stringbuf<Pt::Char>
{
    public:
        explicit BasicStringStreamBuffer(std::ios::openmode mode = std::ios::in | std::ios::out);
        explicit BasicStringStreamBuffer(const Pt::String& str, std::ios::openmode mode = std::ios::in | std::ios::out);
};

} // namespace Pt


namespace std {

template<>
class PT_API basic_stringstream<Pt::Char> : public basic_iostream<Pt::Char>
{
    public:
        typedef Pt::Char char_type;
        typedef std::char_traits<Pt::Char> traits_type;
        typedef std::allocator<Pt::Char> allocator_type;

        typedef traits_type::int_type int_type;
        typedef traits_type::pos_type pos_type;
        typedef traits_type::off_type off_type;

        explicit basic_stringstream(ios_base::openmode mode = ios_base::in | ios_base::out);

        explicit basic_stringstream(const Pt::String& str, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

        virtual ~basic_stringstream();

        basic_stringbuf<Pt::Char>* rdbuf() const;

        Pt::String str() const;

        void str(const Pt::String& newStr);

    private:
        Pt::BasicStringStreamBuffer* _buffer;
};

} // namespace std


namespace Pt {

    typedef std::basic_stringstream<Pt::Char> StringStream;

} // namespace Pt

#endif
