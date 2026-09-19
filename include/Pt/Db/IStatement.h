/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_ISTATEMENT_H
#define PT_DB_ISTATEMENT_H

#include <Pt/RefCounted.h>
#include <Pt/Db/Api.h>
#include <Pt/Db/Blob.h>
#include <Pt/Signal.h>
#include <string>

namespace Pt {

class Date;
class Time;
class DateTime;

namespace Db {

class Result;
class Row;
class Value;
class ICursor;
class IConnection;


/** @brief Prepared-statement backend.
*/
class PT_DB_API IStatement : public RefCounted
{
    friend class IConnection;

    public:
        typedef std::size_t size_type;

        /** @brief Signal emitted when asynchronous execution completes.
        */
        Signal<>& executeFinished()
        { return _executeFinished; }

        /** @brief Signal emitted when an asynchronous select completes.
        */
        Signal<>& selectFinished()
        { return _selectFinished; }

        /** @brief Returns the generated row id of the last insert.
        */
        long long lastInsertId() const
        { return _lastInsertId; }

        /** @brief Returns the connection that owns this statement.
        */
        IConnection* connection()
        { return _conn; }

        /** @brief Sets all host variables to NULL.
        */
        virtual void clear() = 0;

        /** @brief Sets the host variable @a col to NULL.
        */
        virtual void setNull(const std::string& col) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setBool(const std::string& col, bool data) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setInt(const std::string& col, int data) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setUnsigned(const std::string& col, unsigned data) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setFloat(const std::string& col, float data) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setDouble(const std::string& col, double data) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setChar(const std::string& col, char data) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setString(const std::string& col, const std::string& data) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setBlob(const std::string& col, const Blob& data) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setDate(const std::string& col, const Date& data) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setTime(const std::string& col, const Time& data) = 0;

        /** @brief Sets the host variable @a col to @a data.
        */
        virtual void setDatetime(const std::string& col, const DateTime& data) = 0;

    protected:
        explicit IStatement(IConnection* conn)
        : _conn(conn)
        , _lastInsertId(0)
        {}

    protected:
        virtual ICursor*  onCreateCursor() = 0;

    protected:
        virtual size_type onExecute() = 0;

        virtual void onBeginExec() = 0;

        virtual size_type onEndExec() = 0;

    protected:
        virtual Result onSelect() = 0;

        virtual Row onSelectRow() = 0;

        virtual Value onSelectValue() = 0;

        virtual void onBeginSelect() = 0;

        virtual Result onEndSelect() = 0;

    protected:
        void setLastInsertId(long long id)
        { _lastInsertId = id; }

    private:
        Signal<> _executeFinished;
        Signal<> _selectFinished;
        IConnection* _conn;
        long long _lastInsertId;
};

} // namespace Db

} // namespace Pt

#endif // PT_DB_ISTATEMENT_H
