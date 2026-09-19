/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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

    /** @brief Buffered query result with random-access rows.

        %Result is the in-memory result the group described. Rows are
        addressed by index and visited with a random-access iterator.
        The set can be walked more than once. It is a shared value.
        An unbound result is empty; %operator!() is true then.

        %size() is the number of rows. %getFieldCount() is the number
        of columns. %empty() is true when there are no rows.
        %operator[] and %getRow() return a %Row without range
        checking. %getValue() returns a column of a row the same way.
        %begin() and %end() are random-access iterators, so the result
        can be used with standard algorithms.

        Once a %Result is returned, the connection can run another
        statement. For a set that should not be fully buffered, use a
        %Cursor.

        @ingroup Pt-Db-Results
    */
    class PT_DB_API Result
    {
        public:
            class ConstIterator;

            typedef std::size_t size_type;

            typedef Row value_type;

        private:
            SmartPtr<IResult, InternalRefCounted<IResult> > _result;

        public:
            /** @brief Creates an unbound result.
            */
            Result() { }

            /** @brief Takes ownership of the backend @a res.
            */
            Result(IResult* res)
            : _result(res)
            { }

            /** @brief Returns the row at @a row_num without range checking.
            */
            Row getRow(size_type row_num) const;

            /** @brief Returns the value at @a row_num, @a field_num without range checking.
            */
            Value getValue(size_type row_num, size_type field_num) const;

            /** @brief Returns the number of rows.
            */
            size_type size() const           { return _result->size(); }

            /** @brief Returns true if this result has no rows.
            */
            bool empty() const               { return size() == 0; }

            /** @brief Returns the number of columns.
            */
            size_type getFieldCount() const  { return _result->getFieldCount(); }

            /** @brief Returns the row at @a row_num without range checking.
            */
            Row operator[] (size_type row_num) const;

            /** @brief Returns a random-access iterator to the first row.
            */
            ConstIterator begin() const;

            /** @brief Returns a random-access iterator past the last row.
            */
            ConstIterator end() const;

            /** @brief Returns true if this object is not bound to a result.
            */
            bool operator!() const          { return !_result; }

            /** @brief Returns the backend implementation.
            */
            const IResult* getImpl() const  { return &*_result; }
    };

    /** @brief Random-access iterator over the rows of a %Result.
    */
    class Result::ConstIterator
    {
        public:
            typedef std::random_access_iterator_tag iterator_category;
            typedef Row                             value_type;
            typedef std::ptrdiff_t                  difference_type;
            typedef const Row*                      pointer;
            typedef const Row&                      reference;

            typedef const value_type& const_reference;
            typedef const value_type* const_pointer;

        private:
            Result _result;
            Row _current;
            size_type _offset;

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

            ConstIterator(const Result& r, size_type off)
            : _result(r)
            , _offset(off)
            {
                if (_offset < r.size())
                    _current = r.getRow(_offset);
            }

            bool operator== (const ConstIterator& it) const
            { return _offset == it._offset; }

            bool operator!= (const ConstIterator& it) const
            { return !operator== (it); }

            ConstIterator& operator++()
            { setOffset(_offset + 1); return *this; }

            ConstIterator operator++(int)
            {
                ConstIterator ret = *this;
                setOffset(_offset + 1);
                return ret;
            }

            ConstIterator operator--()
            { setOffset(_offset - 1); return *this; }

            ConstIterator operator--(int)
            {
                ConstIterator ret = *this;
                setOffset(_offset - 1);
                return ret;
            }

            const_reference operator*() const
            { return _current; }

            const_pointer operator->() const
            { return &_current; }

            ConstIterator& operator+= (difference_type n)
            {
                setOffset(_offset + n);
                return *this;
            }

            ConstIterator operator+ (difference_type n) const
            {
                ConstIterator it(*this);
                it += n;
                return it;
            }

            ConstIterator& operator-= (difference_type n)
            {
                setOffset(_offset - n);
                return *this;
            }

            ConstIterator operator- (difference_type n) const
            {
                ConstIterator it(*this);
                it -= n;
                return it;
            }

            difference_type operator- (const ConstIterator& it) const
            { return _offset - it._offset; }
    };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_RESULT_H
