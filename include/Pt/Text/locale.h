/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
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

#ifndef PTV_TEXT_LOCALE_H
#define PTV_TEXT_LOCALE_H

#include <Pt/Text/Api.h>

/**
 * This header files provides some basic dummy implementations for locale, locale::facet, ctype_base,
 * ctype, codecvt_base and codecvt for platforms which do not support the locale-classes of the STL.
 * The define PTV_WITHOUT_STD_LOCALE is set for those classes which do not support locale etc.
 * For all other platform's the <locale>-header of the STL is includes in this header as usual.
 *
 * For platform-independent usage only this header-file (locale.h) should be used. Do not include
 * <locale>!
 *
 * Don't use this:
 * \code
 * #include <locale>
 * \endcode
 * But use this:
 * \code
 * #include "Pt/Text/locale.h"
 * \endcode
 */

	#ifdef PT_WITHOUT_STD_LOCALE

		namespace std {

			class PT_TEXT_API locale
			{
				public:
					class PT_TEXT_API facet
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


			class PT_TEXT_API ctype_base
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

					typedef short mask;

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

			//typedef int mbstate_t;

			class PT_TEXT_API codecvt_base
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

	#else
		#include <locale>

	#endif

#endif
