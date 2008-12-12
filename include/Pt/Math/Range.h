/*
 * Copyright (C) 2007 Tobias Mueller
 * Copyright (C) 2007 PTV AG
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

#ifndef PT_MATH_RANGE_H
#define PT_MATH_RANGE_H

#include <Pt/Types.h>
#include <Pt/Math/Api.h>
#include <Pt/Math/Math.h>
#include <Pt/SourceInfo.h>
#include <Pt/SerializationInfo.h>


namespace Pt {

    namespace Math {

        /**
         * @brief Range class which describes a range by specifying a min and max value.
         */
        template<typename T>
        class BasicRange {
            public:
                BasicRange()
                : _min(0)
                , _max(0)
                {}

                BasicRange(T min, T max)
                : _min(min)
                , _max(max)
                {}

                BasicRange(const BasicRange& range)
                : _min(range._min)
                , _max(range._max)
                { }

                T min() const
                { return _min; }

                T max() const
                { return _max; }

                void setMin(T min)
                { _min = min; }

                void setMax(T max)
                { _max = max; }

                void set(T min, T max)
                {
                    _min = min;
                    _max = max;
                }

                T size() const
                {
                    return _max - _min;
                }

                const BasicRange& operator=(const BasicRange& range)
                {
                    _min = range._min;
                    _max = range._max;
                    return *this;
                }

                bool operator==(const BasicRange& range) const
                {
                    return (_min == range._min && _max == range._max);
                }

                bool operator!=(const BasicRange& range) const
                {
                    return (_min != range._min || _max != range._max);
                }

                bool operator<(const BasicRange& range) const
                {
                    if ( (_max - _min) > (range._max - range._min) )
                        return false;

                    return ( range != (*this) );
                }

            protected:
                T _min;
                T _max;
        };


        template <typename T>
        inline void operator >>=(const Pt::SerializationInfo& si, BasicRange<T>& r)
        {
            T min = si.getValue<T>("min");
            T max = si.getValue<T>("max");

            r.setMin(min);
            r.setMax(max);
        }


        template <typename T>
        inline void operator <<=(Pt::SerializationInfo& si, const BasicRange<T>& r)
        {
            si.setTypeName("Range");
            si.addValue( "min", r.min() );
            si.addValue( "max", r.max() );
        }

    } // namespace Math

} // namespace Pt

#endif

