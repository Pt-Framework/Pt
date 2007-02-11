/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
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
 **************************************************************************/

#ifndef PTV_StringList_h
#define PTV_StringList_h

#include <Pt/TypeInfo.h>
#include <Pt/AnyTraits.h>
#include <Pt/Text/Api.h>
#include <Pt/String.h>
#include <list>


namespace Pt {

    namespace Text {

        /**
         * @brief A specialized list-class for Pt::String objects.
         *
         * This class is a convenience class which is derived from std::list and specialized for
         * containing Pt::String.
         *
         * See std::list for more details.
         */
        class PT_TEXT_API StringList : public std::list<Pt::String> {
            public:
                typedef std::list<Pt::String>::iterator Iterator;
                typedef std::list<Pt::String>::const_iterator ConstIterator;

            public:
                StringList();
        };

    } // namespace Text


    template <>
    struct PT_API TypeTraits<Pt::Text::StringList> {
        static const char* typeName()
        { return "Pt::Text::StringList"; }
    };

} // namespace Pt

#endif
