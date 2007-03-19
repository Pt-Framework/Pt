/***************************************************************************
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
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
#ifndef Pt_Gfx_GenericAlgo_h
#define Pt_Gfx_GenericAlgo_h

#include <Pt/Gfx/Api.h>
#include <Pt/IfElse.h>
#include <Pt/Types.h>


namespace Pt {

    namespace Gfx {

        /** @brief Choose the type which has greater size (from the two given types).
         */
        template <typename A, typename B>
        struct LargestSizeOf {
            typedef typename IfElse< (sizeof(A) >= sizeof(B)), A, B >::ResultT Result;
        };



        /** @brief Test if all elements in an array are equal
         */
        template<size_t N, size_t Min, typename ArrayT>
        struct EqualElements
        {
            static inline bool equal(const ArrayT& a, const ArrayT& b)
            {
                if(a[N] != b[N]) return false;
                return EqualElements<N-1, Min, ArrayT>::equal(a, b);
            }
        };

        template<size_t NEqualMin, typename ArrayT>
        struct EqualElements<NEqualMin, NEqualMin, ArrayT>
        {
            static bool equal(const ArrayT& a, const ArrayT& b)
            { return a[NEqualMin] == b[NEqualMin];  }
        };

        template<typename ArrayT>
        struct EqualElements<0, 0, ArrayT>
        {
            static bool equal(const ArrayT& a, const ArrayT& b)
            { return a[0] == b[0];  }
        };

        template<size_t N, size_t Min, typename ArrayT> inline
        bool equalElements(const ArrayT& a, const ArrayT& b)
        { return EqualElements<N-1, Min, ArrayT>::equal(a, b); }



        /** @brief Test if any elements in an array are not equal
         */
        template<size_t N, size_t Min, typename ArrayT>
        struct NotEqualElements
        {
            static inline bool notEqual(const ArrayT& a, const ArrayT& b)
            {
                if(a[N] != b[N]) return true;
                return NotEqualElements<N-1, Min, ArrayT>::notEqual(a, b);
            }
        };

        template<size_t NEqualMin, typename ArrayT>
        struct NotEqualElements<NEqualMin, NEqualMin, ArrayT>
        {
            static bool notEqual(const ArrayT& a, const ArrayT& b)
            { return a[NEqualMin] != b[NEqualMin];  }
        };

        template<typename ArrayT>
        struct NotEqualElements<0, 0, ArrayT>
        {
            static bool notEqual(const ArrayT& a, const ArrayT& b)
            { return a[0] != b[0];  }
        };

        template<size_t N, size_t Min, typename ArrayT> inline
        bool notEqualElements(const ArrayT& a, const ArrayT& b)
        { return NotEqualElements<N-1, Min, ArrayT>::notEqual(a, b); }



        /** @brief Increments all elements in an array
         */
        template<size_t N, size_t Min, typename ArrayT>
        struct IncrementElements
        {
            static void inc(ArrayT& array)
            {
                ++array[N];
                IncrementElements<N-1, Min, ArrayT>::inc(array);
            }
        };

        template<size_t NEqualMin, typename ArrayT>
        struct IncrementElements<NEqualMin, NEqualMin, ArrayT>
        {
            static void inc(ArrayT& array)
            { ++array[NEqualMin]; }
        };

        template<typename ArrayT>
        struct IncrementElements<0, 0, ArrayT>
        {
            static void inc(ArrayT& array)
            { ++array[0]; }
        };

        template<size_t N, size_t Min, typename ArrayT>
        void incrementElements(ArrayT& array)
        { IncrementElements<N-1, Min, ArrayT>::inc(array); }



        /** @brief Decrements all elements in an array
         */
        template<size_t N, size_t Min, typename ArrayT>
        struct DecrementElements
        {
            static void inc(ArrayT& array)
            {
                ++array[N];
                DecrementElements<N-1, Min, ArrayT>::inc(array);
            }
        };

        template<size_t NEqualMin, typename ArrayT>
        struct DecrementElements<NEqualMin, NEqualMin, ArrayT>
        {
            static void inc(ArrayT& array)
            { ++array[NEqualMin]; }
        };

        template<typename ArrayT>
                struct DecrementElements<0, 0, ArrayT>
        {
            static void inc(ArrayT& array)
            { ++array[0]; }
        };

        template<size_t N, size_t Min, typename ArrayT>
        void decrementElements(ArrayT& array)
        { DecrementElements<N-1, Min, ArrayT>::inc(array); }



        /** @brief Adds a value to all elements of an array
         */
        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        struct AddElements
        {
            static void add(ArrayT& to, const ArrayT& from, const ElemT& val)
            {
                to[N] = from[N] + val;
                AddElements<N-1, Min, ArrayT, ElemT>::add(to, from, val);
            }
        };

        template<size_t NEqualMin, typename ArrayT, typename ElemT>
        struct AddElements<NEqualMin, NEqualMin, ArrayT, ElemT>
        {
            static void add(ArrayT& to, const ArrayT& from, const ElemT& val)
            { to[NEqualMin] = from[NEqualMin] + val; }
        };

        template<typename ArrayT, typename ElemT>
        struct AddElements<0, 0, ArrayT, ElemT>
        {
            static void add(ArrayT& to, const ArrayT& from, const ElemT& val)
            { to[0] = from[0] + val; }
        };

        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        void addElements(ArrayT& to, const ArrayT& from, const ElemT& val)
        { AddElements<N-1, Min, ArrayT, ElemT>::add(to, from, val); }



        /** @brief Sustracts a value from all elements of an array
         */
        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        struct SubElements
        {
            static void sub(ArrayT& to, const ArrayT& from, const ElemT& val)
            {
                to[N] = from[N] - val;
                AddElements<N-1, Min, ArrayT, ElemT>::add(to, from, val);
            }
        };

        template<size_t NEqualMin, typename ArrayT, typename ElemT>
        struct SubElements<NEqualMin, NEqualMin, ArrayT, ElemT>
        {
            static void sub(ArrayT& to, const ArrayT& from, const ElemT& val)
            { to[NEqualMin] = from[NEqualMin] - val; }
        };

        template<typename ArrayT, typename ElemT>
        struct SubElements<0, 0, ArrayT, ElemT>
        {
            static void sub(ArrayT& to, const ArrayT& from, const ElemT& val)
            { to[0] = from[0] - val; }
        };

        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        void subElements(ArrayT& to, const ArrayT& from, const ElemT& val)
        { AddElements<N-1, Min, ArrayT, ElemT>::sub(to, from, val); }



        /** @brief Adds and assigns a value to all elements of an array
         */
        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        struct AddAssignElements
        {
            static void add(ArrayT& to, const ElemT& val)
            {
                to[N] += val;
                AddAssignElements<N-1, Min, ArrayT, ElemT>::add(to, val);
            }
        };

        template<size_t NEqualMin, typename ArrayT, typename ElemT>
        struct AddAssignElements<NEqualMin, NEqualMin, ArrayT, ElemT>
        {
            static void add(ArrayT& to, const ElemT& val)
            { to[NEqualMin] += val; }
        };

        template<typename ArrayT, typename ElemT>
        struct AddAssignElements<0, 0, ArrayT, ElemT>
        {
            static void add(ArrayT& to, const ElemT& val)
            { to[0] += val; }
        };

        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        void addAssignElements(ArrayT& to, const ElemT& val)
        { AddAssignElements<N-1, Min, ArrayT, ElemT>::add(to, val); }



        /** @brief Substracts and assigns a value to all elements of an array
         */
        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        struct SubAssignElements
        {
            static void sub(ArrayT& to, const ElemT& val)
            {
                to[N] -= val;
                SubAssignElements<N-1, Min, ArrayT, ElemT>::sub(to, val);
            }
        };

        template<size_t NEqualMin, typename ArrayT, typename ElemT>
        struct SubAssignElements<NEqualMin, NEqualMin, ArrayT, ElemT>
        {
            static void sub(ArrayT& to, const ElemT& val)
            { to[NEqualMin] += val; }
        };

        template<typename ArrayT, typename ElemT>
        struct SubAssignElements<0, 0, ArrayT, ElemT>
        {
            static void sub(ArrayT& to, const ElemT& val)
            { to[0] += val; }
        };

        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        void subAssignElements(ArrayT& to, const ElemT& val)
        { SubAssignElements<N-1, Min, ArrayT, ElemT>::sub(to, val); }

    } // namespace Gfx

} // namespace Pt

#endif

