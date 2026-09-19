/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_VALUE_H
#define PT_DB_VALUE_H

#include <Pt/Db/Api.h>
#include <Pt/Db/IValue.h>
#include <Pt/Db/Blob.h>
#include <Pt/Date.h>
#include <Pt/Time.h>
#include <Pt/DateTime.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Db {

    /** @brief One column value of a query result.

        %Value is one column of a %Row. It is a shared value. An
        unbound value and SQL NULL both make %isNull() true.
        %operator!() is true when the object is not bound.

        Typed getters convert the stored value. A null or a conversion
        that cannot be performed throws %TypeMismatch. %getChar() on a
        string returns the first character. %getBlob() writes binary
        data into a %Blob.

        @ingroup Pt-Db-Results
    */
    class PT_DB_API Value
    {
        public:
            typedef std::size_t size_type;

        private:
            SmartPtr<IValue, InternalRefCounted<IValue> > _value;

        public:
            /** @brief Takes ownership of the backend @a value.
            */
            explicit Value(IValue* value = 0)
            : _value(value)
            { }

            /** @brief Returns true if the value is unbound or SQL NULL.
            */
            bool isNull() const { return !_value || _value->isNull(); }

            /** @brief Returns the value as bool.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            bool getBool() const { return _value->getBool(); }

            /** @brief Returns the value as int.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            int getInt() const { return _value->getInt(); }

            /** @brief Returns the value as unsigned.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            unsigned getUnsigned() const { return _value->getUnsigned(); }

            /** @brief Returns the value as float.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            float getFloat() const { return _value->getFloat(); }

            /** @brief Returns the value as double.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            double getDouble() const { return _value->getDouble(); }

            /** @brief Returns the value as char.

                If the value is a string, the first character is returned.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            char getChar() const { return _value->getChar(); }

            /** @brief Returns the value as string.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            std::string getString() const { return _value->getString(); }

            /** @brief Returns the value as date.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            Date getDate() const { return _value->getDate(); }

            /** @brief Returns the value as time.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            Time getTime() const { return _value->getTime(); }

            /** @brief Returns the value as date-time.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            DateTime getDateTime() const { return _value->getDateTime(); }

            /** @brief Returns true if this object is not bound to a value.
            */
            bool operator!() const { return !_value; }

            /** @brief Returns the backend implementation.
            */
            const IValue* getImpl() const { return &*_value; }

            /** @brief Writes the value as binary data into @a blobdata.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            void getBlob(Blob& blobdata) const
            { return _value->getBlob(blobdata); }
    };

} // namespace Db

} // namespace Pt

#endif // PT_DB_VALUE_H
