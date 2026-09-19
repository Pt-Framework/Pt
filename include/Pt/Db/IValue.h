/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_IVALUE_H
#define PT_DB_IVALUE_H

#include <Pt/RefCounted.h>
#include <Pt/Db/Blob.h>
#include <Pt/Db/Api.h>
#include <string>


namespace Pt {

    class Date;
    class Time;
    class DateTime;
    class Variant;

namespace Db {

    /** @brief Database-value backend.
    */
    class PT_DB_API IValue : public RefCounted
    {
        public:
            /** @brief Returns true if the value is SQL NULL.
            */
            virtual bool isNull() const = 0;

            /** @brief Returns the value as bool.
            */
            virtual bool getBool() const = 0;

            /** @brief Returns the value as int.
            */
            virtual int getInt() const = 0;

            /** @brief Returns the value as unsigned.
            */
            virtual unsigned getUnsigned() const = 0;

            /** @brief Returns the value as float.
            */
            virtual float getFloat() const = 0;

            /** @brief Returns the value as double.
            */
            virtual double getDouble() const = 0;

            /** @brief Returns the value as char.
            */
            virtual char getChar() const = 0;

            /** @brief Returns the value as string.
            */
            virtual std::string getString() const = 0;

            /** @brief Returns the value as date.
            */
            virtual Date getDate() const = 0;

            /** @brief Returns the value as time.
            */
            virtual Time getTime() const = 0;

            /** @brief Returns the value as date-time.
            */
            virtual DateTime getDateTime() const = 0;

            /** @brief Writes the value as binary data into @a blobdata.
            */
            virtual void getBlob(Blob& blobdata) const = 0;
    };

} // namespace Db

} // namespace Pt

#endif // PT_DB_IVALUE_H
