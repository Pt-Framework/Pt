/*
 * Copyright (C) 2004-2011 Marc Boris Duerner
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
#include <Pt/Locale.h>
#include <Pt/String.h>

namespace Pt {

/** @brief Generic %TextCodec base class for specific codecs.
 
    This class contains default implementations for the methods do_unshift(), do_encoding()
    and do_always_noconv() so sub-classes do not have to implement this default behaviour.
 
    Codecs are used to convert one Text-encoding into another Text-encoding. The internal
    and external data type can be specified using the template parameter 'I' (internal) and
    'E' (external).
 
    When used on a platform which supports locales and facets the conversion may use
    locale-specific conversion of the Text.
 
    This class derives from facet std::codecvt. Further documentation can be found there.
 
    @param I The character type associated with the internal code set.
    @param E The character type associated with the external code set.
 
    @ingroup Unicode
*/
template <typename I, typename E>
class TextCodec : public std::codecvt<I, E, Pt::MBState>
{
    public:
        typedef I InternT;
        typedef E ExternT;
    
    public:
        /**
         * @brief Constructs a new TextCodec object.
         *
         * The internal and external type are specified by the template parameters of the class.
         *
         * @param ref This parameter is passed to std::codecvt. When ref == 0 the locale takes care
         * of deleting the facet. If ref == 1 the locale does not destroy the facet.
         */
        TextCodec(std::size_t ref = 0)
        : std::codecvt<InternT, ExternT, MBState>(ref)
        , _refs(ref)
        {}

    public:
        //! Empty desctructor
        virtual ~TextCodec()
        {}

        std::size_t refs() const
        { return _refs; }

    private:
        std::size_t _refs;
};

}

#endif
