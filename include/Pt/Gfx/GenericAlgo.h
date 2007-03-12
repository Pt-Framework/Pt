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


        /** @brief Manipulate all vector's elements recursively using template meta-programming.
         */
        template<size_t N, typename VectorT, typename OffsetT>
        struct RecursiveVectorManipulator {
            static inline void inc(VectorT& vec)
            {
                ++vec[N];
                RecursiveVectorManipulator<N-1, VectorT, OffsetT>::inc(vec);
            }

            static inline void dec(VectorT& vec)
            {
                --vec[N];
                RecursiveVectorManipulator<N-1, VectorT, OffsetT>::dec(vec);
            }

            static inline void add(VectorT& vecDst, const VectorT& vecSrc, const OffsetT& ofs)
            {
                vecDst[N] = vecSrc[N] + ofs;
                RecursiveVectorManipulator<N-1, VectorT, OffsetT>::add(vecDst, vecSrc, ofs);
            }

            static inline void sub(VectorT& vecDst, const VectorT& vecSrc, const OffsetT& ofs)
            {
                vecDst[N] = vecSrc[N] - ofs;
                RecursiveVectorManipulator<N-1, VectorT, OffsetT>::sub(vecDst, vecSrc, ofs);
            }

            static inline void assign_add(VectorT& vec, const OffsetT& ofs)
            {
                vec[N] += ofs;
                RecursiveVectorManipulator<N-1, VectorT, OffsetT>::assign_add(vec, ofs);
            }

            static inline void assign_sub(VectorT& vec, const OffsetT& ofs)
            {
                vec[N] -= ofs;
                RecursiveVectorManipulator<N-1, VectorT, OffsetT>::assign_sub(vec, ofs);
            }

            static inline bool isEqual(const VectorT& vecA, const VectorT& vecB)
            {
                if(vecA[N] != vecB[N]) return false;
                return RecursiveVectorManipulator<N-1, VectorT, OffsetT>::isEqual(vecA, vecB);
            }

            static inline bool isDiffer(const VectorT& vecA, const VectorT& vecB)
            {
                if(vecA[N] != vecB[N]) return true;
                return RecursiveVectorManipulator<N-1, VectorT, OffsetT>::isDiffer(vecA, vecB);
            }
        };

        /** @brief Partial specialization of the class in case N = 0.
         */
        template<typename VectorT, typename OffsetT>
        struct RecursiveVectorManipulator<0, VectorT, OffsetT> {
            static inline void inc(VectorT& vec)
            { ++vec[0]; }

            static inline void dec(VectorT& vec)
            { --vec[0]; }

            static inline void add(VectorT& vecDst, const VectorT& vecSrc, const OffsetT& ofs)
            { vecDst[0] = vecSrc[0] + ofs; }

            static inline void sub(VectorT& vecDst, const VectorT& vecSrc, const OffsetT& ofs)
            { vecDst[0] = vecSrc[0] - ofs; }

            static inline void assign_add(VectorT& vec, const OffsetT& ofs)
            { vec[0] += ofs; }

            static inline void assign_sub(VectorT& vec, const OffsetT& ofs)
            { vec[0] -= ofs; }

            static inline bool isEqual(const VectorT& vecA, const VectorT& vecB)
            { return vecA[0] == vecB[0]; }

            static inline bool isDiffer(const VectorT& vecA, const VectorT& vecB)
            { return vecA[0] != vecB[0]; }
        };

        //
        // Below are convenience wrapper function for the
        // RecursiveVectorManipulator<N, T, O> class
        //
        template<size_t N, typename VectorT> inline
        void recursiveVectorInc(VectorT& vec)
        { RecursiveVectorManipulator<N, VectorT, size_t>::inc(vec); }

        template<size_t N, typename VectorT> inline
        void recursiveVectorDec(VectorT& vec)
        { RecursiveVectorManipulator<N, VectorT, size_t>::dec(vec); }

        template<size_t N, typename VectorT, typename OffsetT> inline
        void recursiveVectorAdd(VectorT& vecDst, const VectorT& vecSrc, const OffsetT& ofs)
        { RecursiveVectorManipulator<N, VectorT, OffsetT>::add(vecDst, vecSrc, ofs); }

        template<size_t N, typename VectorT, typename OffsetT> inline
        void recursiveVectorSub(VectorT& vecDst, const VectorT& vecSrc, const OffsetT& ofs)
        { RecursiveVectorManipulator<N, VectorT, OffsetT>::sub(vecDst, vecSrc, ofs); }

        template<size_t N, typename VectorT, typename OffsetT> inline
        void recursiveVectorAssignAdd(VectorT& vec, const OffsetT& ofs)
        { RecursiveVectorManipulator<N, VectorT, OffsetT>::assign_add(vec, ofs); }

        template<size_t N, typename VectorT, typename OffsetT> inline
        void recursiveVectorAssignSub(VectorT& vec, OffsetT& ofs)
        { RecursiveVectorManipulator<N, VectorT, OffsetT>::assign_sub(vec, ofs); }

        template<size_t N, typename VectorT> inline
        bool recursiveVectorIsEqual(const VectorT& vecA, const VectorT& vecB)
        { return RecursiveVectorManipulator<N, VectorT, size_t>::isEqual(vecA, vecB); }

        template<size_t N, typename VectorT> inline
        bool recursiveVectorIsDiffer(const VectorT& vecA, const VectorT& vecB)
        { return RecursiveVectorManipulator<N, VectorT, size_t>::isDiffer(vecA, vecB); }

    } // namespace Gfx

} // namespace Pt

#endif

