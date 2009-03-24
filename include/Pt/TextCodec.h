/*
 * Copyright (C) 2004 Marc Boris Duerner
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
#ifndef Pt_TextCodec_h
#define Pt_TextCodec_h

#include <Pt/Api.h>
#include <Pt/Char.h>

#ifdef PT_WITHOUT_STD_LOCALE

namespace std {

    class codecvt_base
	{
	    public:
		    enum { ok, partial, error, noconv };
			typedef int result;
			
			virtual ~codecvt_base()
			{ }
	};
	
}

#endif

namespace Pt {

    /**
     * @brief Generic TextCodec class/facet which may be subclassed by specific Codec classes.
     *
     * This class contains default implementations for the methods do_unshift(), do_encoding()
     * and do_always_noconv() so sub-classes do not have to implement this default behaviour.
     *
     * Codecs are used to convert one Text-encoding into another Text-encoding. The internal
     * and external data type can be specified using the template parameter 'I' (internal) and
     * 'E' (external).
     *
     * When used on a platform which supports locales and facets the conversion may use
     * locale-specific conversion of the Text.
     *
     * This class derives from facet std::codecvt. Further documentation can be found there.
     *
     * @param I The character type associated with the internal code set.
     * @param E The character type associated with the external code set.
     *
     * @see Utf8Codec
     * @see Utf16Codec
     * @see Utf32Codec
     */
#ifdef PT_WITHOUT_STD_LOCALE
    template <typename I, typename E>
    class TextCodec : public std::codecvt_base
    {
        public:
            typedef I InternT;
            typedef E ExternT;
	    
		public:

            codecvt_base::result out(Pt::MBState& state, 
                                     const InternT* from,
                                     const InternT* from_end, 
                                     const InternT*& from_next,
                                     ExternT* to, 
                                     ExternT* to_end, 
                                     ExternT*& to_next) const
            { return this->do_out(state, from, from_end, from_next, to, to_end, to_next); }

            codecvt_base::result unshift(Pt::MBState& state, 
                                         ExternT* to, 
                                         ExternT* to_end,
                                         ExternT*& to_next) const
            { return this->do_unshift(state, to, to_end, to_next); }

            codecvt_base::result in(Pt::MBState& state, 
                                    const ExternT* from,
                                    const ExternT* from_end, 
                                    const ExternT*& from_next,
                                    InternT* to, 
                                    InternT* to_end, 
                                    InternT*& to_next) const
            { return this->do_in(state, from, from_end, from_next, to, to_end, to_next); }

            int encoding() const
            { return this->do_encoding(); }

            bool always_noconv() const
            { return this->do_always_noconv(); }

            int length(Pt::MBState& state, const ExternT* from,
                       const ExternT* end, size_t max) const
            { return this->do_length(state, from, end, max); }

            int max_length() const
            { return this->do_max_length(); }
	    
		protected:
            virtual result do_in(MBState& s, const ExternT* fromBegin,
                                 const ExternT* fromEnd, const ExternT*& fromNext,
                                 InternT* toBegin, InternT* toEnd, InternT*& toNext) const = 0;

            virtual result do_out(MBState& s, const InternT* fromBegin,
                                  const InternT* fromEnd, const InternT*& fromNext,
                                  ExternT* toBegin, ExternT* toEnd, ExternT*& toNext) const = 0;

            // inheritdoc
            virtual bool do_always_noconv() const = 0;

            // inheritdoc
            virtual int do_length(MBState& s, const ExternT* fromBegin, 
			                      const ExternT* fromEnd, size_t max) const = 0;

            // inheritdoc
            virtual int do_max_length() const = 0;

            // inheritdoc
            virtual std::codecvt_base::result do_unshift(Pt::MBState&, 
			                                             ExternT*, 
														 ExternT*, 
														 ExternT*&) const = 0;

            // inheritdoc
            virtual int do_encoding() const = 0;

#else
    template <typename I, typename E>
    class TextCodec : public std::codecvt<I, E, Pt::MBState>
    {
        public:
            typedef I InternT;
            typedef E ExternT;
#endif

        public:
            /**
             * @brief Constructs a new TextCodec object.
             *
             * The internal and external type are specified by the template parameters of the class.
             *
             * @param ref This parameter is passed to std::codecvt. When ref == 0 the locale takes care
             * of deleting the facet. If ref == 1 the locale does not destroy the facet.
             */
            TextCodec(size_t ref = 0)
#ifdef PT_WITHOUT_STD_LOCALE
			: _refs(ref)
#else
            : std::codecvt<InternT, ExternT, MBState>(ref)
            , _refs(ref)
#endif
            {}

            //! Empty desctructor
            virtual ~TextCodec()
            {}

            size_t refs() const
            { return _refs; }

        private:
            size_t _refs;
    };

} //namespace Pt

#endif
