/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
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

#ifndef PT_DB_RESULT_H
#define PT_DB_RESULT_H

#include <Pt/Db/Api.h>
#include <Pt/SmartPtr.h>
#include <Pt/Db/IResult.h>
#include <Pt/Db/Row.h>

namespace Pt {

namespace Db {

    class Row;
    class Value;

    /**
    * The class holds a resultset of a query.
    * Querys might return multiple rows, which are accessable here in arbitary
    * order.
    */
    class PT_DB_API Result
    {
        public:
            //! \brief Iterator for iterating through the rows of this result.
            class ConstIterator;

            //! \brief The size-type used for the Result class.
            typedef std::size_t size_type;

            //! \brief The value-type contained in the result.
            typedef Row value_type;

        private:
            //! \brief Reference counted implementation
            SmartPtr<IResult, InternalRefCounted<IResult> > _result;

        public:
            //! \brief default constructor
            Result() { }

            /** \brief Construct a Result from an implemenataion.

                \param res The implementation of a specific result.
            */
            Result(IResult* res)
            : _result(res)
            { }

            /**    \brief Returns a row at a given index.

                Returns a row at a given index, without range checking.
                \param row_num Index of row.
                \result Row at index.
            */
            Row getRow(size_type row_num) const;

            /**    \brief Returns a value at a given result index and row index.

                Returns a value at a given index, without range checking.
                \param row_num Index of row.
                \param field_num Index of value.
                \return Value at indices.
            */
            Value getValue(size_type row_num, size_type field_num) const;

            /** \brief Returns the number of rows of this result.

                Returns the number of rows.

                \return Number of rows in result.
            */
            size_type size() const           { return _result->size(); }

            /** \brief Test if this result-object has no rows.

                Returns true if this result-object has no rows.

                \return True if result is empty.
            */
            bool empty() const               { return size() == 0; }

            /**    \brief Return the number of columns in the result

                Returns the number of columns in the result.

                \return Number of columns.
            */
            size_type getFieldCount() const  { return _result->getFieldCount(); }

            /** \brief Returns a row at a given index.

                Returns a row at a given index, without range checking.

                \param row_num Specific row index.
                \return Row at index.
            */
            Row operator[] (size_type row_num) const;

            /** \brief Returns an iterator to the first row.

                The returned iterator is a random access iterator, thus i can be used
                with all algorithms of the stl.

                \return Iterator to begin of result.
            */
            ConstIterator begin() const;

            /** \brief Returns an iterator past the last row.

                The returned iterator is a random access iterator, thus i can be used
                with all algorithms of the stl.

                \return Iterator to end of result.
            */
            ConstIterator end() const;

            /** \brief Test if bound to a database-result.

                Returns true if not bound to a database-result.

                \return True if unbound.
            */
            bool operator!() const          { return !_result; }

            //! \brief Returns the actual implementation-class.
            const IResult* getImpl() const  { return &*_result; }
    };

    /** \brief Iterator to iterate over the rows of a result.

        The ConstIterator can perform random access to the rows of a result.
        Offsets can be added and substracted and iterators can be compared
        by using relational operators such as < or >.
    */
    class Result::ConstIterator
        : public std::iterator<std::random_access_iterator_tag, Row>
    {
        public:
            typedef const value_type& const_reference;
            typedef const value_type* const_pointer;

        private:
            Result _result;
            Row _current;
            size_type _offset;

            /** \brief Moves this iterator to the row at a given offset.

                Set iterator to row at a given offset.

                \param off The new offset.
            */
            void setOffset(size_type off)
            {
                if (off != _offset)
                {
                    _offset = off;
                    // is range checking needed here ???
                    if (_offset < _result.size())
                        _current = _result.getRow(_offset);
                }
            }

        public:

            /**    \brief Construct a const iterator pointing to a row in a result.

                Construct a const iterator that points to a row at a given index in result.

                \param r Reference to a result.
                \param off Offset of a row.
            */
            ConstIterator(const Result& r, size_type off)
            : _result(r)
            , _offset(off)
            {
                if (_offset < r.size())
                    _current = r.getRow(_offset);
            }

            /** \brief Test it two iterators are equal.

                Two iterators are equal if they occupy the same position.
                The iterators should point to the same resultset. This is
                not checked. Only the offsets are considered.

                \param it Other iterator.
                \return True if equal.
            */
            bool operator== (const ConstIterator& it) const
            { return _offset == it._offset; }

            /** \brief Test it two iterators are not equal.

                Two iterators are not equal if they occupy different positions.
                The iterators should point to the same resultset. This is
                not checked. Only the offsets are considered.

                \param it Other iterator.
                \return True if not equal.
            */
            bool operator!= (const ConstIterator& it) const
            { return !operator== (it); }

            /** \brief Steps forward.

                Steps forward and returns new position.

                \return Iterator to next element.
            */
            ConstIterator& operator++()
            { setOffset(_offset + 1); return *this; }

            /** \brief Steps forward.

                Steps forward and returns old position.

                \return Iterator to old element.
            */
            ConstIterator operator++(int)
            {
                ConstIterator ret = *this;
                setOffset(_offset + 1);
                return ret;
            }

            /** \brief Steps backward.

                Steps backward and returns new position.

                \return Iterator to previous element.
            */
            ConstIterator operator--()
            { setOffset(_offset - 1); return *this; }

            /** \brief Steps backward.

                Steps backward and returns old position.

                \return Iterator to old element.
            */
            ConstIterator operator--(int)
            {
                ConstIterator ret = *this;
                setOffset(_offset - 1);
                return ret;
            }

            /** \brief Get current element.

                Provides read access to the current row in the result.

                \return Const reference to a row.
            */
            const_reference operator*() const
            { return _current; }

            /** \brief Get pointer to current element.

                Provides read access to the current row in the result.

                \return Const pointer to a row.
            */
            const_pointer operator->() const
            { return &_current; }

            /** \brief Steps n elements forward.

                Moves this iterator n elements forward and returns an iterator to new position.

                \param n Positions to move by.
                \return Const iterator to new position.
            */
            ConstIterator& operator+= (difference_type n)
            {
                setOffset(_offset + n);
                return *this;
            }

            /** \brief Get an iterator to the nth next row.

                Returns an iterator pointing to the nth row after the row
                this iterator points to. Leaves this iterator unchanged.

                \param n Positions to move by.
                \return Const iterator to new position.
            */
            ConstIterator operator+ (difference_type n) const
            {
                ConstIterator it(*this);
                it += n;
                return it;
            }

            /** \brief Steps n elements backward.

                Moves this iterator n elements backward and returns an
                iterator to new position.

                \param n Positions to move by.
                \return Const iterator to new position.
            */
            ConstIterator& operator-= (difference_type n)
            {
                setOffset(_offset - n);
                return *this;
            }

            /** \brief Get an iterator to the nth previous row.

                Returns an iterator pointing to the nth row before the row
                this iterator points to. Leaves this iterator unchanged.

                \param n Positions to move by.
                \return Const iterator to new position.
            */
            ConstIterator operator- (difference_type n) const
            {
                ConstIterator it(*this);
                it -= n;
                return it;
            }

            /** Get the distance between two iterators

                Calculate the offset distance between this iterator and
                a given one. The iterators should point to the same resultset. This is
                not checked. Only the offsets are used.

                \param it Other iterator.
                \return Distance.

            */
            difference_type operator- (const ConstIterator& it) const
            { return _offset - it._offset; }
    };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_RESULT_H
