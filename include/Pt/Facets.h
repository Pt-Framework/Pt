/***************************************************************************
 *   Copyright (C) 2004-2007 Marc Boris Duerner                            *
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
#ifndef PT_FACETS_H
#define PT_FACETS_H

#include <Pt/Api.h>
#include <Pt/String.h>
#include <cctype>


#ifndef PT_WITHOUT_STD_LOCALE

    #include <locale>

#else

namespace std {

    class PT_API locale
    {
        public:
            class PT_API facet
            {
                public:
                    facet(size_t refs)
                        :_refs(refs)
                    {}

                private:
                    size_t _refs;
            };

            class id
            {
            };

        private:
            int _dummy;
    };


    class PT_API ctype_base
    {
        public:
            enum {
                alpha  = 1 << 5,
                cntrl  = 1 << 2,
                digit  = 1 << 6,
                lower  = 1 << 4,
                print  = 1 << 1,
                punct  = 1 << 7,
                space  = 1 << 0,
                upper  = 1 << 3,
                xdigit = 1 << 8,
                alnum  = alpha | digit,
                graph  = alnum | punct
            };

            typedef unsigned short mask;

            ctype_base(size_t _refs = 0)
            { }
    };


    template <typename T>
    class ctype
    {
        public:
            ctype()
            { }
    };


    class PT_API codecvt_base
    {
        public:
            enum {
                ok, partial, error, noconv
            };

            typedef int result;

            codecvt_base(size_t _Refs = 0)
            {
            }

            class id
            {
            };
    };


    template<class internT, class externT, class stateT>
    class codecvt : public codecvt_base
    {
        public:
            typedef internT intern_type;
            typedef externT extern_type;
            typedef stateT state_type;

            explicit codecvt(size_t refs = 0)
            : codecvt_base(refs)
            {}

            result in(stateT& state,
                        const externT *from, const externT *from_end, const externT *& from_next,
                        internT *to, internT *to_limit, internT *& to_next) const
            { return 0; }

            result out(stateT& state,
                        const internT *from, const internT *from_end, const internT *& from_next,
                        externT *to, externT *to_limit, externT *& to_next) const
            { return 0; }

            result unshift(stateT& state, externT to, externT to_end, externT*& to_next) const
            { return 0; }

            int encoding() const
            { return 0; }

            bool always_noconv() const
            { return false; }

            int length(stateT& state, const externT* from, const externT* end, size_t max) const
            { return 0; }

            int max_length() const
            { return 0; }

        protected:
            virtual ~codecvt() = 0;

            virtual result do_in(stateT& state,
                                    const externT *from, const externT* from_end, const externT*& from_next,
                                    internT* to, internT* to_limit, internT*& to_next) const = 0;

            virtual result do_out(stateT&,
                                    const internT* from, const internT* from_end, const internT*& from_next,
                                    externT* to, externT* to_limit, externT*& to_next) const = 0;

            virtual result do_unshift(stateT& state, externT* to, externT* to_limit, externT*& to_next) const = 0;

            virtual int do_length(const stateT& state, const externT* from, const externT* end, size_t max) const = 0;

            virtual int do_encoding() const = 0;

            virtual bool do_always_noconv() const = 0;

            virtual int do_max_length() const = 0;
    };

} // namespace std

#endif

namespace std {

    /** @brief Numpunct localization facet
        @ingroup Unicode
    */
    template <>
    class PT_API numpunct<Pt::Char> : public locale::facet {
        public:
            typedef Pt::Char char_type;
            typedef std::basic_string<Pt::Char> string_type;


			#if __GLIBCXX__ == 20050421
			typedef __numpunct_cache<Pt::Char>  __cache_type;
			#endif
            // gcc 3.4.x violates the c++ standard by requiring a __numpunct_cache
			//#if __GNUC__ == 3 && __GNUC_MINOR__ == 4
            //typedef __numpunct_cache<Pt::Char>  __cache_type;
            //#endif

            static locale::id id;

        public:

            explicit numpunct(size_t refs = 0);

            virtual ~numpunct();

            char_type decimal_point() const;

            char_type thousands_sep() const;

            string grouping() const;

            string_type truename() const;

            string_type falsename() const;

        protected:
            virtual char_type do_decimal_point() const;

            virtual char_type do_thousands_sep() const;

            virtual string do_grouping() const;

            virtual string_type do_truename() const;

            virtual string_type do_falsename() const;
    };

    class ios_base;

#if (defined _MSC_VER || defined __QNX__ || defined __xlC__)

// According to the VC compiler warning, ctype_base is declared as class on WinCE
#ifdef _WIN32_WCE
    class PT_API ctype_base;
#else
    struct PT_API ctype_base;
#endif

    /** @brief Ctype localization facet
        @ingroup Unicode
    */
    template <>
    class PT_API ctype< Pt::Char > : public ctype_base {

#else
    /** @brief Ctype localization facet
        @ingroup Unicode
    */
    template <>
    class PT_API ctype<Pt::Char> : public ctype_base, public locale::facet {

#endif

        public:
            typedef ctype_base::mask mask;

            static locale::id id;

        public:
            explicit ctype(size_t refs = 0);

            virtual ~ctype();

            bool is(mask m, Pt::Char c) const
            { return this->do_is(m, c); }

            const Pt::Char* is(const Pt::Char *lo, const Pt::Char *hi, mask *vec) const
            { return this->do_is(lo, hi, vec); }

            const Pt::Char* scan_is(mask m, const Pt::Char* lo, const Pt::Char* hi) const
            { return this->do_scan_is(m, lo, hi); }

            const Pt::Char* scan_not(mask m, const Pt::Char* lo, const Pt::Char* hi) const
            { return this->do_scan_not(m, lo, hi); }

            Pt::Char toupper(Pt::Char c) const
            { return this->do_toupper(c); }

            const Pt::Char* toupper(Pt::Char *lo, const Pt::Char* hi) const
            { return this->do_toupper(lo, hi); }

            Pt::Char tolower(Pt::Char c) const
            { return this->do_tolower(c); }

            const Pt::Char* tolower(Pt::Char* lo, const Pt::Char* hi) const
            { return this->do_tolower(lo, hi); }

            Pt::Char widen(char c) const
            { return this->do_widen(c); }

            const char* widen(const char* lo, const char* hi, Pt::Char* to) const
            { return this->do_widen(lo, hi, to); }

            char narrow(Pt::Char c, char dfault) const
            { return this->do_narrow(c, dfault); }

            const Pt::Char* narrow(const Pt::Char* lo, const Pt::Char* hi,
                                   char dfault, char *to) const
            { return this->do_narrow(lo, hi, dfault, to); }

        protected:
            virtual bool do_is(mask m, Pt::Char c) const;

            virtual const Pt::Char* do_is(const Pt::Char* lo, const Pt::Char* hi,
                                           mask* vec) const;

            virtual const Pt::Char* do_scan_is(mask m, const Pt::Char* lo,
                                                const Pt::Char* hi) const;

            virtual const Pt::Char* do_scan_not(mask m, const Pt::Char* lo,
                                                 const Pt::Char* hi) const;

            virtual Pt::Char do_toupper(Pt::Char) const;

            virtual const Pt::Char* do_toupper(Pt::Char* lo, const Pt::Char* hi) const;

            virtual Pt::Char do_tolower(Pt::Char) const;

            virtual const Pt::Char* do_tolower(Pt::Char* lo, const Pt::Char* hi) const;

            virtual Pt::Char do_widen(char) const;

            virtual const char* do_widen(const char* lo, const char* hi,
                                          Pt::Char* dest) const;

            virtual char do_narrow(Pt::Char, char dfault) const;

            virtual const Pt::Char* do_narrow(const Pt::Char* lo, const Pt::Char* hi,
                                               char dfault, char* dest) const;
    };

#if PT_STLPORT
    template <>
    class PT_API num_put<Pt::Char> : public locale::facet {
	public:
		typedef Pt::Char    char_type;
        typedef ostreambuf_iterator<Pt::Char>   iter_type;

        typedef ostreambuf_iterator<wchar_t,char_traits<wchar_t> > iter_type_w;
        locale loc;
        const num_put<wchar_t,iter_type_w>& numput_wchar;
		
		explicit num_put(size_t refs = 0);

#if !defined (_STLP_NO_BOOL)
		iter_type put(iter_type s, ios_base& f, char_type fill, 
            bool val) const;
#endif
		
        iter_type put(iter_type s, ios_base& f, char_type fill, 
            long val) const;

        iter_type put(iter_type s, ios_base& f, char_type fill, 
            unsigned long val) const;

#if defined (_STLP_LONG_LONG)
        iter_type put(iter_type s, ios_base& f, char_type fill, 
            long long val) const;

        iter_type put(iter_type s, ios_base& f, char_type fill, 
        	unsigned long long val) const;
#endif

        iter_type put(iter_type s, ios_base& f, char_type fill, 
            double val) const;
		
#if !defined (_STLP_NO_LONG_DOUBLE)
        iter_type put(iter_type s, ios_base& f, char_type fill, 
            long double val) const;
#endif
        
        iter_type put(iter_type s, ios_base& f, char_type fill, 
            void* val) const;

        static locale::id id;

	protected:
        virtual ~num_put()
        {
        }

#if !defined (_STLP_NO_BOOL)
		virtual iter_type do_put(iter_type s, ios_base& f, char_type fill,
			bool val) const;
#endif
		
        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill,
			long val) const;
        
        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill,
			unsigned long val) const;
		
#if defined (_STLP_LONG_LONG)
        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill, 
            long long val) const;

        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill, 
            unsigned long long val) const;
#endif

        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill,
			double val) const;
		
#if !defined (_STLP_NO_LONG_DOUBLE)
        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill,
			long double  val) const;
#endif
        
        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill,
            void*) const;
	};

    template <>
    class PT_API num_get<Pt::Char> : public locale::facet {
    public:
        typedef Pt::Char    char_type;
        typedef istreambuf_iterator<Pt::Char>   iter_type;

        typedef istreambuf_iterator<wchar_t,char_traits<wchar_t> > iter_type_w;
        locale loc;
        const num_get<wchar_t,iter_type_w>& numget_wchar;

        explicit num_get(size_t refs = 0);

#if !defined (_STLP_NO_BOOL)
        iter_type get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, bool& val) const;
#endif
        
        iter_type get(iter_type, iter_type, ios_base& ,
            ios_base::iostate&, long&) const;

        iter_type get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, unsigned short& val) const;

        iter_type get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, unsigned int& val) const;

        iter_type get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, unsigned long& val) const;

#if defined (_STLP_LONG_LONG)
        iter_type get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, long long& val) const;

        iter_type get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, unsigned long long& val) const;
#endif

        iter_type get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, float& val) const;

        iter_type get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, double& val) const;

#if !defined (_STLP_NO_LONG_DOUBLE)
        iter_type get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, long double& val) const;
#endif
        
        iter_type get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, void* val) const;

        static locale::id id;
    protected:
        virtual ~num_get()
        {
        }

#if !defined (_STLP_NO_BOOL)
        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, bool& val) const;
#endif
        
        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate&, long& val) const;

        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, unsigned short& val) const;

        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, unsigned int& val) const;

        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, unsigned long& val) const;

#if defined (_STLP_LONG_LONG)
        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, long long& val) const;

        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, unsigned long long& val) const;
#endif

        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, float& val) const;

        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, double& val) const;

#if !defined (_STLP_NO_LONG_DOUBLE)
        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, long double& val) const;
#endif
        
        virtual iter_type do_get(iter_type s, iter_type e, ios_base& f,
            ios_base::iostate& state, void* val) const;
    };


#endif
    
#if (defined _MSC_VER || defined __QNX__)

    class PT_API codecvt_base;

    template<>
    class PT_API codecvt<Pt::Char, char, std::mbstate_t> : public codecvt_base {

#else

    template<>
    class PT_API codecvt<Pt::Char, char, std::mbstate_t> : public codecvt_base, public locale::facet {
#endif

        public:
            static locale::id id;

        public:
            explicit codecvt(size_t ref = 0);

            virtual ~codecvt();

            codecvt_base::result out(std::mbstate_t& state, const Pt::Char* from,
                                     const Pt::Char* from_end, const Pt::Char*& from_next,
                                     char* to, char* to_end, char*& to_next) const
            { return this->do_out(state, from, from_end, from_next, to, to_end, to_next); }

            codecvt_base::result unshift(std::mbstate_t& state, char* to, char* to_end,
                                         char*& to_next) const
            { return this->do_unshift(state, to, to_end, to_next); }

            codecvt_base::result in(std::mbstate_t& state, const char* from,
                                    const char* from_end, const char*& from_next,
                                    Pt::Char* to, Pt::Char* to_end, Pt::Char*& to_next) const
            { return this->do_in(state, from, from_end, from_next, to, to_end, to_next); }

            int encoding() const
            { return this->do_encoding(); }

            bool always_noconv() const
            { return this->do_always_noconv(); }

            int length(std::mbstate_t& state, const char* from,
                       const char* end, size_t max) const
            { return this->do_length(state, from, end, max); }

            int max_length() const
            { return this->do_max_length(); }

        protected:
            virtual codecvt_base::result do_out(std::mbstate_t& state, const Pt::Char* from,
                                                const Pt::Char* from_end, const Pt::Char*& from_next,
                                                char* to, char* to_end, char*& to_next) const = 0;

            virtual codecvt_base::result do_unshift(std::mbstate_t& state, char* to,
                                                    char* to_end, char*& to_next) const = 0;

            virtual codecvt_base::result do_in(std::mbstate_t& state,
                                               const char* from, const char* from_end,
                                               const char*& from_next, Pt::Char* to, Pt::Char* to_end,
                                               Pt::Char*& to_next) const = 0;

            virtual int do_encoding() const throw() = 0;

            virtual bool do_always_noconv() const throw() = 0;

            virtual int do_length(std::mbstate_t&, const char* from,
                                  const char* end, size_t max) const = 0;

            virtual int do_max_length() const throw() = 0;
    };

}

namespace Pt {

static struct PT_API InitLocale
{
    InitLocale()
    {
        #ifndef PT_WITHOUT_STD_LOCALE
        std::locale::global( std::locale(std::locale(), new std::ctype<Pt::Char>) );
        std::locale::global( std::locale(std::locale(), new std::numpunct<Pt::Char>) );
        std::locale::global( std::locale(std::locale(), new std::num_get<Pt::Char>) );
        std::locale::global( std::locale(std::locale(), new std::num_put<Pt::Char>) );
        #endif
    }
} _initLocale;

}

#endif
