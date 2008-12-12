/*
 * Copyright (C) 2006-2007 by Aloysius Indrayanto
 * Copyright (C) 2006-2007 by Marc Boris Duerner
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
#ifndef Pt_Gfx_GenericAlgo_h
#define Pt_Gfx_GenericAlgo_h

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>


namespace Pt {

    namespace Gfx {

        // TODO For all recursive template -> make both the classes and the helper
        //      functions use the same convention -> using Min/Max instead of N

        //! \internal
        template<size_t N, size_t Min, typename ArrayT>
        struct EqualElements
        {
            static inline bool equal(const ArrayT& a, const ArrayT& b)
            {
                if(a[N] != b[N]) return false;
                return EqualElements<N-1, Min, ArrayT>::equal(a, b);
            }
        };

        //! \internal
        template<size_t NEqualMin, typename ArrayT>
        struct EqualElements<NEqualMin, NEqualMin, ArrayT>
        {
            static bool equal(const ArrayT& a, const ArrayT& b)
            { return a[NEqualMin] == b[NEqualMin];  }
        };

        //! \internal
        template<typename ArrayT>
        struct EqualElements<0, 0, ArrayT>
        {
            static bool equal(const ArrayT& a, const ArrayT& b)
            { return a[0] == b[0];  }
        };

        //! \internal
        template<size_t N, size_t Min, typename ArrayT> inline
        bool equalElements(const ArrayT& a, const ArrayT& b)
        { return EqualElements<N-1, Min, ArrayT>::equal(a, b); }



        //! \internal
        template<size_t N, size_t Min, typename ArrayT>
        struct NotEqualElements
        {
            static inline bool notEqual(const ArrayT& a, const ArrayT& b)
            {
                if(a[N] != b[N]) return true;
                return NotEqualElements<N-1, Min, ArrayT>::notEqual(a, b);
            }
        };

        //! \internal
        template<size_t NEqualMin, typename ArrayT>
        struct NotEqualElements<NEqualMin, NEqualMin, ArrayT>
        {
            static bool notEqual(const ArrayT& a, const ArrayT& b)
            { return a[NEqualMin] != b[NEqualMin];  }
        };

        //! \internal
        template<typename ArrayT>
        struct NotEqualElements<0, 0, ArrayT>
        {
            static bool notEqual(const ArrayT& a, const ArrayT& b)
            { return a[0] != b[0];  }
        };

        //! \internal
        template<size_t N, size_t Min, typename ArrayT> inline
        bool notEqualElements(const ArrayT& a, const ArrayT& b)
        { return NotEqualElements<N-1, Min, ArrayT>::notEqual(a, b); }



        //! \internal
        template<size_t N, size_t Min, typename ArrayT>
        struct IncrementElements
        {
            static void inc(ArrayT& array)
            {
                ++array[N];
                IncrementElements<N-1, Min, ArrayT>::inc(array);
            }
        };

        //! \internal
        template<size_t NEqualMin, typename ArrayT>
        struct IncrementElements<NEqualMin, NEqualMin, ArrayT>
        {
            static void inc(ArrayT& array)
            { ++array[NEqualMin]; }
        };

        //! \internal
        template<typename ArrayT>
        struct IncrementElements<0, 0, ArrayT>
        {
            static void inc(ArrayT& array)
            { ++array[0]; }
        };

        //! \internal
        template<size_t N, size_t Min, typename ArrayT>
        void incrementElements(ArrayT& array)
        { IncrementElements<N-1, Min, ArrayT>::inc(array); }



        //! \internal
        template<size_t N, size_t Min, typename ArrayT>
        struct DecrementElements
        {
            static void inc(ArrayT& array)
            {
                ++array[N];
                DecrementElements<N-1, Min, ArrayT>::inc(array);
            }
        };

        //! \internal
        template<size_t NEqualMin, typename ArrayT>
        struct DecrementElements<NEqualMin, NEqualMin, ArrayT>
        {
            static void inc(ArrayT& array)
            { ++array[NEqualMin]; }
        };

        //! \internal
        template<typename ArrayT>
                struct DecrementElements<0, 0, ArrayT>
        {
            static void inc(ArrayT& array)
            { ++array[0]; }
        };

        //! \internal
        template<size_t N, size_t Min, typename ArrayT>
        void decrementElements(ArrayT& array)
        { DecrementElements<N-1, Min, ArrayT>::inc(array); }



        //! \internal
        template<size_t N, size_t Min, typename ArrayA, typename ArrayB, typename ElemT>
        struct AddElements
        {
            static void add(ArrayA& to, const ArrayB& from, const ElemT& val)
            {
                to[N] = from[N] + val;
                AddElements<N-1, Min, ArrayA, ArrayB, ElemT>::add(to, from, val);
            }
        };

        //! \internal
        template<size_t NEqualMin, typename ArrayA, typename ArrayB, typename ElemT>
        struct AddElements<NEqualMin, NEqualMin, ArrayA, ArrayB, ElemT>
        {
            static void add(ArrayA& to, const ArrayB& from, const ElemT& val)
            { to[NEqualMin] = from[NEqualMin] + val; }
        };

        //! \internal
        template<typename ArrayA, typename ArrayB, typename ElemT>
        struct AddElements<0, 0, ArrayA, ArrayB, ElemT>
        {
            static void add(ArrayA& to, const ArrayB& from, const ElemT& val)
            { to[0] = from[0] + val; }
        };

        //! \internal
        template<size_t N, size_t Min, typename ArrayA, typename ArrayB, typename ElemT>
        void addElements(ArrayA& to, const ArrayB& from, const ElemT& val)
        { AddElements<N-1, Min, ArrayA, ArrayB, ElemT>::add(to, from, val); }



        //! \internal
        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        struct SubElements
        {
            static void sub(ArrayT& to, const ArrayT& from, const ElemT& val)
            {
                to[N] = from[N] - val;
                SubElements<N-1, Min, ArrayT, ElemT>::add(to, from, val);
            }
        };

        //! \internal
        template<size_t NEqualMin, typename ArrayT, typename ElemT>
        struct SubElements<NEqualMin, NEqualMin, ArrayT, ElemT>
        {
            static void sub(ArrayT& to, const ArrayT& from, const ElemT& val)
            { to[NEqualMin] = from[NEqualMin] - val; }
        };

        //! \internal
        template<typename ArrayT, typename ElemT>
        struct SubElements<0, 0, ArrayT, ElemT>
        {
            static void sub(ArrayT& to, const ArrayT& from, const ElemT& val)
            { to[0] = from[0] - val; }
        };

        //! \internal
        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        void subElements(ArrayT& to, const ArrayT& from, const ElemT& val)
        { SubElements<N-1, Min, ArrayT, ElemT>::sub(to, from, val); }



        //! \internal
        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        struct AddAssignElements
        {
            static void add(ArrayT& to, const ElemT& val)
            {
                to[N] += val;
                AddAssignElements<N-1, Min, ArrayT, ElemT>::add(to, val);
            }
        };

        //! \internal
        template<size_t NEqualMin, typename ArrayT, typename ElemT>
        struct AddAssignElements<NEqualMin, NEqualMin, ArrayT, ElemT>
        {
            static void add(ArrayT& to, const ElemT& val)
            { to[NEqualMin] += val; }
        };

        //! \internal
        template<typename ArrayT, typename ElemT>
        struct AddAssignElements<0, 0, ArrayT, ElemT>
        {
            static void add(ArrayT& to, const ElemT& val)
            { to[0] += val; }
        };

        //! \internal
        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        void addAssignElements(ArrayT& to, const ElemT& val)
        { AddAssignElements<N-1, Min, ArrayT, ElemT>::add(to, val); }




        //! \internal
        template<size_t N, size_t Min, typename ArrayA, typename ArrayB>
        struct AssignElements
        {
#ifdef __MWERKS_SYMBIAN__
        	static void assign(ArrayA& to, ArrayB& from)
#else
        	static void assign(ArrayA& to, const ArrayB& from)
#endif
        	{
                to[N] = from[N];
                AssignElements<N-1, Min, ArrayA, ArrayB>::assign(to, from);
            }
        };

        //! \internal
        template<size_t NEqualMin, typename ArrayA, typename ArrayB>
        struct AssignElements<NEqualMin, NEqualMin, ArrayA, ArrayB>
        {
            static void assign(ArrayA& to, const ArrayB& from)
            { to[NEqualMin] = from[NEqualMin]; }
        };

        //! \internal
        template<typename ArrayA, typename ArrayB>
        struct AssignElements<0, 0, ArrayA, ArrayB>
        {
#ifdef __MWERKS_SYMBIAN__        	
            static void assign(ArrayA& to, ArrayB& from)
#else
            static void assign(ArrayA& to, const ArrayB& from)
#endif
            { to[0] = from[0]; }
        };

        //! \internal
        template<size_t N, size_t Min, typename ArrayA, typename ArrayB>
#ifdef __MWERKS_SYMBIAN__
        void assignElements(ArrayA& to, ArrayB& from)
#else
        void assignElements(ArrayA& to, const ArrayB& from)
#endif
        { AssignElements<N-1, Min, ArrayA, ArrayB>::assign(to, from); }




        //! \internal
        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        struct SubAssignElements
        {
            static void sub(ArrayT& to, const ElemT& val)
            {
                to[N] -= val;
                SubAssignElements<N-1, Min, ArrayT, ElemT>::sub(to, val);
            }
        };

        //! \internal
        template<size_t NEqualMin, typename ArrayT, typename ElemT>
        struct SubAssignElements<NEqualMin, NEqualMin, ArrayT, ElemT>
        {
            static void sub(ArrayT& to, const ElemT& val)
            { to[NEqualMin] += val; }
        };

        //! \internal
        template<typename ArrayT, typename ElemT>
        struct SubAssignElements<0, 0, ArrayT, ElemT>
        {
            static void sub(ArrayT& to, const ElemT& val)
            { to[0] += val; }
        };

        //! \internal
        template<size_t N, size_t Min, typename ArrayT, typename ElemT>
        void subAssignElements(ArrayT& to, const ElemT& val)
        { SubAssignElements<N-1, Min, ArrayT, ElemT>::sub(to, val); }

    } // namespace Gfx

} // namespace Pt

#endif

