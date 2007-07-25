/***************************************************************************
 *   Copyright (C) 2007 Tobias Mueller                                     *
 *   Copyright (C) 2007 PTV AG                                             *
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

#ifndef PT_MATH_RANGE_H
#define PT_MATH_RANGE_H

#include <Pt/Types.h>
#include <Pt/Math/Api.h>
#include <Pt/Math/Math.h>

#include <Pt/AnyTraits.h>
#include <Pt/SourceInfo.h>


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
        inline const Pt::SerializationNode& operator>>(const Pt::SerializationNode& node, BasicRange<T>& r)
        {
            const Pt::SerializationData* data = node.toData();
            if(!data)
                throw NoSuchEntry("Range", PT_SOURCEINFO);

            T min;
            T max;

            const Pt::Variant* value = data->getEntry(L"min");
            if( value == 0 || !value->get(min) )
                throw Pt::NoSuchEntry("min", PT_SOURCEINFO);

            value = data->getEntry(L"max");
            if( value == 0 || !value->get(max) )
                throw Pt::NoSuchEntry("max", PT_SOURCEINFO);

            r.setMin(min);
            r.setMax(max);

            return node;
        }


        template <typename T>
        inline Pt::SerializationNode& insert(Pt::SerializationData& parent, const BasicRange<T>& r)
        {
            SerializationData& data = parent.addData();
            data.addEntry(L"min", Pt::Variant(r.min()));
            data.addEntry(L"max", Pt::Variant(r.max()));
            return data;
        }

    } // namespace Math



    template <typename T>
    struct AnyTraits<Math::BasicRange<T> > {
        static void output(std::ostream& os, const Math::BasicRange<T>& value);
        static void input (std::istream& is, Math::BasicRange<T>& value);
        static void output(std::basic_ostream<Pt::Char>& os, const Math::BasicRange<T>& value);
        static void input (std::basic_istream<Pt::Char>& is, Math::BasicRange<T>& value);
    };


    template <typename CharT, typename T>
    inline void outputGeneric(std::basic_ostream<CharT>& os, const Math::BasicRange<T>& value)
    {
        os << '[' << value.min() << ' ' << value.max() << ']';
    }

    template <typename T>
    inline void Pt::AnyTraits<Math::BasicRange<T> >::output(std::ostream& os, const Math::BasicRange<T>& value)
    {
        outputGeneric(os, value);
    }


    template <typename T>
    inline void Pt::AnyTraits<Math::BasicRange<T> >::output(std::basic_ostream<Pt::Char>& os, const Math::BasicRange<T>& value)
    {
        outputGeneric(os, value);
    }


    template <typename CharT, typename T>
    inline void inputGeneric(std::basic_istream<CharT>& is, Math::BasicRange<T>& value)
    {
        CharT ch;

        is >> ch;
        if (ch != '[')
        {
            throw std::runtime_error("Could not read Range value" + PT_SOURCEINFO);
        }

        T min;
        T max;

        is >> min;
        is >> max;

        is >> ch;
        if (ch != ']')
        {
            throw std::runtime_error("Could not read Range value" + PT_SOURCEINFO);
        }

        value.set(min, max);
    }


    template <typename T>
    inline void Pt::AnyTraits<Math::BasicRange<T> >::input(std::istream& is, Math::BasicRange<T>& value)
    {
        inputGeneric(is, value);
    }


    template <typename T>
    inline void Pt::AnyTraits<Math::BasicRange<T> >::input(std::basic_istream<Pt::Char>& is, Math::BasicRange<T>& value)
    {
        inputGeneric(is, value);
    }


} // namespace Pt

#endif

